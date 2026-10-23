#ifndef GS_PART_H
#define GS_PART_H

#include <types.h>
#include <GSvec.h>

struct _HSD_JObj;
struct _GSmodel;

/* Part-animation slot mixes with the other joints in the set. */
#define GS_PART_ANIM_MIXED 1u

enum GSpartMixType {
    GS_MODEL_PART_MIX_NONE,
    GS_MODEL_PART_MIX_TRANSLATION,
    GS_MODEL_PART_MIX_ROTATION,
    GS_MODEL_PART_MIX_SCALE,
    GS_MODEL_PART_MIX_COUNT = 16
};

/* Handle returned by the model's part lookup. */
struct GSpart {
    u8 mInUse;
    u8 _01;
    u16 mIndex;
    _GSmodel *mModel;
    _HSD_JObj *mJoint;
};

extern "C" {
GSpart *GSmodelGetPart(_GSmodel *model, s32 partIndex);
s32 GSpartSetTexAnimIndex(GSpart *part, s32 materialIndex, s32 animIndex,
                         s32 animType, f32 frame);
void GSpartStopAnimIndex(GSpart *part, s32 animIndex);
s32 GSpartSetAnimIndex(GSpart *part, s32 animIndex, s32 animType, f32 frame);
void GSpartFree(GSpart *part);
void *GSpartGetMaterial(GSpart *part, s32 materialIndex);
void GSpartGetMatrix(GSpart *part, GSmtx *out);
/* Null outputs leave the corresponding transform component alone. */
void GSpartGetTransform(GSpart *part, GSvec *position, GSvec *rotation, GSvec *scale);
}

#endif
