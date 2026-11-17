#ifndef MENU_MODEL_H
#define MENU_MODEL_H

#include <GSmathTypes.h>
#include <types.h>
#include <GSvec.h>
#include <GStexture.h>

struct Pokemon;
struct _GSmodel;
struct _GScamera;
struct GSlight;
struct GSthread;
class WazaSequence;
struct NullSequence;

struct MENU_MODEL_LOAD {
    u8 isPokemon;
    union {
        u32 objectID;
        struct {
            u16 pkxID;
            u16 wazaID;
        } pokemon;
    } id;
    union {
        u32 pokemonRandom;
        u32 motionIndex;
    };
    u16 loadPad;
    bool isRare;
    u8 animIndex;
};

union MENU_MODEL_OBJECT {
    NullSequence *sequence;
    _GSmodel *model;
};

struct MENU_MODEL {
    u8 state;
    u8 loadRequest;
    u8 initialized;
    u8 renderEnable;
    u8 clip;
    u8 reserved[7];
    f32 unknownFloat;
    MENU_MODEL_LOAD next;
    MENU_MODEL_LOAD current;
    MENU_MODEL_OBJECT object;
    WazaSequence *waza;
    MENU_MODEL_OBJECT nextObject;
    WazaSequence *nextWaza;
    GSvec mAngle;
    GSvec mOffset;
    GSthread *thread;
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    _GScamera *camera;
    GSlight *light[3];
    f32 zoom;
    s32 visibilityDelay;
    u8 unknownFlag;
    GSvec mCameraRotation;
    f32 viewMatrix[3][4];
    u8 boundsValid;
    u8 boundsPadding[3];
    GSvec boundsSize;
    GSvec boundsMin;
    GSvec boundsMax;
};

extern "C" {
void menuModelReSetScale(void);
void menuModelSetScale(f32 scale);
f32 menuModelGetScale(u8 inputSelector, u16 inputID, f32 scale);
u8 menuModelGetPokemonAnimationIndexByID(u16 species);
u8 menuModelGetPokemonAnimationIndexByPointer(Pokemon *pokemon);
void menuModelSetCameraRotation(MENU_MODEL *model, f32 *rotation);
f32 *menuModelGetCameraRotation(MENU_MODEL *model);
_GSmodel *menuModelGetModelPtr(MENU_MODEL *model);
void menuModelSetClip(MENU_MODEL *model, u8 clip);
void menuModelSetZoom(MENU_MODEL *model, f32 zoom);
void menuModelSetOffset(MENU_MODEL *model, f32 *offset);
void menuModelSetAngle(MENU_MODEL *model, f32 *angle);
void menuModelAdjustCamera2(MENU_MODEL *model, MENU_MODEL *other);
void menuModelAdjustCamera(MENU_MODEL *model);
s32 menuModelSetMotion(MENU_MODEL *model, s32 motion);
s32 menuModelRender2(MENU_MODEL *model, MENU_MODEL *other);
GStexture *menuModelRender(MENU_MODEL *model);
void menuModelRenderOffAll(MENU_MODEL *model);
bool menuModelCheck(MENU_MODEL *model, u8 wait);
s32 menuModelFreeNext(MENU_MODEL *model);
void menuModelWazaFree(MENU_MODEL *model);
s32 menuModelFree(MENU_MODEL *model);
s32 menuModelLoadPokemon(MENU_MODEL *model, Pokemon *pokemon);
s32 menuModelLoad(MENU_MODEL *model, Pokemon *pokemon);
s32 menuModelLoadObject(MENU_MODEL *model, u32 resourceID);
bool menuModelComparePokemon(MENU_MODEL *model, Pokemon *pokemon);
bool menuModelCompareObject(MENU_MODEL *model, u32 resourceID);
void menuModelTerm(MENU_MODEL *model);
s32 menuModelInit(MENU_MODEL *model, s32 width, s32 height);
s32 menuModelSetSize(MENU_MODEL *model, s32 x, s32 y, s32 width, s32 height);
}

void menuModelGetPkxRndFromPokemonForSeqenceLoad(u32 &random, u16 &pkxID, Pokemon *pokemon);

#endif
