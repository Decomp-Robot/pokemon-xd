#include <GSmsg.h>
#include <flashsystem.h>
#include <hero.h>
#include <hsdRandom.h>
#include <menuFace.h>
#include <menuItemBios.h>
#include <menuOrre.h>
#include <menuSub.h>
#include <msgctrl.h>
#include <time.h>
#include <winMsg.h>
#include <winSeq.h>
#include <window.h>

#include <cursorBios.h>
#include <fight.h>
#include <fightEncount.h>
#include <fightEncountBios.h>
#include <fightResource.h>
#include <fightTrainer.h>
#include <fightTrainerDB.h>
#include <floor.h>
#include <floorManager.h>
#include <menu.h>
#include <savedata.h>
#include <string.h>
#include <libc/math.h>
#include <GSthread.h>
#include <fade.h>
#include <heroBios.h>

#pragma section RW ".rodata"

#pragma force_active on

/*
 * Orre Colosseum team selection. The window lists the player's own party on
 * the left and the opponent's deck on the right; both panels print the same
 * Pokemon information from their respective records.
 */

#define ORRE_TEAM_MENU_BIOS 253
#define ORRE_TEAM_MENU_SEQ 36

// Messages printed in the team-selection panels.
#define ORRE_TEAM_NAME_PRINT 0x4276
#define ORRE_TEAM_NAME_PRINT_AT_POS 0x4277
#define ORRE_TEAM_LEVEL_LABEL 0x42C1
#define ORRE_TEAM_LEVEL_NUMBER 0x4352
#define ORRE_TEAM_SEX_MALE 0x9C5
#define ORRE_TEAM_SEX_FEMALE 0x9C6
#define ORRE_TEAM_CONFIRM_PRINT 0x8AD1

// Messages of the pick and drop confirmation menus.
#define ORRE_TEAM_PICK_MESSAGE 0x8ACE
#define ORRE_TEAM_DROP_MESSAGE 0x8AF1
#define ORRE_TEAM_PICK_DROP_MESSAGE_2 0x8ACF
#define ORRE_TEAM_PICK_DROP_MESSAGE_3 0x8AD0

// Textures of the panel frame and of the held-item icon.
#define ORRE_TEAM_PANEL_TEXTURE 795
#define ORRE_TEAM_HELD_ITEM_TEXTURE 331

// Frame labels of the team-count flash, and the clip played for each count.
extern const char menuOrreFrameStart[] = "1_start";
extern const char menuOrreFrame1End[] = "1_end";
extern const char menuOrreFrame2End[] = "2_end";
extern const char menuOrreFrame3End[] = "3_end";
extern const char menuOrreFrame4End[] = "4_end";
__declspec(section ".rodata") extern MenuOrreFrameLabels menuOrreTeamFrameLabels = {
    {
        menuOrreFrameStart, menuOrreFrame1End, menuOrreFrame2End,
        menuOrreFrame3End, menuOrreFrame4End,
    },
};

__declspec(section ".rodata") extern MenuOrreFramePairs menuOrreTeamFramePairs = {
    {
        menuOrreFrameStart, menuOrreFrameStart,
        menuOrreFrameStart, menuOrreFrame1End,
        menuOrreFrameStart, menuOrreFrame2End,
        menuOrreFrameStart, menuOrreFrame3End,
        menuOrreFrameStart, menuOrreFrame4End,
    },
};

// Message triples of the pick and drop confirmation menus.
struct MenuOrreMessages { u32 msg[3]; };
extern const MenuOrreMessages menuOrrePickMessages = {
    { ORRE_TEAM_PICK_MESSAGE, ORRE_TEAM_PICK_DROP_MESSAGE_2, ORRE_TEAM_PICK_DROP_MESSAGE_3 }
};
extern const MenuOrreMessages menuOrreDropMessages = {
    { ORRE_TEAM_DROP_MESSAGE, ORRE_TEAM_PICK_DROP_MESSAGE_2, ORRE_TEAM_PICK_DROP_MESSAGE_3 }
};

// Item ids of the temochi and deck rows, in display order.
struct MenuOrreItemIDs { s16 id[6]; };
extern const MenuOrreItemIDs menuOrreTemochiItemIDs = { { 1, 2, 3, 4, 5, 6 } };
extern const MenuOrreItemIDs menuOrreDeckItemIDs = { { 7, 8, 9, 10, 11, 12 } };

// Menu item ids of the level digits and of the selectable rows.
extern const MenuOrreItemIDs menuOrreNumberItems = { { 19, 20, 21, 22, 23, 24 } };
struct MenuOrreCursorItems { s32 id[6]; };
extern const MenuOrreCursorItems menuOrreCursorItems = { { 13, 14, 15, 16, 17, 18 } };

// Texture ids of the two halves of each level digit.
struct MenuOrreNumberTexIDs { u16 id[4]; };
extern const MenuOrreNumberTexIDs menuOrreNumberTexIDs = { { 796, 797, 798, 799 } };


// Records handed to the face loader, and the state of the selection window.
u16 _deckNumber;
u16 _fightresult;
menuOrre *pOrre;
u16 _loadFaceCount;

Pokemon *_loadPokemonCallBack(void *context);
Pokemon *_PokemonStatusDisp_CallBack(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type, long context);
Pokemon *_GetTemochiPokemonPtr(u16 index);

// Flash handles used by the selection loop.
#define ORRE_TEAM_FLASH_WAIT 0x1F5C3000
#define ORRE_TEAM_FLASH_PLAY 0x1F5A3000
#define ORRE_TEAM_FLASH_FADE 0x1F5B3000

// Resource IDs of the colosseum deck archive.
#define ORRE_TEAM_RESOURCE_GROUP 2114
#define ORRE_TEAM_RESOURCE_ID 0x184C3400
#define ORRE_COLOSSEUM_FLOOR 906

Hero _orreHero;

void menuOrreExit(void)
{
    if (pOrre != 0) {
        pOrre->Release();
        if (pOrre != 0) {
            delete pOrre;
        }
        pOrre = 0;
    }
    fightFreeResource(ORRE_TEAM_RESOURCE_GROUP, ORRE_TEAM_RESOURCE_ID);
}

void menuOrreMain(void)
{
    if (pOrre != 0) {
        pOrre->Main();
    }
}

void menuOrreInit(void)
{
    if (GSfloorIsPop()) {
        return;
    }
    pOrre = new menuOrre;
    if (pOrre != 0) {
        pOrre->Init();
    }
}

u16 menuOrreColosseumGetResult(void)
{
    return _fightresult;
}

void menuOrreColosseum(u16 encountNo)
{
    _deckNumber = encountNo;
    _fightresult = 0;
    GSfloorPush(ORRE_COLOSSEUM_FLOOR);
    floorWaitForChange();
}

s32 menuOrreMenuHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (pOrre != 0) {
        return pOrre->Hook(window, sprite);
    }
    return 0;
}

void menuOrreCtrl(tagWINDOW_WORK *window)
{
    if (pOrre != 0) {
        pOrre->Ctrl(window);
    }
}

void menuOrreButton(tagWINDOW_WORK *window)
{
    if (pOrre != 0) {
        pOrre->Button(window);
    }
}

inline void menuOrre::SetFightPokemon(Hero *hero)
{
    u16 i;
    for (i = 0; i < 6; i++) {
        pokemonInit(heroBiosGetPokemonPtr(hero, i));
    }
    for (i = 0; i < 4; i++) {
        Pokemon *pokemon = GetTemochiPokemonPtr(mSelectList[i]);
        pokemonBiosCopy(heroBiosGetPokemonPtr(hero, i), pokemon);
    }
}

inline u8 menuOrreIsFlashEnd(u32 flash)
{
    u32 endFrame = FlashSystemGetEndFrame(flash);

    if (FlashSystemGetNowFrame(flash) >= endFrame) {
        return true;
    }
    return false;
}

inline bool menuOrre::IsSelectedPokemon(u16 id)
{
    for (u16 i = 0; i < 4; i++) {
        if (mSelectList[i] == (s16)id) {
            return true;
        }
    }
    return false;
}

inline u8 menuOrre::AppendSelectedPokemon(u16 id)
{
    u8 full = false;

    mSelectList[mSelectCount++] = id;
    s16 count = mSelectCount;
    if (count >= 4) {
        full = true;
    }
    MenuOrreFramePairs pairs = menuOrreTeamFramePairs;
    FlashSystemSetPlayFrameLabel(ORRE_TEAM_FLASH_PLAY,
        pairs.pair[count * 2], pairs.pair[count * 2 + 1]);
    return full;
}

inline u8 menuOrre::RemoveSelectedPokemon(u16 id)
{
    if (mSelectCount != 0) {
        u16 i;

        for (i = 0; i < 4; i++) {
            if (mSelectList[i] == id) {
                break;
            }
        }
        mSelectCount--;
        for (; i < 3; i++) {
            mSelectList[i] = mSelectList[i + 1];
        }
        MenuOrreFrameLabels labels = menuOrreTeamFrameLabels;
        FlashSystemSetNowFrame(ORRE_TEAM_FLASH_PLAY,
            FlashSystemGetFrameFromLabel(ORRE_TEAM_FLASH_PLAY, labels.label[mSelectCount]));
        MenuOrreFramePairs pairs = menuOrreTeamFramePairs;
        FlashSystemSetPlayFrameLabel(ORRE_TEAM_FLASH_PLAY,
            pairs.pair[mSelectCount * 2], pairs.pair[mSelectCount * 2 + 1]);
        return true;
    }
    return false;
}

inline u8 menuOrre::CancelSelectedPokemon()
{
    if (mSelectCount != 0) {
        mSelectCount--;
        mSelectList[mSelectCount] = -1;
        MenuOrreFrameLabels labels = menuOrreTeamFrameLabels;
        FlashSystemSetNowFrame(ORRE_TEAM_FLASH_PLAY,
            FlashSystemGetFrameFromLabel(ORRE_TEAM_FLASH_PLAY, labels.label[mSelectCount]));
        MenuOrreFramePairs pairs = menuOrreTeamFramePairs;
        FlashSystemSetPlayFrameLabel(ORRE_TEAM_FLASH_PLAY,
            pairs.pair[mSelectCount * 2], pairs.pair[mSelectCount * 2 + 1]);
        return true;
    }
    return false;
}

inline void menuOrre::ClearSelectedPokemon()
{
    mSelectCount = 0;
    for (u16 i = 0; i < 4; i++) {
        mSelectList[i] = -1;
    }
}


void menuOrre::Release()
{
}

void menuOrre::Main()
{
    u8 decided = true;
    u8 selecting;
    bool selected;
    long selectID;

    _loadFaceCount = 0;
    menuFaceLoad(0, _loadPokemonCallBack, &_loadFaceCount);
    menuOpen(255, 0);

    do {
        GSthreadSwitch();
    } while (!FlashSystemIsLastFrame(ORRE_TEAM_FLASH_WAIT));

    MenuOrreFramePairs fadePairs = menuOrreTeamFramePairs;
    FlashSystemSetPlayFrameLabel(ORRE_TEAM_FLASH_FADE, fadePairs.pair[0], fadePairs.pair[1]);

    while (true) {
        mTimer = 1.0f;
        selecting = true;
        MenuOrreFramePairs pairs = menuOrreTeamFramePairs;
        FlashSystemSetPlayFrameLabel(ORRE_TEAM_FLASH_PLAY, pairs.pair[0], pairs.pair[1]);

        do {
            selectID = mSelectID;
            if (menuOpenCustom(ORRE_TEAM_MENU_BIOS, windowGetActiveID(), &selectID, 0, true, 0) >= 0) {
                selected = IsSelectedPokemon(mSelectID);
                switch (SelectPokemon(mSelectID, selected)) {
                case 0:
                    if (selected) {
                        decided = RemoveSelectedPokemon(mSelectID);
                    } else if (AppendSelectedPokemon(mSelectID) == true) {
                        selecting = false;
                    }
                    break;
                case 1:
                    menuPokemonStatusOpenFromPokemonChange(GetTemochiPokemonPtr(mSelectID),
                        _PokemonStatusDisp_CallBack, GetPokemonStatusContext());
                    fadeSet(2, 0.5f);
                    break;
                case 2:
                    break;
                }
            } else {
                decided = CancelSelectedPokemon();
                if (!decided) {
                    selecting = false;
                }
            }
        } while (selecting);

        if (!decided) {
            menuClose(ORRE_TEAM_MENU_BIOS);
            break;
        }
        do {
            GSthreadSwitch();
        } while (!menuOrreIsFlashEnd(ORRE_TEAM_FLASH_PLAY));
        if (ConfirmPokemon()) {
            break;
        }
        ClearSelectedPokemon();
    }

    if (decided == true) {
        while (menuFaceCheckLoading()) {
            GSthreadSwitch();
        }
        do {
            GSthreadSwitch();
        } while (!menuOrreIsFlashEnd(ORRE_TEAM_FLASH_FADE));

        SetFightPokemon(fightGetMenuCtrlHero());
        fightSetMenuCtrlHero(fightGetMenuCtrlHero());
        _fightresult = fightEncount(mEncountNo);
    }

    menuCloseSync(ORRE_TEAM_MENU_BIOS, true);
    menuClose(255);
    menuCloseSync(255, true);
    if (mFloorPushed) {
        GSfloorPop();
    }
}

void menuOrre::Init()
{
    mFloorPushed = GSfloorIsPush();
    InitSub();
}

void menuOrre::InitSub()
{
    fightReadResource(ORRE_TEAM_RESOURCE_GROUP, ORRE_TEAM_RESOURCE_ID);
    mEncountNo = _deckNumber;
    mTrainerDataID = fightEncountDataBiosGetFightTrainerDataId(
        fightEncountDataBiosGetPtr(mEncountNo), 1);
    fightTrainerCreateFightTrainerDataIdToHero(mTrainerDataID, 0, &_orreHero,
                                                mEncountNo);
    Hero *menuCtrlHero = fightGetMenuCtrlHero();
    memcpy(menuCtrlHero, GetHeroPtr(0), sizeof(Hero));
    mMaxPokemonLevel = GetTemochiMaxPokemonLevel();
    CreatePokemonList();
}

Pokemon *_PokemonStatusDisp_CallBack(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type,
                                     long context)
{
    u16 *index = (u16 *)context;
    Pokemon *result = 0;

    switch (type) {
    case MENU_POKEMON_CB_CURSOR_LEFT:
        if (*index == 0) {
            return pokemon;
        }
        (*index)--;
        result = _GetTemochiPokemonPtr(*index);
        break;
    case MENU_POKEMON_CB_CURSOR_RIGHT:
        if (*index == 5) {
            return pokemon;
        }
        (*index)++;
        result = _GetTemochiPokemonPtr(*index);
        break;
    }
    return result;
}

Pokemon *_GetTemochiPokemonPtr(u16 index)
{
    if (pOrre != 0) {
        return pOrre->GetTemochiPokemonPtr(index);
    }
    return 0;
}



s32 menuOrre::Hook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u32 size;

    switch (sprite->mItemID) {
    case 1: case 2: case 3: case 4: case 5: case 6:
        LeftPanel(window, sprite);
        break;
    case 7: case 8: case 9: case 10: case 11: case 12:
        RightPanel(window, sprite);
        break;
    case 19: case 20: case 21: case 22: case 23: case 24:
        SetNumber(window, sprite);
        break;
    case 13: case 14: case 15: case 16: case 17: case 18:
        ControlCursor(window, sprite);
        break;
    case 25: {
        msgctrlSetValue(MSGCTRL_TEXT, (u32)heroBiosGetNamePtr(GetHeroPtr(0)));
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, sprite->mItemID);
        size = GSmsgGetMessageSize(ORRE_TEAM_NAME_PRINT);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), (s16)(item->mWidth - (size >> 16)),
                    0, window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_NAME_PRINT);
        break;
    }
    case 26:
        msgctrlSetValue(MSGCTRL_TEXT,
            (u32)GSmsgGetStringPtr(fightTrainerDB_GetName(mTrainerDataID)));
        size = GSmsgGetMessageSize(ORRE_TEAM_NAME_PRINT);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                    window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_NAME_PRINT);
        break;
    }
    return 0;
}

void menuOrre::ControlCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = window->mCursor;
    MenuOrreCursorItems cursorItems = menuOrreCursorItems;
    s16 row;

    for (row = 0; row < 6; row++) {
        if (sprite->mItemID == cursorItems.id[row]) {
            break;
        }
    }

    if (row == cursor.y) {
        if (window == windowSearchID(windowGetActiveID())) {
            int frame = mFrameCount % 60;
            sprite->mColor = 0xFFFFFFFF;
            if (frame < 30) {
                f32 wave = sin(3.14f * ((f32)frame / 30.0f));
                sprite->mRgba.mAlpha -= (u8)(128.0f * wave);
            }
        } else {
            sprite->mRgba.mAlpha = 0xFF;
        }
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }
}

void menuOrre::SetNumber(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreItemIDs numberItems = menuOrreNumberItems;
    MenuOrreNumberTexIDs texIDs = menuOrreNumberTexIDs;
    u16 row;
    u16 i;

    for (row = 0; row < 6; row++) {
        if (sprite->mItemID == numberItems.id[row]) {
            break;
        }
    }

    for (i = 0; i < 4; i++) {
        if (row == mSelectList[i]) {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight,
                              window->mRgba.mColorA | 0xFFFFFF00, window,
                              texIDs.id[i], 0,
                              winSpriteGetLayerID(window, sprite));
            break;
        }
    }
}

u16 *menuOrre::GetDeckPokemonName(u16 index)
{
    return pokemon_GetNickname(GetDeckPokemonPtr(index));
}

u16 menuOrre::GetDeckPokemonFaceID(u16 index)
{
    return menuFaceGetID(GetDeckPokemonPtr(index));
}

u8 menuOrre::GetDeckPokemonLevel(u16 index)
{
    return pokemonBiosGetLevel(GetDeckPokemonPtr(index));
}

u8 menuOrre::GetDeckPokemonSex(u16 index)
{
    return menuSubGetPokemonSexForFightDisp(GetDeckPokemonPtr(index));
}

u16 menuOrre::GetDeckPokemonMochimono(u16 index)
{
    return pokemonGetSoubiItemDataId(GetDeckPokemonPtr(index));
}

Pokemon *menuOrre::GetDeckPokemonPtr(u16 index)
{
    return heroBiosGetPokemonPtr(GetHeroPtr(1), index);
}

u16 *menuOrre::GetTemochiPokemonName(u16 index)
{
    return pokemon_GetNickname(GetTemochiPokemonPtr(index));
}

u16 menuOrre::GetTemochiPokemonFaceID(u16 index)
{
    return menuFaceGetID(GetTemochiPokemonPtr(index));
}

u8 menuOrre::GetTemochiMaxPokemonLevel()
{
    u8 maxLevel = 0;
    u16 i;

    for (i = 0; i < 6; i++) {
        u8 level = GetTemochiPokemonLevel(i);
        if (maxLevel < level) {
            maxLevel = level;
        }
    }
    return maxLevel;
}

u8 menuOrre::GetTemochiPokemonLevel(u16 index)
{
    return pokemonBiosGetLevel(GetTemochiPokemonPtr(index));
}

u8 menuOrre::GetTemochiPokemonSex(u16 index)
{
    return menuSubGetPokemonSexForFightDisp(GetTemochiPokemonPtr(index));
}

u16 menuOrre::GetTemochiPokemonMochimono(u16 index)
{
    return pokemonGetSoubiItemDataId(GetTemochiPokemonPtr(index));
}

Pokemon *menuOrre::GetTemochiPokemonPtr(u16 index)
{
    return heroBiosGetPokemonPtr(GetHeroPtr(0), index);
}

Hero *menuOrre::GetHeroPtr(u8 trainer)
{
    if (trainer != 0) {
        return &_orreHero;
    }
    return (Hero *)savedataGetStatus(0, 2);
}

void menuOrre::RightPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreItemIDs idStruct = menuOrreDeckItemIDs;
    u16 row;
    tagMENU_ITEM_DATA *bio;
    u32 size;
    u8 sex;
    u16 *name;
    u8 level;
    u16 faceID;
    u32 sexMessage;
    GSivec2 facePosition;
    GSivec2 faceSize;

    for (row = 0; row < 6; row++) {
        if (idStruct.id[row] == sprite->mItemID) {
            break;
        }
    }

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, sprite->mItemID);
    windowDrawSprite2(0, 0, bio->mWidth, bio->mHeight,
        window->mRgba.mColorA | 0xFFFFFF00, window, ORRE_TEAM_PANEL_TEXTURE, 0,
        winSpriteGetLayerID(window, sprite));

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 31);
    name = GetDeckPokemonName(row);
    msgctrlSetValue(MSGCTRL_TEXT, (u32)name);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), bio->mX, bio->mY,
        sprite->mWidth, sprite->mHeight, window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_NAME_PRINT_AT_POS);

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 32);
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), bio->mX, bio->mY + 2,
        window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_LEVEL_LABEL);
    size = GSmsgGetMessageSize(ORRE_TEAM_LEVEL_LABEL);
    level = GetDeckPokemonLevel(row);
    msgctrlSetValue(MSGCTRL_NUM_VALUE, level);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite),
        bio->mX + 4 + (s16)(size >> 16), bio->mY + 2,
        sprite->mWidth, sprite->mHeight, window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_LEVEL_NUMBER);

    sex = GetDeckPokemonSex(row);
    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 33);
    switch (sex) {
    case 0:
        sexMessage = ORRE_TEAM_SEX_MALE;
        break;
    case 1:
        sexMessage = ORRE_TEAM_SEX_FEMALE;
        break;
    case 2:
    default:
        sexMessage = 0;
        break;
    }
    if (sexMessage != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), bio->mX + 18, bio->mY,
            window->mRgba.mColorA | 0xFFFFFF00, sexMessage);
    }

    if (GetDeckPokemonMochimono(row) != 0) {
        bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 30);
        windowDrawSprite2(bio->mX, bio->mY, bio->mWidth, bio->mHeight,
            window->mRgba.mColorA | 0xFFFFFF00, window, ORRE_TEAM_HELD_ITEM_TEXTURE, 0,
            winSpriteGetLayerID(window, sprite));
    }

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 28);
    facePosition.x = bio->mX;
    facePosition.y = bio->mY;
    faceSize.x = bio->mWidth;
    faceSize.y = bio->mHeight;
    faceID = GetDeckPokemonFaceID(row);
    menuFaceDispFromID3(window, sprite, faceID, &faceSize, &facePosition);
}

void menuOrre::LeftPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreItemIDs idStruct = menuOrreTemochiItemIDs;
    u16 row;
    tagMENU_ITEM_DATA *bio;
    u32 size;
    u8 sex;
    u16 *name;
    u8 level;
    u16 faceID;
    u32 sexMessage;
    GSivec2 facePosition;
    GSivec2 faceSize;

    for (row = 0; row < 6; row++) {
        if (idStruct.id[row] == sprite->mItemID) {
            break;
        }
    }

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, sprite->mItemID);
    windowDrawSprite2(0, 0, bio->mWidth, bio->mHeight,
        window->mRgba.mColorA | 0xFFFFFF00, window, ORRE_TEAM_PANEL_TEXTURE, 0,
        winSpriteGetLayerID(window, sprite));

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 37);
    name = GetTemochiPokemonName(row);
    msgctrlSetValue(MSGCTRL_TEXT, (u32)name);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), bio->mX, bio->mY,
        sprite->mWidth, sprite->mHeight, window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_NAME_PRINT_AT_POS);

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 38);
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), bio->mX, bio->mY + 2,
        window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_LEVEL_LABEL);
    size = GSmsgGetMessageSize(ORRE_TEAM_LEVEL_LABEL);
    level = GetTemochiPokemonLevel(row);
    msgctrlSetValue(MSGCTRL_NUM_VALUE, level);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite),
        bio->mX + 4 + (s16)(size >> 16), bio->mY + 2,
        sprite->mWidth, sprite->mHeight, window->mRgba.mColorA | 0xFFFFFF00, ORRE_TEAM_LEVEL_NUMBER);

    sex = GetTemochiPokemonSex(row);
    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 39);
    switch (sex) {
    case 0:
        sexMessage = ORRE_TEAM_SEX_MALE;
        break;
    case 1:
        sexMessage = ORRE_TEAM_SEX_FEMALE;
        break;
    case 2:
    default:
        sexMessage = 0;
        break;
    }
    if (sexMessage != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), bio->mX + 18, bio->mY,
            window->mRgba.mColorA | 0xFFFFFF00, sexMessage);
    }

    if (GetTemochiPokemonMochimono(row) != 0) {
        bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 36);
        windowDrawSprite2(bio->mX, bio->mY, bio->mWidth, bio->mHeight,
            window->mRgba.mColorA | 0xFFFFFF00, window, ORRE_TEAM_HELD_ITEM_TEXTURE, 0,
            winSpriteGetLayerID(window, sprite));
    }

    bio = menuItemBiosGetPtr(ORRE_TEAM_MENU_BIOS, 34);
    facePosition.x = bio->mX;
    facePosition.y = bio->mY;
    faceSize.x = bio->mWidth;
    faceSize.y = bio->mHeight;
    faceID = GetTemochiPokemonFaceID(row);
    menuFaceDispFromID3(window, sprite, faceID, &faceSize, &facePosition);
}

void menuOrre::Button(tagWINDOW_WORK *window)
{
    CURSOR cursor = window->mCursor;
    u16 keyTrg;

    mSelectID = cursor.y;
    if (window != 0) {
        keyTrg = windowGetKeyInfo()->mKeyTrg;
        if ((keyTrg & WINDOW_KEY_ACCEPT) != 0) {
            window->mAccept = 1;
        }
        if (mSelectCount != 0 && (keyTrg & WINDOW_KEY_CANCEL) != 0) {
            window->mAccept = 1;
            window->mCancel = 1;
        }
    }
}

u8 menuOrre::ConfirmPokemon()
{
    s8 answer;
    u8 result;

    winMsgOpen(WINMSG_WINDOW_MENU, ORRE_TEAM_CONFIRM_PRINT, true, true);
    answer = menuSubOpenYesNo(0, 64, 200, 0);
    if (answer == 0) {
        result = true;
    } else {
        result = false;
    }
    winMsgClose(true);
    return result;
}

s32 menuOrre::SelectPokemon(u16 id, bool alreadySelected)
{
    MenuOrreMessages pickStruct = menuOrrePickMessages;
    MenuOrreMessages dropStruct = menuOrreDropMessages;

    if (alreadySelected) {
        return menuSubOpenSelect2(0, dropStruct.msg, 3, 350, 180, 0, 3, 0);
    }
    return menuSubOpenSelect2(0, pickStruct.msg, 3, 350, 180, 0, 3, 0);
}

void menuOrre::Ctrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, ORRE_TEAM_MENU_SEQ);
            window->mFlag = 1;
        }
        break;

    case 2:
        if (mFlashStep < 4) {
            if (menuOrreIsFlashEnd(ORRE_TEAM_FLASH_FADE)) {
                mTimer -= timeGetLastFrameSec();
                if (mTimer <= 0.0f) {
                    u16 r = HSD_Rand();
                    mTimer = (50.0f + r % 50) / 100.0f;
                    s16 step = ++mFlashStep;
                    MenuOrreFramePairs pairTable = menuOrreTeamFramePairs;
                    FlashSystemSetPlayFrameLabel(ORRE_TEAM_FLASH_FADE,
                        pairTable.pair[2 * step], pairTable.pair[2 * step + 1]);
                }
            }
        }
        break;

    case 3:
        if (window->mFlag == 0) {
            window->mFlag = 1;
        }
        break;
    }

    mFrameCount = (s32)(60.0f * timeGetLastFrameSec() + mFrameCount) % 1000;
}

void menuOrre::CreatePokemonList()
{
    u16 slot = 0;

    for (u16 i = 0; i < 6; i++) {
        mPokemonList[slot++] = GetTemochiPokemonPtr(i);
    }
    for (u16 i = 0; i < 6; i++) {
        mPokemonList[slot++] = GetDeckPokemonPtr(i);
    }
}

Pokemon *_loadPokemonCallBack(void *context)
{
    Pokemon *pokemon = 0;

    if (pOrre != 0) {
        u16 *count = (u16 *)context;
        pokemon = pOrre->mPokemonList[*count];
        (*count)++;
    }
    return pokemon;
}
