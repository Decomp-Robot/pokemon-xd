#include <types.h>
#include <savedata.h>
#include <hero.h>
#include <heroBios.h>
#include <camera.h>
#include <PokemonData.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokemonDataBios.h>
#include <darkPokemon.h>
#include <waza.h>
#include <zokusei.h>
#include <GSflag.h>
#include <GSmem.h>
#include <GSmsg.h>
#include <GSgfx.h>
#include <GSmodel.h>
#include <GSsnd.h>
#include <GSsndPlayback.h>
#include <GSres.h>
#include <GSfsys.h>
#include <GSthread.h>
#include <floor.h>
#include <floorManager.h>
#include <fade.h>
#include <msgctrl.h>
#include <window.h>
#include <winMsg.h>
#include <winSprite.h>
#include <sprite.h>
#include <cursorBios.h>
#include <menu.h>
#include <menuOffScreen.h>
#include <menuDataBios.h>
#include <menuItemBios.h>
#include <menuPda.h>
#include <menuPdaSub.h>
#include <menuPdaMailXD.h>
#include <menuPdaMailListSort.h>
#include <menuSpriteBios.h>
#include <menuPokemonMessages.h>
#include <deckDataVirtualBios.h>
#include <flashsystem.h>
#include <script.h>
#include <mail.h>
#include <gameSetup.h>
#include <esaba.h>
#include <fightEncountBios.h>
#include <relglobal.h>
#include <menuScript.h>
#include <menuIds.h>
#include <itemData.h>

enum pdaAddFuncDef {
    PDA_ADD_FUNC_MEMO,
    PDA_ADD_FUNC_DP_MON
};

WAZAOSHIEDEF _wazawork;
u16 _tempwazaid[POKEMON_DATA_LEVEL_UP_MOVE_COUNT + 1];

u16 _WazaNum = 3;
s32 _tutorialSelect = -1;

WAZAOSHIEDEF *_wazalist;
s32 _menuScriptWork;
pdaAddFuncDef _PdaAddFunc;
u32 _menuScriptMailID;
u8 _mailOpenFlag;
u16 _worldmapnum;
u8 _worldMapOpenFlag;
u8 _wazaOboetaFlag;

enum MenuScriptFloorID {
    FLOOR_ID_TUTORIAL = 141,
    FLOOR_ID_EEVEE = 151,
    FLOOR_ID_PDA_ADD_FUNC = 908,
    FLOOR_ID_PDA_MAIL_OPEN = 915,
    FLOOR_ID_WORLD_MAP = 936
};

enum MenuScriptResourceGroup {
    RESOURCE_GROUP_EEVEE = 2250,
    RESOURCE_GROUP_TUTORIAL = 2257,
    RESOURCE_GROUP_USOHACHI = 2339
};

enum MenuScriptEncountID {
    ENCOUNT_ID_TUTORIAL = 13
};

enum MenuScriptCursorSlot {
    CURSOR_SLOT_TUTORIAL = 14,
    CURSOR_SLOT_WAZA_OSHIE = 15
};

enum MenuScriptMenuSequence {
    MENU_SEQUENCE_OPEN = 36,
    MENU_SEQUENCE_CLOSE = 379
};

enum MenuScriptInfoKind {
    INFO_KIND_MAIL = 0,
    INFO_KIND_ESA,
    INFO_KIND_RELIVE
};

enum MenuScriptPdaSoundID {
    SOUND_ID_PDA_MAIL_OPEN = 1103,
    SOUND_ID_PDA_ADD_FUNC = 1108
};

enum MenuScriptTutorialItemID {
    TUTORIAL_ROW_HIGHLIGHT_0_FRAME = 1998,
    TUTORIAL_ROW_HIGHLIGHT_0_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_1_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_1_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_2_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_2_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_3_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_3_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_4_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_4_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_5_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_5_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_6_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_6_LABEL,
    TUTORIAL_ROW_HIGHLIGHT_7_FRAME,
    TUTORIAL_ROW_HIGHLIGHT_7_LABEL,
    TUTORIAL_ITEM_FLAG_0,
    TUTORIAL_ITEM_FLAG_1,
    TUTORIAL_ITEM_FLAG_2,
    TUTORIAL_ITEM_FLAG_3,
    TUTORIAL_ITEM_FLAG_4,
    TUTORIAL_ITEM_FLAG_5,
    TUTORIAL_ITEM_FLAG_6,
    TUTORIAL_ITEM_FLAG_7,
    TUTORIAL_ITEM_NAME_0,
    TUTORIAL_ITEM_NAME_1,
    TUTORIAL_ITEM_NAME_2,
    TUTORIAL_ITEM_NAME_3,
    TUTORIAL_ITEM_NAME_4,
    TUTORIAL_ITEM_NAME_5,
    TUTORIAL_ITEM_NAME_6,
    TUTORIAL_ITEM_NAME_7,
    TUTORIAL_ITEM_DESCRIPTION,
    TUTORIAL_ITEM_ARROW_DOWN = 2032,
    TUTORIAL_ITEM_ARROW_UP
};

enum MenuScriptWazaItemID {
    WAZA_ITEM_TYPE = 1717,
    WAZA_ITEM_TUTOR_PRICE_1,
    WAZA_ITEM_TUTOR_PRICE_2,
    WAZA_ITEM_TUTOR_PRICE_3,
    WAZA_ITEM_ARROW_DOWN,
    WAZA_ITEM_ARROW_UP,
    WAZA_ITEM_ROW_1,
    WAZA_ITEM_ROW_2,
    WAZA_ITEM_ROW_3,
    WAZA_ITEM_ROW_4,
    WAZA_ITEM_ROW_5,
    WAZA_ITEM_ROW_6,
    WAZA_ITEM_ROW_7,
    WAZA_ITEM_ROW_8,
    WAZA_ITEM_ROW_9,
    WAZA_ITEM_AVERAGE = 1741,
    WAZA_ITEM_POWER,
    WAZA_ITEM_PP,
    WAZA_ITEM_DESCRIPTION = 1748,
    WAZA_ITEM_LIST,
    WAZA_ITEM_COUPON
};

enum MenuScriptEeveeItemID {
    EEVEE_ITEM_TYPE_ICON = 1255,
    EEVEE_ITEM_EVOLUTION_ICON_1,
    EEVEE_ITEM_EVOLUTION_ICON_2,
    EEVEE_ITEM_EVOLUTION_ICON_3,
    EEVEE_ITEM_EVOLUTION_NAME,
    EEVEE_ITEM_EVOLUTION_TEXT_1,
    EEVEE_ITEM_TYPE_FRAME,
    EEVEE_ITEM_HELD_ITEM,
    EEVEE_ITEM_STONE_NAME_1,
    EEVEE_ITEM_PREVIEW_COLOR,
    EEVEE_ITEM_POKEMON_NAME,
    EEVEE_ITEM_STONE_NAME_2,
    EEVEE_ITEM_STONE_NAME_3,
    EEVEE_ITEM_STONE_NAME_4,
    EEVEE_ITEM_STONE_NAME_5,
    EEVEE_ITEM_STONE_FRAME_1,
    EEVEE_ITEM_STONE_FRAME_2,
    EEVEE_ITEM_STONE_FRAME_3,
    EEVEE_ITEM_STONE_FRAME_4,
    EEVEE_ITEM_EVOLUTION_TEXT_2,
    EEVEE_ITEM_EVOLUTION_TEXT_3,
    EEVEE_ITEM_EVOLUTION_TEXT_4,
    EEVEE_ITEM_EVOLUTION_TEXT_5,
    EEVEE_ITEM_ABILITY_NAME,
    EEVEE_ITEM_ABILITY_TEXT
};

enum MenuScriptMoneyItemID {
    MONEY_ITEM_ICON = 1626,
    MONEY_ITEM_AMOUNT
};

enum MenuScriptUsohachiItemID {
    USOHACHI_ITEM_ROW = 1633
};

enum MenuScriptSpriteID {
    SPRITE_ID_EEVEE_STONE_MOON = 540,
    SPRITE_ID_EEVEE_STONE_SUN,
    SPRITE_ID_EEVEE_STONE_THUNDER,
    SPRITE_ID_EEVEE_STONE_FIRE,
    SPRITE_ID_EEVEE_STONE_WATER,
    SPRITE_ID_WAZA_TUTOR = 577,
    SPRITE_ID_MONEY_ICON = 723,
    SPRITE_ID_USOHACHI_PICTURE = 728,
    SPRITE_ID_COUPON_ICON = 761,
    SPRITE_ID_USOHACHI_FIELD_PICTURE = 833
};

enum MenuScriptMessageID {
    MSG_STONE_NAME_FIRE = 5076,
    MSG_STONE_NAME_THUNDER,
    MSG_STONE_NAME_WATER,
    MSG_STONE_NAME_SUN = 5265,
    MSG_STONE_NAME_MOON,
    MSG_WAZA_NO_VALUE = 15094,
    MSG_RELIVE_INFO_FIRST = 16001,
    MSG_RELIVE_INFO,
    MSG_ESA_INFO,
    MSG_WAZA_NO_MOVE_TUTOR = 17045,
    MSG_MONEY_AMOUNT = 17230,
    MSG_WAZA_PRICE = 17241,
    MSG_EEVEE_SELECT = 41167,
    MSG_EEVEE_HELD_ITEM = 41172,
    MSG_WAZA_NO_DESCRIPTION_TUTOR = 50906,
    MSG_WAZA_NO_DESCRIPTION_LIST = 51003,
    MSG_WAZA_NO_MOVE_LIST = 51027,
    MSG_TUTORIAL_NO_NAME = 57038,
    MSG_WRONG_PLACE = 61431
};

enum { EEVEE_EVOLUTION_COUNT = 5 };
enum { WAZA_ROW_VISIBLE = 9 };
enum { TUTORIAL_ROW_COUNT = 8 };

enum { WAZA_ROW_PITCH = 31 };
enum { WAZA_ROW_LINE_HEIGHT = 18 };
enum { TOKUSEI_LINE_HEIGHT = 18 };

enum MenuScriptPrice {
    WAZA_TUTOR_PRICE = 1000
};

enum MenuScriptEeveePreviewColor {
    EEVEE_PREVIEW_RED = 213,
    EEVEE_PREVIEW_GREEN = 170,
    EEVEE_PREVIEW_BLUE = 51
};

enum MenuScriptEeveeItemRow {
    EEVEE_ITEM_HELD_ITEM_ROW = 3
};

enum { PDA_RESOURCE_GROUP = 23 };
static const u32 PDA_MODEL_RESOURCE = 0x17CF1000;
static const u32 PDA_CAMERA_RESOURCE = 0x17CF1800;

static const u32 COLOR_WHITE_RGB = 0xFFFFFF00;

static const u32 TOKUSEI_CHAR_SPACE = ' ';
static const u32 TOKUSEI_CHAR_WIDE_SPACE = 0x3000;
static const u32 TOKUSEI_CHAR_SKIP = 0xFFFF;
static const u32 TOKUSEI_CHAR_END = 0;

bool _checkWazaOverlap(u16 *wazaList, u16 wazaID, u16 count);
bool _checkRelive(Pokemon *pokemon);
Pokemon *_getPokemonPtr(s32 index);

void _wazaOshieCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
u16 _wazaOshieWazaNum(tagWINDOW_WORK *window);
WAZAOSHIEDEF *_getWaza2Pointer(s32 index);
u16 _checkWaza2(void);
void _checkWaza2Release(void);
u16 _checkWaza(s32 index);
void _checkWazaRelease(void);
u16 _checkWazaSub(Pokemon *pokemon);
void _menuScriptEeveeStoneName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _menuScriptTokuseiSetumei(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                               CURSOR *cursor, POKEMON_DATA_dd_ *pokemon);
void _menuScriptPdaAddFunc(pdaAddFuncDef func);
void _menuScriptReadMail(s32 mailID);

inline bool _checkProgress(u32 entry)
{
    return GSflagGet(ESABA_PROGRESS_FLAG) >= flag_etc[entry].progressValue;
}

static inline u32 CopyTokuseiWords(u16 *destination, u16 *source, s32 lastWord)
{
    s32 space;
    u32 n;

    space = 0;
    n = 0;
    while (true) {
        if (source[n] == TOKUSEI_CHAR_SPACE || source[n] == TOKUSEI_CHAR_WIDE_SPACE) {
            if (space == lastWord) {
                break;
            }
            destination[n] = source[n];
            space++;
        } else if (source[n] == TOKUSEI_CHAR_END) {
            break;
        } else {
            destination[n] = source[n];
        }
        n++;
    }
    destination[n++] = 0;

    return n;
}

static inline void SplitTokuseiWords(u16 *firstLine, u16 *secondLine, u16 *source, s32 words)
{
    s32 space;
    u32 len1;
    u32 len2;
    bool second;
    u32 length;
    u32 n;

    space = 0;
    len1 = 0;
    len2 = 0;
    second = false;
    length = GSmsgGetCharLength(source);
    n = 0;
    while (true) {
        if (source[n] != TOKUSEI_CHAR_SKIP) {
            if (source[n] == TOKUSEI_CHAR_SPACE || source[n] == TOKUSEI_CHAR_WIDE_SPACE) {
                if (space == words - 1) {
                    second = true;
                } else if (second) {
                    secondLine[len2++] = source[n];
                } else {
                    firstLine[len1++] = source[n];
                }
                space++;
            } else if (source[n] == TOKUSEI_CHAR_END) {
                if (length <= n) {
                    break;
                }
            } else if (second) {
                secondLine[len2++] = source[n];
            } else {
                firstLine[len1++] = source[n];
            }
        }
        n++;
    }
    firstLine[len1] = 0;
    secondLine[len2] = 0;
}

void menuScriptReciveMail(void)
{
    _menuScriptWork = INFO_KIND_MAIL;
    menuOpenCustom(MENU_ID_SCRIPT_INFO, windowGetActiveID(), 0, 0, 1, 1, &_menuScriptWork);
    menuCloseCustom(MENU_ID_SCRIPT_INFO, 0, 1);
}

void menuScriptInfoEsa(void)
{
    _menuScriptWork = INFO_KIND_ESA;
    menuOpenCustom(MENU_ID_SCRIPT_INFO, windowGetActiveID(), 0, 0, 1, 1, &_menuScriptWork);
    menuCloseCustom(MENU_ID_SCRIPT_INFO, 0, 1);
    winMsgOpen(WINMSG_WINDOW_FIELD, MSG_ESA_INFO, 1, 1);
    winMsgClose(1);
}

void menuScriptInfoRelive(Pokemon *work, u8 mode)
{
    Pokemon *pokemon = (Pokemon *)work;
    u16 darkID;
    u16 *name;

    darkID = pokemonBiosGetDarkpokemonDataId(pokemon);
    _menuScriptWork = INFO_KIND_RELIVE;
    menuOpenCustom(MENU_ID_SCRIPT_INFO, windowGetActiveID(), 0, 0, 1, 1, &_menuScriptWork);
    menuCloseCustom(MENU_ID_SCRIPT_INFO, 0, 1);

    if (pokemon != 0) {
        name = pokemon_GetNickname(pokemon);
        msgctrlSetValue(MSGCTRL_POKEMON_NAME, (u32)name);
        if (mode != 0) {
            winMsgOpen(WINMSG_WINDOW_FIELD, MSG_RELIVE_INFO_FIRST, 1, 1);
        } else {
            winMsgOpen(WINMSG_WINDOW_FIELD, MSG_RELIVE_INFO, 1, 1);
        }
        winMsgClose(1);
        darkPokemonBiosSetInformationFlag(darkID, 1);
    }
}

void _menuScriptReadMail(s32 mailID)
{
    u32 openFlag;

    GSmodelAllPauseAnimation();
    menuCreateOffScreen(20.0f / (f32)GSgfxVideoGetVsyncRate(), 50.0f);
    menuOffScreenCheckEnable(true);
    _menuScriptMailID = mailID;
    GSfloorPush(FLOOR_ID_PDA_MAIL_OPEN);
    openFlag = mailGetOpenFlag(mailID);
    if (openFlag != 0) {
        GSflagSet(openFlag, 1);
    }
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    menuReleaseOffScreen(0.0f / (f32)GSgfxVideoGetVsyncRate());
    GSmodelAllUnpauseAnimation();
}

void menuScriptReadMail(s32 mailID)
{
    _menuScriptReadMail(mailID);
}

void _menuScriptPdaAddFunc(pdaAddFuncDef func)
{
    GSmodelAllPauseAnimation();
    menuCreateOffScreen(20.0f / (f32)GSgfxVideoGetVsyncRate(), 50.0f);
    menuOffScreenCheckEnable(true);
    _PdaAddFunc = func;
    GSfloorPush(FLOOR_ID_PDA_ADD_FUNC);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    menuReleaseOffScreen(0.0f / (f32)GSgfxVideoGetVsyncRate());
    GSmodelAllUnpauseAnimation();
}

void menuScriptPdaAddFunc(u32 func)
{
    _menuScriptPdaAddFunc((pdaAddFuncDef)func);
}

void menuScriptMailOpenInit(void)
{
    cameraPlayAnime(PDA_RESOURCE_GROUP, PDA_CAMERA_RESOURCE, 0.0f, false);
    GSsndPlayNow(SOUND_ID_PDA_MAIL_OPEN);
    menuOffScreenSetPriority(0);
    menuScriptSetMailOpen(true);
    menuPdaSetModelPointer();
    menuPdaZoomIn();
}

u8 menuScriptCheckMailOpen(void)
{
    return _mailOpenFlag;
}

void menuScriptSetMailOpen(u8 open)
{
    _mailOpenFlag = open;
}

void menuScriptMailOpenMain(void)
{
    s32 index;

    GSgfxVideoSetLockFrameRate(0);
    index = menuPdaMailListSortInit(_menuScriptMailID);
    if (index >= 0) {
        menuPdaMailXDScript(index);
    }
    GSgfxVideoSetLockFrameRate(1);
    menuPdaReleaseModelPointer();
    GSmodelAllPauseAnimation();
    menuScriptSetMailOpen(false);
    GSfloorPop();
}

void menuScriptMailOpenExit(void)
{
    menuPdaMailListSortExit();
}

s32 menuScriptEevee(void)
{
    s32 selected;
    u32 placeName;

    if (GSfloorGetCurrent() != FLOOR_ID_EEVEE) {
        placeName = floorGetPlaceName(FLOOR_ID_EEVEE);
        msgctrlSetValue(MSGCTRL_VALUE_MESSAGE_ID, placeName);
        winMsgOpen(WINMSG_WINDOW_FIELD, MSG_WRONG_PLACE, 1, 1);
        winMsgClose(1);
        return -1;
    }

    GSfsysReadWait(RESOURCE_GROUP_EEVEE);
    winMsgOpen(WINMSG_WINDOW_FIELD, MSG_EEVEE_SELECT, 0, 1);
    selected = menuOpen(MENU_ID_EEVEE, 1);
    menuClose(MENU_ID_EEVEE);
    winMsgClose(1);
    menuCloseSync(MENU_ID_EEVEE, 1);
    GSresFreeGroup(RESOURCE_GROUP_EEVEE);
    return selected;
}

s32 menuScriptTutorial(void)
{
    s32 result = 0;
    s32 selected;
    u32 placeName;

    if (GSfloorGetCurrent() != FLOOR_ID_TUTORIAL) {
        placeName = floorGetPlaceName(FLOOR_ID_TUTORIAL);
        msgctrlSetValue(MSGCTRL_VALUE_MESSAGE_ID, placeName);
        winMsgOpen(WINMSG_WINDOW_FIELD, MSG_WRONG_PLACE, 1, 1);
        winMsgClose(1);
        return -1;
    }

    GSfsysReadWait(RESOURCE_GROUP_TUTORIAL);
    CURSOR cursor = cursorBiosGetPos(CURSOR_SLOT_TUTORIAL);
    cursor.y = 0;
    cursor.x = 0;
    cursorBiosSetPos(CURSOR_SLOT_TUTORIAL, cursor);

    selected = menuOpenCustom(MENU_ID_TUTORIAL, windowGetActiveID(), &result, 0, 1, 0);
    if (selected < 0) {
        _tutorialSelect = selected;
    } else {
        fightEncountDataBiosSetDiskId(fightEncountDataBiosGetPtr(ENCOUNT_ID_TUTORIAL), tutorialdata[selected]);
    }

    menuClose(MENU_ID_TUTORIAL);
    menuCloseSync(MENU_ID_TUTORIAL, 1);
    GSresFreeGroup(RESOURCE_GROUP_TUTORIAL);

    if (*tutorialdata_number == _tutorialSelect + 1) {
        _tutorialSelect = -1;
    }
    return _tutorialSelect;
}

void menuScriptOpenWorldMap(u16 mapID)
{
    _worldmapnum = mapID;
    GSthreadSwitch();
    menuScriptSetOpenWorldMapFlag(true);
    GSfloorPush(FLOOR_ID_WORLD_MAP);
    floorWaitForChange();
}

u16 menuScriptOpenWorldMapGetMapNumber(void)
{
    return _worldmapnum;
}

u8 menuScriptGetOpenWorldMapFlag(void)
{
    return _worldMapOpenFlag;
}

void menuScriptSetOpenWorldMapFlag(u8 flag)
{
    _worldMapOpenFlag = flag;
}

void menuScriptUsohachi(void)
{
    GSfsysReadWait(RESOURCE_GROUP_USOHACHI);
    menuOpen(MENU_ID_USOHACHI, 1);
    menuClose(MENU_ID_USOHACHI);
    menuCloseSync(MENU_ID_USOHACHI, 1);
    GSresFreeGroup(RESOURCE_GROUP_USOHACHI);
}

void menuScriptUsohachi2(void)
{
    GSfsysReadWait(RESOURCE_GROUP_USOHACHI);
    menuOpen(MENU_ID_USOHACHI2, 1);
    menuClose(MENU_ID_USOHACHI2);
    menuCloseSync(MENU_ID_USOHACHI2, 1);
    GSresFreeGroup(RESOURCE_GROUP_USOHACHI);
}

s32 menuScriptWazaMeijin(s32 partySlot)
{
    s32 select;

    _WazaNum = _checkWaza(partySlot);
    if (_WazaNum == 0) {
        select = -1;
    } else {
        select = menuOpen(MENU_ID_WAZA_TUTOR, 1);
        if (select >= 0) {
            if (_WazaNum == select + 1) {
                select = -1;
            } else {
                select = _wazalist[select].wazaID;
            }
        }
        menuClose(MENU_ID_WAZA_TUTOR);
        menuCloseSync(MENU_ID_WAZA_TUTOR, 1);
    }
    _checkWazaRelease();
    return select;
}

u8 menuScriptWazaBasanCheck(void)
{
    return _wazaOboetaFlag;
}

s32 menuScriptWazaBasan(bool *tutorFlag)
{
    s32 select;

    *tutorFlag = false;
    _wazaOboetaFlag = false;

    _WazaNum = _checkWaza2();
    if (_WazaNum == 0) {
        select = 0;
    } else {
        CURSOR cursor = cursorBiosGetPos(CURSOR_SLOT_WAZA_OSHIE);
        s32 cursorY = cursor.y;
        select = menuOpenCustom(MENU_ID_WAZA_OSHIE, windowGetActiveID(), &cursorY, 0, 1, 0);
        if (select >= 0) {
            if (_WazaNum == select + 1) {
                select = -1;
            } else {
                if (_wazalist[select].tutorFlagSet) {
                    if (tutorFlag != 0) {
                        *tutorFlag = true;
                    }
                    _wazaOboetaFlag = true;
                }
                select = wazaoshie_data[(u16)select].mMoveId;
            }
        }
        menuClose(MENU_ID_WAZA_OSHIE);
        menuCloseSync(MENU_ID_WAZA_OSHIE, 1);
    }
    _checkWaza2Release();
    return select;
}

void menuScriptSetWazaOboetaFlag(u16 wazaID)
{
    u16 i;

    for (i = 0; i < *wazaoshie_data_number; i++) {
        if (wazaID == wazaoshie_data[i].mMoveId) {
            GSflagOn(wazaoshie_data[i].mLearnedFlagId);
        }
    }
}

void menuScriptMoneyOpen(s16 x, s16 y)
{
    menuOpen(MENU_ID_MONEY, 0);
    menuSetPosition(MENU_ID_MONEY, x, y);
}

void menuScriptMoneyClose(void)
{
    menuClose(MENU_ID_MONEY);
}

void menuScriptCouponOpen(s16 x, s16 y)
{
    menuOpen(MENU_ID_COUPON, 0);
    menuSetPosition(MENU_ID_COUPON, x, y);
}

void menuScriptCouponClose(void)
{
    menuClose(MENU_ID_COUPON);
}

s32 menuScriptMoneyDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s16 textWidth;

    if (window->mMenuID == MENU_ID_MONEY) {
        switch (sprite->mItemID) {
        case MONEY_ITEM_AMOUNT:
            msgctrlSetValue(MSG_VAR_KANE, heroGetStatus(NULL, HERO_STATUS_POKEDORU, 0));
            textWidth = GSmsgGetMessageSize(MSG_MONEY_AMOUNT) >> 16;
            GSmsgPrint2(window->mLayerID, (s16)(sprite->mWidth - textWidth), 0, -1,
                        MSG_MONEY_AMOUNT);
            break;
        case MONEY_ITEM_ICON:
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor, window,
                              SPRITE_ID_MONEY_ICON, 0, winSpriteGetLayerID(window, sprite));
            break;
        }
    } else {
        switch (sprite->mItemID) {
        case MONEY_ITEM_AMOUNT:
            msgctrlSetValue(MSG_VAR_KANE, heroGetStatus(NULL, HERO_STATUS_POKECOUPON, 0));
            textWidth = GSmsgGetMessageSize(MSG_MONEY_AMOUNT) >> 16;
            GSmsgPrint2(window->mLayerID, (s16)(sprite->mWidth - textWidth), 0, -1,
                        MSG_MONEY_AMOUNT);
            break;
        case MONEY_ITEM_ICON:
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor, window,
                              SPRITE_ID_COUPON_ICON, 0, winSpriteGetLayerID(window, sprite));
            break;
        }
    }
    return 0;
}

void _getWazaData(u16 wazaID, WAZAOSHIEDEF *work)
{
    WazaData *waza = wazaDataBiosGetPtr(wazaID);
    WAZAOSHIEDEF *data;

    if (work != 0) {
        data = work;
    } else {
        data = &_wazawork;
    }
    data->wazaID = wazaID;
    data->nameID = wazaDataBiosGetName(waza);
    data->pp = wazaDataBiosGetPp(waza);
    data->zokuseiDataID = wazaDataBiosGetZokuseiDataId(waza);
    data->iryoku = wazaDataBiosGetIryoku(waza);
    data->avg = wazaDataBiosGetAvg(waza);
    data->docID = wazaDataBiosGetDoc(waza);
    data->tutorFlagSet = 0;
}

Pokemon *_getPokemonPtr(s32 index)
{
    Pokemon *pokemon;

    pokemon = heroBiosGetPokemonPtr((Hero *)savedataGetStatus(0, SAVE_DATA_HERO), index);

    if (pokemonCheckValid(pokemon)) {
        return pokemon;
    }

    return 0;
}

bool _checkRelive(Pokemon *pokemon)
{
    u16 darkID;

    if (!pokemonCheckValid(pokemon)) {
        return false;
    }

    darkID = pokemonBiosGetDarkpokemonDataId(pokemon);
    if (darkID != 0) {
        return darkPokemonBiosGetReliveFlag(darkID) != 0;
    }

    return true;
}

bool _checkWazaOverlap(u16 *wazaList, u16 wazaID, u16 count)
{
    u16 i;

    for (i = 0; i < count; i++) {
        if (wazaList[i] == wazaID) {
            return true;
        }
    }
    wazaList[count] = wazaID;
    return false;
}

u16 _checkWazaSub(Pokemon *pokemon)
{
    POKEMON_DATA_dd_ *data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon));
    u16 count = 0;
    u16 i;
    u8 level;

    memset(_tempwazaid, 0, sizeof(_tempwazaid));
    if (data != 0) {
        for (i = 0; i < POKEMON_DATA_LEVEL_UP_MOVE_COUNT; i++) {
            level = pokemonDataBiosGetGetWazaLevel(data, i);
            if (level == 0) {
                break;
            }
            if (pokemonBiosGetLevel(pokemon) >= level) {
                if (!_checkWazaOverlap(_tempwazaid, pokemonDataBiosGetGetWazaDataId(data, i), count)) {
                    count++;
                }
            }
        }
    }
    return count;
}

u16 _checkWaza(s32 index)
{
    u16 count = 0;
    Pokemon *pokemon;
    u16 i;

    pokemon = _getPokemonPtr(index);
    if (pokemon != 0 && _checkRelive(pokemon)) {
        count = _checkWazaSub(pokemon);
    }

    if (count != 0) {
        count++;
        _wazalist = new WAZAOSHIEDEF[count];
        for (i = 0; i < count - 1; i++) {
            _getWazaData(_tempwazaid[i], &_wazalist[i]);
        }
        _wazalist[i].wazaID = 0;
    }
    return count;
}

void _checkWazaRelease()
{
    if (_wazalist != 0) {
        delete[] _wazalist;
        _wazalist = 0;
    }
}

u16 _checkWaza2()
{
    s32 i;
    u16 count = 0;
    u16 oboetaCount = 0;
    u16 *wazaList;
    u8 *oboetaList;

    wazaList = new u16[*wazaoshie_data_number];
    oboetaList = new u8[*wazaoshie_data_number];

    for (i = 0; i < *wazaoshie_data_number; i++) {
        if (_checkProgress(wazaoshie_data[i].mProgressionGateIndex)) {
            wazaList[count] = wazaoshie_data[i].mMoveId;
            oboetaList[count] = GSflagTest(wazaoshie_data[i].mLearnedFlagId);
            if (oboetaList[count]) {
                oboetaCount++;
            }
            count++;
        }
    }

    if (oboetaCount == count) {
        // everything has been taught already
        count = 0;
    } else if (count != 0) {
        count++;
        _wazalist = new WAZAOSHIEDEF[count];
        for (i = 0; i < count - 1; i++) {
            _getWazaData(wazaList[i], &_wazalist[i]);
            _wazalist[i].tutorFlagSet = oboetaList[i];
        }
        _wazalist[i].wazaID = 0;
    }

    delete[] wazaList;
    delete[] oboetaList;
    return count;
}

void _checkWaza2Release()
{
    if (_wazalist != 0) {
        delete[] _wazalist;
        _wazalist = 0;
    }
}

WAZAOSHIEDEF *_getWaza2Pointer(s32 index)
{
    if (_wazalist == 0) {
        return 0;
    }
    return &_wazalist[index];
}

void menuScriptEeveeCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQUENCE_OPEN);
            window->mFlag = 1;
            switch (window->mMenuID) {
            case MENU_ID_EEVEE:
                menuSetStatus(window->mMenuID, MENU_STATUS_NO_CANCEL);
                break;
            case MENU_ID_USOHACHI:
                break;
            }
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

void _menuScriptTokuseiSetumei(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                               CURSOR *cursor, POKEMON_DATA_dd_ *pokemon)
{
    tagMENU_ITEM_DATA *item;
    u16 *doc;
    u16 tokusei;
    u32 length;
    u16 *line1;
    u16 *line2;
    u32 n;
    u32 size;
    s32 wordCount;
    s32 fitCount;
    bool overflow;

    item = menuItemBiosGetPtr(MENU_ID_EEVEE, sprite->mItemID);
    tokusei = pokemonDataBiosGetTokuseiDataId(pokemon, 0);
    if (tokusei == 0) {
        return;
    }

    doc = GSmsgGetStringPtr(pokemonTokuseiDataBiosGetDoc(pokemonTokuseiDataBiosGetPtr(tokusei)));
    length = GSmsgGetCharLength(doc);
    line1 = new u16[length + 2];
    line2 = new u16[length + 2];

    wordCount = 0;
    fitCount = 0;
    overflow = false;
    do {
        n = CopyTokuseiWords(line1, doc, wordCount++);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)line1);
        size = GSmsgGetMessageSize(MSG_PRINT_TEXT);
        if ((size >> 16) <= item->mWidth) {
            fitCount = wordCount;
        } else {
            overflow = true;
        }
    } while (n < length);

    if (overflow) {
        SplitTokuseiWords(line1, line2, doc, fitCount);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)line1);
        GSmsgPrint2(window->mLayerID, 0, 0,
                    window->mRgba.mColorA | COLOR_WHITE_RGB, MSG_PRINT_TEXT);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)line2);
        GSmsgPrint2(window->mLayerID, 0, TOKUSEI_LINE_HEIGHT,
                    window->mRgba.mColorA | COLOR_WHITE_RGB, MSG_PRINT_TEXT);
    } else {
        msgctrlSetValue(MSGCTRL_TEXT, (u32)doc);
        GSmsgPrint2(window->mLayerID, 0, 0,
                    window->mRgba.mColorA | COLOR_WHITE_RGB, MSG_PRINT_TEXT);
    }

    delete[] line1;
    delete[] line2;
}

void menuUsohachiHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case USOHACHI_ITEM_ROW:
        if (window->mMenuID == MENU_ID_USOHACHI) {
            windowDrawSprite(0, 0, window, SPRITE_ID_USOHACHI_PICTURE, 0,
                             winSpriteGetLayerID(window, sprite));
        } else {
            windowDrawSprite(0, 0, window, SPRITE_ID_USOHACHI_FIELD_PICTURE, 0,
                             winSpriteGetLayerID(window, sprite));
        }
        break;
    }
}

void _menuScriptEeveeStoneName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u16 stoneLabelIDs[EEVEE_EVOLUTION_COUNT] = {
        EEVEE_ITEM_STONE_NAME_1, EEVEE_ITEM_STONE_NAME_2, EEVEE_ITEM_STONE_NAME_3,
        EEVEE_ITEM_STONE_NAME_4, EEVEE_ITEM_STONE_NAME_5
    };
    u32 messages[EEVEE_EVOLUTION_COUNT] = {
        MSG_STONE_NAME_WATER, MSG_STONE_NAME_THUNDER, MSG_STONE_NAME_FIRE,
        MSG_STONE_NAME_MOON, MSG_STONE_NAME_SUN
    };
    u16 *name;
    s16 i;

    for (i = 0; i < EEVEE_EVOLUTION_COUNT; i++) {
        if ((u16)sprite->mItemID == stoneLabelIDs[i]) {
            name = GSmsgGetStringPtr(messages[i]);
            msgctrlSetValue(MSGCTRL_TEXT, (u32)name);
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                        window->mRgba.mColorA | COLOR_WHITE_RGB, MSG_POKEMON_NAME);
            break;
        }
    }
}

void menuScriptEeveeHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = window->mCursor;
    u16 evolutions[EEVEE_EVOLUTION_COUNT] = {
        POKEMON_SPECIES_VAPOREON, POKEMON_SPECIES_JOLTEON, POKEMON_SPECIES_FLAREON,
        POKEMON_SPECIES_UMBREON, POKEMON_SPECIES_ESPEON
    };
    POKEMON_DATA_dd_ *pokemon;
    s16 i;
    u8 tokusei;

    pokemon = pokemonDataBiosGetPtr(evolutions[cursor.y]);

    switch (sprite->mItemID) {
    case EEVEE_ITEM_HELD_ITEM:
        if (cursor.y >= EEVEE_ITEM_HELD_ITEM_ROW) {
            winSpriteSetDisp(sprite, true);
            u16 itemID = cursor.y == EEVEE_ITEM_HELD_ITEM_ROW ? ITEM_MOON_SHARD : ITEM_SUN_SHARD;
            msgctrlSetValue(MSGCTRL_NUM_ITEM, itemID);
            msgctrlSetValue(MSGCTRL_VALUE_POKEMON_ID, POKEMON_SPECIES_EEVEE);
            sprite->mMsgID = MSG_EEVEE_HELD_ITEM;
        } else {
            winSpriteSetDisp(sprite, false);
            sprite->mMsgID = 0;
        }
        break;

    case EEVEE_ITEM_EVOLUTION_ICON_1:
    case EEVEE_ITEM_EVOLUTION_ICON_2:
    case EEVEE_ITEM_EVOLUTION_ICON_3:
        if (cursor.y >= EEVEE_ITEM_HELD_ITEM_ROW) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;

    case EEVEE_ITEM_EVOLUTION_NAME: {
        u16 stoneSprites[EEVEE_EVOLUTION_COUNT] = {
            SPRITE_ID_EEVEE_STONE_WATER, SPRITE_ID_EEVEE_STONE_THUNDER,
            SPRITE_ID_EEVEE_STONE_FIRE, SPRITE_ID_EEVEE_STONE_MOON,
            SPRITE_ID_EEVEE_STONE_SUN
        };
        windowDrawSprite(0, 0, window, stoneSprites[cursor.y], 0, winSpriteGetLayerID(window, sprite));
        break;
    }

    case EEVEE_ITEM_ABILITY_TEXT:
        _menuScriptTokuseiSetumei(window, sprite, &cursor, pokemon);
        break;

    case EEVEE_ITEM_ABILITY_NAME:
        tokusei = pokemonDataBiosGetTokuseiDataId(pokemon, 0);
        if (tokusei != 0) {
            u16 *name = GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokusei)));
            msgctrlSetValue(MSGCTRL_TEXT, (u32)name);
            GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | COLOR_WHITE_RGB, MSG_PRINT_TEXT);
        }
        break;

    case EEVEE_ITEM_PREVIEW_COLOR:
        sprite->mRgba.mRed = EEVEE_PREVIEW_RED;
        sprite->mRgba.mGreen = EEVEE_PREVIEW_GREEN;
        sprite->mRgba.mBlue = EEVEE_PREVIEW_BLUE;
        break;

    case EEVEE_ITEM_POKEMON_NAME: {
        u16 *name = GSmsgGetStringPtr(pokemonDataBiosGetName(pokemon));
        msgctrlSetValue(MSGCTRL_TEXT, (u32)name);
        GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | COLOR_WHITE_RGB, MSG_PRINT_TEXT);
        break;
    }

    case EEVEE_ITEM_EVOLUTION_TEXT_1:
    case EEVEE_ITEM_EVOLUTION_TEXT_2:
    case EEVEE_ITEM_EVOLUTION_TEXT_3:
    case EEVEE_ITEM_EVOLUTION_TEXT_4:
    case EEVEE_ITEM_EVOLUTION_TEXT_5: {
        s32 evolutionRows[EEVEE_EVOLUTION_COUNT] = {
            EEVEE_ITEM_EVOLUTION_TEXT_1, EEVEE_ITEM_EVOLUTION_TEXT_2, EEVEE_ITEM_EVOLUTION_TEXT_3,
            EEVEE_ITEM_EVOLUTION_TEXT_4, EEVEE_ITEM_EVOLUTION_TEXT_5
        };
        for (i = 0; i < EEVEE_EVOLUTION_COUNT; i++) {
            if (sprite->mItemID == evolutionRows[i]) {
                break;
            }
        }
        if (i == cursor.y) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }

    case EEVEE_ITEM_TYPE_ICON:
        windowDrawSprite(0, 0, window, zokuseiGetPokemonSpriteID(pokemon, 0), 0, window->mLayerID);
        break;

    case EEVEE_ITEM_STONE_NAME_1:
    case EEVEE_ITEM_STONE_NAME_2:
    case EEVEE_ITEM_STONE_NAME_3:
    case EEVEE_ITEM_STONE_NAME_4:
    case EEVEE_ITEM_STONE_NAME_5:
        _menuScriptEeveeStoneName(window, sprite);
        break;
    }
}

void menuScriptTutorialCtrl(tagWINDOW_WORK *window)
{
    menuDataBiosGetPtr(window->mMenuID);
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQUENCE_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

void menuScriptTutorialCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u16 repeat;
    s32 entryCount;
    s32 visibleCount;

    windowGetKeyInfo();
    entryCount = *tutorialdata_number;
    visibleCount = TUTORIAL_ROW_COUNT;
    if (entryCount < TUTORIAL_ROW_COUNT) {
        visibleCount = entryCount;
    }
    repeat = windowGetKeyRepeat();
    CURSOR cursor = cursorBiosGetPos(CURSOR_SLOT_TUTORIAL);
    if (visibleCount != 0) {
        if (repeat & WINDOW_KEY_UP) {
            cursor.y--;
        } else if (repeat & WINDOW_KEY_DOWN) {
            cursor.y++;
        }
        if (cursor.y < 0) {
            cursor.x += cursor.y;
            cursor.y = 0;
            if (cursor.x < 0) {
                cursor.x = 0;
                cursor.y = 0;
            }
        } else if (cursor.y >= visibleCount) {
            cursor.x += cursor.y - (visibleCount - 1);
            cursor.y = (s8)(visibleCount - 1);
            if (cursor.x + cursor.y >= entryCount) {
                cursor.y = (s8)(visibleCount - 1);
                cursor.x = (s8)(entryCount - visibleCount);
            }
        }
    }
    _tutorialSelect = cursor.y == (s8)(*tutorialdata_number - 1) ? -1 : cursor.y + cursor.x;
    window->mCursor = cursor;
    cursorBiosSetPos(CURSOR_SLOT_TUTORIAL, cursor);
}

void menuScriptUsohachiCtrl(tagWINDOW_WORK *window)
{
    menuScriptEeveeCtrl(window);
}

void menuScriptWazaOshieCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQUENCE_OPEN);
            window->mFlag = 1;
            if (window->mMenuID == MENU_ID_WAZA_OSHIE) {
                window->mCursor = cursorBiosGetPos(CURSOR_SLOT_WAZA_OSHIE);
            }
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

u16 _wazaOshieWazaNum(tagWINDOW_WORK *window)
{
    if (window->mMenuID == MENU_ID_WAZA_TUTOR) {
        return _WazaNum;
    }
    return _WazaNum;
}

void _wazaOshieCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = window->mCursor;
    s32 rowItemIDs[WAZA_ROW_VISIBLE] = {
        WAZA_ITEM_ROW_1, WAZA_ITEM_ROW_2, WAZA_ITEM_ROW_3, WAZA_ITEM_ROW_4, WAZA_ITEM_ROW_5,
        WAZA_ITEM_ROW_6, WAZA_ITEM_ROW_7, WAZA_ITEM_ROW_8, WAZA_ITEM_ROW_9
    };
    s32 i;

    for (i = 0; i < WAZA_ROW_VISIBLE; i++) {
        if (sprite->mItemID == rowItemIDs[i]) {
            break;
        }
    }

    if (i == cursor.y) {
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }
}

void menuScriptWazaOshieHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = window->mCursor;
    u16 count = _wazaOshieWazaNum(window);
    WAZAOSHIEDEF *waza = _getWaza2Pointer(cursor.y + cursor.x);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    u32 color = window->mRgba.mColorA | COLOR_WHITE_RGB;

    switch (sprite->mItemID) {
    case WAZA_ITEM_ROW_1:
    case WAZA_ITEM_ROW_2:
    case WAZA_ITEM_ROW_3:
    case WAZA_ITEM_ROW_4:
    case WAZA_ITEM_ROW_5:
    case WAZA_ITEM_ROW_6:
    case WAZA_ITEM_ROW_7:
    case WAZA_ITEM_ROW_8:
    case WAZA_ITEM_ROW_9:
        _wazaOshieCursor(window, sprite);
        break;
    case WAZA_ITEM_TUTOR_PRICE_1:
    case WAZA_ITEM_TUTOR_PRICE_2:
    case WAZA_ITEM_TUTOR_PRICE_3:
        if (window->mMenuID == MENU_ID_WAZA_TUTOR) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case WAZA_ITEM_ARROW_DOWN:
        menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, WAZA_ROW_VISIBLE,
                                    false);
        break;
    case WAZA_ITEM_ARROW_UP:
        menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, WAZA_ROW_VISIBLE,
                                    true);
        break;
    case WAZA_ITEM_LIST: {
        tagMENU_ITEM_DATA *listItem;
        tagMENU_ITEM_DATA *item;
        MenuSpriteValue *mark;
        s16 y;
        u16 i;

        if (count == 0) {
            break;
        }

        listItem = menuItemBiosGetPtr(window->mMenuID, WAZA_ITEM_LIST);
        item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
        spriteSetScissor(layer, listItem->mX - item->mX, listItem->mY - item->mY,
                         listItem->mWidth, listItem->mHeight);

        i = 0;
        y = -cursor.x * WAZA_ROW_PITCH;
        while (i < count) {
            if (i >= cursor.x - 1 && i < cursor.x + WAZA_ROW_VISIBLE + 1) {
                waza = _getWaza2Pointer(i);
                if (waza->wazaID != 0) {
                    GSmsgPrint2(layer, 0, y, color, waza->nameID);
                    if (window->mMenuID == MENU_ID_WAZA_TUTOR) {
                        msgctrlSetValue(MSG_VAR_KANE, WAZA_TUTOR_PRICE);
                        GSmsgPrint2(
                            layer,
                            (s16)(sprite->mWidth -
                                  (GSmsgGetMessageSize(MSG_WAZA_PRICE) >> 16)),
                            y, color, MSG_WAZA_PRICE);
                    } else if (waza->tutorFlagSet != 0) {
                        mark = menuSpriteBiosGetValPtr(
                            menuSpriteBiosGetPtr(SPRITE_ID_WAZA_TUTOR));
                        windowDrawSprite(
                            (s16)(item->mWidth - mark->mWidth),
                            (s16)(y + (WAZA_ROW_LINE_HEIGHT - mark->mHeight) / 2),
                            window, SPRITE_ID_WAZA_TUTOR, 0, layer);
                    }
                } else if (window->mMenuID == MENU_ID_WAZA_TUTOR) {
                    GSmsgPrint2(layer, 0, y, color, MSG_WAZA_NO_MOVE_TUTOR);
                } else {
                    GSmsgPrint2(layer, 0, y, color, MSG_WAZA_NO_MOVE_LIST);
                }
            }
            y += WAZA_ROW_PITCH;
            i++;
        }
        spriteClearScissor(layer);
        break;
    }
    case WAZA_ITEM_DESCRIPTION:
        if (waza != NULL) {
            if (waza->wazaID != 0) {
                GSmsgPrint2(layer, 0, 0, color, waza->docID);
            } else {
                GSmsgPrint2(layer, 0, 0, color,
                            window->mMenuID == MENU_ID_WAZA_TUTOR
                                ? MSG_WAZA_NO_DESCRIPTION_TUTOR
                                : MSG_WAZA_NO_DESCRIPTION_LIST);
            }
        }
        break;
    case WAZA_ITEM_COUPON:
        if (window->mMenuID == MENU_ID_WAZA_TUTOR) {
            s32 coupon =
                heroBiosGetPokecoupon((Hero *)savedataGetStatus(0, SAVE_DATA_HERO));
            msgctrlSetValue(MSG_VAR_KANE, coupon);
            GSmsgPrint2(layer,
                        (s16)(sprite->mWidth -
                              (GSmsgGetMessageSize(MSG_MONEY_AMOUNT) >> 16)),
                        0, color, MSG_MONEY_AMOUNT);
        }
        break;
    case WAZA_ITEM_PP:
        if (waza != NULL && waza->wazaID != 0) {
            msgctrlSetValue(MSGCTRL_NUM_VALUE, waza->pp);
            GSmsgPrintRect(layer, 0, 0, sprite->mWidth, sprite->mHeight, color,
                           MSG_STATUS_NUMBER);
        }
        break;
    case WAZA_ITEM_AVERAGE:
        if (waza != NULL && waza->wazaID != 0) {
            if (waza->avg <= 1) {
                GSmsgPrintRight(layer, 0, 0, sprite->mWidth, sprite->mHeight, color,
                                MSG_WAZA_NO_VALUE);
            } else {
                msgctrlSetValue(MSGCTRL_NUM_PANEL, waza->avg);
                GSmsgPrintRect(layer, 0, 0, sprite->mWidth, sprite->mHeight, color,
                               MSG_STATUS_NUMBER);
            }
        }
        break;
    case WAZA_ITEM_POWER:
        if (waza != NULL && waza->wazaID != 0) {
            if (waza->iryoku <= 1) {
                GSmsgPrintRight(layer, 0, 0, sprite->mWidth, sprite->mHeight, color,
                                MSG_WAZA_NO_VALUE);
            } else {
                msgctrlSetValue(MSGCTRL_NUM_PANEL, waza->iryoku);
                GSmsgPrintRect(layer, 0, 0, sprite->mWidth, sprite->mHeight, color,
                               MSG_STATUS_NUMBER);
            }
        }
        break;
    case WAZA_ITEM_TYPE: {
        tagMENU_ITEM_DATA *item;

        if (waza != NULL && waza->wazaID != 0) {
            item = menuItemBiosGetPtr(window->mMenuID, WAZA_ITEM_TYPE);
            windowDrawSprite2(0, 0, item->mWidth, item->mHeight, color, window,
                              zokuseiBiosGetSpriteID(waza->zokuseiDataID), 0, layer);
        }
        break;
    }
    }
}

s32 menuScriptWazaOshieCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u16 key;
    u16 count;
    s32 visible;

    windowGetKeyInfo();
    key = windowGetKeyRepeat();
    CURSOR cursor = window->mCursor;

    count = _wazaOshieWazaNum(window);
    visible = WAZA_ROW_VISIBLE;
    if (count < visible) {
        visible = count;
    }

    if (visible != 0) {
        if (key & WINDOW_KEY_UP) {
            cursor.y--;
        } else if (key & WINDOW_KEY_DOWN) {
            cursor.y++;
        }

        if (cursor.y < 0) {
            cursor.x += cursor.y;
            cursor.y = 0;
            if (cursor.x < 0) {
                cursor.x = 0;
                cursor.y = 0;
            }
        } else if (cursor.y >= visible) {
            cursor.x += cursor.y - (visible - 1);
            cursor.y = (s8)(visible - 1);
            if (cursor.x + cursor.y >= count) {
                cursor.y = (s8)(visible - 1);
                cursor.x = (s8)(count - visible);
            }
        }
    }

    window->mCursor = cursor;

    if (window->mMenuID == MENU_ID_WAZA_OSHIE) {
        cursorBiosSetPos(CURSOR_SLOT_WAZA_OSHIE, cursor);
    }

    return 0;
}

void menuScriptTutorialHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = cursorBiosGetPos(CURSOR_SLOT_TUTORIAL);
    s16 i;

    switch (sprite->mItemID) {
    case TUTORIAL_ITEM_NAME_0:
    case TUTORIAL_ITEM_NAME_1:
    case TUTORIAL_ITEM_NAME_2:
    case TUTORIAL_ITEM_NAME_3:
    case TUTORIAL_ITEM_NAME_4:
    case TUTORIAL_ITEM_NAME_5:
    case TUTORIAL_ITEM_NAME_6:
    case TUTORIAL_ITEM_NAME_7:
        {
            s32 rows[TUTORIAL_ROW_COUNT] = {
                TUTORIAL_ITEM_NAME_0, TUTORIAL_ITEM_NAME_1, TUTORIAL_ITEM_NAME_2,
                TUTORIAL_ITEM_NAME_3, TUTORIAL_ITEM_NAME_4, TUTORIAL_ITEM_NAME_5,
                TUTORIAL_ITEM_NAME_6, TUTORIAL_ITEM_NAME_7
            };
            u16 id;

            for (i = 0; i < TUTORIAL_ROW_COUNT; i++) {
                if (sprite->mItemID == rows[i]) {
                    break;
                }
            }
            if (*tutorialdata_number > i) {
                id = tutorialdata[i + cursor.x];
                sprite->mMsgID = id ? deckDataVirtualDB_GetTitle(id) : MSG_TUTORIAL_NO_NAME;
            } else {
                sprite->mMsgID = 0;
            }
        }
        break;
    case TUTORIAL_ROW_HIGHLIGHT_0_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_1_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_2_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_3_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_4_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_5_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_6_LABEL:
    case TUTORIAL_ROW_HIGHLIGHT_7_LABEL:
        {
            s32 rows[TUTORIAL_ROW_COUNT] = {
                TUTORIAL_ROW_HIGHLIGHT_0_LABEL, TUTORIAL_ROW_HIGHLIGHT_1_LABEL,
                TUTORIAL_ROW_HIGHLIGHT_2_LABEL, TUTORIAL_ROW_HIGHLIGHT_3_LABEL,
                TUTORIAL_ROW_HIGHLIGHT_4_LABEL, TUTORIAL_ROW_HIGHLIGHT_5_LABEL,
                TUTORIAL_ROW_HIGHLIGHT_6_LABEL, TUTORIAL_ROW_HIGHLIGHT_7_LABEL
            };

            for (i = 0; i < TUTORIAL_ROW_COUNT; i++) {
                if (sprite->mItemID == rows[i]) {
                    break;
                }
            }
            if (i == cursor.y) {
                winSpriteSetDisp(sprite, true);
            } else {
                winSpriteSetDisp(sprite, false);
            }
        }
        break;
    case TUTORIAL_ROW_HIGHLIGHT_0_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_1_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_2_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_3_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_4_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_5_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_6_FRAME:
    case TUTORIAL_ROW_HIGHLIGHT_7_FRAME:
        {
            s32 rows[TUTORIAL_ROW_COUNT] = {
                TUTORIAL_ROW_HIGHLIGHT_0_FRAME, TUTORIAL_ROW_HIGHLIGHT_1_FRAME,
                TUTORIAL_ROW_HIGHLIGHT_2_FRAME, TUTORIAL_ROW_HIGHLIGHT_3_FRAME,
                TUTORIAL_ROW_HIGHLIGHT_4_FRAME, TUTORIAL_ROW_HIGHLIGHT_5_FRAME,
                TUTORIAL_ROW_HIGHLIGHT_6_FRAME, TUTORIAL_ROW_HIGHLIGHT_7_FRAME
            };

            for (i = 0; i < TUTORIAL_ROW_COUNT; i++) {
                if (sprite->mItemID == rows[i]) {
                    break;
                }
            }
            if (i == cursor.y) {
                winSpriteSetDisp(sprite, true);
            } else {
                winSpriteSetDisp(sprite, false);
            }
        }
        break;
    case TUTORIAL_ITEM_FLAG_0:
    case TUTORIAL_ITEM_FLAG_1:
    case TUTORIAL_ITEM_FLAG_2:
    case TUTORIAL_ITEM_FLAG_3:
    case TUTORIAL_ITEM_FLAG_4:
    case TUTORIAL_ITEM_FLAG_5:
    case TUTORIAL_ITEM_FLAG_6:
    case TUTORIAL_ITEM_FLAG_7:
        {
            s32 rows[TUTORIAL_ROW_COUNT] = {
                TUTORIAL_ITEM_FLAG_0, TUTORIAL_ITEM_FLAG_1, TUTORIAL_ITEM_FLAG_2,
                TUTORIAL_ITEM_FLAG_3, TUTORIAL_ITEM_FLAG_4, TUTORIAL_ITEM_FLAG_5,
                TUTORIAL_ITEM_FLAG_6, TUTORIAL_ITEM_FLAG_7
            };
            u16 id;
            u32 flag;

            for (i = 0; i < TUTORIAL_ROW_COUNT; i++) {
                if (sprite->mItemID == rows[i]) {
                    break;
                }
            }
            id = tutorialdata[i + cursor.x];
            flag = id ? deckDataVirtualDB_GetFlag(id) : 0;
            if (GSflagTest(flag) != 0 && flag != 0) {
                winSpriteSetDisp(sprite, true);
            } else {
                winSpriteSetDisp(sprite, false);
            }
        }
        break;
    case TUTORIAL_ITEM_DESCRIPTION:
        {
            u16 id = tutorialdata[cursor.y + cursor.x];

            sprite->mMsgID = id ? deckDataVirtualDB_GetExplanetion2(id) : 0;
        }
        break;
    case TUTORIAL_ITEM_ARROW_DOWN:
        menuPdaSubCheckUpDownCursor(window, sprite, &cursor, *tutorialdata_number, TUTORIAL_ROW_COUNT, false);
        break;
    case TUTORIAL_ITEM_ARROW_UP:
        menuPdaSubCheckUpDownCursor(window, sprite, &cursor, *tutorialdata_number, TUTORIAL_ROW_COUNT, true);
        break;
    }
}

void menuScriptPdaAddFuncInit(void)
{
    GSsndPlayNow(SOUND_ID_PDA_ADD_FUNC);
    cameraPlayAnime(PDA_RESOURCE_GROUP, PDA_CAMERA_RESOURCE, 0.0f, false);
    menuPdaModelVisibility(PDA_MODEL_RESOURCE, false);
}

void menuScriptPdaAddFuncMain(void)
{
    GSgfxVideoSetLockFrameRate(0);
    menuOffScreenSetPriority(0);
    menuPdaSetModelPointer();
    menuPdaModelVisibility(PDA_MODEL_RESOURCE, true);
    menuPdaAddFunc();
    menuPdaScript(_PdaAddFunc);
    GSgfxVideoSetLockFrameRate(1);
    menuPdaReleaseModelPointer();
    GSmodelAllPauseAnimation();
    GSfloorPop();
}

void menuScriptPdaAddFuncExit(void)
{
}
