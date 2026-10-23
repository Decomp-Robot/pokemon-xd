#ifndef HSD_SCENE_H
#define HSD_SCENE_H

struct _HSD_Joint;
struct _HSD_AnimJoint;
struct _HSD_MatAnimJoint;
struct _HSD_ShapeAnimJoint;

struct _HSD_SceneModelSet {
    _HSD_Joint *mJoint;
    _HSD_AnimJoint **mAnimations;
    _HSD_MatAnimJoint **mTextureAnimations;
    _HSD_ShapeAnimJoint **mShapeAnimations;
};

#endif
