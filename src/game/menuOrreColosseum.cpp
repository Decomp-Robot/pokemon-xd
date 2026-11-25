#include <libc/math.h>
#include <libc/string.h>
#include <libc/memory.h>
#include <fightTrainer.h>
#include <fightTrainerDB.h>
#include <msgctrl.h>
#include <menuPokemonMessages.h>
#include <fade.h>
#include <floor.h>
#include <menu.h>
#include <hsdRandom.h>
#include <savedata.h>
#include <cursorBios.h>
#include <menuOrre.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <heroBios.h>
#include <window.h>
#include <menuItemBios.h>
#include <menuSub.h>
#include <winMsg.h>
#include <time.h>
#include <GSmsg.h>
#include <menuFace.h>
#include <fightEncount.h>
#include <fightEncountData.h>
#include <fight.h>
#include <fightResource.h>
#include <deck.h>
#include <flashsystem.h>

enum MenuOrreTexID {
    TEX_ID_ROW = 795,
    TEX_ID_ITEM = 331,
    TEX_ID_TEAM_SLOT_1 = 796,
    TEX_ID_TEAM_SLOT_2 = 797,
    TEX_ID_TEAM_SLOT_3 = 798,
    TEX_ID_TEAM_SLOT_4 = 799,
};

enum MenuOrreItem {
    MENU_ITEM_TEMOCHI_ROW_1 = 1,
    MENU_ITEM_TEMOCHI_ROW_2,
    MENU_ITEM_TEMOCHI_ROW_3,
    MENU_ITEM_TEMOCHI_ROW_4,
    MENU_ITEM_TEMOCHI_ROW_5,
    MENU_ITEM_TEMOCHI_ROW_6,
    MENU_ITEM_DECK_ROW_1,
    MENU_ITEM_DECK_ROW_2,
    MENU_ITEM_DECK_ROW_3,
    MENU_ITEM_DECK_ROW_4,
    MENU_ITEM_DECK_ROW_5,
    MENU_ITEM_DECK_ROW_6,
    MENU_ITEM_SELECTION_CURSOR_1,
    MENU_ITEM_SELECTION_CURSOR_2,
    MENU_ITEM_SELECTION_CURSOR_3,
    MENU_ITEM_SELECTION_CURSOR_4,
    MENU_ITEM_SELECTION_CURSOR_5,
    MENU_ITEM_SELECTION_CURSOR_6,
    MENU_ITEM_TEAM_SLOT_NUMBER_1,
    MENU_ITEM_TEAM_SLOT_NUMBER_2,
    MENU_ITEM_TEAM_SLOT_NUMBER_3,
    MENU_ITEM_TEAM_SLOT_NUMBER_4,
    MENU_ITEM_TEAM_SLOT_NUMBER_5,
    MENU_ITEM_TEAM_SLOT_NUMBER_6,
    MENU_ITEM_PLAYER_CAPTION,
    MENU_ITEM_OPPONENT_CAPTION,
    MENU_ITEM_DECK_FACE = 28,
    MENU_ITEM_DECK_ITEM = 30,
    MENU_ITEM_DECK_NAME,
    MENU_ITEM_DECK_LEVEL,
    MENU_ITEM_DECK_GENDER,
    MENU_ITEM_TEMOCHI_FACE = 34,
    MENU_ITEM_TEMOCHI_ITEM = 36,
    MENU_ITEM_TEMOCHI_NAME,
    MENU_ITEM_TEMOCHI_LEVEL,
    MENU_ITEM_TEMOCHI_GENDER,
};

enum MenuOrreSequence {
    ORRE_SEQ_OPEN = 36,
};

enum MenuOrreFloorID {
    FLOOR_ID_ORRE_COLOSSEUM = 906,
};

enum OrreFlashHandle {
    FLASH_SELECT = 0x1F5A3000,
    FLASH_CONFIRM = 0x1F5B3000,
    FLASH_INTRO = 0x1F5C3000,
};

enum MenuOrreSelectResult {
    MENU_ORRE_SELECT_PICKED = 0,
    MENU_ORRE_SELECT_STATUS = 1,
    MENU_ORRE_SELECT_CANCELLED = 2,
};

enum MenuOrreTextColor {
    MENU_ORRE_TEXT_WHITE_RGB = (255u << 24) | (255u << 16) | (255u << 8),
};

struct MenuOrreNumberTexIDs {
    u16 texID[4];
};

struct MenuOrreItemRowIDs {
    s16 itemID[6];
};

struct MenuOrreCursorItemIDs {
    s32 itemID[6];
};

struct MenuOrreSelectMessages {
    u32 messageID[3];
};

Hero _orreHero;

u16 _deckNumber;
u16 _fightresult;
menuOrre *pOrre;
u16 _loadFaceCount;

static inline void menuOrreJumpToTeamSlotFrame(u32 flash, s32 index)
{
    MenuOrreFrameLabels labels = {
        { "1_start", "1_end", "2_end", "3_end", "4_end" },
    };

    FlashSystemSetNowFrame(flash, FlashSystemGetFrameFromLabel(flash, labels.label[index]));
}

static inline u8 menuOrreFlashFrameReached(u32 flash)
{
    u8 reached;

    if (FlashSystemGetNowFrame(flash) >= FlashSystemGetEndFrame(flash)) {
        reached = 1;
    } else {
        reached = 0;
    }
    return reached;
}

static inline void menuOrrePlayTeamSlotFrame(u32 flash, s32 index)
{
    MenuOrreFramePairs pairs = {
        { "1_start", "1_start",
          "1_start", "1_end",
          "1_start", "2_end",
          "1_start", "3_end",
          "1_start", "4_end" },
    };

    FlashSystemSetPlayFrameLabel(flash, pairs.pair[index * 2], pairs.pair[index * 2 + 1]);
}

static inline MenuOrreNumberTexIDs menuOrreGetNumberTexIDs()
{
    MenuOrreNumberTexIDs ids = {
        { TEX_ID_TEAM_SLOT_1, TEX_ID_TEAM_SLOT_2, TEX_ID_TEAM_SLOT_3, TEX_ID_TEAM_SLOT_4 },
    };

    return ids;
}

inline u8 menuOrreTeamSelectRemoveLastSlot(menuOrre *work)
{
    u8 removed;

    if (work->mSelectCount != 0) {
        work->mSelectCount--;
        work->mSelectList[work->mSelectCount] = -1;
        menuOrreJumpToTeamSlotFrame(FLASH_SELECT, work->mSelectCount);
        menuOrrePlayTeamSlotFrame(FLASH_SELECT, work->mSelectCount);
        removed = 1;
    } else {
        removed = 0;
    }
    return removed;
}

static inline MenuOrreSelectMessages menuOrreGetPickMessages()
{
    MenuOrreSelectMessages messages = {
        { MSG_SELECT_POKEMON, MSG_POKEMON_STATUS, MSG_SELECTION_CANCEL },
    };

    return messages;
}

static inline MenuOrreSelectMessages menuOrreGetDropMessages()
{
    MenuOrreSelectMessages messages = {
        { MSG_DROP_POKEMON, MSG_POKEMON_STATUS, MSG_SELECTION_CANCEL },
    };

    return messages;
}

static inline MenuOrreItemRowIDs menuOrreGetTemochiItemIDs()
{
    MenuOrreItemRowIDs items = {
        { MENU_ITEM_TEMOCHI_ROW_1, MENU_ITEM_TEMOCHI_ROW_2, MENU_ITEM_TEMOCHI_ROW_3,
          MENU_ITEM_TEMOCHI_ROW_4, MENU_ITEM_TEMOCHI_ROW_5, MENU_ITEM_TEMOCHI_ROW_6 },
    };

    return items;
}

static inline MenuOrreItemRowIDs menuOrreGetDeckItemIDs()
{
    MenuOrreItemRowIDs items = {
        { MENU_ITEM_DECK_ROW_1, MENU_ITEM_DECK_ROW_2, MENU_ITEM_DECK_ROW_3,
          MENU_ITEM_DECK_ROW_4, MENU_ITEM_DECK_ROW_5, MENU_ITEM_DECK_ROW_6 },
    };

    return items;
}

static inline MenuOrreItemRowIDs menuOrreGetNumberItems()
{
    MenuOrreItemRowIDs items = {
        { MENU_ITEM_TEAM_SLOT_NUMBER_1, MENU_ITEM_TEAM_SLOT_NUMBER_2,
          MENU_ITEM_TEAM_SLOT_NUMBER_3, MENU_ITEM_TEAM_SLOT_NUMBER_4,
          MENU_ITEM_TEAM_SLOT_NUMBER_5, MENU_ITEM_TEAM_SLOT_NUMBER_6 },
    };

    return items;
}

static inline MenuOrreCursorItemIDs menuOrreGetCursorItems()
{
    MenuOrreCursorItemIDs items = {
        { MENU_ITEM_SELECTION_CURSOR_1, MENU_ITEM_SELECTION_CURSOR_2,
          MENU_ITEM_SELECTION_CURSOR_3, MENU_ITEM_SELECTION_CURSOR_4,
          MENU_ITEM_SELECTION_CURSOR_5, MENU_ITEM_SELECTION_CURSOR_6 },
    };

    return items;
}

extern Pokemon *_loadPokemonCallBack(void *context);

Pokemon *_PokemonStatusDisp_CallBack(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type,
                                     long arg);

__declspec(export) void menuOrreExit(void)
{
    if (pOrre != 0) {
        pOrre->Release();
        if (pOrre != 0) {
            delete pOrre;
        }
        pOrre = 0;
    }
    fightFreeResource(DECK_RESOURCE_GROUP, DECK_RESOURCE_COLOSSEUM);
}

__declspec(export) void menuOrreMain(void)
{
    if (pOrre != 0) {
        pOrre->Main();
    }
}

__declspec(export) void menuOrreInit(void)
{
    if (GSfloorIsPop() != 0) {
        return;
    }
    pOrre = new menuOrre();
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
    GSfloorPush(FLOOR_ID_ORRE_COLOSSEUM);
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

void menuOrre::Release()
{
}

inline bool menuOrreTeamSelectSlotPicked(menuOrre *work, s16 sel)
{
    u16 i;

    for (i = 0; i < 4; i++) {
        if (work->mSelectList[i] == sel) {
            return true;
        }
    }
    return false;
}

inline void menuOrreTeamSelectInitParty(menuOrre *work, Hero *hero)
{
    u16 i;
    Pokemon *pk;

    for (i = 0; i < 6; i++) {
        pk = heroBiosGetPokemonPtr(hero, i);
        pokemonInit(pk);
    }
    for (i = 0; i < 4; i++) {
        pk = work->GetTemochiPokemonPtr(work->mSelectList[i]);
        pokemonBiosCopy(heroBiosGetPokemonPtr(hero, i), pk);
    }
}

inline u8 menuOrre::AppendSelectedPokemon()
{
    u8 full;

    mSelectList[mSelectCount++] = mSelectID;
    full = 0;
    if (mSelectCount >= 4) {
        full = 1;
    }
    return full;
}

void menuOrre::Main(void)
{
    Hero *fightWork;
    u16 search;
    bool found;
    u8 full;
    long statusContext;
    u8 confirmed;
    u8 removedSlot;
    u8 running;
    u16 selectIndex;
    s32 count;

    confirmed = 1;
    _loadFaceCount = 0;
    menuFaceLoad(0, _loadPokemonCallBack, &_loadFaceCount);
    menuOpen(MENU_ID_ORRE_BACKDROP, 0);
    do {
        GSthreadSwitch();
    } while (FlashSystemIsLastFrame(FLASH_INTRO) == 0);

    menuOrrePlayTeamSlotFrame(FLASH_CONFIRM, 0);

    while (true) {
        statusContext = GetPokemonStatusContext();
        running = 1;
        mTimer = 1.0f;
        menuOrrePlayTeamSlotFrame(FLASH_SELECT, 0);

        do {
            s32 selectID = mSelectID;

            if (menuOpenCustom(MENU_ID_ORRE_TEAM_SELECT, windowGetActiveID(),
                               &selectID, 0, 1, 0) >= 0) {
                found = menuOrreTeamSelectSlotPicked(this, (s16)mSelectID);

                switch (SelectPokemon(mSelectID, found)) {
                case MENU_ORRE_SELECT_PICKED:
                    if (found != 0) {
                        selectIndex = mSelectID;
                        if (mSelectCount != 0) {
                            for (search = 0; search < 4; search++) {
                                if (mSelectList[search] == selectIndex) {
                                    break;
                                }
                            }
                            mSelectCount--;
                            for (; search < 3; search++) {
                                mSelectList[search] = mSelectList[search + 1];
                            }
                            menuOrreJumpToTeamSlotFrame(FLASH_SELECT, mSelectCount);
                            menuOrrePlayTeamSlotFrame(FLASH_SELECT, mSelectCount);
                            removedSlot = 1;
                        } else {
                            removedSlot = 0;
                        }
                        confirmed = removedSlot;
                    } else {
                        full = AppendSelectedPokemon();
                        count = mSelectCount;
                        menuOrrePlayTeamSlotFrame(FLASH_SELECT, count);
                        if (full == 1) {
                            running = 0;
                        }
                    }
                    break;

                case MENU_ORRE_SELECT_STATUS:
                    menuPokemonStatusOpenFromPokemonChange(
                        GetTemochiPokemonPtr(mSelectID),
                        _PokemonStatusDisp_CallBack, statusContext);
                    fadeSet(FADE_IN_BLACK, 0.5f);
                    break;

                case MENU_ORRE_SELECT_CANCELLED:
                    break;
                }
            } else {
                confirmed = menuOrreTeamSelectRemoveLastSlot(this);
                if (confirmed == 0) {
                    running = 0;
                }
            }
        } while (running != 0);

        if (confirmed == 0) {
            menuClose(MENU_ID_ORRE_TEAM_SELECT);
            break;
        }

        do {
            GSthreadSwitch();
        } while (menuOrreFlashFrameReached(FLASH_SELECT) == 0);

        if (ConfirmPokemon() != 0) {
            break;
        }

        mSelectCount = 0;
        mSelectList[0] = -1;
        mSelectList[1] = -1;
        mSelectList[2] = -1;
        mSelectList[3] = -1;
    }

    if (confirmed == 1) {
        while (menuFaceCheckLoading() != 0) {
            GSthreadSwitch();
        }
        do {
            GSthreadSwitch();
        } while (menuOrreFlashFrameReached(FLASH_CONFIRM) == 0);

        fightWork = fightGetMenuCtrlHero();
        menuOrreTeamSelectInitParty(this, fightWork);
        fightSetMenuCtrlHero(fightGetMenuCtrlHero());
        _fightresult = fightEncount(mEncountNo);
    }

    menuCloseSync(MENU_ID_ORRE_TEAM_SELECT, 1);
    menuClose(MENU_ID_ORRE_BACKDROP);
    menuCloseSync(MENU_ID_ORRE_BACKDROP, 1);
    if (mFloorPushed != 0) {
        GSfloorPop();
    }
}

void menuOrre::Init(void)
{
    mFloorPushed = GSfloorIsPush();
    InitSub();
}

void menuOrre::InitSub(void)
{
    Hero *hero;

    fightReadResource(DECK_RESOURCE_GROUP, DECK_RESOURCE_COLOSSEUM);
    mEncountNo = _deckNumber;
    mTrainerDataID = fightEncountDataBiosGetFightTrainerDataId(
        fightEncountDataBiosGetPtr(mEncountNo), 1);
    fightTrainerCreateFightTrainerDataIdToHero(mTrainerDataID, 0,
                                               &_orreHero, mEncountNo);
    hero = fightGetMenuCtrlHero();
    memcpy(hero, GetHeroPtr(0), sizeof(Hero));
    mMaxPokemonLevel = GetTemochiMaxPokemonLevel();
    CreatePokemonList();
}

Pokemon *_PokemonStatusDisp_CallBack(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type, long arg)
{
    u16 *selectID = (u16 *)arg;
    Pokemon *result = 0;

    switch (type) {
    case MENU_POKEMON_CB_CURSOR_LEFT:
        if (*selectID == 0) {
            return pokemon;
        }
        (*selectID)--;
        result = _GetTemochiPokemonPtr(*selectID);
        break;

    case MENU_POKEMON_CB_CURSOR_RIGHT:
        if (*selectID == 5) {
            return pokemon;
        }
        (*selectID)++;
        result = _GetTemochiPokemonPtr(*selectID);
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
    tagMENU_ITEM_DATA *item;
    u32 width;

    switch (sprite->mItemID) {
    case MENU_ITEM_TEMOCHI_ROW_1:
    case MENU_ITEM_TEMOCHI_ROW_2:
    case MENU_ITEM_TEMOCHI_ROW_3:
    case MENU_ITEM_TEMOCHI_ROW_4:
    case MENU_ITEM_TEMOCHI_ROW_5:
    case MENU_ITEM_TEMOCHI_ROW_6:
        LeftPanel(window, sprite);
        break;

    case MENU_ITEM_DECK_ROW_1:
    case MENU_ITEM_DECK_ROW_2:
    case MENU_ITEM_DECK_ROW_3:
    case MENU_ITEM_DECK_ROW_4:
    case MENU_ITEM_DECK_ROW_5:
    case MENU_ITEM_DECK_ROW_6:
        RightPanel(window, sprite);
        break;

    case MENU_ITEM_TEAM_SLOT_NUMBER_1:
    case MENU_ITEM_TEAM_SLOT_NUMBER_2:
    case MENU_ITEM_TEAM_SLOT_NUMBER_3:
    case MENU_ITEM_TEAM_SLOT_NUMBER_4:
    case MENU_ITEM_TEAM_SLOT_NUMBER_5:
    case MENU_ITEM_TEAM_SLOT_NUMBER_6:
        SetNumber(window, sprite);
        break;

    case MENU_ITEM_SELECTION_CURSOR_1:
    case MENU_ITEM_SELECTION_CURSOR_2:
    case MENU_ITEM_SELECTION_CURSOR_3:
    case MENU_ITEM_SELECTION_CURSOR_4:
    case MENU_ITEM_SELECTION_CURSOR_5:
    case MENU_ITEM_SELECTION_CURSOR_6:
        ControlCursor(window, sprite);
        break;

    case MENU_ITEM_PLAYER_CAPTION:
        msgctrlSetValue(MSGCTRL_VALUE_STRING,
                        (s32)heroBiosGetNamePtr(GetHeroPtr(0)));
        item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT, sprite->mItemID);
        width = GSmsgGetMessageSize(MSG_PRINT_TEXT);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite),
                    (s16)(item->mWidth - (int)(width >> 16)), 0,
                    MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_PRINT_TEXT);
        break;

    case MENU_ITEM_OPPONENT_CAPTION:
        msgctrlSetValue(MSGCTRL_VALUE_STRING,
                        (s32)GSmsgGetStringPtr(
                            fightTrainerDB_GetName(
                                mTrainerDataID)));
        width = GSmsgGetMessageSize(MSG_PRINT_TEXT);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                    MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_PRINT_TEXT);
        break;
    }
    return 0;
}

void menuOrre::ControlCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = window->mCursor;
    MenuOrreCursorItemIDs cursorItems = menuOrreGetCursorItems();
    s16 i;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == cursorItems.itemID[i]) {
            break;
        }
    }

    if (i == cursor.y) {
        if (window == windowSearchID(windowGetActiveID())) {
            s32 phase = mFrameCount % 60;
            u8 delta;

            sprite->mColor = -1;
            if (phase < 30) {
                f32 value = (f32)sin(3.14f * ((f32)phase / 30.0f));
                delta = (u8)(value * 128.0f);
                sprite->mRgba.mAlpha -= delta;
            }
        } else {
            sprite->mRgba.mAlpha = 255;
        }
        winSpriteSetDisp(sprite, 1);
    } else {
        winSpriteSetDisp(sprite, 0);
    }
}

void menuOrre::SetNumber(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreItemRowIDs numberItems = menuOrreGetNumberItems();
    MenuOrreNumberTexIDs texIDs = menuOrreGetNumberTexIDs();
    u16 i;
    u16 j;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == numberItems.itemID[i]) {
            break;
        }
    }

    for (j = 0; j < 4; j++) {
        if (i == mSelectList[j]) {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight,
                                    MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, window,
                                    texIDs.texID[j], 0,
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

u8 menuOrre::GetTemochiMaxPokemonLevel(void)
{
    u8 max = 0;
    u8 level;
    u16 i;

    for (i = 0; i < 6; i++) {
        level = GetTemochiPokemonLevel(i);
        if (max < level) {
            max = level;
        }
    }
    return max;
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
    return (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
}

void menuOrre::RightPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreItemRowIDs rowItems = menuOrreGetDeckItemIDs();
    tagMENU_ITEM_DATA *item;
    GSivec2 pos;
    GSivec2 size;
    u32 width;
    u32 gender;
    u16 *name;
    u8 level;
    u16 faceID;
    u8 sex;
    u16 i;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == rowItems.itemID[i]) {
            break;
        }
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                sprite->mItemID);
    windowDrawSprite2(0, 0, item->mWidth, item->mHeight,
                            MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, window,
                            TEX_ID_ROW, 0,
                            winSpriteGetLayerID(window, sprite));

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_NAME);
    name = GetDeckPokemonName(i);
    msgctrlSetValue(MSGCTRL_VALUE_STRING, (s32)name);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), item->mX, item->mY,
                sprite->mWidth, sprite->mHeight,
                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_POKEMON_NAME);

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_LEVEL);
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mX, item->mY + 2,
                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_ROW_LEVEL_LABEL);
    width = GSmsgGetMessageSize(MSG_ROW_LEVEL_LABEL);
    level = GetDeckPokemonLevel(i);
    msgctrlSetValue(MSGCTRL_VALUE_LEVEL, level);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite),
                item->mX + 4 + (s16)(width >> 16), item->mY + 2,
                sprite->mWidth, sprite->mHeight,
                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_ROW_LEVEL);

    sex = GetDeckPokemonSex(i);
    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_GENDER);
    switch (sex) {
    case POKEMON_SEX_MALE:
        gender = MSG_SEX_MALE;
        break;
    case POKEMON_SEX_FEMALE:
        gender = MSG_SEX_FEMALE;
        break;
    case POKEMON_SEX_NONE:
    default:
        gender = 0;
        break;
    }
    if (gender != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mX + 18, item->mY,
                    MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, gender);
    }

    if (GetDeckPokemonMochimono(i) != 0) {
        item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                    MENU_ITEM_DECK_ITEM);
        windowDrawSprite2(item->mX, item->mY, item->mWidth, item->mHeight,
                                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, window,
                                TEX_ID_ITEM, 0,
                                winSpriteGetLayerID(window, sprite));
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_FACE);
    pos.x = item->mX;
    pos.y = item->mY;
    size.x = item->mWidth;
    size.y = item->mHeight;
    faceID = GetDeckPokemonFaceID(i);
    menuFaceDispFromID3(window, sprite, faceID, &size, &pos);
}

void menuOrre::LeftPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreItemRowIDs rowItems = menuOrreGetTemochiItemIDs();
    tagMENU_ITEM_DATA *item;
    GSivec2 pos;
    GSivec2 size;
    u32 width;
    u32 gender;
    u16 *name;
    u8 level;
    u16 faceID;
    u8 sex;
    u16 i;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == rowItems.itemID[i]) {
            break;
        }
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                sprite->mItemID);
    windowDrawSprite2(0, 0, item->mWidth, item->mHeight,
                            MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, window,
                            TEX_ID_ROW, 0,
                            winSpriteGetLayerID(window, sprite));

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_NAME);
    name = GetTemochiPokemonName(i);
    msgctrlSetValue(MSGCTRL_VALUE_STRING, (s32)name);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), item->mX, item->mY,
                sprite->mWidth, sprite->mHeight,
                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_POKEMON_NAME);

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_LEVEL);
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mX, item->mY + 2,
                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_ROW_LEVEL_LABEL);
    width = GSmsgGetMessageSize(MSG_ROW_LEVEL_LABEL);
    level = GetTemochiPokemonLevel(i);
    msgctrlSetValue(MSGCTRL_VALUE_LEVEL, level);
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite),
                item->mX + 4 + (s16)(width >> 16), item->mY + 2,
                sprite->mWidth, sprite->mHeight,
                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, MSG_ROW_LEVEL);

    sex = GetTemochiPokemonSex(i);
    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_GENDER);
    switch (sex) {
    case POKEMON_SEX_MALE:
        gender = MSG_SEX_MALE;
        break;
    case POKEMON_SEX_FEMALE:
        gender = MSG_SEX_FEMALE;
        break;
    case POKEMON_SEX_NONE:
    default:
        gender = 0;
        break;
    }
    if (gender != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mX + 18, item->mY,
                    MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, gender);
    }

    if (GetTemochiPokemonMochimono(i) != 0) {
        item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                    MENU_ITEM_TEMOCHI_ITEM);
        windowDrawSprite2(item->mX, item->mY, item->mWidth, item->mHeight,
                                MENU_ORRE_TEXT_WHITE_RGB | window->mRgba.mColorA, window,
                                TEX_ID_ITEM, 0,
                                winSpriteGetLayerID(window, sprite));
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_FACE);
    pos.x = item->mX;
    pos.y = item->mY;
    size.x = item->mWidth;
    size.y = item->mHeight;
    faceID = GetTemochiPokemonFaceID(i);
    menuFaceDispFromID3(window, sprite, faceID, &size, &pos);
}

void menuOrre::Button(tagWINDOW_WORK *window)
{
    CURSOR cursor = window->mCursor;
    u16 keys;

    mSelectID = cursor.y;
    if (window == 0) {
        return;
    }
    keys = windowGetKeyInfo()->mKeyTrg;
    if (keys & WINDOW_KEY_ACCEPT) {
        window->mAccept = 1;
    }
    if (mSelectCount != 0) {
        if (keys & WINDOW_KEY_CANCEL) {
            window->mAccept = 1;
            window->mCancel = 1;
        }
    }
}

u8 menuOrre::ConfirmPokemon(void)
{
    u8 confirmed;

    winMsgOpen(WINMSG_WINDOW_MENU, MSG_CONFIRM, true, true);
    if (menuSubOpenYesNo(0, 64, 200, 0) == 0) {
        confirmed = 1;
    } else {
        confirmed = 0;
    }
    winMsgClose(1);
    return confirmed;
}

s32 menuOrre::SelectPokemon(u16 id, bool alreadySelected)
{
    MenuOrreSelectMessages pick = menuOrreGetPickMessages();
    MenuOrreSelectMessages drop = menuOrreGetDropMessages();

    if (alreadySelected) {
        return menuSubOpenSelect2(0, drop.messageID, 3, 350, 180, 0, 3, 0);
    }
    return menuSubOpenSelect2(0, pick.messageID, 3, 350, 180, 0, 3, 0);
}

void menuOrre::Ctrl(tagWINDOW_WORK *window)
{
    f32 frame;

    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, ORRE_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;

    case WINDOW_STATE_ACTIVE: {
        f32 dt;
        s16 step;

        if (mFlashStep >= 4) {
            break;
        }
        if (menuOrreFlashFrameReached(FLASH_CONFIRM) != 0) {
            dt = timeGetLastFrameSec();
            mTimer -= dt;
            if (mTimer <= 0.0f) {
                s32 roll = (u16)HSD_Rand() % 50;

                mTimer = (50.0f + (f32)roll) / 100.0f;
                step = ++mFlashStep;

                menuOrrePlayTeamSlotFrame(FLASH_CONFIRM, step);
            }
        }
        break;
    }

    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            window->mFlag = 1;
        }
        break;
    }

    frame = (f32)mFrameCount + 60.0f * timeGetLastFrameSec();
    mFrameCount = (s16)((s32)frame % 1000);
}

void menuOrre::CreatePokemonList(void)
{
    u16 count = 0;
    u16 i;
    u16 j;

    for (i = 0; i < 6; i++) {
        mPokemonList[count++] = GetTemochiPokemonPtr(i);
    }
    for (j = 0; j < 6; j++) {
        mPokemonList[count++] = GetDeckPokemonPtr(j);
    }
}

Pokemon *_loadPokemonCallBack(void *context)
{
    u16 *count = (u16 *)context;
    Pokemon *pokemon = 0;

    if (pOrre != 0) {
        pokemon = pOrre->mPokemonList[*count];
        (*count)++;
    }
    return pokemon;
}
