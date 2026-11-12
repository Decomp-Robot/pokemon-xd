#ifndef GS_PART_H
#define GS_PART_H

#include <types.h>
#include <GSvec.h>

struct _HSD_JObj;
struct _HSD_AnimJoint;
struct _HSD_MatAnimJoint;
struct _HSD_ShapeAnimJoint;
struct _GSmodel;
struct GSmaterial;

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
void GSpartShapeAnimWalkTree(_HSD_ShapeAnimJoint *joint, void (*callback)(_HSD_ShapeAnimJoint *, void **, int), void **arg);
void GSpartMatAnimWalkTree(_HSD_MatAnimJoint *joint, void (*callback)(_HSD_MatAnimJoint *, void **, int), void **arg);
void GSpartAnimWalkTree(_HSD_AnimJoint *joint, void (*callback)(_HSD_AnimJoint *, void **, int), void **arg);
_HSD_JObj *GSpartGetJObjPtr(_HSD_JObj *joint, u32 index);
u32 GSpartGetJObjIndex(_HSD_JObj *root, _HSD_JObj *joint);
GSpart *GSmodelGetPart(_GSmodel *model, s32 partIndex);
_HSD_ShapeAnimJoint *GSpartGetShapeAnimPtr(_HSD_ShapeAnimJoint *shapeAnimation, u32 partIndex);
_HSD_MatAnimJoint *GSpartGetMatAnimPtr(_HSD_MatAnimJoint *materialAnimation, u32 partIndex);
_HSD_AnimJoint *GSpartGetAnimPtr(_HSD_AnimJoint *jointAnimation, u32 partIndex);
s32 GSpartSetTexAnimIndex(GSpart *part, s32 materialIndex, s32 animIndex,
                         s32 animType, f32 frame);
void GSpartStopAnimIndex(GSpart *part, s32 animIndex);
s32 GSpartSetAnimIndex(GSpart *part, s32 animIndex, s32 animType, f32 frame);
void GSpartFree(GSpart *part);
void *GSpartGetMaterial(GSpart *part, s32 materialIndex);
void GSpartGetMatrix(GSpart *part, GSmtx *out);
/* Null outputs leave the corresponding transform component alone. */
void GSpartGetTransform(GSpart *part, GSvec *position, GSvec *rotation, GSvec *scale);
GSpart *GSpartCreate(void);
void GSpartInit(s32 count);
void GSpartRegisterAnimMix(GSpart *part, GSpartMixType type, u32 flags,
                          f32 (*callback)(GSpart *, GSpartMixType, void *, GSvec *),
                          void *callbackData);
void GSpartUnregisterAnimMix(GSpart *part, GSpartMixType type);
void GSpartInitMaterial(GSmaterial *material, GSpart *part, s32 index);
s32 GSpartGetMaterialCount(GSpart *part);
}

#endif
