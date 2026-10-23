#include <textureBios.h>
#include <libc/memory.h>
#include <fightTrainer.h>
#include <fightTrainerDB.h>
#include <msgctrl.h>
#include <fade.h>
#include <floor.h>
#include <menu.h>
#include <hsdRandom.h>
#include <savedata.h>
#include <menuOrre.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <heroBios.h>
#include <window.h>
#include <menuItemBios.h>
#include <menuSub.h>
#include <time.h>
#include <hero.h>
#include <GSmsg.h>
#include <menuFace.h>
#include <winMsg.h>
#include <winSeq.h>
#include <GSthread.h>
#include <winSprite.h>
#include <fightEncountData.h>
#include <fightResource.h>

/*
 * menuOrreColosseum.cpp
 *
 * The Orre colosseum team selection: the flash scenes in which the player
 * picks the four Pokemon that enter the colosseum battle from their own
 * party (temochi); the deck rows on the window show the opposing
 * trainer's party. Opening the menu loads
 * both parties as face models; the selection window then plays a flash step
 * per picked Pokemon until four are confirmed, and the winner fights the
 * encounter through fightEncount.
 */

// Menu data ids this module drives: the selection window and the backdrop
// window opened behind it.
enum {
    MENU_ID_ORRE_TEAM_SELECT = 0xFD,
    MENU_ID_ORRE_BACKDROP = 0xFF,
};

// Flash scene instances: the selection scene itself, the scene the
// confirmation waits for and the intro that plays before the selection opens.
enum {
    FLASH_SELECT = 0x1F5A3000,
    FLASH_CONFIRM = 0x1F5B3000,
    FLASH_INTRO = 0x1F5C3000,
};

// Message variables the selection writes before it prints a message: the
// name a row or the caption shows and the level of a row.
enum {
    MSGVAR_NAME = 0x37,
    MSGVAR_LEVEL = 0x5F,
};

// Messages of the selection: the caption and the confirmation, the labels and
// the values of a row, and the two gender marks.
enum {
    MSG_ID_TEAM_CAPTION = 0x4276,
    MSG_ID_ROW_NAME = 0x4277,
    MSG_ID_ROW_LEVEL_LABEL = 0x42C1,
    MSG_ID_ROW_LEVEL = 0x4352,
    MSG_ID_CONFIRM = 0x8AD1,
    MSG_ID_SELECT_POKEMON = 0x8ACE,
    MSG_ID_POKEMON_STATUS = 0x8ACF,
    MSG_ID_SELECTION_CANCEL = 0x8AD0,
    MSG_ID_DROP_POKEMON = 0x8AF1,
    MSG_ID_GENDER_MALE = 2501,
    MSG_ID_GENDER_FEMALE = 2502,
};

// Textures a row draws with: its background and the icon of the item its
// Pokemon holds.
enum {
    TEX_ID_ROW = 0x31B,
    TEX_ID_ITEM = 0x14B,
    TEX_ID_TEAM_SLOT_1 = 0x31C,
    TEX_ID_TEAM_SLOT_2 = 0x31D,
    TEX_ID_TEAM_SLOT_3 = 0x31E,
    TEX_ID_TEAM_SLOT_4 = 0x31F,
};

// Selection window rows, cursors, slot numbers and trainer captions.
enum {
    MENU_ITEM_TEMOCHI_ROW_1 = 1,
    MENU_ITEM_TEMOCHI_ROW_2 = 2,
    MENU_ITEM_TEMOCHI_ROW_3 = 3,
    MENU_ITEM_TEMOCHI_ROW_4 = 4,
    MENU_ITEM_TEMOCHI_ROW_5 = 5,
    MENU_ITEM_TEMOCHI_ROW_6 = 6,
    MENU_ITEM_DECK_ROW_1 = 7,
    MENU_ITEM_DECK_ROW_2 = 8,
    MENU_ITEM_DECK_ROW_3 = 9,
    MENU_ITEM_DECK_ROW_4 = 10,
    MENU_ITEM_DECK_ROW_5 = 11,
    MENU_ITEM_DECK_ROW_6 = 12,
    MENU_ITEM_SELECTION_CURSOR_1 = 13,
    MENU_ITEM_SELECTION_CURSOR_2 = 14,
    MENU_ITEM_SELECTION_CURSOR_3 = 15,
    MENU_ITEM_SELECTION_CURSOR_4 = 16,
    MENU_ITEM_SELECTION_CURSOR_5 = 17,
    MENU_ITEM_SELECTION_CURSOR_6 = 18,
    MENU_ITEM_TEAM_SLOT_NUMBER_1 = 19,
    MENU_ITEM_TEAM_SLOT_NUMBER_2 = 20,
    MENU_ITEM_TEAM_SLOT_NUMBER_3 = 21,
    MENU_ITEM_TEAM_SLOT_NUMBER_4 = 22,
    MENU_ITEM_TEAM_SLOT_NUMBER_5 = 23,
    MENU_ITEM_TEAM_SLOT_NUMBER_6 = 24,
    MENU_ITEM_PLAYER_CAPTION = 25,
    MENU_ITEM_OPPONENT_CAPTION = 26,
};

// Item ids of the two row groups of the selection window's item table: the
// face model, the held-item icon, the name, the level and the gender of a
// row. The rows themselves are the party and deck ids of the two tables.
enum {
    MENU_ITEM_DECK_FACE = 28,
    MENU_ITEM_DECK_ITEM = 30,
    MENU_ITEM_DECK_NAME = 31,
    MENU_ITEM_DECK_LEVEL = 32,
    MENU_ITEM_DECK_GENDER = 33,
    MENU_ITEM_TEMOCHI_FACE = 34,
    MENU_ITEM_TEMOCHI_ITEM = 36,
    MENU_ITEM_TEMOCHI_NAME = 37,
    MENU_ITEM_TEMOCHI_LEVEL = 38,
    MENU_ITEM_TEMOCHI_GENDER = 39,
};

// Sequence the selection window advances to while its first step runs.
enum {
    ORRE_SEQ_OPEN = 36,
};

// The floor the colosseum selection pushes onto the floor stack.
enum {
    FLOOR_ID_ORRE_COLOSSEUM = 0x38A,
    RESOURCE_ID_ORRE_FIGHT = 0x842,
    RESOURCE_PARAM_ORRE_FIGHT = 0x184C3400,
    KEY_ACCEPT = 0x10,
    KEY_CANCEL = 0x20,
};

// The opposing trainer record created for this encounter.
Hero _orreHero;

// Pokemon face loading: hands out the records of pOrre one per call.
extern Pokemon *_loadPokemonCallBack(void *context);

// The status screen switcher of the team selection (defined below).
Pokemon *_PokemonStatusDisp_CallBack(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type,
                                     long arg);

extern "C" {
u16 windowGetKeyTrg(void);
u8 menuOffScreenSetDisp(s32 flag);
// Floor stack and fight-scene support shared with the fight module.
void *fightGetMenuCtrlHero(void);

u8 FlashSystemIsLastFrame(u32 handle);
void FlashSystemSetPlayFrameLabel(u32 handle, const char *from, const char *to);
u32 FlashSystemGetFrameFromLabel(u32 handle, const char *label);
void FlashSystemSetNowFrame(u32 handle, u32 frame);
u32 FlashSystemGetEndFrame(u32 handle);
u32 FlashSystemGetNowFrame(u32 handle);
void pokemonInit(Pokemon *pokemon);
void pokemonBiosCopy(Pokemon *dest, Pokemon *src);
void fightSetMenuCtrlHero(void *work);
u16 fightEncount(u16 encountNo);
u16 menuFaceGetID(Pokemon *pokemon);
u16 *pokemon_GetNickname(Pokemon *pokemon);
double sin(double x);
s32 windowGetActiveID(void);

}

// The singleton request state.
u16 _deckNumber;
u16 _fightresult;
menuOrre *pOrre;
u16 _loadFaceCount;

// The labels used by the team-count flash scene.
const char menuOrreFrameStart[] = "1_start";
const char menuOrreFrame1End[] = "1_end";
const char menuOrreFrame2End[] = "2_end";
const char menuOrreFrame3End[] = "3_end";
const char menuOrreFrame4End[] = "4_end";

// Numeral textures drawn on the party-size counters.
struct MenuOrreNumberTexIDs {
    u16 id[4];
};
extern const MenuOrreNumberTexIDs menuOrreNumberTexIDs = {
    { TEX_ID_TEAM_SLOT_1, TEX_ID_TEAM_SLOT_2, TEX_ID_TEAM_SLOT_3, TEX_ID_TEAM_SLOT_4 },
};


// Item id tables of the selection window: party rows, deck rows and cursors.
struct MenuOrreNumberItems {
    s16 id[6];
};
struct MenuOrreCursorItems {
    s32 id[6];
};

// The sprite item ids of the two row groups.

// The message lists of the team selection dialog: the labels of picking a
// free Pokemon and of dropping one that is already in the team.
struct MenuOrreSelectMsgList {
    s32 id[3];
};

/* -------------------------------------------------------------------------
 * Module entry points
 * ---------------------------------------------------------------------- */

// Script entry points.
__declspec(export) void menuOrreExit(void)
{
    if (pOrre != 0) {
        pOrre->Release();
        if (pOrre != 0) {
            delete pOrre;
        }
        pOrre = 0;
    }
    fightFreeResource(RESOURCE_ID_ORRE_FIGHT, RESOURCE_PARAM_ORRE_FIGHT);
}

/* Run the selection with the current singleton; a no-op before menuOrreInit. */
__declspec(export) void menuOrreMain(void)
{
    if (pOrre != 0) {
        pOrre->Main();
    }
}

__declspec(export) void menuOrreInit(void)
{
    u8 busy = GSfloorIsPop();

    if (busy != 0) {
        return;
    }
    pOrre = new menuOrre();
    if (pOrre != 0) {
        pOrre->Init();
    }
}

/* The fight result of the last colosseum selection (0 = backed out). */
u16 menuOrreColosseumGetResult(void)
{
    return _fightresult;
}

/* Enter the colosseum for encounter `encountNo`; the script system polls
 * menuOrreColosseumGetResult until the selection and battle are done. */
void menuOrreColosseum(u16 encountNo)
{
    _deckNumber = encountNo;
    _fightresult = 0;
    GSfloorPush(FLOOR_ID_ORRE_COLOSSEUM);
    floorWaitForChange();
}

/* Window hook: run the item dispatcher of the selection window. */
s32 menuOrreMenuHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    menuOrre *work = pOrre;

    if (work != 0) {
        return work->Hook(window, sprite);
    }
    return 0;
}

/* Window hooks of the selection window and its neighbours. */
void menuOrreCtrl(tagWINDOW_WORK *window)
{
    menuOrre *work = pOrre;

    if (work != 0) {
        work->Ctrl(window);
    }
}

void menuOrreButton(tagWINDOW_WORK *window)
{
    menuOrre *work = pOrre;

    if (work != 0) {
        work->Button(window);
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
            return 1;
        }
    }
    return 0;
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
    mSelectList[mSelectCount++] = mSelectID;
    u8 full = 0;
    if (mSelectCount >= 4) {
        full = 1;
    }
    return full;
}

static inline MenuOrreFrameLabels menuOrreGetTeamFrameLabels()
{
    const MenuOrreFrameLabels frames = {
        { menuOrreFrameStart, menuOrreFrame1End, menuOrreFrame2End,
          menuOrreFrame3End, menuOrreFrame4End },
    };
    return frames;
}

static inline MenuOrreFramePairs menuOrreGetTeamFramePairs()
{
    const MenuOrreFramePairs frames = {
        { menuOrreFrameStart, menuOrreFrameStart,
          menuOrreFrameStart, menuOrreFrame1End,
          menuOrreFrameStart, menuOrreFrame2End,
          menuOrreFrameStart, menuOrreFrame3End,
          menuOrreFrameStart, menuOrreFrame4End },
    };
    return frames;
}

static const MenuOrreSelectMsgList menuOrrePickMessages = { { MSG_ID_SELECT_POKEMON, MSG_ID_POKEMON_STATUS, MSG_ID_SELECTION_CANCEL } };
static const MenuOrreSelectMsgList menuOrreDropMessages = { { MSG_ID_DROP_POKEMON, MSG_ID_POKEMON_STATUS, MSG_ID_SELECTION_CANCEL } };
static const MenuOrreNumberItems menuOrreTemochiItemIDs = { { MENU_ITEM_TEMOCHI_ROW_1, MENU_ITEM_TEMOCHI_ROW_2, MENU_ITEM_TEMOCHI_ROW_3, MENU_ITEM_TEMOCHI_ROW_4, MENU_ITEM_TEMOCHI_ROW_5, MENU_ITEM_TEMOCHI_ROW_6 } };
static const MenuOrreNumberItems menuOrreDeckItemIDs = { { MENU_ITEM_DECK_ROW_1, MENU_ITEM_DECK_ROW_2, MENU_ITEM_DECK_ROW_3, MENU_ITEM_DECK_ROW_4, MENU_ITEM_DECK_ROW_5, MENU_ITEM_DECK_ROW_6 } };
static const MenuOrreNumberItems menuOrreNumberItems = { { MENU_ITEM_TEAM_SLOT_NUMBER_1, MENU_ITEM_TEAM_SLOT_NUMBER_2, MENU_ITEM_TEAM_SLOT_NUMBER_3, MENU_ITEM_TEAM_SLOT_NUMBER_4, MENU_ITEM_TEAM_SLOT_NUMBER_5, MENU_ITEM_TEAM_SLOT_NUMBER_6 } };
static const MenuOrreCursorItems menuOrreCursorItems = { { MENU_ITEM_SELECTION_CURSOR_1, MENU_ITEM_SELECTION_CURSOR_2, MENU_ITEM_SELECTION_CURSOR_3, MENU_ITEM_SELECTION_CURSOR_4, MENU_ITEM_SELECTION_CURSOR_5, MENU_ITEM_SELECTION_CURSOR_6 } };

/* The selection proper: step through the flash scenes until four Pokemon are
 * confirmed or the player backs out, then fight the encounter. */
void menuOrre::Main(void)
{
    void *fightWork;
    u8 waitFrame;
    u16 search;
    bool found;
    u8 full;
    long statusContext;
    u8 confirmed;
    u8 ok;

    confirmed = 1;
    _loadFaceCount = 0;
    menuFaceLoadPokemon(0, (void *(*)(void *))_loadPokemonCallBack, &_loadFaceCount);
    menuOpen(MENU_ID_ORRE_BACKDROP, 0);
    do {
        GSthreadSwitch();
    } while (FlashSystemIsLastFrame(FLASH_INTRO) == 0);

    {
        MenuOrreFramePairs pairs = menuOrreGetTeamFramePairs();
        FlashSystemSetPlayFrameLabel(FLASH_CONFIRM, pairs.pair[0], pairs.pair[1]);
    }

    /* Walk through the flash steps: each pass plays the selection scene from
     * its first label to the label of the current team size. */
    for (;;) {
        statusContext = GetPokemonStatusContext();
        u8 running = 1;
        mTimer = 1.0f;
        MenuOrreFramePairs pairs = menuOrreGetTeamFramePairs();
        FlashSystemSetPlayFrameLabel(FLASH_SELECT, pairs.pair[0], pairs.pair[1]);

        do {
            /* The cursor the selection menu opens on. */
            s32 selectID = mSelectID;

            if (menuOpenCustom(MENU_ID_ORRE_TEAM_SELECT, windowGetActiveID(),
                               &selectID, 0, 1, 0) >= 0) {
                found = menuOrreTeamSelectSlotPicked(this, (s16)mSelectID);

                switch (SelectPokemon(mSelectID, found)) {
                case 0:
                    if (found != 0) {
                        u16 sel2 = mSelectID;
                        if (mSelectCount != 0) {
                            for (search = 0; search < 4; search++) {
                                if (mSelectList[search] == sel2) {
                                    break;
                                }
                            }
                            mSelectCount--;
                            for (; search < 3; search++) {
                                mSelectList[search] = mSelectList[search + 1];
                            }
                            MenuOrreFrameLabels frames = menuOrreGetTeamFrameLabels();
                            FlashSystemSetNowFrame(
                                FLASH_SELECT,
                                FlashSystemGetFrameFromLabel(FLASH_SELECT,
                                                             frames.label[mSelectCount]));
                            MenuOrreFramePairs pairs = menuOrreGetTeamFramePairs();
                            FlashSystemSetPlayFrameLabel(
                                FLASH_SELECT, pairs.pair[mSelectCount * 2],
                                pairs.pair[mSelectCount * 2 + 1]);
                            ok = 1;
                        } else {
                            ok = 0;
                        }
                        confirmed = ok;
                    } else {
                        full = AppendSelectedPokemon();
                        s32 count = mSelectCount;
                        MenuOrreFramePairs pairs = menuOrreGetTeamFramePairs();
                        FlashSystemSetPlayFrameLabel(
                            FLASH_SELECT, pairs.pair[count * 2], pairs.pair[count * 2 + 1]);
                        if (full == 1) {
                            running = 0;
                        }
                    }
                    break;

                case 1:
                    menuPokemonStatusOpenFromPokemonChange(
                        GetTemochiPokemonPtr(mSelectID),
                        _PokemonStatusDisp_CallBack, statusContext);
                    fadeSet(2, 0.5f);
                    break;

                case 2:
                    /* Nothing to pick: keep waiting. */
                    break;
                }
            } else {
                if (mSelectCount != 0) {
                    mSelectCount--;
                    mSelectList[mSelectCount] = -1;
                    MenuOrreFrameLabels frames = menuOrreGetTeamFrameLabels();
                    FlashSystemSetNowFrame(
                        FLASH_SELECT,
                        FlashSystemGetFrameFromLabel(FLASH_SELECT,
                                                     frames.label[mSelectCount]));
                    MenuOrreFramePairs pairs = menuOrreGetTeamFramePairs();
                    FlashSystemSetPlayFrameLabel(
                        FLASH_SELECT, pairs.pair[mSelectCount * 2],
                        pairs.pair[mSelectCount * 2 + 1]);
                    ok = 1;
                } else {
                    ok = 0;
                }
                confirmed = ok;
                if (confirmed == 0) {
                    running = 0;
                }
            }
        } while (running != 0);

        if (confirmed == 0) {
            menuClose(MENU_ID_ORRE_TEAM_SELECT);
            break;
        }

        /* The flash step has to play out before the team is confirmed. */
        do {
            GSthreadSwitch();
            if (FlashSystemGetNowFrame(FLASH_SELECT)
                >= FlashSystemGetEndFrame(FLASH_SELECT)) {
                waitFrame = 1;
            } else {
                waitFrame = 0;
            }
        } while (waitFrame == 0);

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
            if (FlashSystemGetNowFrame(FLASH_CONFIRM)
                >= FlashSystemGetEndFrame(FLASH_CONFIRM)) {
                waitFrame = 1;
            } else {
                waitFrame = 0;
            }
        } while (waitFrame == 0);

        fightWork = fightGetMenuCtrlHero();
        menuOrreTeamSelectInitParty(this, (Hero *)fightWork);
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

    fightReadResource(RESOURCE_ID_ORRE_FIGHT, RESOURCE_PARAM_ORRE_FIGHT);
    mEncountNo = _deckNumber;
    mTrainerDataID = fightEncountDataBiosGetFightTrainerDataId(
        fightEncountDataBiosGetPtr(mEncountNo), 1);
    fightTrainerCreateFightTrainerDataIdToHero(mTrainerDataID, 0,
                                               &_orreHero, mEncountNo);
    hero = (Hero *)fightGetMenuCtrlHero();
    memcpy(hero, GetHeroPtr(0), 0x978);
    mMaxPokemonLevel = GetTemochiMaxPokemonLevel();
    CreatePokemonList();
}


/* -------------------------------------------------------------------------
 * Pokemon accessors
 * ---------------------------------------------------------------------- */


/* Status screen switcher: step the picked Pokemon id and report the Pokemon
 * now shown. */
Pokemon *_PokemonStatusDisp_CallBack(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type, long arg)
{
    u16 *selectID = (u16 *)arg;
    Pokemon *result = 0;

    switch (type) {
    case MENU_POKEMON_CB_CURSOR_LEFT:
        /* Already showing the first party Pokemon. */
        if (*selectID == 0) {
            return pokemon;
        }
        (*selectID)--;
        result = _GetTemochiPokemonPtr(*selectID);
        break;

    case MENU_POKEMON_CB_CURSOR_RIGHT:
        /* Already showing the last party Pokemon. */
        if (*selectID == 5) {
            return pokemon;
        }
        (*selectID)++;
        result = _GetTemochiPokemonPtr(*selectID);
        break;

    }
    return result;
}


/* The party Pokemon the status screen's switcher currently points at. */
Pokemon *_GetTemochiPokemonPtr(u16 index)
{
    menuOrre *work = pOrre;

    if (work != 0) {
        return work->GetTemochiPokemonPtr(index);
    }
    return 0;
}

/* The item hook of the selection window: everything on screen is one of the
 * two parties' status rows, the step counters, a cursor sprite or a caption. */
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
        /* The player's name as the caption. */
        msgctrlSetValue(MSGVAR_NAME,
                        (s32)heroBiosGetNamePtr(GetHeroPtr(0)));
        item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT, sprite->mItemID);
        width = GSmsgGetMessageSize(MSG_ID_TEAM_CAPTION);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite),
                    (s16)(item->mOffsetB - (int)(width >> 16)), 0,
                    0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_TEAM_CAPTION);
        break;

    case MENU_ITEM_OPPONENT_CAPTION:
        /* The deck trainer's name as the caption. */
        msgctrlSetValue(MSGVAR_NAME,
                        (s32)GSmsgGetStringPtr(
                            fightTrainerDB_GetName(
                                mTrainerDataID)));
        GSmsgGetMessageSize(MSG_ID_TEAM_CAPTION);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                    0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_TEAM_CAPTION);
        break;
    }
    return 0;
}

/* The cursor sprite: shown while it is the active window's cursor and its
 * item is the picked one; its colour breathes for the first half second. */
void menuOrre::ControlCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    WindowCursorState windowWord;
    MenuOrreCursorItems cursorItems;
    s16 i;

    windowWord = window->mCursorState;
    cursorItems = menuOrreCursorItems;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == cursorItems.id[i]) {
            break;
        }
    }

    if (i == windowWord.mValue.cursor) {
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

/* The party-size counter sprite: draw the numeral of the team slot this
 * sprite stands for. */
void menuOrre::SetNumber(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreNumberTexIDs texIDs;
    MenuOrreNumberItems numberItems = menuOrreNumberItems;
    u16 i;
    u16 j;

    texIDs = menuOrreNumberTexIDs;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == numberItems.id[i]) {
            break;
        }
    }

    for (j = 0; j < 4; j++) {
        if (i == mSelectList[j]) {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight,
                                    0xFFFFFF00 | window->mRgba.mColorA, window,
                                    texIDs.id[j], 0,
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
    u16 i;

    for (i = 0; i < 6; i++) {
        u8 level = GetTemochiPokemonLevel(i);

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

/* The opponent's deck trainer is the record the selection created; the
 * player's own hero record comes out of the trainer data area. */
Hero *menuOrre::GetHeroPtr(u8 trainer)
{
    if (trainer != 0) {
        return &_orreHero;
    }
    return (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
}



/* -------------------------------------------------------------------------
 * Rows of the selection window
 * ---------------------------------------------------------------------- */

/* One row of the opponent's deck: the row background, the name, the level,
 * the gender mark, the icon of the held item and the face model the row
 * shows. */
void menuOrre::RightPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreNumberItems rowItems = menuOrreDeckItemIDs;
    tagMENU_ITEM_DATA *item;
    GSivec2 pos;
    GSivec2 size;
    u32 width;
    u32 gender;
    u16 i;
    u8 sex;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == rowItems.id[i]) {
            break;
        }
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                sprite->mItemID);
    windowDrawSprite2(0, 0, item->mOffsetB, item->mOffsetC,
                            0xFFFFFF00 | window->mRgba.mColorA, window,
                            TEX_ID_ROW, 0,
                            winSpriteGetLayerID(window, sprite));

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_NAME);
    {
        u16 *name = GetDeckPokemonName(i);

        msgctrlSetValue(MSGVAR_NAME, (s32)name);
    }
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), item->mMessageX, item->mOffsetA,
                sprite->mWidth, sprite->mHeight,
                0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_ROW_NAME);

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_LEVEL);
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mMessageX, item->mOffsetA + 2,
                0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_ROW_LEVEL_LABEL);
    width = GSmsgGetMessageSize(MSG_ID_ROW_LEVEL_LABEL);
    {
        u8 level = GetDeckPokemonLevel(i);

        msgctrlSetValue(MSGVAR_LEVEL, level);
    }
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite),
                item->mMessageX + 4 + (s16)(width >> 16), item->mOffsetA + 2,
                sprite->mWidth, sprite->mHeight,
                0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_ROW_LEVEL);

    sex = GetDeckPokemonSex(i);
    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_GENDER);
    switch (sex) {
    case 0:
        gender = MSG_ID_GENDER_MALE;
        break;
    case 1:
        gender = MSG_ID_GENDER_FEMALE;
        break;
    case 2:
    default:
        gender = 0;
        break;
    }
    if (gender != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mMessageX + 18, item->mOffsetA,
                    0xFFFFFF00 | window->mRgba.mColorA, gender);
    }

    if ((u16)GetDeckPokemonMochimono(i) != 0) {
        item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                    MENU_ITEM_DECK_ITEM);
        windowDrawSprite2(item->mMessageX, item->mOffsetA, item->mOffsetB, item->mOffsetC,
                                0xFFFFFF00 | window->mRgba.mColorA, window,
                                TEX_ID_ITEM, 0,
                                winSpriteGetLayerID(window, sprite));
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_DECK_FACE);
    pos.x = item->mMessageX;
    pos.y = item->mOffsetA;
    size.x = item->mOffsetB;
    size.y = item->mOffsetC;
    {
        u16 faceID = GetDeckPokemonFaceID(i);

        menuFaceDispFromID3(window, sprite, faceID, &size, &pos);
    }
}

/* The same row of the player's party. */
void menuOrre::LeftPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuOrreNumberItems rowItems = menuOrreTemochiItemIDs;
    tagMENU_ITEM_DATA *item;
    GSivec2 pos;
    GSivec2 size;
    u32 width;
    u32 gender;
    u16 i;
    u8 sex;

    for (i = 0; i < 6; i++) {
        if (sprite->mItemID == rowItems.id[i]) {
            break;
        }
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                sprite->mItemID);
    windowDrawSprite2(0, 0, item->mOffsetB, item->mOffsetC,
                            0xFFFFFF00 | window->mRgba.mColorA, window,
                            TEX_ID_ROW, 0,
                            winSpriteGetLayerID(window, sprite));

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_NAME);
    {
        u16 *name = GetTemochiPokemonName(i);

        msgctrlSetValue(MSGVAR_NAME, (s32)name);
    }
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), item->mMessageX, item->mOffsetA,
                sprite->mWidth, sprite->mHeight,
                0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_ROW_NAME);

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_LEVEL);
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mMessageX, item->mOffsetA + 2,
                0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_ROW_LEVEL_LABEL);
    width = GSmsgGetMessageSize(MSG_ID_ROW_LEVEL_LABEL);
    {
        u8 level = GetTemochiPokemonLevel(i);

        msgctrlSetValue(MSGVAR_LEVEL, level);
    }
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite),
                item->mMessageX + 4 + (s16)(width >> 16), item->mOffsetA + 2,
                sprite->mWidth, sprite->mHeight,
                0xFFFFFF00 | window->mRgba.mColorA, MSG_ID_ROW_LEVEL);

    sex = GetTemochiPokemonSex(i);
    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_GENDER);
    switch (sex) {
    case 0:
        gender = MSG_ID_GENDER_MALE;
        break;
    case 1:
        gender = MSG_ID_GENDER_FEMALE;
        break;
    case 2:
    default:
        gender = 0;
        break;
    }
    if (gender != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), item->mMessageX + 18, item->mOffsetA,
                    0xFFFFFF00 | window->mRgba.mColorA, gender);
    }

    if ((u16)GetTemochiPokemonMochimono(i) != 0) {
        item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                    MENU_ITEM_TEMOCHI_ITEM);
        windowDrawSprite2(item->mMessageX, item->mOffsetA, item->mOffsetB, item->mOffsetC,
                                0xFFFFFF00 | window->mRgba.mColorA, window,
                                TEX_ID_ITEM, 0,
                                winSpriteGetLayerID(window, sprite));
    }

    item = menuItemBiosGetPtr(MENU_ID_ORRE_TEAM_SELECT,
                                                MENU_ITEM_TEMOCHI_FACE);
    pos.x = item->mMessageX;
    pos.y = item->mOffsetA;
    size.x = item->mOffsetB;
    size.y = item->mOffsetC;
    {
        u16 faceID = GetTemochiPokemonFaceID(i);

        menuFaceDispFromID3(window, sprite, faceID, &size, &pos);
    }
}


/* -------------------------------------------------------------------------
 * Selection and confirmation
 * ---------------------------------------------------------------------- */

/* The window's button hook: report the Pokemon the cursor is on and latch the
 * accept and cancel flags of the window the menu reads. */
void menuOrre::Button(tagWINDOW_WORK *window)
{
    WindowCursorState windowWord;
    u16 keys;

    windowWord = window->mCursorState;
    mSelectID = windowWord.mValue.cursor;
    if (window == 0) {
        return;
    }
    keys = windowGetKeyInfo()->mKeyTrg;
    if (keys & KEY_ACCEPT) {
        window->mAccept = 1;
    }
    if (mSelectCount != 0) {
        if (keys & KEY_CANCEL) {
            window->mAccept = 1;
            window->mCancel = 1;
        }
    }
}

/* Ask whether the team the player picked is the one that fights. */
u8 menuOrre::ConfirmPokemon(void)
{
    u8 confirmed;

    winMsgOpen(2, MSG_ID_CONFIRM, 1, 1);
    if (menuSubOpenYesNo(0, 64, 200, 0) == 0) {
        confirmed = 1;
    } else {
        confirmed = 0;
    }
    winMsgClose(1);
    return confirmed;
}

/* Open the row selection dialog for the Pokemon the cursor is on: the dialog
 * offers to pick it, or to drop it when it is already in the team. */
s32 menuOrre::SelectPokemon(u16 id, bool alreadySelected)
{
    MenuOrreSelectMsgList pick = menuOrrePickMessages;
    MenuOrreSelectMsgList drop = menuOrreDropMessages;

    if (alreadySelected) {
        return menuSubOpenSelect2(0, (u32 *)drop.id, 3, 350, 180, 0, 3, 0);
    }
    return menuSubOpenSelect2(0, (u32 *)pick.id, 3, 350, 180, 0, 3, 0);
}


/* -------------------------------------------------------------------------
 * Window control
 * ---------------------------------------------------------------------- */

/* Step control of the selection window: step 0 moves the window to the
 * selection sequence, step 2 runs the flash of the picked Pokemon and step 3
 * is the step the confirmation runs in. */
void menuOrre::Ctrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, ORRE_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;

    case WINDOW_STATE_ACTIVE: {
        bool done;
        f32 dt;
        s16 step;

        if (mFlashStep >= 4) {
            break;
        }
        if (FlashSystemGetNowFrame(FLASH_CONFIRM)
            >= FlashSystemGetEndFrame(FLASH_CONFIRM)) {
            done = 1;
        } else {
            done = 0;
        }
        if (done != 0) {
            dt = timeGetLastFrameSec();
            mTimer -= dt;
            if (mTimer <= 0.0f) {
                s32 roll = (u16)HSD_Rand() % 50;

                mTimer = (50.0f + (f32)roll) / 100.0f;
                step = ++mFlashStep;

                MenuOrreFramePairs pairs = menuOrreGetTeamFramePairs();

                FlashSystemSetPlayFrameLabel(FLASH_CONFIRM,
                                             pairs.pair[step * 2],
                                             pairs.pair[step * 2 + 1]);
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

    {
        f32 frame = (f32)mFrameCount + 60.0f * timeGetLastFrameSec();

        mFrameCount = (s16)((s32)frame % 1000);
    }
}


/* -------------------------------------------------------------------------
 * Pokemon list
 * ---------------------------------------------------------------------- */

/* Fill the list of Pokemon the selection window shows: the party (temochi)
 * Pokemon of the player first, then the Pokemon of the opponent's deck. */
void menuOrre::CreatePokemonList(void)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < 6; i++) {
        mPokemonList[count++] = GetTemochiPokemonPtr(i);
    }
    {
        u16 j;

        for (j = 0; j < 6; j++) {
            mPokemonList[count++] = GetDeckPokemonPtr(j);
        }
    }
}

/* The face loader of the selection: hand out the Pokemon records of the list
 * one per callback and count them off. */
Pokemon *_loadPokemonCallBack(void *context)
{
    u16 *count = (u16 *)context;
    menuOrre *work = pOrre;
    Pokemon *pokemon = 0;

    if (work != 0) {
        pokemon = work->mPokemonList[*count];
        (*count)++;
    }
    return pokemon;
}
