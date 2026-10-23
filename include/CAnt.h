#ifndef CANT_H
#define CANT_H

#include <Pokemon.h>
#include <types.h>
#include <PocketVec.h>

// Sprite-animation state used by party and link-battle panels.
class CAnt {
public:
    void init();
    void setPos(PocketVec<s16> position) { mPosition.x = position.x; mPosition.y = position.y; }
    void setAnimation(long animation);
    void use();
    void drawAndUpdate(PocketVec<short> position);
    void drawAndUpdate();
    static void systemUsecountReset();
    static void systemUnUseResourceRelease();

    u32 mBodyResourceID;
    void *mResource;
    PocketVec<s16> mPosition;
    f32 mScale;
    u8 mAlpha;
    s32 mDrawState;
    s16 mAnimationId;
    s16 mFrame;
    f32 mFrameTime;
    s32 mFrameOffsetX;
    s32 mFrameOffsetY;
    s32 mFrameIndex;
    f32 mFrameDuration;
    s32 mDrawMode;
};

extern "C" void battlePokemonGetPokeBody(CAnt *ant, Pokemon *pokemon);

#endif
