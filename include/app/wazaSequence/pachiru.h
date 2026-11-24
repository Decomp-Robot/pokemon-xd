#ifndef WAZA_SEQUENCE_PACHIRU_H
#define WAZA_SEQUENCE_PACHIRU_H

#include <types.h>

struct _GSmodel;
struct GSmaterial;
struct GStexture;

struct PachiruEffect {
    GSmaterial* mMaterial;
    GStexture* mTexture;
};

extern "C" {
void pachiruEffectFree(PachiruEffect* effect);
u8 pachiruEffectCreate(PachiruEffect* effect, _GSmodel* model,
                       u32 personality);
}

#endif
