#ifndef CANT_H
#define CANT_H

#include <Pokemon.h>
#include <PocketVec.h>
#include <types.h>

// CAnt -- model unit drawn by the pocket menus.
class CAnt {
public:
    void init();
    void setAnimation(long animation);
    void drawAndUpdate(PocketVec<short> position);
    void drawAndUpdate();

private:
    u32 unk_00;
    u32 unk_04;
    u32 unk_08;

public:
    f32 mScale; // 0x0C: scale of the drawn image.

private:
    u8 unk_10;
    u32 unk_14;
    s16 unk_18;
    s16 unk_1A;
    f32 unk_1C;
    u32 unk_20;
    u32 unk_24;
    u32 unk_28;
    f32 unk_2C;
    u32 unk_30;
};

#ifdef __cplusplus
extern "C" {
#endif

// Fill the model state in `ant` from the pokemon record in `pokemon`.
void battlePokemonGetPokeBody(CAnt *ant, Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
