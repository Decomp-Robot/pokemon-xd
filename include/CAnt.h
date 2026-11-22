#ifndef CANT_H
#define CANT_H

#include <Pokemon.h>
#include <PocketVec.h>
#include <types.h>

struct AntData;

// Use state of a registry slot, stored in CAntCacheEntry::useCount.
enum AntUseState {
    ANT_USE_FREE,
    ANT_USE_ACTIVE,
    ANT_USE_PENDING_READ
};

// Colour animation applied to a sprite, stored in CAnt::mColorMode.
enum AntColorMode {
    ANT_COLOR_NONE,
    ANT_COLOR_DARK,
    ANT_COLOR_RELIVE_READY
};

// One slot of the animation resource registry shared by every CAnt instance.
// resourceID is the GS resource key, animData is the loaded animation blob and
// useCount holds an AntUseState: free, in use or read still in flight.
struct CAntCacheEntry {
    u32 resourceID;
    AntData* animData;
    AntUseState useCount;
};

// Sprite-animation state used by party and link-battle panels.
class CAnt {
public:
    static void systemInit();
    static void systemFree();
    static void systemUsecountReset();
    static void systemUnUseResourceRelease();
    static void systemUpdate(bool update);
    static void waitEndOfReading();
    static void allRequestCancel();

    void init();
    void initByPokemon(Pokemon* pokemon);
    void initByPokemonAndSetColorAnimationInAny(Pokemon* pokemon);
    void initByPokemonAndSetColorAnimationInReliveHallVisitor(Pokemon* pokemon);
    void setAnimation(s32 animation);
    void use();
    void draw(PocketVec<short> offset);
    void drawAndUpdate();
    void drawAndUpdate(PocketVec<short> offset);
    void drawAndUpdateWithOffset(PocketVec<short> offset);
    void setPosition(PocketVec<short> position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
    }

    // Phase clocks shared by all sprites for the dark and Purify Chamber
    // colour animations.
    static f32 colorAnimationTimeDark;
    static f32 colorAnimationTimeReliveReady;

    u32 mResource;
    AntData* mCachedAnimData;
    PocketVec<short> mPosition;
    f32 mScale;
    u8 mAlpha;
    s32 mHideFlag;
    s16 mAnimIndex;
    s16 mCommandCursor;
    f32 mTimerSeconds;
    s32 mOffsetX;
    s32 mOffsetY;
    s32 mFrameSet;
    f32 mWaitSeconds;
    AntColorMode mColorMode;

private:
    AntData* resolveCacheEntry();
    AntData* getAnimData();
    void readCommand(AntData* animData);
    void nextCommand();
    void update();
    void initByPokemonAndSetColorAnimation(Pokemon* pokemon);
};

#endif
