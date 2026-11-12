#include <menuPdaMemoXD.h>
#include <menuPdaMemoListXD.h>
#include <menuPdaMemoWaveXD.h>
#include <menuPda.h>
#include <menuModel.h>
#include <menuItemBios.h>
#include <memo.h>
#include <menu.h>
#include <window.h>
#include <GSsndPlayback.h>
#include <GSinput.h>
#include <GSmodel.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <pokecolo.h>
#include <PokemonData.h>
#include <pokemonBios.h>
#include <zokusei.h>
#include <dolphin/mtx.h>
#include <dolphin/pad.h>
#include <math.h>

extern u8 memoUsePokemonAisyou;

enum {
    MENU_ID_PDA_MEMO = 124,
    MENU_ID_PDA_MEMO_MODEL = 125,
    MENU_ID_PDA_MEMO_STATUS = 126,
};

enum {
    MEMO_PAGE_LIST = 0,
    MEMO_PAGE_MODEL = 1,
    MEMO_PAGE_SIZE = 2,
    MEMO_PAGE_STATUS = 3,
    MEMO_PAGE_MAX = 3,
};

enum {
    MEMO_LIST_LINES = 8,
    MEMO_SE_CURSOR = 1104,
    MEMO_SE_PAGE = 1107,
    MEMO_WINSEQ_OPEN = 306,
    MEMO_WINSEQ_CLOSE = 310,
    MEMO_TOKUSEI_LINE_HEIGHT = 18,
    MEMO_AISYOU_COUNT = 17,
};

// Model views drawn by menuPdaMemoListDispPokemonModel.
enum {
    MEMO_MODEL_STATUS = 0,
    MEMO_MODEL_LIST = 1,
    MEMO_MODEL_MODEL = 2,
    MEMO_MODEL_SIZE = 3,
};

// Sprites of the memo background menu.
enum {
    MEMO_BACK_SPRITE_MODEL_PAGE_1 = 173,
    MEMO_BACK_SPRITE_MODEL_PAGE_2 = 174,
    MEMO_BACK_SPRITE_MODEL_PAGE_3 = 175,
    MEMO_BACK_SPRITE_MODEL_PAGE_4 = 176,
    MEMO_BACK_SPRITE_LIST_PAGE_1 = 177,
    MEMO_BACK_SPRITE_LIST_PAGE_2 = 178,
    MEMO_BACK_SPRITE_LIST_PAGE_3 = 179,
    MEMO_BACK_SPRITE_LIST_PAGE_4 = 180,
    MEMO_BACK_SPRITE_STATUS_PAGE_1 = 181,
    MEMO_BACK_SPRITE_STATUS_PAGE_2 = 182,
    MEMO_BACK_SPRITE_STATUS_PAGE_3 = 183,
    MEMO_BACK_SPRITE_STATUS_PAGE_4 = 184,
    MEMO_BACK_SPRITE_MODEL_ONLY_1 = 188,
    MEMO_BACK_SPRITE_MODEL_ONLY_2 = 189,
    MEMO_BACK_SPRITE_MODEL_ONLY_3 = 190,
    MEMO_BACK_SPRITE_MODEL_ONLY_4 = 191,
    MEMO_BACK_SPRITE_MODEL_ONLY_5 = 192,
    MEMO_BACK_SPRITE_MODEL_ONLY_6 = 193,
    MEMO_BACK_SPRITE_MODEL_ONLY_7 = 194,
    MEMO_BACK_SPRITE_MODEL_ONLY_8 = 195,
    MEMO_BACK_SPRITE_STATUS_MODEL = 185,
    MEMO_BACK_SPRITE_MODEL_MODEL = 186,
    MEMO_BACK_SPRITE_LIST_MODEL = 187,
    MEMO_BACK_SPRITE_MEMO_COUNT = 196,
    MEMO_BACK_SPRITE_WEIGHT = 362,
    MEMO_BACK_SPRITE_HEIGHT = 363,
    MEMO_BACK_SPRITE_SIZE_MODEL = 455,
};

// Sprites of the memo page and status menus.
enum {
    MEMO_SPRITE_TYPE_1 = 253,
    MEMO_SPRITE_TYPE_2 = 254,
    MEMO_SPRITE_EMPTY = 255,
    MEMO_SPRITE_WEIGHT = 256,
    MEMO_SPRITE_HEIGHT = 257,
    MEMO_SPRITE_TOKUSEI_2 = 258,
    MEMO_SPRITE_TOKUSEI_1 = 259,
    MEMO_SPRITE_SIZE_FRAME = 260,
    MEMO_SPRITE_SYUZOKU = 261,
    MEMO_SPRITE_NAME = 262,
    MEMO_SPRITE_WAVE = 267,
    MEMO_STATUS_SPRITE_AISYOU_NORMAL = 324,
    MEMO_STATUS_SPRITE_AISYOU_FIGHTING = 325,
    MEMO_STATUS_SPRITE_AISYOU_POISON = 326,
    MEMO_STATUS_SPRITE_AISYOU_GROUND = 327,
    MEMO_STATUS_SPRITE_AISYOU_FLYING = 328,
    MEMO_STATUS_SPRITE_AISYOU_BUG = 329,
    MEMO_STATUS_SPRITE_AISYOU_ROCK = 330,
    MEMO_STATUS_SPRITE_AISYOU_GHOST = 331,
    MEMO_STATUS_SPRITE_AISYOU_STEEL = 332,
    MEMO_STATUS_SPRITE_AISYOU_FIRE = 333,
    MEMO_STATUS_SPRITE_AISYOU_WATER = 334,
    MEMO_STATUS_SPRITE_AISYOU_ELECTRIC = 335,
    MEMO_STATUS_SPRITE_AISYOU_GRASS = 336,
    MEMO_STATUS_SPRITE_AISYOU_ICE = 337,
    MEMO_STATUS_SPRITE_AISYOU_PSYCHIC = 338,
    MEMO_STATUS_SPRITE_AISYOU_DRAGON = 339,
    MEMO_STATUS_SPRITE_AISYOU_DARK = 340,
    MEMO_STATUS_SPRITE_TYPE_1 = 341,
    MEMO_STATUS_SPRITE_TYPE_2 = 342,
    MEMO_STATUS_SPRITE_SETUMEI_2 = 343,
    MEMO_STATUS_SPRITE_TOKUSEI_2 = 344,
    MEMO_STATUS_SPRITE_TOKUSEI_2_LABEL = 345,
    MEMO_STATUS_SPRITE_SETUMEI_1 = 346,
    MEMO_STATUS_SPRITE_TOKUSEI_1 = 347,
    MEMO_STATUS_SPRITE_TOKUSEI_1_LABEL = 348,
    MEMO_STATUS_SPRITE_SYUZOKU = 349,
    MEMO_STATUS_SPRITE_NAME = 350,
    MEMO_SPRITE_JOUTAI_NASHI = 362,
    MEMO_SPRITE_JOUTAI_IMAHITOTSU = 363,
    MEMO_SPRITE_JOUTAI_NORMAL = 364,
    MEMO_SPRITE_JOUTAI_BATSUGUN = 365,
};

enum {
    MEMO_MSG_VAR_LABEL = 77,
    MEMO_MSG_SIZE_WHOLE = 52,
    MEMO_MSG_SIZE_FRACTION = 53,
    MSG_MEMO_SYUZOKU = 15259,
    MSG_MEMO_WEIGHT = 15265,
    MSG_MEMO_HEIGHT = 15266,
    MSG_MEMO_TOKUSEI_TEXT = 15292,
    MSG_MEMO_POKEMON_NAME = 17013,
    MSG_MEMO_TEXT = 17014,
    MSG_MEMO_TOKUSEI_LABEL = 15208,
    MSG_MEMO_TOKUSEI_1 = 15209,
    MSG_MEMO_TOKUSEI_2 = 15210,
    MSG_MEMO_LABEL_TEXT = 17016,
};

// Colour of the height and weight panels.
#define MEMO_TEXT_RGB 0xFFFFFF00
#define MEMO_SIZE_COLOR_R 213
#define MEMO_SIZE_COLOR_G 170
#define MEMO_SIZE_COLOR_B 51

enum {
    LANGUAGE_ENGLISH = 2,
};

enum {
    MEMO_MENU_DATA_PAGE = 124,
    MEMO_MENU_DATA_TOKUSEI = 126,
};

enum {
    MEMO_TYPE_NORMAL = 0,
    MEMO_TYPE_FIGHTING = 1,
    MEMO_TYPE_FLYING = 2,
    MEMO_TYPE_POISON = 3,
    MEMO_TYPE_GROUND = 4,
    MEMO_TYPE_ROCK = 5,
    MEMO_TYPE_BUG = 6,
    MEMO_TYPE_GHOST = 7,
    MEMO_TYPE_STEEL = 8,
    MEMO_TYPE_MYSTERY = 9,
    MEMO_TYPE_FIRE = 10,
    MEMO_TYPE_WATER = 11,
    MEMO_TYPE_GRASS = 12,
    MEMO_TYPE_ELECTRIC = 13,
    MEMO_TYPE_PSYCHIC = 14,
    MEMO_TYPE_ICE = 15,
    MEMO_TYPE_DRAGON = 16,
    MEMO_TYPE_DARK = 17,
    MEMO_TYPE_COUNT = 18,
};

// Type-matchup results.
enum {
    WAZA_JOUTAI_NORMAL = 63,
    WAZA_JOUTAI_BATSUGUN = 65,
    WAZA_JOUTAI_IMAHITOTSU = 66,
    WAZA_JOUTAI_NASHI = 67,
};

/* The PDA memo screen that is currently open, or null. */
menuPdaMemoXD *pMemo;

/* The memo's cursor position, kept across memo screens. */
CURSOR _cursor;

static u16 _heightString[12];

void menuPdaMemoXD::_menuClose(long menuID, bool sync)
{
    menuClose(menuID);
    menuCloseSync(menuID, sync);
}

bool menuPdaMemoXD::TaskCloseCheck()
{
    u16 menuID = 0;
    s32 page = mPage;

    switch (page) {
    case MEMO_PAGE_LIST:
        menuID = MENU_ID_PDA_MEMO;
        break;
    case MEMO_PAGE_MODEL:
        menuID = MENU_ID_PDA_MEMO_MODEL;
        break;
    case MEMO_PAGE_SIZE:
        menuID = MENU_ID_PDA_MEMO_MODEL;
        break;
    case MEMO_PAGE_STATUS:
        menuID = MENU_ID_PDA_MEMO_STATUS;
        break;
    }
    if (menuID == 0) {
        return true;
    }
    return !winSeqCheckMove(menuID);
}

void menuPdaMemoXD::TaskClose(tagWINDOW_WORK *window, bool sync)
{
    s32 page = mPage;
    _GSmodel *model;

    if (window != 0) {
        window->mAccept = 1;
    }
    model = menuModelGetModelPtr(menuPdaMemoListGetMenuModelWork());
    if (model != 0) {
        GSmodelDestroyLinkedParticles(model);
    }
    switch (page) {
    case MEMO_PAGE_LIST:
        if (!winSeqCheckMove(MENU_ID_PDA_MEMO)) {
            _menuClose(MENU_ID_PDA_MEMO, sync);
        }
        break;
    case MEMO_PAGE_MODEL:
        if (!winSeqCheckMove(MENU_ID_PDA_MEMO_MODEL)) {
            _menuClose(MENU_ID_PDA_MEMO_MODEL, sync);
            menuPdaMemoWaveInit();
        }
        break;
    case MEMO_PAGE_SIZE:
        if (!winSeqCheckMove(MENU_ID_PDA_MEMO_MODEL)) {
            _menuClose(MENU_ID_PDA_MEMO_MODEL, sync);
        }
        break;
    case MEMO_PAGE_STATUS:
        if (!winSeqCheckMove(MENU_ID_PDA_MEMO_STATUS)) {
            _menuClose(MENU_ID_PDA_MEMO_STATUS, sync);
        }
        break;
    }
}

long menuPdaMemoXD::TaskOpen()
{
    long ret = -1;

    switch (mPage) {
    case MEMO_PAGE_LIST:
        menuCloseSync(MENU_ID_PDA_MEMO, true);
        ret = menuOpenCustom(MENU_ID_PDA_MEMO, windowGetActiveID(), 0, 0, true, 1, this);
        break;
    case MEMO_PAGE_MODEL:
        ret = menuOpenCustom(MENU_ID_PDA_MEMO_MODEL, windowGetActiveID(), 0, 0, true, 1, this);
        break;
    case MEMO_PAGE_SIZE:
        ret = menuOpenCustom(MENU_ID_PDA_MEMO_MODEL, windowGetActiveID(), 0, 0, true, 1, this);
        break;
    case MEMO_PAGE_STATUS:
        menuCloseSync(MENU_ID_PDA_MEMO_STATUS, true);
        ret = menuOpenCustom(MENU_ID_PDA_MEMO_STATUS, windowGetActiveID(), 0, 0, true, 1, this);
        break;
    }
    return ret;
}

void menuPdaMemoXD::Init(CURSOR *cursor)
{
    SetPage(MEMO_PAGE_LIST);
    mPrevPage = MEMO_PAGE_LIST;
    mMenuID = MENU_ID_PDA_MEMO;
    mContents.mCursor = &_cursor;
    mAngle[0] = mAngle[1] = mAngle[2] = 0.0f;
    mZoom = 0.75f;
    SetModelAngle(MEMO_PAGE_LIST);
    menuModelSetZoom(menuPdaMemoListGetMenuModelWork(), mZoom);
}

void menuPdaMemoXD::LoadPokemon(CURSOR *cursor)
{
    menuPdaMemoListLoadPokemon(cursor->y + cursor->x);
}

void menuPdaMemoXD::Release()
{
}

void menuPdaMemoXD::CameraControl()
{
    f32 move[3];
    s8 stickX;
    s8 stickY;

    move[0] = 0.0f;
    move[1] = 0.0f;
    move[2] = 0.0f;
    stickX = GSinputGetRightStickXData(GS_INPUT_PORT_1, true);
    stickY = GSinputGetRightStickYData(GS_INPUT_PORT_1, true);
    atan2(stickY, stickX);
    move[0] = (1.0f / 60.0f) * (-M_PI * stickY / 128.0f);
    move[1] = (1.0f / 60.0f) * (M_PI * stickX / 128.0f);
    PSVECAdd((Vec *)mAngle, (Vec *)move, (Vec *)mAngle);
    if (mAngle[0] >= M_PI / 2) {
        mAngle[0] = M_PI / 2 - move[0];
    }
    if (mAngle[0] <= -M_PI / 2) {
        mAngle[0] = -M_PI / 2 - move[0];
    }
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_R) {
        if (mZoom <= 1.2f) {
            mZoom += 1.0f / 60.0f;
        }
    } else if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_L) {
        if (mZoom > 0.3f) {
            mZoom -= 1.0f / 60.0f;
        }
    }
    menuModelSetAngle(menuPdaMemoListGetMenuModelWork(), mAngle);
    menuModelSetZoom(menuPdaMemoListGetMenuModelWork(), mZoom);
}

void menuPdaMemoXD::OokisaCameraControl()
{
    f32 move[3];
    s8 stickX;
    s8 stickY;
    f32 *rotation;

    move[0] = 0.0f;
    move[1] = 0.0f;
    move[2] = 0.0f;
    stickX = GSinputGetRightStickXData(GS_INPUT_PORT_1, true);
    stickY = GSinputGetRightStickYData(GS_INPUT_PORT_1, true);
    rotation = menuModelGetCameraRotation(menuPdaMemoListGetMenuModelWork());
    atan2(stickY, stickX);
    move[0] = (1.0f / 60.0f) * (-M_PI * stickY / 128.0f);
    move[1] = (1.0f / 60.0f) * (M_PI * stickX / 128.0f);
    PSVECAdd((Vec *)rotation, (Vec *)move, (Vec *)rotation);
    if (rotation[0] >= M_PI / 4) {
        rotation[0] = M_PI / 4;
    }
    if (rotation[0] <= -M_PI / 4) {
        rotation[0] = -M_PI / 4;
    }
    menuModelSetCameraRotation(menuPdaMemoListGetMenuModelWork(), rotation);
}

void menuPdaMemoXD::SetModelAngle(s16 page)
{
    f32 rotation[3];

    switch (page) {
    case MEMO_PAGE_MODEL:
        mAngle[0] = 0.0f;
        mAngle[1] = 0.0f;
        mAngle[2] = 0.0f;
        break;
    case MEMO_PAGE_SIZE:
        mAngle[0] = 0.0f;
        mAngle[1] = 0.0f;
        mAngle[2] = 0.0f;
        break;
    default:
        f32 z = menuPdaMemoListGetDefaultAngle(2);
        f32 y = menuPdaMemoListGetDefaultAngle(1);
        mAngle[0] = menuPdaMemoListGetDefaultAngle(0);
        mAngle[1] = y;
        mAngle[2] = z;
        break;
    }
    if (page == MEMO_PAGE_MODEL) {
        mZoom = 0.6f;
    } else {
        mZoom = 1.0f;
    }
    menuModelSetAngle(menuPdaMemoListGetMenuModelWork(), mAngle);
    menuModelSetZoom(menuPdaMemoListGetMenuModelWork(), mZoom);
    rotation[0] = 0.0f;
    rotation[1] = 0.0f;
    rotation[2] = 0.0f;
    menuModelSetCameraRotation(menuPdaMemoListGetMenuModelWork(), rotation);
    if (page == MEMO_PAGE_SIZE) {
        menuPdaMemoListLoadHero(true);
    } else {
        menuPdaMemoListLoadHero(false);
    }
}

void menuPdaMemoXD::Cursor(tagWINDOW_WORK *window)
{
    long index;
    CURSOR *cursor;
    long num;
    long lines;
    s16 page;
    s16 oldPage;
    tagWINDOW_KEY *key;
    u16 repeat;

    key = windowGetKeyInfo();
    cursor = ((MEMO_CONTENTS *)window->mArg)->mCursor;
    num = memoDataGetCount(0);
    repeat = key->mKeyRepeat;
    lines = MEMO_LIST_LINES;
    page = mPage;

    if (menuPdaMemoListIsTermModel() == 1) {
        return;
    }
    index = cursor->y + cursor->x;
    if (num < MEMO_LIST_LINES) {
        lines = num;
    }
    oldPage = mPage;
    if (repeat & WINDOW_KEY_UP) {
        if (index != 0) {
            GSsndPlayNow(MEMO_SE_CURSOR);
        }
        cursor->y--;
        if (page == MEMO_PAGE_MODEL && cursor->y + cursor->x >= 0) {
            SetModelAngle(MEMO_PAGE_MODEL);
        }
        if (page == MEMO_PAGE_SIZE) {
            f32 *rotation = menuModelGetCameraRotation(menuPdaMemoListGetMenuModelWork());
            rotation[0] = 0.0f;
            rotation[1] = 0.0f;
            rotation[2] = 0.0f;
        }
    } else if (repeat & WINDOW_KEY_DOWN) {
        if (cursor->y + cursor->x != num - 1) {
            GSsndPlayNow(MEMO_SE_CURSOR);
        }
        cursor->y++;
        if (page == MEMO_PAGE_MODEL && cursor->y + cursor->x != num) {
            SetModelAngle(MEMO_PAGE_MODEL);
        }
        if (page == MEMO_PAGE_SIZE) {
            f32 *rotation = menuModelGetCameraRotation(menuPdaMemoListGetMenuModelWork());
            rotation[0] = 0.0f;
            rotation[1] = 0.0f;
            rotation[2] = 0.0f;
        }
    } else if (repeat & WINDOW_KEY_LEFT) {
        if (TaskCloseCheck()) {
            if (page == MEMO_PAGE_MODEL) {
                menuPdaMemoWaveInit();
            }
            if (page > MEMO_PAGE_LIST) {
                GSsndPlayNow(MEMO_SE_PAGE);
                TaskClose(window, false);
                page--;
                SetPage(page);
            }
            SetModelAngle(page);
        }
    } else if (repeat & WINDOW_KEY_RIGHT) {
        if (TaskCloseCheck()) {
            if (page == MEMO_PAGE_MODEL) {
                menuPdaMemoWaveInit();
            }
            if (page < MEMO_PAGE_MAX) {
                GSsndPlayNow(MEMO_SE_PAGE);
                TaskClose(window, false);
                page++;
                SetPage(page);
            }
            SetModelAngle(page);
        }
    }

    page = mPage;
    if (page == MEMO_PAGE_MODEL) {
        CameraControl();
    } else if (page == MEMO_PAGE_SIZE) {
        OokisaCameraControl();
    }

    if (cursor->y < 0) {
        cursor->x += cursor->y;
        cursor->y = 0;
        if (cursor->x < 0) {
            cursor->x = 0;
            cursor->y = 0;
        }
    } else if (cursor->y >= lines) {
        cursor->x += cursor->y - (lines - 1);
        cursor->y = (s16)(lines - 1);
        if (cursor->x + cursor->y >= num) {
            cursor->y = (s16)(lines - 1);
            cursor->x = (s16)(num - lines);
        }
    }
    if (index != cursor->y + cursor->x) {
        LoadPokemon(cursor);
        menuPlaySe(window->mMenuID, 1);
    }
    if (oldPage != mPage) {
        menuPlaySe(window->mMenuID, 1);
    }
}

void menuPdaMemoXD::Ctrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, MEMO_WINSEQ_OPEN);
            window->mFlag = 1;
            menuPdaSetSubres(-1, 2);
        }
        break;
    case WINDOW_STATE_ACTIVE:
        if (mPage == MEMO_PAGE_SIZE && !menuPdaMemoListIsLoadHero()) {
            menuPdaMemoListSetHeroMotion();
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, MEMO_WINSEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

void menuPdaMemoXD::Button(tagWINDOW_WORK *window)
{
    if (window != 0) {
        u16 trg = windowGetKeyInfo()->mKeyTrg;
        menuPdaGetRes();
        menuPdaGetSubres();
        if (!winSeqCheckMove(window->mMenuID)) {
            if (trg & WINDOW_KEY_ACCEPT) {
                s32 page = mPage;
                if (page == MEMO_PAGE_MODEL) {
                    mPrevPage = page;
                    mPage = page;
                    window->mAccept = 1;
                }
            }
            if (trg & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
            }
        }
    }
}

CURSOR *menuPdaMemoXD::Main(CURSOR *cursor)
{
    _cursor.y = cursor->y;
    _cursor.x = cursor->x;
    while (true) {
        mContents.mCursor = &_cursor;
        if (TaskOpen() == -1) {
            break;
        }
        s32 page = mPage;
        if (page == mPrevPage && page == MEMO_PAGE_MODEL) {
            menuPdaMemoWave(_cursor.y + _cursor.x);
        }
    }
    TaskClose(0, true);
    menuPdaSetSubres(-1, 1);
    menuPdaMemoListLoadHero(false);
    return &_cursor;
}

void menuPdaMemoXD::PokemonName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents) {
    u16 *name = menuPdaMemoListGetPokemonName(contents->mCursor->y + contents->mCursor->x);

    if (name == NULL) {
        name = GSmsgGetStringPtr(1);
    }
    msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)name);
    GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_POKEMON_NAME);
}

void menuPdaMemoXD::PokemonType(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents, unsigned short slot) {
    PokemonSpeciesStats *data = pokemonDataBiosGetPtr(menuPdaMemoListGetPokemonID(contents->mCursor->y + contents->mCursor->x));

    if (slot == 0) {
        windowDrawSprite(0, 0, window, zokuseiGetPokemonSpriteID(data, slot), 0, window->mLayerID);
    } else {
        u16 firstSprite = zokuseiGetPokemonSpriteID(data, 0);
        u16 spriteID = zokuseiGetPokemonSpriteID(data, slot);
        if (spriteID != 0 && firstSprite != spriteID) {
            windowDrawSprite(0, 0, window, spriteID, 0, window->mLayerID);
        }
    }
}

void menuPdaMemoXD::PokemonTokusei(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents, unsigned short slot) {
    PokemonSpeciesStats *data = pokemonDataBiosGetPtr(menuPdaMemoListGetPokemonID(contents->mCursor->y + contents->mCursor->x));
    u16 firstTokusei = pokemonDataBiosGetTokuseiDataId(data, 0);
    u16 tokusei = pokemonDataBiosGetTokuseiDataId(data, slot);

    if (slot == 0) {
        if (tokusei != 0) {
            u16 *name = GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokusei)));
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)name);
            GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_LABEL_TEXT);
        }
    } else {
        if (tokusei != 0 && tokusei != firstTokusei) {
            u16 *name = GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokusei)));
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)name);
            GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_LABEL_TEXT);
        }
    }
}

static const unsigned long MEMO_CHAR_SPACE = 32;
static const unsigned long MEMO_CHAR_WIDE_SPACE = 12288;
static const unsigned long MEMO_CHAR_SKIP = 0xFFFF;
static const unsigned long MEMO_CHAR_END = 0;

static inline void SplitMemoWords(u16 *firstLine, u16 *secondLine, u16 *source, long words)
{
    long space;
    long len1;
    long len2;
    bool second;
    unsigned long length;
    unsigned long n;

    space = 0;
    len1 = 0;
    len2 = 0;
    second = false;
    length = GSmsgGetCharLength(source);
    for (n = 0;; n++) {
        if (source[n] == MEMO_CHAR_SKIP) {
            continue;
        }
        if (source[n] == MEMO_CHAR_SPACE || source[n] == MEMO_CHAR_WIDE_SPACE) {
            if (space == words - 1) {
                second = true;
            } else if (second) {
                secondLine[len2++] = source[n];
            } else {
                firstLine[len1++] = source[n];
            }
            space++;
        } else if (source[n] == MEMO_CHAR_END) {
            if (length <= n) {
                break;
            }
        } else if (second) {
            secondLine[len2++] = source[n];
        } else {
            firstLine[len1++] = source[n];
        }
    }
    firstLine[len1] = 0;
    secondLine[len2] = 0;
}

static inline unsigned long CopyMemoWords(u16 *destination, u16 *source, long lastWord)
{
    long space = 0;
    unsigned long n = 0;

    for (;;) {
        if (source[n] == MEMO_CHAR_SPACE || source[n] == MEMO_CHAR_WIDE_SPACE) {
            if (space == lastWord) {
                break;
            }
            destination[n] = source[n];
            space++;
        } else if (source[n] == MEMO_CHAR_END) {
            break;
        } else {
            destination[n] = source[n];
        }
        n++;
    }
    destination[n++] = 0;
    return n;
}

void menuPdaMemoXD::PokemonTokuseiSetumei(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents, unsigned short slot) {
    PokemonSpeciesStats *data = pokemonDataBiosGetPtr(menuPdaMemoListGetPokemonID(contents->mCursor->y + contents->mCursor->x));
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MEMO_MENU_DATA_TOKUSEI, sprite->mItemID);
    u16 *doc;
    u16 tokusei;
    u16 firstTokusei;
    u32 length;
    u16 *line1;
    u16 *line2;
    s32 wordCount;
    s32 fitCount;
    bool overflow;
    u32 n;
    u32 size;

    GSmsgGetStringPtr(MSG_MEMO_TOKUSEI_TEXT);
    firstTokusei = pokemonDataBiosGetTokuseiDataId(data, 0);
    tokusei = pokemonDataBiosGetTokuseiDataId(data, slot);
    if (slot != 0 && tokusei == firstTokusei) {
        tokusei = 0;
    }
    if (tokusei != 0) {
        doc = GSmsgGetStringPtr(pokemonTokuseiDataBiosGetDoc(pokemonTokuseiDataBiosGetPtr(tokusei)));
        length = GSmsgGetCharLength(doc);
        line1 = new u16[length + 2];
        line2 = new u16[length + 2];

        wordCount = 0;
        fitCount = 0;
        overflow = false;
        do {
            n = CopyMemoWords(line1, doc, wordCount++);
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)line1);
            size = GSmsgGetMessageSize(MSG_MEMO_TEXT);
            if ((size >> 16) <= item->mWidth) {
                fitCount = wordCount;
            } else {
                overflow = true;
            }
        } while (n < length);

        if (overflow) {
            SplitMemoWords(line1, line2, doc, fitCount);
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)line1);
            GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_TEXT);
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)line2);
            GSmsgPrint2(window->mLayerID, 0, MEMO_TOKUSEI_LINE_HEIGHT, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_TEXT);
        } else {
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)doc);
            GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_TEXT);
        }
        delete[] line1;
        delete[] line2;
    }
}

u16 menuPdaMemoXD::GetAisyouIndex(unsigned short index) {
    u16 table[MEMO_TYPE_COUNT] = {
        MEMO_TYPE_NORMAL, MEMO_TYPE_FIGHTING, MEMO_TYPE_POISON, MEMO_TYPE_GROUND, MEMO_TYPE_FLYING, MEMO_TYPE_BUG, MEMO_TYPE_ROCK, MEMO_TYPE_GHOST, MEMO_TYPE_STEEL,
        MEMO_TYPE_FIRE, MEMO_TYPE_WATER, MEMO_TYPE_ELECTRIC, MEMO_TYPE_GRASS, MEMO_TYPE_ICE, MEMO_TYPE_PSYCHIC, MEMO_TYPE_DRAGON, MEMO_TYPE_DARK, MEMO_TYPE_MYSTERY,
    };

    if ((s32)index > MEMO_TYPE_COUNT) {
        return 0;
    }
    return table[index];
}

void menuPdaMemoXD::SetJoutaiSprite(tagWINDOW_WORK *window, unsigned short joutai) {
    u16 joutaiTable[4] = { WAZA_JOUTAI_NASHI, WAZA_JOUTAI_IMAHITOTSU, WAZA_JOUTAI_NORMAL, WAZA_JOUTAI_BATSUGUN };
    u16 spriteTable[4] = { MEMO_SPRITE_JOUTAI_NASHI, MEMO_SPRITE_JOUTAI_IMAHITOTSU, MEMO_SPRITE_JOUTAI_NORMAL, MEMO_SPRITE_JOUTAI_BATSUGUN };
    u16 i;

    for (i = 0; i < 4; i++) {
        if (joutai == joutaiTable[i]) {
            windowDrawSprite(0, 0, window, spriteTable[i], 0, window->mLayerID);
        }
    }
}

void menuPdaMemoXD::PokemonAisyou(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents) {
    s32 itemTable[MEMO_AISYOU_COUNT] = {
        MEMO_STATUS_SPRITE_AISYOU_NORMAL,
        MEMO_STATUS_SPRITE_AISYOU_FIGHTING,
        MEMO_STATUS_SPRITE_AISYOU_POISON,
        MEMO_STATUS_SPRITE_AISYOU_GROUND,
        MEMO_STATUS_SPRITE_AISYOU_FLYING,
        MEMO_STATUS_SPRITE_AISYOU_BUG,
        MEMO_STATUS_SPRITE_AISYOU_ROCK,
        MEMO_STATUS_SPRITE_AISYOU_GHOST,
        MEMO_STATUS_SPRITE_AISYOU_STEEL,
        MEMO_STATUS_SPRITE_AISYOU_FIRE,
        MEMO_STATUS_SPRITE_AISYOU_WATER,
        MEMO_STATUS_SPRITE_AISYOU_ELECTRIC,
        MEMO_STATUS_SPRITE_AISYOU_GRASS,
        MEMO_STATUS_SPRITE_AISYOU_ICE,
        MEMO_STATUS_SPRITE_AISYOU_PSYCHIC,
        MEMO_STATUS_SPRITE_AISYOU_DRAGON,
        MEMO_STATUS_SPRITE_AISYOU_DARK,
    };
    u16 zokusei[2];
    long index = contents->mCursor->y + contents->mCursor->x;
    PokemonSpeciesStats *data = menuPdaMemoListGetPokemonDataPointer(index);
    u16 pokemonID = menuPdaMemoListGetPokemonID(index);
    u16 i;
    u16 attack;
    u16 count;
    u16 joutai;

    for (i = 0; i < MEMO_AISYOU_COUNT; i++) {
        if (sprite->mItemID == itemTable[i]) {
            break;
        }
    }
    attack = GetAisyouIndex(i);

    zokusei[0] = (u8)pokemonDataBiosGetZokuseiDataId(data, 0);
    count = 1;
    zokusei[1] = (u8)pokemonDataBiosGetZokuseiDataId(data, 1);
    if (zokusei[1] != 0) {
        count = 2;
    }

    if (memoUsePokemonAisyou) {
        joutai = zokuseiGetWazaJoutai_PokemonId(attack, pokemonID);
    } else {
        joutai = zokuseiGetWazaJoutaiAsDefenseZokuseiAry(attack, zokusei, count);
    }
    SetJoutaiSprite(window, joutai);
}

void menuPdaMemoXD::PokemonSyuzoku(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents) {
    msgctrlSetValue(MSGCTRL_VALUE_MESSAGE_ID,
                    pokemonDataBiosGetTypeName(menuPdaMemoListGetPokemonDataPointer(contents->mCursor->y + contents->mCursor->x)));
    GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_SYUZOKU);
}

void menuPdaMemoXD::PokemonWeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents) {
    PokemonSpeciesStats *data = menuPdaMemoListGetPokemonDataPointer(contents->mCursor->y + contents->mCursor->x);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MEMO_MENU_DATA_PAGE, sprite->mItemID);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    u16 weight = pokemonDataBiosGetWeight(data);

    if (pokecoloGetLanguage() == LANGUAGE_ENGLISH) {
        u32 pound = 2.2045856f * weight + 0.5f;
        msgctrlSetValue(MEMO_MSG_SIZE_WHOLE, pound / 10);
        msgctrlSetValue(MEMO_MSG_SIZE_FRACTION, pound % 10);
        GSmsgPrintRect(layer, 0, 0, item->mWidth, item->mHeight, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_WEIGHT);
    } else {
        msgctrlSetValue(MEMO_MSG_SIZE_WHOLE, weight / 10);
        msgctrlSetValue(MEMO_MSG_SIZE_FRACTION, weight % 10);
        GSmsgPrintRect(layer, 0, 0, item->mWidth, item->mHeight, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_WEIGHT);
    }
}

void menuPdaMemoXD::PokemonHeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, MEMO_CONTENTS *contents) {
    PokemonSpeciesStats *data = menuPdaMemoListGetPokemonDataPointer(contents->mCursor->y + contents->mCursor->x);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MEMO_MENU_DATA_PAGE, sprite->mItemID);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    u16 height = pokemonDataBiosGetHeight(data);

    if (pokecoloGetLanguage() == LANGUAGE_ENGLISH) {
        // height is stored in tenths of a metre; show it as feet'inches''
        f32 meter = height / 10.0f;
        f32 fInch = 39.37f * meter;
        u32 inch = 0.5f + fInch;
        u32 value;
        s32 count;
        s32 pos;
        u32 digit;

        value = inch % 12 + inch / 12 * 100;
        pos = 10;
        _heightString[--pos] = 0;
        _heightString[--pos] = '\'';
        _heightString[--pos] = '\'';
        count = 3;
        while (true) {
            digit = value % 10;
            value /= 10;
            if (count <= 0) {
                if (digit == 0) {
                    break;
                }
            } else {
                count--;
            }
            _heightString[--pos] = digit + '0';
            if (count == 1) {
                _heightString[--pos] = '\'';
            }
        }
        msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)&_heightString[pos]);
        GSmsgPrintRight(layer, 0, 0, item->mWidth, item->mHeight, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_TEXT);
    } else {
        msgctrlSetValue(MEMO_MSG_SIZE_WHOLE, height / 10);
        msgctrlSetValue(MEMO_MSG_SIZE_FRACTION, height % 10);
        GSmsgPrintRect(layer, 0, 0, item->mWidth, item->mHeight, window->mRgba.mColorA | MEMO_TEXT_RGB, MSG_MEMO_HEIGHT);
    }
}

extern "C" long menuPdaMemoXDCtrl(tagWINDOW_WORK *window)
{
    if (pMemo == 0) {
        return 0;
    }
    pMemo->Ctrl(window);
    return 0;
}

extern "C" long menuPdaMemoXDCursor(tagWINDOW_WORK *window)
{
    if (pMemo == 0) {
        return 0;
    }
    pMemo->Cursor(window);
    return 0;
}

extern "C" void menuPdaMemoXDButton(tagWINDOW_WORK *window)
{
    if (pMemo != 0) {
        pMemo->Button(window);
    }
}

extern "C" CURSOR *menuPdaMemoXDMain(CURSOR *cursor)
{
    pMemo = new menuPdaMemoXD(cursor);
    pMemo->Main(cursor);
    delete pMemo;
    pMemo = 0;
    return &_cursor;
}

extern "C" long menuPdaMemoXDHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (pMemo == 0) {
        return 0;
    }
    MEMO_CONTENTS *contents = (MEMO_CONTENTS *)window->mArg;
    switch (sprite->mItemID) {
    case MEMO_SPRITE_TYPE_1:
        pMemo->PokemonType(window, sprite, contents, 0);
        break;
    case MEMO_SPRITE_TYPE_2:
        pMemo->PokemonType(window, sprite, contents, 1);
        break;
    case MEMO_SPRITE_TOKUSEI_1:
        pMemo->PokemonTokusei(window, sprite, contents, 0);
        break;
    case MEMO_SPRITE_TOKUSEI_2:
        pMemo->PokemonTokusei(window, sprite, contents, 1);
        break;
    case MEMO_SPRITE_SYUZOKU:
        pMemo->PokemonSyuzoku(window, sprite, contents);
        break;
    case MEMO_SPRITE_NAME:
        pMemo->PokemonName(window, sprite, contents);
        break;
    case MEMO_SPRITE_EMPTY:
        break;
    case MEMO_SPRITE_WEIGHT:
        sprite->mRgba.mRed = MEMO_SIZE_COLOR_R;
        sprite->mRgba.mGreen = MEMO_SIZE_COLOR_G;
        sprite->mRgba.mBlue = MEMO_SIZE_COLOR_B;
        pMemo->PokemonWeight(window, sprite, contents);
        break;
    case MEMO_SPRITE_HEIGHT:
        sprite->mRgba.mRed = MEMO_SIZE_COLOR_R;
        sprite->mRgba.mGreen = MEMO_SIZE_COLOR_G;
        sprite->mRgba.mBlue = MEMO_SIZE_COLOR_B;
        pMemo->PokemonHeight(window, sprite, contents);
        break;
    case MEMO_SPRITE_SIZE_FRAME:
        sprite->mRgba.mRed = MEMO_SIZE_COLOR_R;
        sprite->mRgba.mGreen = MEMO_SIZE_COLOR_G;
        sprite->mRgba.mBlue = MEMO_SIZE_COLOR_B;
        break;
    case MEMO_SPRITE_WAVE:
        menuPdaMemoWaveHook(window, sprite);
        break;
    // Cells of the type matchup table.
    case MEMO_STATUS_SPRITE_AISYOU_NORMAL: case MEMO_STATUS_SPRITE_AISYOU_FIGHTING: case MEMO_STATUS_SPRITE_AISYOU_POISON: case MEMO_STATUS_SPRITE_AISYOU_GROUND: case MEMO_STATUS_SPRITE_AISYOU_FLYING: case MEMO_STATUS_SPRITE_AISYOU_BUG: case MEMO_STATUS_SPRITE_AISYOU_ROCK: case MEMO_STATUS_SPRITE_AISYOU_GHOST: case MEMO_STATUS_SPRITE_AISYOU_STEEL:
    case MEMO_STATUS_SPRITE_AISYOU_FIRE: case MEMO_STATUS_SPRITE_AISYOU_WATER: case MEMO_STATUS_SPRITE_AISYOU_ELECTRIC: case MEMO_STATUS_SPRITE_AISYOU_GRASS: case MEMO_STATUS_SPRITE_AISYOU_ICE: case MEMO_STATUS_SPRITE_AISYOU_PSYCHIC: case MEMO_STATUS_SPRITE_AISYOU_DRAGON: case MEMO_STATUS_SPRITE_AISYOU_DARK:
        pMemo->PokemonAisyou(window, sprite, contents);
        break;
    case MEMO_STATUS_SPRITE_SETUMEI_1:
        pMemo->PokemonTokuseiSetumei(window, sprite, contents, 0);
        break;
    case MEMO_STATUS_SPRITE_TOKUSEI_1:
        pMemo->PokemonTokusei(window, sprite, contents, 0);
        break;
    case MEMO_STATUS_SPRITE_SETUMEI_2:
        pMemo->PokemonTokuseiSetumei(window, sprite, contents, 1);
        break;
    case MEMO_STATUS_SPRITE_TOKUSEI_2:
        pMemo->PokemonTokusei(window, sprite, contents, 1);
        break;
    case MEMO_STATUS_SPRITE_SYUZOKU:
        pMemo->PokemonSyuzoku(window, sprite, contents);
        break;
    case MEMO_STATUS_SPRITE_NAME:
        pMemo->PokemonName(window, sprite, contents);
        break;
    case MEMO_STATUS_SPRITE_TYPE_1:
        pMemo->PokemonType(window, sprite, contents, 0);
        break;
    case MEMO_STATUS_SPRITE_TYPE_2:
        pMemo->PokemonType(window, sprite, contents, 1);
        break;
    case MEMO_STATUS_SPRITE_TOKUSEI_1_LABEL: {
        u16 *label = GSmsgGetStringPtr(MSG_MEMO_TOKUSEI_LABEL);
        msgctrlSetValue(MEMO_MSG_VAR_LABEL, (unsigned long)label);
        u16 *text = GSmsgGetStringPtr(MSG_MEMO_TOKUSEI_1);
        msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)text);
        GSmsgPrint2(window->mLayerID, 0, 0, MEMO_TEXT_RGB | window->mRgba.mColorA, MSG_MEMO_LABEL_TEXT);
        break;
    }
    case MEMO_STATUS_SPRITE_TOKUSEI_2_LABEL: {
        CURSOR *cursor = contents->mCursor;
        PokemonSpeciesStats *data = pokemonDataBiosGetPtr(menuPdaMemoListGetPokemonID(cursor->y + cursor->x));
        // A Pokemon whose second ability repeats the first has only one.
        u32 first = pokemonDataBiosGetTokuseiDataId(data, 0);
        u16 second = pokemonDataBiosGetTokuseiDataId(data, 1);
        if (second == first) {
            second = 0;
        }
        if (second != 0) {
            u16 *label = GSmsgGetStringPtr(MSG_MEMO_TOKUSEI_LABEL);
            msgctrlSetValue(MEMO_MSG_VAR_LABEL, (unsigned long)label);
            u16 *text = GSmsgGetStringPtr(MSG_MEMO_TOKUSEI_2);
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)text);
            GSmsgPrint2(window->mLayerID, 0, 0, MEMO_TEXT_RGB | window->mRgba.mColorA, MSG_MEMO_LABEL_TEXT);
        }
        break;
    }
    }
    return 0;
}

extern "C" long menuPdaMemoXDGetItem(void)
{
    if (pMemo == 0) {
        return 0;
    }
    return pMemo->mPage;
}

extern "C" void menuPdaMemoXDWeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (pMemo != 0) {
        sprite->mRgba.mRed = MEMO_SIZE_COLOR_R;
        sprite->mRgba.mGreen = MEMO_SIZE_COLOR_G;
        sprite->mRgba.mBlue = MEMO_SIZE_COLOR_B;
        pMemo->PokemonWeight(window, sprite, &pMemo->mContents);
    }
}

extern "C" void menuPdaMemoXDHeight(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (pMemo != 0) {
        sprite->mRgba.mRed = MEMO_SIZE_COLOR_R;
        sprite->mRgba.mGreen = MEMO_SIZE_COLOR_G;
        sprite->mRgba.mBlue = MEMO_SIZE_COLOR_B;
        pMemo->PokemonHeight(window, sprite, &pMemo->mContents);
    }
}

extern "C" void menuPdaMemoBackHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    menuPdaGetRes();
    menuPdaGetSubres();
    switch (sprite->mItemID) {
    // Sprites shown only on the status page.
    case MEMO_BACK_SPRITE_STATUS_PAGE_1:
    case MEMO_BACK_SPRITE_STATUS_PAGE_2:
    case MEMO_BACK_SPRITE_STATUS_PAGE_3:
    case MEMO_BACK_SPRITE_STATUS_PAGE_4:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_STATUS) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    // Sprites shown only on the list page.
    case MEMO_BACK_SPRITE_LIST_PAGE_1:
    case MEMO_BACK_SPRITE_LIST_PAGE_2:
    case MEMO_BACK_SPRITE_LIST_PAGE_3:
    case MEMO_BACK_SPRITE_LIST_PAGE_4:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_LIST) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    // Sprites shared by the model and size pages.
    case MEMO_BACK_SPRITE_MODEL_PAGE_1:
    case MEMO_BACK_SPRITE_MODEL_PAGE_2:
    case MEMO_BACK_SPRITE_MODEL_PAGE_3:
    case MEMO_BACK_SPRITE_MODEL_PAGE_4:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_MODEL || menuPdaMemoXDGetItem() == MEMO_PAGE_SIZE) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    // Sprites shown only on the model page.
    case MEMO_BACK_SPRITE_MODEL_ONLY_1:
    case MEMO_BACK_SPRITE_MODEL_ONLY_2:
    case MEMO_BACK_SPRITE_MODEL_ONLY_3:
    case MEMO_BACK_SPRITE_MODEL_ONLY_4:
    case MEMO_BACK_SPRITE_MODEL_ONLY_5:
    case MEMO_BACK_SPRITE_MODEL_ONLY_6:
    case MEMO_BACK_SPRITE_MODEL_ONLY_7:
    case MEMO_BACK_SPRITE_MODEL_ONLY_8:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_MODEL) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case MEMO_BACK_SPRITE_MEMO_COUNT:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_LIST) {
            winSpriteSetDisp(sprite, true);
            msgctrlSetValue(MSGCTRL_VALUE_NUMBER, memoDataGetCount(0));
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case MEMO_BACK_SPRITE_MODEL_MODEL:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_MODEL) {
            menuPdaMemoListDispPokemonModel(window, sprite, MEMO_MODEL_MODEL);
        }
        break;
    case MEMO_BACK_SPRITE_SIZE_MODEL:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_SIZE) {
            menuPdaMemoListDispPokemonModel(window, sprite, MEMO_MODEL_SIZE);
        }
        break;
    case MEMO_BACK_SPRITE_STATUS_MODEL:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_STATUS) {
            menuPdaMemoListDispPokemonModel(window, sprite, MEMO_MODEL_STATUS);
        }
        break;
    case MEMO_BACK_SPRITE_LIST_MODEL:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_LIST) {
            menuPdaMemoListDispPokemonModel(window, sprite, MEMO_MODEL_LIST);
        }
        break;
    case MEMO_BACK_SPRITE_WEIGHT:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_SIZE) {
            winSpriteSetDisp(sprite, true);
            menuPdaMemoXDWeight(window, sprite);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case MEMO_BACK_SPRITE_HEIGHT:
        if (menuPdaMemoXDGetItem() == MEMO_PAGE_SIZE) {
            winSpriteSetDisp(sprite, true);
            menuPdaMemoXDHeight(window, sprite);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
}
