#ifndef MENU_POCKET_TOOL_H
#define MENU_POCKET_TOOL_H

#include <PocketVec.h>
#include <types.h>
#include <Pokemon.h>
#include <menuPtSpriteWork.h>
#include <menuSpriteBios.h>
#include <GSgfxTypes.h>

// Move a value toward zero by the specified step.
void menuPocketToolFloatToZero(f32 &value, f32 step);

// Play a sound effect from the pocket tool menu.
void menuPtPlaySe(unsigned long seID);

// Angle helpers the pocket menus share. The refresh helpers wrap a value
// into a fixed range and the subtraction helpers wrap the difference of two
// values, so callers can normalise a heading before comparing or drawing it.

// True when dir lies inside the clockwise region that runs from `from` to
// `to`; a region whose ends are equal covers the whole circle.
bool isDirRegionIn(f32 dir, f32 from, f32 to);

// Angle of the vector (x, y), with the screen-space y axis pointing down.
f32 xy2dir(f32 x, f32 y);

// Distance from center to point.
f32 xyxy2distance(const PocketVec<short>& point, const PocketVec<short>& center);

// Direction from center to point.
f32 xyxy2dir(const PocketVec<short> &point, const PocketVec<short> &center);

// Direction from center to point, rounded to the nearest degree.
long xyxy2dirDegree(const PocketVec<short> &point, const PocketVec<short> &center);

// Wrap a degree value into the [-180, 180) range.
long degreeRefresh180(long degree);

// Wrap a degree value into the [0, 360) range.
long degreeRefresh360(long degree);

// Wrap a direction value into the [0, 2*PI) range.
f32 dirRefresh2PI(f32 dir);

// Wrap a direction value into the [-PI, PI) range.
f32 dirRefreshPI(f32 dir);

// Wrap the difference of two degree values into [-180, 180).
long degreeSubHalf(long degree1, long degree2);

// Wrap the difference of two directions into [0, 2*PI).
f32 dirSubOne(f32 dir1, f32 dir2);

// Wrap the difference of two directions into [-PI, PI).
f32 dirSubHalf(f32 dir1, f32 dir2);

void menuPtSpriteDrawColor(unsigned short textureID, long x, long y, unsigned long color);

struct MenuPtWorkSound {
    unsigned long mBgmID;
    unsigned long mEnvID;
    unsigned char mBgmVolume;
    unsigned char mEnvVolume;
};

void menuPtSaveSound(MenuPtWorkSound *sound);
void menuPtRestoreSound(MenuPtWorkSound *sound);
void menuPtPlayBgm(unsigned long bgmID);
void menuPtPlaySe(unsigned long seID);
void menuPocketSetDarkPokemon(Pokemon &pokemon, float dp, unsigned short darkID);
void menuPocketToolFloatToZero(f32 &value, f32 speed);

u16 pokemonindex(int index);
void setBitTable(unsigned long *table, long index, bool flag);
bool getBitTable(unsigned long *table, long index);
extern "C" long menuPocketToolCMetaButton(tagWINDOW_WORK *);
extern "C" long menuPocketToolCMetaDraw(tagWINDOW_WORK *);
extern "C" long menuPocketToolCMetaCtrl(tagWINDOW_WORK *);
void menuPtMenuLoad(long menuID);
tagSPRITE_WORK *menuPtSpriteWorkGetByItemID(long menuID, unsigned long itemID, long offset);
void menuPtSpriteWorkSetMsgId(long menuID, unsigned long itemID, long offset, unsigned long message);
tagMENU_ITEM_DATA *menuPtItemGetByMenuIdAndItemId(long menuID, unsigned long itemID, long offset);
void menuPtWindowWorkDraw(long menuID);
void menuPtWindowWorkDraw(tagWINDOW_WORK *window);
void menuPtWindowWorkDrawPrime(tagWINDOW_WORK *window);
void menuPtSpriteDrawCenterColor(unsigned short spriteID, long x, long y, unsigned long color);
void menuPtSpriteDrawColor(unsigned short spriteID, long x, long y, long width, long height, unsigned long color);
void menuPtSpriteDraw(unsigned short spriteID, long x, long y);
void menuPtItemDrawOffsetXWColor(MENU_ITEM_dd_ *item, long x, long width, unsigned long color);
void menuPtItemDrawOffset(MENU_ITEM_dd_ *item, long x, long y);
void menuPtItemDrawAlpha(MENU_ITEM_dd_ *item, unsigned char alpha);
unsigned long GSgfxColAu8To32(GSgfxColAu8 color);
void menuPtSpriteWorkSetWidthRateColor(tagWINDOW_WORK *window, unsigned long itemID, long offset, float rate, GSgfxColAu8 color);
void menuPtSpriteWorkSetSw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, bool visible);
void menuPtSpriteWorkSetSw(tagWINDOW_WORK *window, unsigned long itemID, long offset, bool visible);
void menuPtSpriteWorkSetSpriteId(tagSPRITE_WORK *sprite, unsigned long spriteID);
void menuPtSpriteWorkSetSpriteData(tagSPRITE_WORK *sprite, MenuSpriteData *data);
void menuPtSpriteWorkSetMenuItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long itemID);
void menuPtStopSound(MenuPtWorkSound *sound);
void menuPocketButtonWait();
float menuGetPokemonScale(unsigned long modelID, int pokemonID);
void menuPocketDebugMakeDarkPokemonByID(Pokemon &pokemon, unsigned short darkID, float rate);
void menuPocketDebugMakePokemonByID(Pokemon &pokemon, unsigned short species, bool dark, float rate);
void makePocketDeubugMakeDarkPokemonReset();
void menuPocketDebugMakePokemonBySN(Pokemon &pokemon, unsigned short serial, bool dark, float rate);
float xyxy2distance(const PocketVec<short> &point, const PocketVec<short> &center);
void menuPtSpriteDrawAndRollScale(unsigned short spriteID, unsigned long rgba, float x, float y, float roll, float scale);
void _tutorialWindowCursorFunc(tagWINDOW_WORK *window);
void _tutorialWindowButtonFunc(tagWINDOW_WORK *window);

#endif
