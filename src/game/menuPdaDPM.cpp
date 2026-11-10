#include <winSprite.h>
#include <menu.h>
#include <menuPda.h>
#include <menuItemBios.h>
#include <menuModel.h>
#include <msgctrl.h>
#include <GSmsg.h>
#include <GSgfx.h>
#include <GSsndPlayback.h>
#include <gamedata.h>
#include <Pokemon.h>
#include <menuPokemonMessages.h>
#include <pokemonBios.h>
#include <darkPokemon.h>
#include <menuPdaDPM.h>

extern "C" {
void menuDrawReliveUpdate(void);
u16 mballBiosGetSpriteID(u8 ball);
u32 floorGetPlaceName(u32 place);
void menuPdaSetSubres(s32 res, s32 subres);
long menuPdaDPMListDispRelive(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long x, long y, long index,
                              u8 alpha, bool frame);
void menuDrawReliveMeterByDarkPokemon(DarkPokemon *pokemon, long x, long y, s16 width, s16 height, u8 alpha,
                                      long layerID);
u32 menuPdaDPMListGetDarkPokemonPlaceMsgID(long index, u8 *number);
u16 menuPdaDPMListGetDarkPokemonID(long index);
u16 menuPdaDPMListGetPokemonID(long index);
u16 *menuPdaDPMListGetPokemonName(long index);
u16 menuPdaDPMListGetDarkpokemonID(long index);
DarkPokemon *menuPdaDPMListGetDPointer(long index);
long menuPdaDPMListGetDPNum(void);
CURSOR *menuPdaDPMon(CURSOR *cursor);
}

menuPdaDPM *_pDPM;
static CURSOR _cursor;

// Menu data of the dark pokemon monitor window and of its row sprites.
enum {
    MENU_ID_PDA_DPMON = 117,
    MENU_ID_PDA_DPMON_LIST = 116,
    DPM_ITEM_RELIVE = 74,
    DPM_LIST_LINES = 10,
    DPM_PDA_SUBRES = 2,
    DPM_SEQ_OPEN = 306,
    DPM_SEQ_CLOSE = 310,
    DPM_POKEMON_LEVEL = 10,
};

enum { DPM_SE_CURSOR = 1104 };

enum {
    MSG_VAR_PLACE_NAME = 77,
    MSG_VAR_TRAINER_NAME = 87,
};

enum {
    MSG_EMPTY = 1,
    MSG_SEX_UNKNOWN = 2503,
    MSG_DPM_INFO_SNATCHED = 15203,
    MSG_DPM_INFO_NOT_SNATCHED = 15205,
    MSG_DPM_PLACE_UNKNOWN = 15272,
    MSG_DPM_INFO_TOGEPI = 15407,
    MSG_DPM_INFO_FLOOR_181 = 15408,
    MSG_DPM_STATUS_RELIVED = 15371,
};

enum {
    DPM_SPECIAL_FLOOR = 181, // floor whose dark pokemon report uses its own text.
    POKEMON_SEX_INVALID = 128,
};


CURSOR *menuPdaDPMon(CURSOR *cursor)
{
    _pDPM = new menuPdaDPM(cursor);
    _pDPM->Main(cursor);
    delete _pDPM;
    _pDPM = 0;
    return &_cursor;
}

menuPdaDPM::~menuPdaDPM()
{
    Release();
}

menuPdaDPM::menuPdaDPM(CURSOR *cursor)
{
    Init(cursor);
}

void menuPdaDPM::Release()
{
    menuModelTerm(&mModel);
}

void menuPdaDPM::LoadPokemon(CURSOR *cursor)
{
    Pokemon *pokemon = GetPokemonPointer(cursor->y + cursor->x);
    menuModelLoadPokemon(&mModel, pokemon);
}

void menuPdaDPM::Init(CURSOR *cursor)
{
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MENU_ID_PDA_DPMON, DPM_SPRITE_DISP_POKEMON_MODEL);
    menuModelInit(&mModel, item->mWidth, item->mHeight);
    LoadPokemon(cursor);
}

CURSOR *menuPdaDPM::Main(CURSOR *cursor)
{
    DPM_CONTENTS contents;

    _cursor.y = cursor->y;
    _cursor.x = cursor->x;
    do {
        contents.mCursor = &_cursor;
    } while (menuOpenCustom(MENU_ID_PDA_DPMON, windowGetActiveID(), 0, 0, true, 1, &contents) != -1);
    _menuClose(MENU_ID_PDA_DPMON);
    return &_cursor;
}

void menuPdaDPM::Button(tagWINDOW_WORK *window)
{
    if (window != 0 && !winSeqCheckMove(window->mMenuID)) {
        if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
            window->mAccept = 1;
            window->mCancel = 1;
        }
    }
}

void menuPdaDPM::Ctrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, DPM_SEQ_OPEN);
            window->mFlag = 1;
            menuPdaSetSubres(PDATOP_MENU_RES_DP_MON, DPM_PDA_SUBRES);
        }
        break;
    case WINDOW_STATE_ACTIVE:
        menuDrawReliveUpdate();
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, DPM_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
}

void menuPdaDPM::Cursor(tagWINDOW_WORK *window)
{
    long index;
    CURSOR *cursor;
    long num;
    long lines;
    u16 repeat;

    windowGetKeyInfo();
    cursor = ((DPM_CONTENTS *)window->mArg)->mCursor;
    num = menuPdaDPMListGetDPNum();
    lines = DPM_LIST_LINES;
    repeat = windowGetKeyRepeat();
    index = cursor->y + cursor->x;

    if (num < DPM_LIST_LINES) {
        lines = num;
    }
    if (repeat & WINDOW_KEY_UP) {
        if (index != 0) {
            GSsndPlayNow(DPM_SE_CURSOR);
        }
        cursor->y--;
    } else if (repeat & WINDOW_KEY_DOWN) {
        if (cursor->y + cursor->x != num - 1) {
            GSsndPlayNow(DPM_SE_CURSOR);
        }
        cursor->y++;
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
        cursor->y = (s8)(lines - 1);
        if (cursor->x + cursor->y >= num) {
            cursor->y = (s8)(lines - 1);
            cursor->x = (s8)(num - lines);
        }
    }
    if (index != cursor->y + cursor->x) {
        SetViewFlag(cursor);
        LoadPokemon(cursor);
        menuPlaySe(window->mMenuID, 1);
    }
}

void menuPdaDPM::SetViewFlag(CURSOR *cursor)
{
    DarkPokemon *darkPokemon = darkPokemonGetDarkPokemon(menuPdaDPMListGetDarkPokemonID(cursor->y + cursor->x));
    if (darkPokemon->isUpdate()) {
        darkPokemon->setUpdate(false);
    }
}

void menuPdaDPM::DispPokemonModel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    f32 angle[3];

    angle[0] = 0.0f;
    angle[1] = 0.3f;
    angle[2] = 0.0f;
    if (menuModelCheck(&mModel, 0)) {
        return;
    }
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MENU_ID_PDA_DPMON, DPM_SPRITE_DISP_POKEMON_MODEL);
    menuModelAdjustCamera(&mModel);
    menuModelSetAngle(&mModel, angle);
    menuModelSetSize(&mModel, item->mX, item->mY, item->mWidth, item->mHeight);
    menuModelSetZoom(&mModel, 0.85f);
    menuModelRender(&mModel);
    GStexture *texture = menuModelRender(&mModel);
    if (texture == 0) {
        return;
    }
    GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)window->mLayerID);
    if (layer == 0) {
        return;
    }
    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(7));
    layer->texSelect(0, texture);
    layer->beginSurface(4);
    // The model texture is drawn as a texture mapped quad: position, colour
    // and texture coordinate of each corner go straight into the write pipe.
    s16 x, y;
    u8 alpha;
    GSgfxFifo.s16 = 0;
    GSgfxFifo.s16 = 0;
    alpha = window->mRgba.mColorA;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    x = sprite->mWidth;
    GSgfxFifo.s16 = x;
    GSgfxFifo.u16 = 0;
    alpha = window->mRgba.mColorA;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 0.0f;
    y = sprite->mHeight;
    GSgfxFifo.u16 = 0;
    GSgfxFifo.s16 = y;
    alpha = window->mRgba.mColorA;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 1.0f;
    y = sprite->mHeight;
    x = sprite->mWidth;
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y;
    alpha = window->mRgba.mColorA;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 1.0f;
    layer->endSurface();
}

void menuPdaDPM::DarkPokemonRelive(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    menuPdaDPMListDispRelive(window, sprite, 0, 0, cursor->y + cursor->x, window->mRgba.mColorA, true);
}

void menuPdaDPM::DarkPokemonRibbon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    if (menuPdaDPMListGetDPointer(cursor->y + cursor->x)->getStatus() >= DARK_POKEMON_STATUS_RELIVED) {
        sprite->mFlags |= SPRITE_FLAG_DISP;
    } else {
        sprite->mFlags &= ~SPRITE_FLAG_DISP;
    }
}

void menuPdaDPM::DarkPokemonInfo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    darkPokemonStatus status;
    DarkPokemon *darkPokemon;
    u32 floorID;
    GSgfxLayerID layer;
    u32 msgID;

    darkPokemon = menuPdaDPMListGetDPointer(cursor->y + cursor->x);
    floorID = darkPokemon->getCatchFloorId();
    status = darkPokemon->getStatus();
    layer = winSpriteGetLayerID(window, sprite);

    if (darkPokemon->getPokemonID() == POKEMON_SPECIES_TOGEPI) {
        msgID = MSG_DPM_INFO_TOGEPI;
    } else if (status >= DARK_POKEMON_SNATCHED) {
        msgID = MSG_DPM_INFO_SNATCHED;
    } else {
        msgID = MSG_DPM_INFO_NOT_SNATCHED;
    }
    if (floorID == DPM_SPECIAL_FLOOR) {
        msgID = MSG_DPM_INFO_FLOOR_181;
    }
    if (msgID != 0) {
        darkPokemon->getOrgTrainer();
        u32 placeMsgID = floorGetPlaceName(floorID);
        if (placeMsgID == 0) {
            placeMsgID = MSG_DPM_PLACE_UNKNOWN;
        }
        u16 *place = GSmsgGetStringPtr(placeMsgID);
        msgctrlSetValue(MSG_VAR_PLACE_NAME, (unsigned long)place);
        msgctrlSetValue(MSG_VAR_TRAINER_NAME, (unsigned long)darkPokemon->getTrainerName());
        GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | 0xFFFFFF00, msgID);
    }
}

void menuPdaDPM::DarkPokemonPlace(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    u8 number;
    u32 msgID = menuPdaDPMListGetDarkPokemonPlaceMsgID(cursor->y + cursor->x, &number);

    if (msgID != 0) {
        msgctrlSetValue(MSGCTRL_NUM_INPUT, number + 1);
        u16 *place = GSmsgGetStringPtr(msgID);
        msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)place);
        GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | 0xFFFFFF00, MSG_PRINT_STRING);
    }
}

void menuPdaDPM::PokemonName(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    u16 *name = menuPdaDPMListGetPokemonName(cursor->y + cursor->x);

    if (name == 0) {
        name = GSmsgGetStringPtr(MSG_EMPTY);
    }
    msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)name);
    GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | 0xFFFFFF00, MSG_PRINT_STRING);
}

void menuPdaDPM::PokemonSex(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    long index = cursor->y + cursor->x;
    Pokemon *pokemon;
    DarkPokemon *darkPokemon;
    u16 *str;

    darkPokemon = menuPdaDPMListGetDPointer(index);
    pokemon = GetPokemonPointer(index);

    u32 rnd = darkPokemon->getRnd();
    pokemonBiosSetRnd(pokemon, rnd);
    u8 sex = pokemonGetSex(pokemon);
    pokemonBiosSetRnd(pokemon, mRnd);
    if (sex == POKEMON_SEX_INVALID) {
        str = GSmsgGetStringPtr(MSG_EMPTY);
    } else {
        switch (sex) {
        case POKEMON_SEX_MALE:
            str = GSmsgGetStringPtr(MSG_SEX_MALE);
            break;
        case POKEMON_SEX_FEMALE:
            str = GSmsgGetStringPtr(MSG_SEX_FEMALE);
            break;
        case POKEMON_SEX_NONE:
            str = GSmsgGetStringPtr(MSG_EMPTY);
            break;
        default:
            str = GSmsgGetStringPtr(MSG_SEX_FEMALE);
            break;
        }
    }
    if (darkPokemon->getCatchFloorId() == DPM_SPECIAL_FLOOR && sex != POKEMON_SEX_NONE) {
        str = GSmsgGetStringPtr(MSG_SEX_UNKNOWN);
    }
    msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)str);
    GSmsgPrint2(window->mLayerID, 0, 0, window->mRgba.mColorA | 0xFFFFFF00, MSG_PRINT_STRING);
}

void menuPdaDPM::PokemonBall(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, DPM_CONTENTS *contents)
{
    CURSOR *cursor = contents->mCursor;
    DarkPokemon *darkPokemon = menuPdaDPMListGetDPointer(cursor->y + cursor->x);

    if (darkPokemon->isSnach()) {
        u16 spriteID = mballBiosGetSpriteID(darkPokemon->getCatchBallID());
        if (spriteID != 0) {
            windowDrawSprite(0, 0, window, spriteID, 0, window->mLayerID);
        }
    }
}

void menuPdaDPM::_menuClose(long menuID)
{
    menuClose(menuID);
    menuCloseSync(menuID, true);
}

Pokemon *menuPdaDPM::GetPokemonPointer(long index)
{
    u16 pokemonID = menuPdaDPMListGetPokemonID(index);
    pokemonCreate(&mPokemon, pokemonID, DPM_POKEMON_LEVEL, gamedataGetStatus(0, GAMEDATA_STATUS_ATTEST));
    DarkPokemon *darkPokemon = menuPdaDPMListGetDPointer(index);
    u16 darkID = menuPdaDPMListGetDarkpokemonID(index);
    darkPokemon->getDarkPointDirect();
    mRnd = pokemonCreateRndFit(&mPokemon, -1, -1, 0, 0);
    pokemonBiosSetRnd(&mPokemon, mRnd);
    if (!darkPokemon->isRelive()) {
        pokemonBiosSetDarkpokemonDataId(&mPokemon, darkID);
    } else {
        pokemonBiosSetDarkpokemonDataId(&mPokemon, 0);
    }
    return &mPokemon;
}

extern "C" long menuPdaDPMListDispRelive(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long x, long y,
                                         long index, u8 alpha, bool frame)
{
    DarkPokemon *pokemon = menuPdaDPMListGetDPointer(index);
    darkPokemonStatus status = pokemon->getStatus();
    pokemon->getDarkPoint();

    switch (status) {
    case DARK_POKEMON_STATUS_NONE:
    case DARK_POKEMON_STATUS_FIGHT:
    case DARK_POKEMON_STATUS_UNKNOWN:
        break;
    case DARK_POKEMON_SNATCHED: {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MENU_ID_PDA_DPMON_LIST, DPM_ITEM_RELIVE);
        menuDrawReliveMeterByDarkPokemon(pokemon, x, y, item->mWidth, item->mHeight, window->mRgba.mColorA,
                                         window->mLayerID);
        break;
    }
    case DARK_POKEMON_STATUS_RELIVED: {
        u16 *text = GSmsgGetStringPtr(MSG_DPM_STATUS_RELIVED);
        msgctrlSetValue(MSGCTRL_VALUE_STRING, (unsigned long)text);
        if (frame) {
            GSmsgPrint2(window->mLayerID, x, y, 0xEE444400 | alpha, MSG_PRINT_TEXT);
        } else {
            GSmsgPrint2(window->mLayerID, x, y, 0xEE444400 | alpha, MSG_PRINT_STRING);
        }
        break;
    }
    }
    return 1;
}
