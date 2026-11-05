#include <types.h>
#include <dolphin/pad.h>
#include <GSinput.h>
#include <GSthread.h>
#include <winSprite.h>
#include <window.h>
#include <menu.h>
#include <menuCBCommon.h>
#include <menuSub.h>
#include <GSmsg.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <pokemonDataBios.h>
#include <pokemonDB.h>
#include <string.h>
#include <time.h>

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
s32 menuSubOpenSelect_gba(u8 type, const u32 *selectList, s32 listNum, s16 x, s16 y,
                          s32 work, s32 cursor, u8 a, u8 b)
{
    s32 result = 0;

    menuSubCursorAnimeInit();
    menuOpenCustom(0xAE, windowGetActiveID(), &work, 0, false, 6, type, selectList, listNum,
                   cursor, a, b);
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
s32 menuSubOpenSelect3(u8 type, const u32 *selectList, s32 listNum, s16 x, s16 y, s32 work,
                       s32 cursor, u8 a, u8 b)
{
    s32 result;

    menuSubCursorAnimeInit();
    menuOpenCustom(0xAE, windowGetActiveID(), &work, 0, false, 6, type, selectList, listNum,
                   cursor, a, b);
    menuSetPosition(0xAE, x, y);
    windowCheckCursor(0xAE, 1);
    result = windowGetValue(0xAE);
    menuCloseCustom(0xAE, 0, true);
    return result;
}

// Opens a message select menu and waits for the selection.
s32 menuSubOpenSelect2(u8 type, u32 *messageIDs, s32 count, s32 x, s32 y, s32 initialCursor,
                       s32 displayedCount, u8 selectMode)
{
    s32 result;

    menuSubCursorAnimeInit();
    menuOpenCustom(0xAE, windowGetActiveID(), &initialCursor, 0, false, 6, type, messageIDs,
                   count, displayedCount, selectMode, 1);
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
