#include <GSmodelInternal.h>
#include <hsd/jobj.h>
#include <GSmem.h>
#include <GSlog.h>
#include <GSmtx.h>
#include <GSquat.h>

GSpart *_partStructList;
s32 _partStructCount;
u8 _partSearchDone;
u32 _partFindCurIdx;
u32 _partToFindIdx;
s32 _partFoundIndex;
_HSD_JObj *_partToFindJObj;
_HSD_JObj *_partFoundJObj;
_HSD_AnimJoint *_partFoundAnim;
_HSD_MatAnimJoint *_partFoundMatAnim;
_HSD_ShapeAnimJoint *_partFoundShapeAnim;

static inline _HSD_JObj *partGetParent(_HSD_JObj *joint)
{
    if (joint == NULL) {
        return NULL;
    }
    return joint->mParent;
}


typedef void (*PartShapeAnimCallback)(_HSD_ShapeAnimJoint *, void **, int);
typedef void (*PartMatAnimCallback)(_HSD_MatAnimJoint *, void **, int);
typedef void (*PartAnimCallback)(_HSD_AnimJoint *, void **, int);

void _partShapeAnimWalkTree0(_HSD_ShapeAnimJoint *, PartShapeAnimCallback, void **);
void _partFindShapeAnimCB(_HSD_ShapeAnimJoint *, void **, int);
void _partMatAnimWalkTree0(_HSD_MatAnimJoint *, PartMatAnimCallback, void **);
void _partFindMatAnimCB(_HSD_MatAnimJoint *, void **, int);
void _partAnimWalkTree0(_HSD_AnimJoint *, PartAnimCallback, void **);
void _partFindAnimCB(_HSD_AnimJoint *, void **, int);
void _partFindJObjCB(_HSD_JObj *, void **, int);
void _partFindIndexCB(_HSD_JObj *, void **, int);

/* Particle and spline joints draw from a primitive list, not from a drawable chain. */
static inline BOOL jobjHasDrawable(_HSD_JObj *jobj)
{
    return (jobj->flags & (HSD_JOBJ_FLAG_PARTICLE | HSD_JOBJ_FLAG_SPLINE)) ? FALSE : TRUE;
}

static inline _HSD_DObj *jobjGetDrawable(_HSD_JObj *jobj)
{
    return jobjHasDrawable(jobj) ? jobj->mDrawable : NULL;
}

/* Returns the material of the index-th drawable of a chain. */
static inline _HSD_MObj *dobjGetMaterial(_HSD_DObj *dobj, u32 index)
{
    u32 i = 0;

    while (dobj != NULL) {
        if (i++ == index) {
            return dobj->mMaterial;
        }
        dobj = dobj->mNext;
    }
    return NULL;
}

/* Counts the drawables of a chain, reporting -1 instead of wrapping around. */
static inline void dobjGetCount(_HSD_DObj *dobj, u32 *count)
{
    u32 n = 0;

    while (dobj != NULL) {
        if (n++ == 0xFFFFFFFF) {
            if (count != NULL) {
                *count = 0xFFFFFFFF;
            }
            return;
        }
        dobj = dobj->mNext;
    }
    if (count != NULL) {
        *count = n;
    }
}

static inline GSmaterial *modelGetMaterials(_GSmodel *model)
{
    return (model != NULL) ? model->mMaterials : NULL;
}

static inline u32 modelGetMaterialCount(_GSmodel *model)
{
    return (model != NULL) ? model->mMaterialCount : 0;
}

/* Looks an HSD material up in a model's material table. */
static inline GSmaterial *materialFind(GSmaterial *materials, u32 count, _HSD_MObj *hsdMaterial)
{
    u32 i;

    if (materials == NULL || count == 0) {
        return NULL;
    }
    for (i = 0; i < count; i++, materials++) {
        if (materials->mMaterial == hsdMaterial) {
            return materials;
        }
    }
    return NULL;
}

/* Resolves the material table entry of the index-th drawable of a part. */
static inline GSmaterial *partGetMaterial(GSpart *part, u32 materialIndex)
{
    _HSD_JObj *joint = part->mJoint;
    _HSD_DObj *dobj;
    _HSD_MObj *hsdMaterial;

    dobj = jobjGetDrawable(joint);
    if (dobj == NULL) {
        return NULL;
    }
    hsdMaterial = dobjGetMaterial(dobj, materialIndex);
    if (hsdMaterial == NULL) {
        return NULL;
    }
    return materialFind(modelGetMaterials(part->mModel),
                        modelGetMaterialCount(part->mModel), hsdMaterial);
}

/* Returns a free mix slot for the part, or NULL when the part is already mixed. */
static inline GSmodelPartMix *partRegisterMix(GSpart *part, GSpartMixType type)
{
    _GSmodel *model = part->mModel;
    GSmodelPartMix *mix;
    s32 i;

    for (i = 0; i < GS_MODEL_PART_MIX_COUNT; i++) {
        mix = &model->mPartMix[i];
        if (mix->mType != GS_MODEL_PART_MIX_NONE && mix->mType == type &&
            (u32)mix->mPartIndex == part->mIndex) {
            return NULL;
        }
    }
    for (i = 0; i < GS_MODEL_PART_MIX_COUNT; i++) {
        mix = &model->mPartMix[i];
        if (mix->mType == GS_MODEL_PART_MIX_NONE) {
            return mix;
        }
    }
    return NULL;
}

/* Returns the first free slot of the part pool. */
static inline GSpart *partFindFree(void)
{
    GSpart *part = _partStructList;
    u32 i;

    for (i = 0; i < _partStructCount; i++) {
        if (part->mInUse == 0) {
            return part;
        }
        part++;
    }
    return NULL;
}

void _partFindIndexCB(_HSD_JObj *joint, void **arg, int index)
{
    if (_partSearchDone == 1) {
        return;
    }
    if (_partToFindJObj == joint) {
        _partFoundIndex = _partFindCurIdx;
        _partSearchDone = 1;
    }
    _partFindCurIdx++;
}

void _partFindJObjCB(_HSD_JObj *jobj, void **arg, int index)
{
    if (_partSearchDone == 1) {
        return;
    }
    if (_partFindCurIdx == _partToFindIdx) {
        _partFoundJObj = jobj;
        _partSearchDone = 1;
    }
    _partFindCurIdx++;
}

void _partFindAnimCB(_HSD_AnimJoint *joint, void **arg, int index)
{
    if (_partSearchDone == 1) {
        return;
    }
    if (_partFindCurIdx == _partToFindIdx) {
        _partFoundAnim = joint;
        _partSearchDone = 1;
    }
    _partFindCurIdx++;
}

void _partFindMatAnimCB(_HSD_MatAnimJoint *joint, void **arg, int index)
{
    if (_partSearchDone == 1) {
        return;
    }
    if (_partFindCurIdx == _partToFindIdx) {
        _partFoundMatAnim = joint;
        _partSearchDone = 1;
    }
    _partFindCurIdx++;
}

void _partFindShapeAnimCB(_HSD_ShapeAnimJoint *joint, void **arg, int index)
{
    if (_partSearchDone == 1) {
        return;
    }
    if (_partFindCurIdx == _partToFindIdx) {
        _partFoundShapeAnim = joint;
        _partSearchDone = 1;
    }
    _partFindCurIdx++;
}

void _partAnimWalkTree0(_HSD_AnimJoint *joint, PartAnimCallback callback, void **arg)
{
    _HSD_AnimJoint *child;

    if (joint == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(joint, arg, 1);
    }
    for (child = joint->mChild; child != NULL; child = child->mNext) {
        _partAnimWalkTree0(child, callback, arg);
    }
}

void _partMatAnimWalkTree0(_HSD_MatAnimJoint *joint, PartMatAnimCallback callback, void **arg)
{
    _HSD_MatAnimJoint *child;

    if (joint == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(joint, arg, 1);
    }
    for (child = joint->mChild; child != NULL; child = child->mNext) {
        _partMatAnimWalkTree0(child, callback, arg);
    }
}

void _partShapeAnimWalkTree0(_HSD_ShapeAnimJoint *joint, PartShapeAnimCallback callback, void **arg)
{
    _HSD_ShapeAnimJoint *child;

    if (joint == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(joint, arg, 1);
    }
    for (child = joint->mChild; child != NULL; child = child->mNext) {
        _partShapeAnimWalkTree0(child, callback, arg);
    }
}

void GSpartInit(s32 count)
{
    u32 i;

    _partStructCount = count;
    _partStructList = (GSpart *)GSmemAlloc(count * sizeof(GSpart));
    if (_partStructList == NULL) {
        return;
    }
    for (i = 0; i < _partStructCount; i++) {
        _partStructList[i].mInUse = 0;
    }
}

GSpart *GSpartCreate(void)
{
    GSpart *part = partFindFree();

    if (part == NULL) {
        return NULL;
    }
    part->mInUse = 1;
    return part;
}

void GSpartFree(GSpart *part)
{
    part->mInUse = 0;
}

s32 GSpartGetMaterialCount(GSpart *part)
{
    _HSD_DObj *dobj;
    u32 count;

    dobj = jobjGetDrawable(part->mJoint);
    if (dobj == NULL) {
        return 0;
    }
    dobjGetCount(dobj, &count);
    return count;
}

void *GSpartGetMaterial(GSpart *part, s32 materialIndex)
{
    _HSD_DObj *dobj;
    _HSD_MObj *hsdMaterial;
    _GSmodel *model;
    GSmaterial *materials;
    u32 count;
    u32 i;

    dobj = jobjGetDrawable(part->mJoint);
    if (dobj == NULL) {
        return NULL;
    }
    hsdMaterial = dobjGetMaterial(dobj, materialIndex);
    if (hsdMaterial == NULL) {
        return NULL;
    }
    model = part->mModel;
    materials = modelGetMaterials(model);
    count = modelGetMaterialCount(model);
    if (materials == NULL || count == 0) {
        return NULL;
    }
    for (i = 0; i < count; i++, materials++) {
        if (materials->mMaterial == hsdMaterial) {
            return materials;
        }
    }
    return NULL;
}

void GSpartInitMaterial(GSmaterial *material, GSpart *part, s32 index)
{
    _HSD_DObj *dobj;

    material->mMaterial = NULL;
    dobj = jobjGetDrawable(part->mJoint);
    if (dobj == NULL) {
        return;
    }
    material->mMaterial = dobjGetMaterial(dobj, index);
}

void GSpartGetTransform(GSpart *part, GSvec *position, GSvec *rotation, GSvec *scale)
{
    static _HSD_JObj *parentList[24];
    u32 depth = 0;
    _HSD_JObj *joint = part->mJoint;
    _HSD_JObj *node;
    u32 i;
    u32 j;
    f32 x, y, z;
    Vec totalRotation;
    Vec angle;
    f32 scaleX, scaleY, scaleZ;
    f32 sx, sy, sz;

    GSmodelUpdate(part->mModel);
    if (!GSmodelIsBlending(part->mModel)) {
        HSD_JObjSetupMatrix(joint);
        x = joint->mMatrix[0][3];
        y = joint->mMatrix[1][3];
        z = joint->mMatrix[2][3];
    } else if (joint->flags & HSD_JOBJ_TYPE_MASK) {
        x = joint->mTrans[0];
        y = joint->mTrans[1];
        z = joint->mTrans[2];
    } else {
        x = joint->mMatrix[0][3];
        y = joint->mMatrix[1][3];
        z = joint->mMatrix[2][3];
    }

    if (rotation != NULL || scale != NULL) {
        while (joint != NULL) {
            parentList[depth++] = joint;
            joint = partGetParent(joint);
            if (depth >= 24) {
                GSlogWrite("GSpart: child node too deep\n");
                joint = NULL;
            }
        }
        if (rotation != NULL) {
            totalRotation.x = 0.0f;
            totalRotation.y = 0.0f;
            totalRotation.z = 0.0f;
            for (i = depth; i != 0; i--) {
                node = parentList[i - 1];
                angle.x = HSD_JObjGetRotationX(node);
                angle.y = HSD_JObjGetRotationY(node);
                angle.z = HSD_JObjGetRotationZ(node);
                PSVECAdd(&totalRotation, &angle, &totalRotation);
            }
        }
        if (scale != NULL) {
            scaleX = 1.0f;
            scaleY = 1.0f;
            scaleZ = 1.0f;
            for (j = depth; j != 0; j--) {
                node = parentList[j - 1];
                sx = HSD_JObjGetScaleX(node);
                sy = HSD_JObjGetScaleY(node);
                sz = HSD_JObjGetScaleZ(node);
                scaleX *= sx;
                scaleY *= sy;
                scaleZ *= sz;
            }
        }
    }

    if (position != NULL) {
        position->x = x;
        position->y = y;
        position->z = z;
    }
    if (rotation != NULL) {
        rotation->x = totalRotation.x;
        rotation->y = totalRotation.y;
        rotation->z = totalRotation.z;
    }
    if (scale != NULL) {
        scale->x = scaleX;
        scale->y = scaleY;
        scale->z = scaleZ;
    }
}

void GSpartGetMatrix(GSpart *part, GSmtx *out)
{
    _HSD_JObj *joint = part->mJoint;

    if (out == NULL) {
        return;
    }
    GSmodelUpdate(part->mModel);
    HSD_JObjGetMatrix(joint, out->m);
}

void GSpartRegisterAnimMix(GSpart *part, GSpartMixType type, u32 flags,
                           f32 (*callback)(GSpart *, GSpartMixType, void *, GSvec *),
                           void *callbackData)
{
    GSmodelPartMix *mix;
    GSquat rotation;

    if (type == GS_MODEL_PART_MIX_NONE) {
        return;
    }
    if (callback == NULL) {
        return;
    }
    mix = partRegisterMix(part, type);
    if (mix == NULL) {
        return;
    }
    mix->mType = type;
    mix->mPartIndex = part->mIndex;
    mix->mCallback = callback;
    mix->mCallbackData = callbackData;
    mix->mFlags = flags;
    HSD_JObjGetTranslation(part->mJoint, (Vec *)&mix->mBasePosition);
    HSD_JObjGetRotation(part->mJoint, (Quaternion *)&rotation);
    HSD_JObjGetScale(part->mJoint, (Vec *)&mix->mBaseScale);
    mix->mBaseRotation.x = rotation.x;
    mix->mBaseRotation.y = rotation.y;
    mix->mBaseRotation.z = rotation.z;
}

void GSpartUnregisterAnimMix(GSpart *part, GSpartMixType type)
{
    _GSmodel *model = part->mModel;
    GSmodelPartMix *mix;
    s32 i;

    for (i = 0; i < GS_MODEL_PART_MIX_COUNT; i++) {
        mix = &model->mPartMix[i];
        if (mix->mType != GS_MODEL_PART_MIX_NONE && mix->mType == type &&
            (u32)mix->mPartIndex == part->mIndex) {
            mix->mType = GS_MODEL_PART_MIX_NONE;
            return;
        }
    }
}

s32 GSpartSetAnimIndex(GSpart *part, s32 animIndex, s32 animType, f32 frame)
{
    _GSmodel *model = part->mModel;
    _HSD_JObj *joint = part->mJoint;
    GSmodelPartAnim *slot;
    s32 slotIndex;
    _HSD_AnimJoint *jointAnimation;
    _HSD_ShapeAnimJoint *shapeAnimation;
    _HSD_AObj *animation;

    if (animIndex < 0) {
        return -1;
    }
    if (!GSmodelCanAnimate(model)) {
        return -1;
    }
    if ((u32)animIndex >= GSmodelGetMaxAnimIndex(model)) {
        return -1;
    }
    for (slotIndex = 0; slotIndex < MODEL_PART_ANIM_COUNT; slotIndex++) {
        slot = &model->mPartAnim[slotIndex];
        if (slot->mAnimation == NULL) {
            break;
        }
    }
    if (slotIndex >= MODEL_PART_ANIM_COUNT) {
        return -1;
    }
    jointAnimation = GSpartGetAnimPtr(model->mRoot->mAnimations[animIndex], part->mIndex);
    if (GSmodelCanShapeAnimate(model)) {
        shapeAnimation = GSpartGetShapeAnimPtr(model->mRoot->mShapeAnimations[animIndex], part->mIndex);
    } else {
        shapeAnimation = NULL;
    }
    if (jointAnimation == NULL && shapeAnimation == NULL) {
        return -1;
    }
    if (shapeAnimation == NULL && jointAnimation->mAnimation == NULL) {
        return -1;
    }
    HSD_JObjRemoveAnim(joint);
    HSD_JObjAddAnim(joint, jointAnimation, NULL, shapeAnimation);
    HSD_JObjReqAnim(joint, frame);
    animation = joint->mAObj;
    if (animation == NULL) {
        return -1;
    }
    slot->mAnimation = animation;
    slot->mType = animType;
    slot->mIndex = animIndex;
    slot->mRate = model->mAnim.mRate;
    slot->mFrame = frame;
    slot->mRequestedFrame = slot->mFrame;
    slot->mEndFrame = animation->mEndFrame;
    switch (slot->mType) {
    case GS_MODEL_ANIM_ONCE:
        HSD_AObjClearFlags(animation, HSD_AOBJ_FLAG_LOOP);
        break;
    case GS_MODEL_ANIM_LOOP:
        HSD_AObjSetFlags(animation, HSD_AOBJ_FLAG_LOOP);
        break;
    }
    model->mHasPartAnim = 1;
    return slotIndex;
}

s32 GSpartSetTexAnimIndex(GSpart *part, s32 materialIndex, s32 animIndex,
                          s32 animType, f32 frame)
{
    _GSmodel *model = part->mModel;
    GSmodelPartAnim *slot;
    _HSD_MatAnim *matAnim;
    s32 slotIndex;
    _HSD_MatAnimJoint *matAnimJoint;
    s32 i;
    GSmaterial *material;
    _HSD_TObj *tobj;
    _HSD_AObj *aobj;

    if (materialIndex < 0 || animIndex < 0) {
        return -1;
    }
    if (!GSmodelCanTexAnimate(model)) {
        return -1;
    }
    if ((u32)animIndex >= GSmodelGetMaxTexAnimIndex(model)) {
        return -1;
    }
    for (slotIndex = 0; slotIndex < MODEL_PART_TEX_ANIM_COUNT; slotIndex++) {
        slot = &model->mPartTexAnim[slotIndex];
        if (slot->mAnimation == NULL) {
            break;
        }
    }
    if (slotIndex >= MODEL_PART_TEX_ANIM_COUNT) {
        return -1;
    }
    matAnimJoint = GSpartGetMatAnimPtr(modelResource(model)->mTextureAnimations[animIndex], part->mIndex);
    if (matAnimJoint == NULL) {
        return -1;
    }
    matAnim = matAnimJoint->mMatAnim;
    for (i = materialIndex; i-- != 0;) {
        matAnim = matAnim->mNext;
        if (matAnim == NULL) {
            return -1;
        }
    }
    material = partGetMaterial(part, materialIndex);
    if (material == NULL) {
        return -1;
    }
    tobj = material->mMaterial->mTObj;
    if (tobj == NULL) {
        return -1;
    }
    HSD_TObjRemoveAnim(tobj);
    HSD_TObjAddAnim(tobj, matAnim->mTexAnim);
    HSD_TObjReqAnim(tobj, frame);
    aobj = tobj->mAObj;
    slot->mAnimation = aobj;
    slot->mType = animType;
    slot->mIndex = animIndex;
    slot->mRate = model->mTexAnim.mRate;
    slot->mFrame = frame;
    slot->mRequestedFrame = slot->mFrame;
    slot->mEndFrame = aobj->mEndFrame;
    switch (slot->mType) {
    case GS_MODEL_ANIM_ONCE:
        HSD_AObjClearFlags(aobj, HSD_AOBJ_FLAG_LOOP);
        break;
    case GS_MODEL_ANIM_LOOP:
        HSD_AObjSetFlags(aobj, HSD_AOBJ_FLAG_LOOP);
        break;
    }
    model->mHasPartTexAnim = 1;
    return slotIndex;
}

void GSpartStopAnimIndex(GSpart *part, s32 animIndex)
{
    _GSmodel *model = part->mModel;
    s32 index;

    if (!(animIndex >= 0 && animIndex < MODEL_PART_ANIM_COUNT)) {
        return;
    }
    if (model->mPartAnim[animIndex].mAnimation == NULL) {
        return;
    }
    model->mPartAnim[animIndex].mAnimation = NULL;
    index = GSpartSetAnimIndex(part, 0, GS_MODEL_ANIM_LOOP, 0.0f);
    model->mPartAnim[index].mAnimation = NULL;
}

u32 GSpartGetJObjIndex(_HSD_JObj *root, _HSD_JObj *joint)
{
    if (root == joint) {
        return 0;
    }
    _partSearchDone = 0;
    _partToFindJObj = joint;
    _partFindCurIdx = 0;
    _partFoundIndex = -1;
    HSD_JObjWalkTree(root, _partFindIndexCB, NULL);
    return _partFoundIndex;
}

_HSD_JObj *GSpartGetJObjPtr(_HSD_JObj *joint, u32 index)
{
    if (index == 0) {
        return joint;
    }
    _partSearchDone = 0;
    _partToFindIdx = index;
    _partFindCurIdx = 0;
    _partFoundJObj = NULL;
    HSD_JObjWalkTree(joint, _partFindJObjCB, NULL);
    return _partFoundJObj;
}

_HSD_AnimJoint *GSpartGetAnimPtr(_HSD_AnimJoint *joint, u32 index)
{
    if (index == 0) {
        return joint;
    }
    _partSearchDone = 0;
    _partToFindIdx = index;
    _partFindCurIdx = 0;
    _partFoundAnim = NULL;
    GSpartAnimWalkTree(joint, _partFindAnimCB, NULL);
    return _partFoundAnim;
}

_HSD_MatAnimJoint *GSpartGetMatAnimPtr(_HSD_MatAnimJoint *joint, u32 index)
{
    if (index == 0) {
        return joint;
    }
    _partSearchDone = 0;
    _partToFindIdx = index;
    _partFindCurIdx = 0;
    _partFoundMatAnim = NULL;
    GSpartMatAnimWalkTree(joint, _partFindMatAnimCB, NULL);
    return _partFoundMatAnim;
}

_HSD_ShapeAnimJoint *GSpartGetShapeAnimPtr(_HSD_ShapeAnimJoint *joint, u32 index)
{
    if (index == 0) {
        return joint;
    }
    _partSearchDone = 0;
    _partToFindIdx = index;
    _partFindCurIdx = 0;
    _partFoundShapeAnim = NULL;
    GSpartShapeAnimWalkTree(joint, _partFindShapeAnimCB, NULL);
    return _partFoundShapeAnim;
}

void GSpartAnimWalkTree(_HSD_AnimJoint *joint, PartAnimCallback callback, void **arg)
{
    _HSD_AnimJoint *child;

    if (joint == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(joint, arg, 0);
    }
    for (child = joint->mChild; child != NULL; child = child->mNext) {
        _partAnimWalkTree0(child, callback, arg);
    }
}

void GSpartMatAnimWalkTree(_HSD_MatAnimJoint *joint, PartMatAnimCallback callback, void **arg)
{
    _HSD_MatAnimJoint *child;

    if (joint == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(joint, arg, 0);
    }
    for (child = joint->mChild; child != NULL; child = child->mNext) {
        _partMatAnimWalkTree0(child, callback, arg);
    }
}

void GSpartShapeAnimWalkTree(_HSD_ShapeAnimJoint *joint, PartShapeAnimCallback callback, void **arg)
{
    _HSD_ShapeAnimJoint *child;

    if (joint == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(joint, arg, 0);
    }
    for (child = joint->mChild; child != NULL; child = child->mNext) {
        _partShapeAnimWalkTree0(child, callback, arg);
    }
}
