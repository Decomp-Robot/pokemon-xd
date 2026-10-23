#ifndef HSD_SCENE_INTERFACE_H
#define HSD_SCENE_INTERFACE_H
#include <types.h>

struct _HSD_JObj;
typedef _HSD_JObj HSD_JObj;
struct HSD_Joint;
struct HSD_AnimJoint;
struct HSD_MatAnimJoint;
struct HSD_ShapeAnimJoint;
struct _HSD_AObj;
typedef _HSD_AObj HSD_AObj;

struct HSD_SceneModel {
    HSD_Joint *mJoint;
    HSD_AnimJoint **mJointAnimations;
    HSD_MatAnimJoint **mMaterialAnimations;
    HSD_ShapeAnimJoint **mShapeAnimations;
};

struct HSD_SceneDesc {
    HSD_SceneModel **mModels;
};

enum HSD_AnimationTraversal {
    HSD_ANIMATION_JOINT = 6,
    HSD_ANIMATION_ALL = 0xFFFF,
    HSD_ANIMATION_FLOAT_ARGUMENT = 1,
    HSD_ANIMATION_POINTER_ARGUMENT = 2,
    HSD_ANIMATION_FLAGS_ARGUMENT = 3,
    HSD_ANIMATION_LOOP = 0x20000000,
};

extern "C" {
HSD_JObj *HSD_JObjLoadJoint(HSD_Joint *joint);
void HSD_JObjUnref(HSD_JObj *joint);
void HSD_JObjAddAnimAll(HSD_JObj *joint, HSD_AnimJoint *animation,
                       HSD_MatAnimJoint *materialAnimation,
                       HSD_ShapeAnimJoint *shapeAnimation);
void HSD_JObjReqAnimAll(HSD_JObj *joint, f32 frame);
void HSD_JObjAnimAll(HSD_JObj *joint);
void HSD_AObjSetFlags(HSD_AObj *animation, u32 flags);
void HSD_AObjClearFlags(HSD_AObj *animation, u32 flags);
void HSD_AObjSetRate(HSD_AObj *animation, f32 rate);
void HSD_ForeachAnim(void *object, s32 objectType, u32 animationMask,
                     void (*callback)(), s32 argumentType, ...);
void _modelGetEndFrame(HSD_AObj *animation, f32 *endFrame);
}
#endif
