#ifndef MENU_POCKET_TOOL_H
#define MENU_POCKET_TOOL_H

#include <PocketVec.h>
#include <types.h>
#include <GSgfxLight.h>
#include <menuItemBios.h>
#include <menuPtSpriteWork.h>
#include <menuSpriteBios.h>
#include <menuPokemonCursor.h>
#include <menuInfoWindow.h>

class CMetaMenu;
extern const GSgfxColAu8 GSgfxColAu8_Black;

class CMetaMenuStorage {
protected:
    enum { LOCAL_WINDOW_COUNT = 64 };
    enum AlphaFadeState {
        ALPHA_FADE_VISIBLE = 0,
        ALPHA_FADE_OUT = 1,
        ALPHA_FADE_BLACK = 2,
        ALPHA_FADE_IN = 3
    };
    long mAlphaFadeFrames;
    AlphaFadeState mAlphaFadeState;
    u8 mSoundConfiguration[4];
    unsigned long mAcceptSound;
    u8 mFadeConfiguration[0x0c];
    long mLocalWindowIds[LOCAL_WINDOW_COUNT];
    void (*mLocalWindowCallbacks[LOCAL_WINDOW_COUNT])(CMetaMenu*, long);
    long* mLocalWindowTable;
    tagWINDOW_KEY mKeyInfo;
    long mKeyRepeatFrames;
    u16 mPreviousKeyOn;
    bool mTutorialActive;
    u8 mInputAndStateStorage[0xdd];
    u64 mAnimationFrameTime;
    u8 mAdditionalState[8];
};

class CMetaMenu : public CMetaMenuStorage {
public:
    static CMetaMenu* pCurrentMenu;
    static long defautlLocalWindowOpenTable[1];
    virtual void drawAll() = 0;
    virtual int isAnimationStateNone();
    virtual void updateAll();
    virtual void tutorialExitKick();
    void waitAtTime(float seconds);
    long calcAnimationTimeRate0toX(float seconds, long maximum);
    float calcAnimationTimeRate0to1to0(float seconds);
    float calcAnimationTimeRate0to1(float seconds);
    void drawHook();
    void tutorialManageUpdate();
    void vsyncUpdate();
    void systemFadeWaitToEnd();
    void systemFadeOutSetAndWait(float seconds);
    void systemFadeOutKick(float seconds);
    void systemFadeInKick();
    void systemFadeOutKeepAfterFloorPush();
    void alphaFadeDraw();
    u8 getAlphaFadeAlpha() const;
    void alphaFadeOutAndWait();
    void alphaFadeUpdate();
    void alphaFadeOutKick();
    void alphaFadeInKick();
    bool isAlphaFadeEnd() const {
        return mAlphaFadeState == ALPHA_FADE_VISIBLE || mAlphaFadeState == ALPHA_FADE_BLACK;
    }
    tagWINDOW_KEY* getKeyInfo();
    void aButtonWait();
    long actionMenu(unsigned long* choices, int quantity, long x, long y, long initialChoice);
    u16 tutorialManage();
    void localWindowRemove(long windowID);
    void localWindowDrawAll(long first, long last);
    void localWindowDrawAll();
    void localWindowEntry(long windowID, long index, void (*callback)(CMetaMenu*, long));
    long localWindowSearch(long windowID);
    void localWindowDeleteAll();
    void localWindowCreateAll();
    void localWindowSetTable(long* windowTable);
};

class CState {
public:
    enum { STATE_NONE = -1 };
    void update();
    void addTime();
    void init();

    long mState;
    long mNextState;
    long mPreviousState;
    long mFrameCount;
    float mElapsedTime;
    float mFrameTime;
    long mParameters[16];
    long mPreviousParameters[16];
    unsigned long mStateFlags[4];
};

class CMenuGlobalState {
public:
    void init();

    u8 mMenuContext[0x0c];
    CMenuPokemonCursor mPokemonCursor;
    u8 mCursorSelectionContext[4];
    CState mState;
};

struct MenuPtWorkSound {
    unsigned long mBgmID;
    unsigned long mEnvID;
    u8 mBgmVolume;
    u8 mEnvVolume;
};

enum PocketMenuKey {
    POCKET_MENU_KEY_UP = 0x1,
    POCKET_MENU_KEY_DOWN = 0x2,
    POCKET_MENU_KEY_A = 0x10,
    POCKET_MENU_KEY_B = 0x20
};

enum PocketMenuInputTiming {
    POCKET_MENU_KEY_REPEAT_DELAY_FRAMES = 12
};

enum PocketMenuWindowID {
    POCKET_MENU_WINDOW_ACTION = 0x59,
    POCKET_MENU_WINDOW_INPUT = 0xda,
    POCKET_MENU_WINDOW_TUTORIAL_TALK = 0xe3
};

enum PocketMenuDrawing {
    POCKET_MENU_WORLD_LAYER = 2,
    POCKET_MENU_SPRITE_LAYER = 7,
    POCKET_MENU_WINDOW_OPEN_LOAD = 4,
    POCKET_MENU_BGM_FADE_FRAMES = 50,
    POCKET_MENU_VOLUME_MAX = 255,
    POCKET_MENU_BLEND_SOURCE_ALPHA = 4,
    POCKET_MENU_BLEND_INVERSE_SOURCE_ALPHA = 5
};

static const unsigned long POCKET_MENU_SPRITE_COLOR_WHITE = 0xffffffff;
static const unsigned long POCKET_MENU_SPRITE_COLOR_RGB_MASK = 0xffffff00;

void _tutorialWindowCursorFunc(tagWINDOW_WORK* window);
void _tutorialWindowButtonFunc(tagWINDOW_WORK* window);
void menuPtRestoreSound(MenuPtWorkSound* sound);
void menuPtSaveSound(MenuPtWorkSound* sound);
void menuPtStopSound(MenuPtWorkSound* sound);
void menuPocketButtonWait();
float menuGetPokemonScale(unsigned long sequence, int species);
u16 menuPocketDebugMakeDarkPokemonByID(Pokemon& pokemon, u16 shadowID, float darkRate);
void menuPocketDebugMakePokemonByID(Pokemon& pokemon, u16 species, bool dark, float darkRate);
void makePocketDeubugMakeDarkPokemonReset();
void menuPocketDebugMakePokemonBySN(Pokemon& pokemon, u16 serial, bool dark, float darkRate);
void menuPocketSetDarkPokemon(Pokemon& pokemon, float darkRate, u16 shadowID);
u16 pokemonindex(int index);

void menuPtPlayBgm(unsigned long soundID);
void menuPtPlaySe(unsigned long soundID);
void menuPtWindowWorkDraw(long windowID);
void menuPtWindowWorkDraw(tagWINDOW_WORK* window);
void menuPtWindowWorkDrawPrime(tagWINDOW_WORK* window);
void menuPtMenuLoad(long menuID);
void menuPtSpriteDrawAndRollScale(u16 spriteID, unsigned long color,
                                  float x, float y, float angle, float scale);
void menuPtSpriteDrawCenterColor(u16 spriteID, long x, long y, unsigned long color);
void menuPtSpriteDrawColor(u16 spriteID, long x, long y, long width, long height, unsigned long color);
void menuPtSpriteDrawColor(u16 spriteID, long x, long y, unsigned long color);
void menuPtSpriteDraw(u16 spriteID, long x, long y);
void menuPtItemDrawOffsetXWColor(MENU_ITEM_dd_* item, long x, long width, unsigned long color);
void menuPtItemDrawOffset(MENU_ITEM_dd_* item, long x, long y);
void menuPtItemDrawAlpha(MENU_ITEM_dd_* item, u8 alpha);
unsigned long GSgfxColAu8To32(GSgfxColAu8 color);
void menuPocketToolFloatToZero(float& value, float step);

extern "C" {
long menuPocketToolCMetaButton(tagWINDOW_WORK* window);
long menuPocketToolCMetaDraw(tagWINDOW_WORK* window);
long menuPocketToolCMetaCtrl(tagWINDOW_WORK* window);
}

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

#endif
