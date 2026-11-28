#include <types.h>
#include <dolphin/pad.h>
#include <GSinput.h>
#include <GSthread.h>
#include <winSprite.h>
#include <menuSpriteBios.h>
#include <window.h>
#include <GSgfx.h>
#include <menu.h>
#include <menuCBCommon.h>
#include <menuSub.h>
#include <menuItemBios.h>
#include <GSmsg.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <pokemonDataBios.h>
#include <pokemonDB.h>
#include <string.h>
#include <time.h>
#include <math.h>

static f32 _cursorSec = 0.5f;
static f32 _max_alpha = 255.0f;
static f32 _min_alpha = 128.0f;
static f32 _cursorTimer;

static const u32 COLOR_WHITE_RGB = (255u << 24) | (255u << 16) | (255u << 8);

u8 _animAlpha(void);
void menuSubCheckSelectSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);

enum MenuSubSelectItem {
    MENU_SUB_SELECT_FIRST = 57,
    MENU_SUB_SELECT_SECOND = 58,
    MENU_SUB_SELECT_FIRST_ALT = 75,
    MENU_SUB_SELECT_SECOND_ALT = 76
};

enum MenuSubColosseumItem {
    MENU_SUB_COLOSSEUM_ITEM_FIRST = 52,
    MENU_SUB_COLOSSEUM_ITEM_SECOND = 53,
    MENU_SUB_COLOSSEUM_ITEM_THIRD = 54
};

void _menuSubDrawCursorBackSub(GSgfxLayerID layer, s32 x, s32 y, s32 width, s32 height,
                               u32 color, u8 alpha, u8 style, menuSubCursorBackColor backColor,
                               bool animeOnOff);

void menuSubDrawCursorBack2(GSgfxLayerID layer, s32 x, s32 y, s32 width, s32 height,
                            u8 alpha, u8 style, menuSubCursorBackColor backColor)
{
    _menuSubDrawCursorBackSub(layer, x, y, width, height, COLOR_WHITE_RGB, alpha,
                              style, backColor, true);
}

void menuSubDrawCursorBackAnimeOnOffAuto(tagWINDOW_WORK *window, GSgfxLayerID layer,
                                         s32 x, s32 y, s32 width, s32 height,
                                         u8 alpha, u8 style)
{
    if (window == windowSearchID(windowGetActiveID())) {
        _menuSubDrawCursorBackSub(layer, x, y, width, height, COLOR_WHITE_RGB, alpha, style,
                                  MENU_SUB_CURSOR_BACK_COLOR_REGULAR, true);
    } else {
        _menuSubDrawCursorBackSub(layer, x, y, width, height, COLOR_WHITE_RGB, alpha, style,
                                  MENU_SUB_CURSOR_BACK_COLOR_REGULAR, false);
    }
}

void menuSubDrawCursorBackAnimeOnOff(GSgfxLayerID layer, s32 x, s32 y, s32 width,
                                     s32 height, u8 alpha, u8 style, bool animeOnOff)
{
    _menuSubDrawCursorBackSub(layer, x, y, width, height, COLOR_WHITE_RGB, alpha, style,
                              MENU_SUB_CURSOR_BACK_COLOR_REGULAR, animeOnOff);
}

void menuSubDrawCursorBack(GSgfxLayerID layer, s32 x, s32 y, s32 width, s32 height,
                           u8 alpha, u8 style)
{
    _menuSubDrawCursorBackSub(layer, x, y, width, height, COLOR_WHITE_RGB, alpha, style,
                              MENU_SUB_CURSOR_BACK_COLOR_REGULAR, true);
}

void _menuSubDrawCursorBackSub(GSgfxLayerID layer, s32 x, s32 y, s32 width, s32 height,
                               u32 color, u8 alpha, u8 style, menuSubCursorBackColor backColor,
                               bool animeOnOff)
{
    _animAlpha();
    u8 anim;
    if (animeOnOff) {
        anim = _animAlpha();
    } else {
        anim = MENU_ITEM_ALPHA_OPAQUE;
    }
    u8 drawAlpha = (u16)(anim * alpha) / MENU_ITEM_ALPHA_OPAQUE;
    if (GSgfxLayerFindByID(layer) == 0) {
        return;
    }

    MenuSpriteData *capData;
    MenuSpriteData *middleData;
    if (style == MENU_SUB_CURSOR_BACK_STYLE_PLAIN) {
        GSivec2 cursor;
        cursor.x = x;
        cursor.y = y;
        capData = menuSpriteBiosGetPtr(TOOL_SPRITE_CURSOR_BACK_CAP);
        middleData = menuSpriteBiosGetPtr(TOOL_SPRITE_CURSOR_BACK_MIDDLE);
        MenuSpriteValue *cap = menuSpriteBiosGetValPtr(capData);
        MenuSpriteValue *middle = menuSpriteBiosGetValPtr(middleData);
        s32 middleWidth = width - cap->mWidth - cap->mWidth;
        windowDrawSprite2(cursor.x, cursor.y, cap->mWidth, cap->mHeight, color | drawAlpha, 0, TOOL_SPRITE_CURSOR_BACK_CAP, 0, layer);
        cursor.x += cap->mWidth;
        windowDrawSprite2(cursor.x, cursor.y, middleWidth, middle->mHeight, color | drawAlpha, 0, TOOL_SPRITE_CURSOR_BACK_MIDDLE, 0, layer);
        cursor.x += middleWidth;
        windowDrawSprite2(cursor.x, cursor.y, cap->mWidth, cap->mHeight, color | drawAlpha, 0, TOOL_SPRITE_CURSOR_BACK_CAP, WINDOW_DRAW_FLIP_X, layer);
    } else {
        s16 xpos = x;
        s16 ypos = y;
        u16 spriteIds[2];
        if (backColor == MENU_SUB_CURSOR_BACK_COLOR_REGULAR) {
            spriteIds[0] = TOOL_SPRITE_CURSOR_BACK_VARIANT_CAP;
            spriteIds[1] = TOOL_SPRITE_CURSOR_BACK_VARIANT_MIDDLE;
        } else {
            spriteIds[0] = TOOL_SPRITE_CURSOR_BACK_EXCHANGE_CAP;
            spriteIds[1] = TOOL_SPRITE_CURSOR_BACK_EXCHANGE_MIDDLE;
        }
        capData = menuSpriteBiosGetPtr(spriteIds[0]);
        middleData = menuSpriteBiosGetPtr(spriteIds[1]);
        MenuSpriteValue *cap = menuSpriteBiosGetValPtr(capData);
        MenuSpriteValue *middle = menuSpriteBiosGetValPtr(middleData);
        s32 middleWidth = width - cap->mWidth - cap->mWidth;
        windowDrawSprite2(xpos, ypos, cap->mWidth, cap->mHeight, color | drawAlpha, 0, spriteIds[0], 0, layer);
        xpos += cap->mWidth;
        windowDrawSprite2(xpos, ypos, middleWidth, middle->mHeight, color | drawAlpha, 0, spriteIds[1], 0, layer);
        xpos += middleWidth;
        windowDrawSprite2(xpos, ypos, cap->mWidth, cap->mHeight, color | drawAlpha, 0, spriteIds[0], WINDOW_DRAW_FLIP_X, layer);
    }
}

u8 menuSubAnimAlpha(void)
{
    return _animAlpha();
}

u8 _animAlpha(void)
{
    f32 speed = (2.0f * M_PI) / _cursorSec;
    f32 timer = menuGetTotalTime();
    timer *= speed;
    _cursorTimer = timer;
    f32 range = _max_alpha - _min_alpha;
    f32 alpha = (1.0f + (f32)cos(_cursorTimer)) / 2.0f;
    alpha = range * alpha;
    alpha += _min_alpha;
    if (alpha > 255.0f)
        alpha = 255.0f;
    return alpha;
}

s32 menuSubCheckSelect(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case MENU_SUB_SELECT_FIRST:
    case MENU_SUB_SELECT_FIRST_ALT:
        menuSubCheckSelectSub(window, sprite, 0);
        break;
    case MENU_SUB_SELECT_SECOND:
    case MENU_SUB_SELECT_SECOND_ALT:
        menuSubCheckSelectSub(window, sprite, 1);
        break;
    }
    return 0;
}

void menuSubCheckSelectSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    if (window->mCursor.y == row)
        winSpriteSetDisp(sprite, true);
    else
        winSpriteSetDisp(sprite, false);
}

// Cooperative wait: yields until waitSec of in-game time has elapsed.
void menuSubSleep(f32 seconds)
{
    f32 waited = 0.0f;
    while (waited < seconds) {
        GSthreadSwitch();
        waited += timeGetLastFrameSec();
    }
}

// Waits until a menu key on controller one is newly pressed.
void menuSubKeyWait()
{
    while (((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1)) &
            (PAD_BUTTON_A | PAD_BUTTON_B)) == 0) {
        GSthreadSwitch();
    }
}

// Blends the window item color with the sprite color, one channel at a time.
u32 menuSubCalcColor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    return ((sprite->mRgba.mRed * window->mRgba.mColorR / 255) & 0xFF) << 24 |
           ((sprite->mRgba.mGreen * window->mRgba.mColorG / 255) & 0xFF) << 16 |
           ((sprite->mRgba.mBlue * window->mRgba.mColorB / 255) & 0xFF) << 8 |
           ((sprite->mRgba.mAlpha * window->mRgba.mColorA / 255) & 0xFF);
}

extern "C" {
// Opens the select menu on the GBA screen and waits for the selection. If the
// GBA is disconnected midway the menu is force-closed and the wait returns -2.
s32 menuSubOpenSelect_gba(u8 type, const u32 *selectList, s32 listNum, s32 x, s32 y,
                          s32 initialCursor, s32 displayedCount, u8 wrapCursor, u8 allowCancel)
{
    s32 result = 0;

    menuSubCursorAnimeInit();
    menuOpenCustom(0xAE, windowGetActiveID(), &initialCursor, 0, false, 6, type, selectList, listNum,
                   displayedCount, wrapCursor, allowCancel);
    menuSetPosition(0xAE, x, y);
    while (true) {
        GSthreadSwitch();
        if (menuCB_GetDisconnectedGBA() != 0) {
            result = -2;
            tagWINDOW_WORK *win = windowSearchID(0xAE);
            win->mAccept = 1;
            win->mCancel = 1;
        }
        if (windowCheckCursor(0xAE, 0) == 0)
            break;
    }
    if (result != -2)
        result = windowGetValue(0xAE);
    menuCloseCustom(0xAE, 0, true);
    return result;
}

// Opens the select menu and waits for the selection.
s32 menuSubOpenSelect3(u8 type, const u32 *selectList, s32 listNum, s32 x, s32 y, s32 initialCursor,
                       s32 displayedCount, u8 wrapCursor, u8 allowCancel)
{
    s32 result;

    menuSubCursorAnimeInit();
    menuOpenCustom(0xAE, windowGetActiveID(), &initialCursor, 0, false, 6, type, selectList, listNum,
                   displayedCount, wrapCursor, allowCancel);
    menuSetPosition(0xAE, x, y);
    windowCheckCursor(0xAE, 1);
    result = windowGetValue(0xAE);
    menuCloseCustom(0xAE, 0, true);
    return result;
}

// Opens a message select menu and waits for the selection.
s32 menuSubOpenSelect2(u8 type, u32 *messageIDs, s32 count, s32 x, s32 y, s32 initialCursor,
                       s32 displayedCount, bool wrapCursor)
{
    s32 result;

    menuSubCursorAnimeInit();
    menuOpenCustom(0xAE, windowGetActiveID(), &initialCursor, 0, false, 6, type, messageIDs,
                   count, displayedCount, wrapCursor, 1);
    menuSetPosition(0xAE, (s16)x, (s16)y);
    windowCheckCursor(0xAE, 1);
    result = windowGetValue(0xAE);
    menuCloseCustom(0xAE, 0, true);
    return result;
}

// Opens a message select menu and waits for the selection.
s32 menuSubOpenSelect(u8 type, u32 *messages, int count, s32 x, s32 y, s32 cursor)
{
    s32 result;

    menuSubCursorAnimeInit();
    menuOpenCustom(0x59, windowGetActiveID(), &cursor, 0, false, 3, type, messages, count);
    menuSetPosition(0x59, (s16)x, (s16)y);
    windowCheckCursor(0x59, 1);
    result = windowGetValue(0x59);
    menuCloseCustom(0x59, 0, true);
    return result;
}
}

// Animation state of the animated cursor backs. menuSubCursorAnimeInit resets
// it, menuSubCursorAnimeStart seeds a slide-in and menuSubCursorAnimeMain
// advances it once per frame.
struct MenuSubCursorAnimeWork {
    f32 mValue[2];      // x/y offsets the cursor is drawn with.
    f32 mFrom[2];       // offsets the animation starts from.
    f32 mTo[2];         // offsets the animation runs to.
    s32 mPackedColor;   // blend color packed R,G,B,A for the cursor draw call.
    s16 mFrame;         // frame the animation is on.
    s16 mFrameCount;    // frame the animation runs to.
    f32 mTime;          // seconds spent on the current frame.
    GXColor mColor;     // blend color the cursor back is drawn with.
    GXColor mFromColor; // color the blend starts at.
    GXColor mToColor;   // color the blend ends at.
};

extern MenuSubCursorAnimeWork _cursorAnimeWork;

// Hands the current cursor animation offsets and packed blend color to the
// cursor draw call.
void menuSubCursorAnimeValGet(f32 *values, s32 *animation)
{
    values[0] = _cursorAnimeWork.mValue[0];
    values[1] = _cursorAnimeWork.mValue[1];
    *animation = _cursorAnimeWork.mPackedColor;
}

// Advances the cursor slide-in animation one frame. While the x offset is
// off-screen it eases toward the target offset and blends the cursor back
// color from the start color to the target color.
void menuSubCursorAnimeMain()
{
    MenuSubCursorAnimeWork *work = &_cursorAnimeWork;

    if (work->mValue[0] != 0.0f) {
        s16 frame = (work->mFrame = work->mFrame + timeGetLastFrameTime());
        f32 progress = (f32)work->mFrame * (f32)work->mFrame;
        progress = progress / (f32)work->mFrameCount / (f32)work->mFrameCount;
        f32 xOffset = (f32)(s16)(work->mFrom[0] + progress * (work->mTo[0] - work->mFrom[0]));
        f32 ratio = (f32)frame / (f32)work->mFrameCount;

        work->mValue[0] = xOffset;
        work->mColor.r = work->mFromColor.r + ratio * (work->mToColor.r - work->mFromColor.r);
        work->mColor.g = work->mFromColor.g + ratio * (work->mToColor.g - work->mFromColor.g);
        work->mColor.b = work->mFromColor.b + ratio * (work->mToColor.b - work->mFromColor.b);
        work->mColor.a = work->mFromColor.a + ratio * (work->mToColor.a - work->mFromColor.a);
        if (xOffset > 0.0f) {
            work->mValue[0] = 0.0f;
            work->mColor.r = work->mColor.g = work->mColor.b = work->mColor.a = 255;
        }
        work->mPackedColor = work->mColor.r << 24 | work->mColor.g << 16 | work->mColor.b << 8 | work->mColor.a;
    }
    _cursorAnimeWork.mTime += timeGetLastFrameSec();
}

// Starts the cursor slide-in from off-screen with a white back at the moment
// the select menu opens.
void menuSubCursorAnimeStart()
{
    _cursorAnimeWork.mValue[0] = -8.0f;
    _cursorAnimeWork.mValue[1] = 0.0f;
    _cursorAnimeWork.mTo[0] = 0.0f;
    _cursorAnimeWork.mTo[1] = 0.0f;
    _cursorAnimeWork.mFrom[0] = -8.0f;
    _cursorAnimeWork.mFrom[1] = 0.0f;
    _cursorAnimeWork.mPackedColor = -256;
    _cursorAnimeWork.mToColor.r = 255;
    _cursorAnimeWork.mToColor.g = 255;
    _cursorAnimeWork.mToColor.b = 255;
    _cursorAnimeWork.mToColor.a = 255;
    _cursorAnimeWork.mFromColor.r = 255;
    _cursorAnimeWork.mFromColor.g = 255;
    _cursorAnimeWork.mFromColor.b = 255;
    _cursorAnimeWork.mFromColor.a = 0;
    _cursorAnimeWork.mColor.r = 255;
    _cursorAnimeWork.mColor.g = 255;
    _cursorAnimeWork.mColor.b = 255;
    _cursorAnimeWork.mColor.a = 0;
    _cursorAnimeWork.mTime = 0.0f;
    _cursorAnimeWork.mFrame = 0;
    _cursorAnimeWork.mFrameCount = 5;
}

// Clears the cursor back animation state and packs it as opaque black.
void menuSubCursorAnimeInit()
{
    memset(&_cursorAnimeWork, 0, sizeof(MenuSubCursorAnimeWork));
    _cursorAnimeWork.mPackedColor = -1;
    _cursorAnimeWork.mValue[0] = 0.0f;
    _cursorAnimeWork.mValue[1] = 0.0f;
}

// Sex reported for a pokemon that has no valid data.
enum { POKEMON_SEX_INVALID = 255 };

// Sex displayed for a fight-record pokemon. The Nidoran species encode their
// sex in the species id; one that still carries its default name shows no
// sex, a renamed one shows its real sex.
u8 menuSubGetPokemonSexForFightDisp(Pokemon *pokemon)
{
    u16 species;
    PokemonSpeciesStats *stats;
    u16 *defaultName;
    u16 *nickname;

    if (!pokemonCheckValid(pokemon))
        return POKEMON_SEX_INVALID;
    species = pokemonBiosGetPokemonDataId(pokemon);
    if (species == POKEMON_DATA_ID_NIDORAN_F || species == POKEMON_DATA_ID_NIDORAN_M) {
        stats = pokemonDataBiosGetPtr((u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0));
        if (stats != NULL) {
            defaultName = GSmsgGetStringPtr(pokemonDataBiosGetName(stats));
            nickname = (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0);
            if (GScharCmp(nickname, defaultName) == 0)
                return POKEMON_SEX_NONE;
        }
    }
    return pokemonGetSex(pokemon);
}

// Sex displayed for a party pokemon. The Nidoran species encode their sex in
// the species id and report none here.
u8 menuSubGetPokemonSexForDisp(Pokemon *pokemon)
{
    u16 species;

    if (!pokemonCheckValid(pokemon))
        return POKEMON_SEX_INVALID;
    species = pokemonBiosGetPokemonDataId(pokemon);
    if (species == POKEMON_DATA_ID_NIDORAN_F || species == POKEMON_DATA_ID_NIDORAN_M)
        return POKEMON_SEX_NONE;
    return pokemonGetSex(pokemon);
}

// Use the colosseum menu's three choice messages.
s32 menuSubOpenColosseumEnter(s32 type)
{
    u32 messageIDs[3];

    messageIDs[0] = menuItemBiosGetPtr(MENU_ID_COLOSSEUM_ENTRY, MENU_SUB_COLOSSEUM_ITEM_FIRST)->mMessageID;
    messageIDs[1] = menuItemBiosGetPtr(MENU_ID_COLOSSEUM_ENTRY, MENU_SUB_COLOSSEUM_ITEM_SECOND)->mMessageID;
    messageIDs[2] = menuItemBiosGetPtr(MENU_ID_COLOSSEUM_ENTRY, MENU_SUB_COLOSSEUM_ITEM_THIRD)->mMessageID;
    return menuSubOpenSelect2(0, messageIDs, 3, 50, 50, 0, 3, 1);
}

s8 menuSubOpenYesNo(u8 type, s16 x, s16 y, s32 answer)
{
    s8 result;
    s16 menuID;

    if (answer != 0)
        answer = 1;
    switch (type) {
    case MENU_SUB_YES_NO_DIALOG:
        menuID = MENU_ID_YES_NO;
        break;
    case MENU_SUB_YES_NO_FIELD_DIALOG:
        menuID = MENU_ID_YES_NO;
        break;
    case MENU_SUB_YES_NO_DEBUG_DIALOG:
    default:
        menuID = MENU_ID_DEBUG_EDIT_CONFIRMATION;
        break;
    }
    menuOpenCustom(menuID, windowGetActiveID(), &answer, 0, false, 0);
    if (x >= 0 && y >= 0)
        menuSetPosition(menuID, x, y);
    windowCheckCursor(menuID, 1);
    result = windowGetValue(menuID);
    menuCloseCustom(menuID, 0, true);
    return result;
}

s8 menuSubOpenYesNoField(void)
{
    return menuSubOpenYesNo(MENU_SUB_YES_NO_FIELD_DIALOG, -1, -1, 0);
}

void menuSubCloseNumberInputWait(void)
{
    menuCloseSync(MENU_ID_NUMBER_INPUT, true);
}

void menuSubCloseNumberInput(void)
{
    menuClose(MENU_ID_NUMBER_INPUT);
}

u8 menuSubOpenNumberInputSub(u32 initialValue, u32 *result, u8 mode,
                              s16 x, s16 y, bool closeOnExit,
                              u16 *(*getName)(u32))
{
    u8 accepted = 0;
    tagWINDOW_WORK *window;

    menuOpenCustom(MENU_ID_NUMBER_INPUT, windowGetActiveID(), NULL, 0, false,
                   3, initialValue, mode, getName);
    menuSetPosition(MENU_ID_NUMBER_INPUT, x, y);
    windowCheckCursor(MENU_ID_NUMBER_INPUT, 1);
    windowGetValue(MENU_ID_NUMBER_INPUT);
    window = windowSearchID(MENU_ID_NUMBER_INPUT);
    if (window != NULL) {
        if (result != NULL)
            *result = window->mCursorIndex;
        if (window->mCancel == 0)
            accepted = 1;
        if (closeOnExit)
            menuSubCloseNumberInput();
    }
    return accepted;
}

u8 menuSubOpenNumberInput(u32 initialValue, u32 *result, u8 mode,
                           s16 x, s16 y, MENU_SUB_CLOSE_ON_EXIT closeOnExit)
{
    return menuSubOpenNumberInputSub(initialValue, result, mode, x, y, closeOnExit, NULL);
}

u8 menuSubOpenNumberInputID(u32 initialValue, u32 *result, u16 *(*getName)(u32))
{
    return menuSubOpenNumberInputSub(initialValue, result, MENU_SUB_NUMBER_UNSIGNED_DECIMAL,
                                    50, 60, true, getName);
}

u8 menuSubOpenNumberInputDigit(u32 initialValue, u32 *result)
{
    return menuSubOpenNumberInputSub(initialValue, result, MENU_SUB_NUMBER_UNSIGNED_DECIMAL,
                                    50, 60, true, NULL);
}

// White filled triangle through the vertices x1/y1, x2/y2 and x3/y3.
void menuSubDrawTriangle(s32 layerID, s32 x1, s32 y1, s32 x2, s32 y2,
                         s32 x3, s32 y3)
{
    GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    if (layer == 0) {
        return;
    }
    layer->mPrimitiveType = GSGFX_PRIM_TRIANGLES;
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(GX_POS_XY, GX_S16, 0, GX_NRM_XYZ, 1, GX_RGBA8, 0, 0, 0));
    layer->beginSurface(3);
    GSgfxFifo.s16 = (s16)x1;
    GSgfxFifo.s16 = (s16)y1;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.s16 = (s16)x2;
    GSgfxFifo.s16 = (s16)y2;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.s16 = (s16)x3;
    GSgfxFifo.s16 = (s16)y3;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    layer->endSurface();
}

// Fills the rectangle between the near corner x/y and the far corner
// x+width/y+height on a GSgfx layer.
void menuSubDrawTile(s32 layerID, s32 x, s32 y, s32 width, s32 height, GXColor color)
{
    GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    if (layer == 0) {
        return;
    }
    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(GX_POS_XY, GX_S16, 0, GX_NRM_XYZ, 1, GX_RGBA8, 0, 0, 0));
    width += x;
    height += y;
    layer->beginSurface(4);
    GSgfxFifo.s16 = (s16)x;
    GSgfxFifo.s16 = (s16)y;
    GXColor4u8(color.r, color.g, color.b, color.a);
    GSgfxFifo.s16 = (s16)width;
    GSgfxFifo.s16 = (s16)y;
    GXColor4u8(color.r, color.g, color.b, color.a);
    GSgfxFifo.s16 = (s16)x;
    GSgfxFifo.s16 = (s16)height;
    GXColor4u8(color.r, color.g, color.b, color.a);
    GSgfxFifo.s16 = (s16)width;
    GSgfxFifo.s16 = (s16)height;
    GXColor4u8(color.r, color.g, color.b, color.a);
    layer->endSurface();
}

// Translucent black fill with a white outline ten pixels outside the
// rectangle.
void menuSubDrawSimpleWindow(s32 layerID, s32 x, s32 y, s32 width, s32 height)
{
    GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    if (layer == 0) {
        return;
    }
    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(GX_POS_XY, GX_S16, 0, GX_NRM_XYZ, 1, GX_RGBA8, 0, 0, 0));
    layer->beginSurface(4);
    GSgfxFifo.s16 = (s16)(x - 10);
    GSgfxFifo.s16 = (s16)(y - 10);
    GSgfxFifo.u32 = 0x000000C0;
    GSgfxFifo.s16 = (s16)(x + width + 10);
    GSgfxFifo.s16 = (s16)(y - 10);
    GSgfxFifo.u32 = 0x000000C0;
    GSgfxFifo.s16 = (s16)(x - 10);
    GSgfxFifo.s16 = (s16)(y + height + 10);
    GSgfxFifo.u32 = 0x000000C0;
    GSgfxFifo.s16 = (s16)(x + width + 10);
    GSgfxFifo.s16 = (s16)(y + height + 10);
    GSgfxFifo.u32 = 0x000000C0;
    layer->endSurface();
    layer->lineWidth(6, GX_TO_ZERO);
    layer->mPrimitiveType = GSGFX_PRIM_LINE_STRIP;
    layer->beginSurface(5);
    GSgfxFifo.s16 = (s16)(x - 10);
    GSgfxFifo.s16 = (s16)(y - 10);
    GSgfxFifo.u32 = 0xFFFFFFFF;
    GSgfxFifo.s16 = (s16)(x + width + 10);
    GSgfxFifo.s16 = (s16)(y - 10);
    GSgfxFifo.u32 = 0xFFFFFFFF;
    GSgfxFifo.s16 = (s16)(x + width + 10);
    GSgfxFifo.s16 = (s16)(y + height + 10);
    GSgfxFifo.u32 = 0xFFFFFFFF;
    GSgfxFifo.s16 = (s16)(x - 10);
    GSgfxFifo.s16 = (s16)(y + height + 10);
    GSgfxFifo.u32 = 0xFFFFFFFF;
    GSgfxFifo.s16 = (s16)(x - 10);
    GSgfxFifo.s16 = (s16)(y - 10);
    GSgfxFifo.u32 = 0xFFFFFFFF;
    layer->endSurface();
}
