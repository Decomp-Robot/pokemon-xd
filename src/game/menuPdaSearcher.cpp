#include <math.h>

#include <GSflag.h>
#include <GSgfx.h>
#include <GSmath.h>
#include <GSmsg.h>
#include <GSres.h>
#include <GStexture.h>
#include <WorldMap.h>
#include <esaba.h>
#include <menu.h>
#include <menuItemBios.h>
#include <menuSpriteBios.h>
#include <time.h>
#include <winSprite.h>

// Poke Spot searcher display and window control.

enum {
    // Sprite item ids of the searcher rows. The first spot owns the first ten
    // bait meters, the second spot the next ten and the third spot the last
    // ten, so _esaMeterIndex0[0] and friends hold the first meter of a spot
    // and the sprite hook only needs the spot index of a meter.
    SEARCHER_SPRITE_METER0 = 408,
    SEARCHER_SPRITE_METER1 = 418,
    SEARCHER_SPRITE_METER2 = 428,
    SEARCHER_SPRITE_MSG0 = 439,
    SEARCHER_SPRITE_MSG1 = 442,
    SEARCHER_SPRITE_ICON0 = 445,
    SEARCHER_SPRITE_CURSOR0 = 448,
    SEARCHER_SPRITE_MOJI0 = 451,
    // Number of bait meters one Poke Spot has.
    SEARCHER_METER_MAX = 10,
};

// Number of spots the searcher shows.
enum { SEARCHER_SPOT_MAX = 3 };

// Message ids the searcher rows print.
enum {
    SEARCHER_MSG_SPOT0 = 15383,
    SEARCHER_MSG_SPOT1 = 15384,
    SEARCHER_MSG_SPOT2 = 15385,
};

// Menu sequence ids the window runs when it opens and when it closes.
enum {
    SEARCHER_SEQ_OPEN = 306,
    SEARCHER_SEQ_CLOSE = 310,
};

// Menus the searcher opens.
enum {
    SEARCHER_MENU_WINDOW = 119,
    SEARCHER_MENU_LIST = 109,
};

// Item id of the sprite that holds the animated spot cursor texture.
enum { SEARCHER_SPRITE_CURSOR_TEXTURE = 734 };

enum {
    SEARCHER_MAP_SPOT0 = 15,
    SEARCHER_MAP_SPOT1 = 16,
    SEARCHER_MAP_SPOT2 = 17,
    SEARCHER_VERTEX_XY16_RGBA8_ST = 7,
};

static s32 _esaMeterIndex0[SEARCHER_METER_MAX] = {
    408, 409, 410, 411, 412, 413, 414, 415, 416, 417,
};

// The second spot numbers its bait meters out of sprite-id order.
static s32 _esaMeterIndex1[SEARCHER_METER_MAX] = {
    418, 419, 420, 423, 421, 422, 424, 425, 426, 427,
};

static s32 _esaMeterIndex2[SEARCHER_METER_MAX] = {
    428, 429, 430, 431, 432, 433, 434, 435, 436, 437,
};

// Meter sprite ids of each spot.
s32 *_esaMeterIndex[SEARCHER_SPOT_MAX] = {
    _esaMeterIndex0,
    _esaMeterIndex1,
    _esaMeterIndex2,
};

// Animation phase of the spot cursors, one per spot.
f32 _esaTimer[SEARCHER_SPOT_MAX];

// Angle the spot cursor is rotated to.
f32 _startangle;

// Scale the spot cursor grows from and to while it animates.
f32 _esaCursorScale[2] = {0.5f, 1.5f};

static void _animMojiCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 index);
static void _animCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 index);
static void _updateTimer(u16 index);
static void _esaPrintMsg(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID);
static void _esaNumSet(tagSPRITE_WORK *sprite, u16 index);
static bool _getWorldMapFlag(u16 index);
static void _esaMonInit(void);
static void _menuClose(long menuID);
static f32 _gettLastTime(void);

// Tells whether the Poke Spot with the given index is marked on the world map.
static inline bool _esaSpotVisible(u16 index) {
    u16 spot[SEARCHER_SPOT_MAX] = {SEARCHER_MAP_SPOT0, SEARCHER_MAP_SPOT1, SEARCHER_MAP_SPOT2};

    if (index > SEARCHER_SPOT_MAX - 1)
        return false;

    WorldMapMapData *data = worldmapXD_data + spot[index];

    return GSflagGet(data->mFlagID);
}

extern "C" long menuPdaSeatcherHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    switch (sprite->mItemID) {
    // Messages of the three spots, one row each for the first and the second
    // spot block of the list.
    case SEARCHER_SPRITE_MSG0:
        if (_getWorldMapFlag(0))
            _esaPrintMsg(window, sprite, SEARCHER_MSG_SPOT0);
        break;
    case SEARCHER_SPRITE_MSG0 + 1:
        if (_getWorldMapFlag(1))
            _esaPrintMsg(window, sprite, SEARCHER_MSG_SPOT1);
        break;
    case SEARCHER_SPRITE_MSG0 + 2:
        if (_getWorldMapFlag(2))
            _esaPrintMsg(window, sprite, SEARCHER_MSG_SPOT2);
        break;
    case SEARCHER_SPRITE_MSG1:
        if (_getWorldMapFlag(0))
            _esaPrintMsg(window, sprite, SEARCHER_MSG_SPOT0);
        break;
    case SEARCHER_SPRITE_MSG1 + 1:
        if (_getWorldMapFlag(1))
            _esaPrintMsg(window, sprite, SEARCHER_MSG_SPOT1);
        break;
    case SEARCHER_SPRITE_MSG1 + 2:
        if (_getWorldMapFlag(2))
            _esaPrintMsg(window, sprite, SEARCHER_MSG_SPOT2);
        break;
    // Icons of the three spots: drawn while the spot is unlocked.
    case SEARCHER_SPRITE_ICON0:
        if (_getWorldMapFlag(0))
            winSpriteSetDisp(sprite, true);
        else
            winSpriteSetDisp(sprite, false);
        break;
    case SEARCHER_SPRITE_ICON0 + 1:
        if (_getWorldMapFlag(1))
            winSpriteSetDisp(sprite, true);
        else
            winSpriteSetDisp(sprite, false);
        break;
    case SEARCHER_SPRITE_ICON0 + 2:
        if (_getWorldMapFlag(2))
            winSpriteSetDisp(sprite, true);
        else
            winSpriteSetDisp(sprite, false);
        break;
    // Animate the spot indicator while a Pokemon is present.
    case SEARCHER_SPRITE_CURSOR0:
        if (_getWorldMapFlag(0) && esabaGetPokemonID(0) != 0) {
            winSpriteSetDisp(sprite, true);
            _animCursor(window, sprite, 0);
        }
        else
            winSpriteSetDisp(sprite, false);
        break;
    case SEARCHER_SPRITE_CURSOR0 + 1:
        if (_getWorldMapFlag(1) && esabaGetPokemonID(1) != 0) {
            winSpriteSetDisp(sprite, true);
            _animCursor(window, sprite, 1);
        }
        else
            winSpriteSetDisp(sprite, false);
        break;
    case SEARCHER_SPRITE_CURSOR0 + 2:
        if (_getWorldMapFlag(2) && esabaGetPokemonID(2) != 0) {
            winSpriteSetDisp(sprite, true);
            _animCursor(window, sprite, 2);
        }
        else
            winSpriteSetDisp(sprite, false);
        break;
    // Pulsing cursors drawn over the spots that are occupied.
    case SEARCHER_SPRITE_MOJI0:
        if (_getWorldMapFlag(0) && esabaGetPokemonID(0) != 0) {
            winSpriteSetDisp(sprite, true);
            _animMojiCursor(window, sprite, 0);
        }
        else
            winSpriteSetDisp(sprite, false);
        break;
    case SEARCHER_SPRITE_MOJI0 + 1:
        if (_getWorldMapFlag(1) && esabaGetPokemonID(1) != 0) {
            winSpriteSetDisp(sprite, true);
            _animMojiCursor(window, sprite, 1);
        }
        else
            winSpriteSetDisp(sprite, false);
        break;
    case SEARCHER_SPRITE_MOJI0 + 2:
        if (_getWorldMapFlag(2) && esabaGetPokemonID(2) != 0) {
            winSpriteSetDisp(sprite, true);
            _animMojiCursor(window, sprite, 2);
        }
        else
            winSpriteSetDisp(sprite, false);
        break;
    // Bait meters, one row per bait slot of a spot.
    case SEARCHER_SPRITE_METER0:
    case SEARCHER_SPRITE_METER0 + 1:
    case SEARCHER_SPRITE_METER0 + 2:
    case SEARCHER_SPRITE_METER0 + 3:
    case SEARCHER_SPRITE_METER0 + 4:
    case SEARCHER_SPRITE_METER0 + 5:
    case SEARCHER_SPRITE_METER0 + 6:
    case SEARCHER_SPRITE_METER0 + 7:
    case SEARCHER_SPRITE_METER0 + 8:
    case SEARCHER_SPRITE_METER0 + 9:
        _esaNumSet(sprite, 0);
        break;
    case SEARCHER_SPRITE_METER1:
    case SEARCHER_SPRITE_METER1 + 1:
    case SEARCHER_SPRITE_METER1 + 2:
    case SEARCHER_SPRITE_METER1 + 3:
    case SEARCHER_SPRITE_METER1 + 4:
    case SEARCHER_SPRITE_METER1 + 5:
    case SEARCHER_SPRITE_METER1 + 6:
    case SEARCHER_SPRITE_METER1 + 7:
    case SEARCHER_SPRITE_METER1 + 8:
    case SEARCHER_SPRITE_METER1 + 9:
        _esaNumSet(sprite, 1);
        break;
    case SEARCHER_SPRITE_METER2:
    case SEARCHER_SPRITE_METER2 + 1:
    case SEARCHER_SPRITE_METER2 + 2:
    case SEARCHER_SPRITE_METER2 + 3:
    case SEARCHER_SPRITE_METER2 + 4:
    case SEARCHER_SPRITE_METER2 + 5:
    case SEARCHER_SPRITE_METER2 + 6:
    case SEARCHER_SPRITE_METER2 + 7:
    case SEARCHER_SPRITE_METER2 + 8:
    case SEARCHER_SPRITE_METER2 + 9:
        _esaNumSet(sprite, 2);
        break;
    }

    return 0;
}

// Draws the pulsing cursor: the sprite fades in and out with the spot timer and
// is tinted with the alpha of the window.
static void _animMojiCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 index) {
    f32 pulse = (f32)cos(_esaTimer[index]);
    f32 alpha = 0.5f * (1.0f + pulse);
    alpha = 255.0f * alpha;
    u8 color;

    if (alpha >= 255.0f)
        alpha = 255.0f;

    color = (u8)alpha;

    sprite->mRgba.mAlpha = window->mRgba.mColorA * color / 255;
}

static void _animCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u16 index) {
    f32 sinValue;
    f32 cosValue;
    f32 time;
    f32 alpha;
    f32 scale;
    f32 centerX;
    f32 angle;
    f32 centerY;
    f32 radius;
    u16 xsize;
    u16 ysize;
    s32 i;
    GSgfxLayer *layer;
    MenuSpriteValue *value;
    GStexture *texture;
    GSvec texCoord[4];
    GSgfxVF *vf;
    u8 vertexAlpha;
    _updateTimer(index);

    time = _esaTimer[index] / 6.2831855f;
    alpha = 1.0f - time;
    alpha = 255.0f * alpha;
    if (alpha >= 255.0f)
        alpha = 255.0f;

    sprite->mRgba.mAlpha = window->mRgba.mColorA * (u8)alpha / 255;

    scale = GSlerpGetLinearInterpolationFloat(_esaCursorScale[0], _esaCursorScale[1], time);
    layer = GSgfxLayerFindByID(winSpriteGetLayerID(window, sprite));
    value = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(SEARCHER_SPRITE_CURSOR_TEXTURE));
    texture = (GStexture *)GSresGetResourceAllGroup((unsigned long)value->mDrawValue);
    xsize = GStextureGetXsize(texture);
    ysize = GStextureGetYsize(texture);

    angle = _startangle;
    radius = scale * (f32)(value->mWidth / 2);
    menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);

    centerX = (f32)(value->mWidth / 2);
    centerY = (f32)(value->mHeight / 2);
    texCoord[0].x = texCoord[3].x = (f32)value->mX / (f32)xsize;
    texCoord[1].x = texCoord[2].x = (f32)(value->mX + value->mWidth) / (f32)xsize;
    texCoord[0].y = texCoord[1].y = (f32)value->mY / (f32)ysize;
    texCoord[2].y = texCoord[3].y = (f32)(value->mY + value->mHeight) / (f32)ysize;

    layer->pixelMode(true, false, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_ALWAYS);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->texCoordGeneratorCount(1);
    layer->tgenFunc(0, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, 0, false, 0);
    layer->cunitCount(1);
    layer->cunitChannelGen(0, GSGFX_CUNIT_COLOR_AND_ALPHA, GSGFX_CUNIT_VERTEX_COLOR,
                           GSGFX_CUNIT_VERTEX_COLOR, 0, 0, GSGFX_LIGHT_DIFFUSE, GSGFX_DIFFUSE_NONE);
    layer->tunitCount(1);
    layer->tunitMapping(0, 0, 0, 0);
    layer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true,
                        GSGFX_TUNIT_REG_PREV);
    layer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO,
                         GSGFX_TUNIT_C_TEXC);
    layer->tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true,
                        GSGFX_TUNIT_REG_PREV);
    layer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_TEXA,
                         GSGFX_TUNIT_A_ZERO);
    layer->mPrimitiveType = GSGFX_QUADS;
    vf = GSgfxVFGetPredef(SEARCHER_VERTEX_XY16_RGBA8_ST);
    layer->mPrimitiveType = GSGFX_PRIM_TRIANGLE_FAN;
    layer->setRenderStateAndSelect(vf);
    layer->texSelect(0, texture);
    layer->beginSurface(4);

    for (i = 0; i < 4; i++) {
        sinValue = (f32)sin(angle);
        cosValue = (f32)cos(angle);

        GSgfxFifo.u16 = (u16)(radius * sinValue + centerX);
        GSgfxFifo.u16 = (u16)(radius * cosValue + centerY);
        vertexAlpha = sprite->mRgba.mAlpha;
        GSgfxFifo.u8 = 255;
        GSgfxFifo.u8 = 255;
        GSgfxFifo.u8 = 255;
        GSgfxFifo.u8 = vertexAlpha;
        GSgfxFifo.f32 = texCoord[i].x;
        GSgfxFifo.f32 = texCoord[i].y;
        angle += 1.5707964f;
    }
    layer->endSurface();
}

// Advances the animation phase of one spot once per frame. The spot timer runs
// from 0 to 2*pi, so the cursor animations repeat once per turn.
static void _updateTimer(u16 index) {
    f32 rate = esabaGetDigestRate(index);
    f32 frameTime = _gettLastTime();

    _esaTimer[index] += (3.1415927f * (2.0f * frameTime)) / (rate / 100.0f);

    if (_esaTimer[index] > 6.2831855f)
        _esaTimer[index] -= 6.2831855f;
}

// Prints the message of a row centred on the sprite.
static void _esaPrintMsg(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, u32 msgID) {
    u32 color = window->mRgba.mColorA | 0xFFFFFF00;
    u32 width = GSmsgGetMessageSize(msgID) >> 16;

    GSmsgPrintRect(window->mLayerID, (s16)((sprite->mWidth - width) / 2), 0, sprite->mWidth,
                   sprite->mHeight, color, msgID);
}

// Shows the bait meter of one row while the spot still has bait left at that
// slot.
static void _esaNumSet(tagSPRITE_WORK *sprite, u16 index) {
    s32 *meter = _esaMeterIndex[index];
    s32 esaNum = esabaGetEsaNum(index);
    s32 slot;

    if (esaNum == 0)
        winSpriteSetDisp(sprite, false);

    for (slot = 0; slot < SEARCHER_METER_MAX; slot++) {
        if (sprite->mItemID == meter[slot])
            break;
    }

    if (esaNum >= slot + 1)
        winSpriteSetDisp(sprite, true);
    else
        winSpriteSetDisp(sprite, false);
}

// Runs the window's menu sequence. The searcher plays its open sequence once
// the window starts and its close sequence when the player leaves it.
extern "C" long menuPdaSearcherCtrl(tagWINDOW_WORK *window) {
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, SEARCHER_SEQ_OPEN);
            window->mFlag = 1;
            _esaMonInit();
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, SEARCHER_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }

    return 0;
}

// Reads the world map flag of one Poke Spot through the world map data of the
// spot. The three spots are the three Poke Spots of the world map table.
static bool _getWorldMapFlag(u16 index) {
    return _esaSpotVisible(index);
}

// Clears the spot timers when the searcher opens.
static void _esaMonInit(void) {
    u32 i;

    for (i = 0; i < *esabadata_number; i++)
        _esaTimer[i] = 0.0f;
}

// B button handler of the searcher window: cancelling the window closes the
// searcher.
extern "C" void menuPdaSearcherButton(tagWINDOW_WORK *window) {
    if (window == NULL)
        return;

    if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

extern "C" void menuPdaSearcher(void) {
    menuOpen(SEARCHER_MENU_WINDOW, 0);

    while (menuOpen(SEARCHER_MENU_LIST, 1) >= 0) {
    }

    _menuClose(SEARCHER_MENU_LIST);
    _menuClose(SEARCHER_MENU_WINDOW);
}

static void _menuClose(long menuID) {
    menuClose(menuID);
    menuCloseSync(menuID, true);
}

static f32 _gettLastTime(void) {
    return timeGetLastFrameSec();
}
