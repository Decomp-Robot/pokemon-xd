#ifndef MENU_MODEL_H
#define MENU_MODEL_H

#include <GSmathTypes.h>
#include <types.h>
#include <GSvec.h>
#include <GStexture.h>

struct Pokemon;

struct MENU_MODEL {
    MENU_MODEL() {}

    u8 _00[0x24];
    u16 mSpecies;
    u8 _26[0x1A];
    GSvec mAngle;
    GSvec mOffset;
    u8 _58[0x30];
    GSvec mCameraRotation;
    u8 _94[0x58];
};


extern "C" {
void menuModelInit(MENU_MODEL *model, int modelKind, int cameraKind);
void menuModelFree(MENU_MODEL *model);
s32 menuModelSetSize(MENU_MODEL *model, s32 x, s32 y, s32 width, s32 height);
void menuModelSetAngle(MENU_MODEL *model, const f32 *angle);
void menuModelAdjustCamera(MENU_MODEL *model);
u8 menuModelCheck(MENU_MODEL *model, u8 wait);
GStexture *menuModelRender(MENU_MODEL *model);
u8 menuModelSetupPokemon(MENU_MODEL *model, Pokemon *pokemon);
void menuModelSetMotion(MENU_MODEL *model, int motion);
void menuModelSetFlag(MENU_MODEL *model, s32 flag);
void menuModelSetZoom(MENU_MODEL *model, f32 zoom);
void menuModelSetOffset(MENU_MODEL *model, const f32 *offset);
void menuModelLoad(MENU_MODEL *model, u32 resourceID);
void menuModelLoadPokemon(MENU_MODEL *model, Pokemon *pokemon);
void menuModelTerm(MENU_MODEL *model);
}

#endif
