#ifndef CANT_H
#define CANT_H

#include <Pokemon.h>
#include <PocketVec.h>
#include <types.h>
#include <PocketVec.h>

// Sprite-animation state used by party and link-battle panels.
class CAnt {
public:
    static void systemInit();
    static void systemFree();
    void init();
    void initByPokemonAndSetColorAnimationInAny(Pokemon *pokemon);
    void setAnimation(long animation);
    static void systemUsecountReset();
    static void systemUnUseResourceRelease();
    void use();
    void drawAndUpdate();
    void drawAndUpdate(PocketVec<short>);
    void setPosition(PocketVec<short> position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
    }
    void *mResource;

private:
    u32 reserved04;
    PocketVec<short> mPosition;
public:
    f32 mScale;
    u8 mAlpha;
private:
    u32 reserved14;
    s16 reserved18;
    s16 reserved1A;
    f32 reserved1C;
    u32 reserved20;
    u32 reserved24;
    u32 reserved28;
    f32 reserved2C;
    u32 reserved30;
};

#endif
