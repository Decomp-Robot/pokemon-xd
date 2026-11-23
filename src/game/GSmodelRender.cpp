#include <GSmodelInternal.h>

/* Set while the model carries the joint its own bound is measured from. */
#define MODEL_FLAG_CENTER_NULL 0x10000u

/* Counts the models that currently render with an environment map; the layer's
 * shared framebuffer copy is dropped when the last one of them turns it off. */
extern u8 gsEnvMapUserCount;

/* Puts the original texture back on every material of the model. */
void GSmodelResetTextureChange(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialResetTexture(material);
    }
}

/* Gives every material of the model the same replacement texture. */
void GSmodelSetTextureChange(_GSmodel *model, GStexture *texture)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetTexture(material, texture);
    }
}

/* All materials of a model carry the same flags, so the first one stands for
 * the whole model. */
u8 GSmodelIsModulationEnabled(_GSmodel *model)
{
    if (model->mMaterialCount == 0) {
        return 0;
    }
    if (model->mMaterials == NULL) {
        return 0;
    }
    if (GSmaterialGetEnabledExtensions(model->mMaterials) & GS_MATERIAL_EXT_MODULATION) {
        return 1;
    }
    return 0;
}

/* Reports mid grey for a model that does not render with modulation. */
void GSmodelGetModulationColor(_GSmodel *model, GScolor *color)
{
    if (GSmodelIsModulationEnabled(model)) {
        GSmaterialGetModulate(model->mMaterials, color);
    } else {
        color->channels.r = color->channels.g = color->channels.b = 127;
        color->channels.a = 255;
    }
}

void GSmodelDisableModulation(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialDisableExtension(material, GS_MATERIAL_EXT_MODULATION);
    }
}

/* Stores the tint on every material without touching the modulation flag. */
void GSmodelSetModulationColor(_GSmodel *model, GScolor *color)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetModulate(material, color);
    }
}

void GSmodelEnableModulation(_GSmodel *model, GScolor *color)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetModulate(material, color);
        GSmaterialEnableExtension(material, GS_MATERIAL_EXT_MODULATION);
    }
}

void GSmodelDisableColorSwap(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialDisableExtension(material, GS_MATERIAL_EXT_COLOR_SWAP);
    }
}

/* Color swap renders the colours given by the channels instead of the ones of
 * the material textures. */
void GSmodelEnableColorSwap(_GSmodel *model, GScolor *color)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetColorChannels(material, color);
        GSmaterialEnableExtension(material, GS_MATERIAL_EXT_COLOR_SWAP);
    }
}

u8 GSmodelIsEnvMapEnabled(_GSmodel *model)
{
    if (model->mMaterialCount == 0) {
        return 0;
    }
    if (model->mMaterials == NULL) {
        return 0;
    }
    if (GSmaterialGetEnabledExtensions(model->mMaterials) & GS_MATERIAL_EXT_ENV_MAP) {
        return 1;
    }
    return 0;
}

/* Clears the environment map of every material and drops the layer's shared
 * framebuffer copy once no model uses it any more. */
void GSmodelDisableEnvMap(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;
    GSgfxLayer *layer;

    if (!GSmodelIsEnvMapEnabled(model)) {
        return;
    }
    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialDisableExtension(material, GS_MATERIAL_EXT_ENV_MAP);
    }
    layer = GSgfxLayerFindByID(GSGFX_LAYER_BASE);
    if (layer == NULL) {
        return;
    }
    gsEnvMapUserCount--;
    if (gsEnvMapUserCount == 0) {
        layer->SetTextureCopy(NULL, false, NULL);
    }
}

void GSmodelSetEnvMapBlendValue(_GSmodel *model, f32 blend)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    if (!GSmodelIsEnvMapEnabled(model)) {
        return;
    }
    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetEnvMapBlendValue(material, blend);
    }
}

/* Starts environment mapping for the model: the layer hands out its copy of the
 * framebuffer and every material is set up to reflect it. */
void GSmodelEnableEnvMap(_GSmodel *model, s32 envMapMode, s32 envMapType, f32 blend)
{
    u32 i;
    u32 count;
    GSmaterial *material;
    GSgfxLayer *layer;
    GStexture *texture;

    if (GSmodelIsEnvMapEnabled(model)) {
        return;
    }
    layer = GSgfxLayerFindByID(GSGFX_LAYER_BASE);
    if (layer == NULL) {
        return;
    }
    gsEnvMapUserCount++;
    layer->SetTextureCopy(GS_GFX_LAYER_SHARED_TEXTURE_COPY, false, NULL);
    texture = layer->GetTextureCopy();
    if (texture == NULL) {
        return;
    }
    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetEnvMapParams(material, envMapMode, envMapType, texture, blend);
        GSmaterialEnableExtension(material, GS_MATERIAL_EXT_ENV_MAP);
    }
}
void GSmodelResetPEdescr(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialResetPEdescr(material);
    }
}

void GSmodelSetPEdescr(_GSmodel *model, HSD_PEDesc *descriptor)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetPEdescr(material, descriptor);
    }
}

void GSmodelResetMaterialAlpha(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialResetAlpha(material);
    }
}

void GSmodelInitMaterialAlpha(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialStoreAlpha(material);
    }
}

void GSmodelSetMaterialAlpha(_GSmodel *model, f32 alpha)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetAlpha(material, material->mSavedAlpha * alpha / 255.0f);
    }
}

void GSmodelResetRenderFlags(_GSmodel *model)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    material = model->mMaterials;
    count = model->mMaterialCount;
    for (i = 0; i < count; i++, material++) {
        GSmaterialResetFlags(material);
    }
}

void GSmodelSetRenderFlags(_GSmodel *model, u32 flags)
{
    u32 i;
    u32 count;
    GSmaterial *material;

    count = model->mMaterialCount;
    material = model->mMaterials;
    for (i = 0; i < count; i++, material++) {
        GSmaterialSetFlags(material, flags | GSmaterialGetFlags(material));
    }
}

/* The bound of a model is measured from a joint of its own, the last child of
 * the joint the model is drawn with. Removes that joint again. */
void modelRemoveCenterNull(_GSmodel *model)
{
    _HSD_JObj *jobj;
    _HSD_JObj *joint;

    if ((model->mFlags & MODEL_FLAG_CENTER_NULL) != MODEL_FLAG_CENTER_NULL) {
        return;
    }
    jobj = model->mJObj;
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        jobj = jobj->child;
    }
    joint = HSD_JObjGetChild(jobj);
    while (HSD_JObjGetNext(joint) != NULL) {
        joint = HSD_JObjGetNext(joint);
    }
    HSD_JObjRemove(joint);
    model->mFlags &= ~MODEL_FLAG_CENTER_NULL;
}

/* Adds the joint the model bound is measured from and returns the part that
 * joint stands for. A model keeps one of them, so a model that already has one
 * only reports it. */
u16 GSmodelCenterNull(_GSmodel *model)
{
    _HSD_JObj *joint;
    _HSD_JObj *parent;
    _HSD_JObj *sibling;

    if ((model->mFlags & MODEL_FLAG_CENTER_NULL) == MODEL_FLAG_CENTER_NULL) {
        return GSmodelGetPartCount(model) - 1;
    }
    joint = HSD_JObjAlloc();
    if (joint == NULL) {
        return 0;
    }
    parent = model->mJObj;
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        parent = parent->child;
    }
    sibling = HSD_JObjGetChild(parent);
    if (sibling == NULL) {
        HSD_JObjAddChild(parent, joint);
    } else {
        while (HSD_JObjGetNext(sibling) != NULL) {
            sibling = HSD_JObjGetNext(sibling);
        }
        HSD_JObjAddNext(sibling, joint);
    }
    HSD_JObjSetFlags(joint, HSD_JObjGetFlags(parent));
    model->mFlags |= MODEL_FLAG_CENTER_NULL;
    modelUpdateCenterNull(model);
    return GSmodelGetPartCount(model) - 1;
}

/* Puts the centre joint back on the middle of the model bound. */
void modelUpdateCenterNull(_GSmodel *model)
{
    GSbound *bound;
    GSvec center;
    GSvec min;
    GSvec max;
    _HSD_JObj *jobj;
    _HSD_JObj *joint;

    bound = &model->mBound;
    if ((model->mFlags & MODEL_FLAG_CENTER_NULL) != MODEL_FLAG_CENTER_NULL) {
        return;
    }
    jobj = model->mJObj;
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        jobj = jobj->child;
    }
    joint = HSD_JObjGetChild(jobj);
    while (HSD_JObjGetNext(joint) != NULL) {
        joint = HSD_JObjGetNext(joint);
    }
    GSboundGetMinMax(bound, &min, &max, false, false, false, true);
    GSlerpGetLinearInterpolationVector(&center, &min, &max, 0.5f);
    HSD_JObjSetTranslation(joint, (Vec *)&center);
    HSD_JObjSetRotationX(joint, 0.0f);
    HSD_JObjSetRotationY(joint, 0.0f);
    HSD_JObjSetRotationZ(joint, 0.0f);
    HSD_JObjSetScale(joint, (Vec *)&unitScaleVector);
    HSD_JObjSetupMatrix(joint);
}

/* Tells whether the model root sits under a null joint added by
 * GSmodelAddNull. */
bool GSmodelIsRootNullAdded(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT;
}

/* Drops the null joint around the model root again, keeping the pose the root
 * was drawn with. */
void GSmodelRemoveNull(_GSmodel *model)
{
    GSvec translation;
    GSvec scale;
    f32 rotationX;
    f32 rotationY;
    f32 rotationZ;
    _HSD_JObj *root;
    _HSD_JObj *joint;

    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) != MODEL_FLAG_NULL_JOINT) {
        return;
    }
    root = model->mJObj;
    joint = HSD_JObjGetChild(root);
    HSD_JObjGetTranslation(root, (Vec *)&translation);
    rotationX = HSD_JObjGetRotationX(root);
    rotationY = HSD_JObjGetRotationY(root);
    rotationZ = HSD_JObjGetRotationZ(root);
    HSD_JObjGetScale(root, (Vec *)&scale);
    HSD_JObjSetTranslation(joint, (Vec *)&translation);
    HSD_JObjSetRotationX(joint, rotationX);
    HSD_JObjSetRotationY(joint, rotationY);
    HSD_JObjSetRotationZ(joint, rotationZ);
    HSD_JObjSetScale(joint, (Vec *)&scale);
    root = HSD_JObjRemove(root);
    HSD_JObjSetupMatrix(joint);
    model->mFlags &= ~MODEL_FLAG_NULL_JOINT;
    model->mJObj = root;
}

/* Reads the scale of the joint that carries the model root, or a neutral scale
 * if the root is not wrapped in a null joint. */
void GSmodelGetRootScale(_GSmodel *model, GSvec *scale)
{
    _HSD_JObj *joint = model->mJObj;

    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        joint = joint->child;
        HSD_JObjGetScale(joint, (Vec *)scale);
    } else {
        scale->x = 1.0f;
        scale->y = 1.0f;
        scale->z = 1.0f;
    }
}

/* Reads the rotation of the joint that carries the model root. */
void GSmodelGetRootRotation(_GSmodel *model, GSvec *rotation)
{
    _HSD_JObj *joint = model->mJObj;

    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        joint = joint->child;
        rotation->x = HSD_JObjGetRotationX(joint);
        rotation->y = HSD_JObjGetRotationY(joint);
        rotation->z = HSD_JObjGetRotationZ(joint);
    } else {
        rotation->x = 0.0f;
        rotation->y = 0.0f;
        rotation->z = 0.0f;
    }
}

/* Reads the translation of the joint that carries the model root. */
void GSmodelGetRootPosition(_GSmodel *model, GSvec *position)
{
    _HSD_JObj *joint = model->mJObj;

    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        joint = joint->child;
        HSD_JObjGetTranslation(joint, (Vec *)position);
    } else {
        position->x = 0.0f;
        position->y = 0.0f;
        position->z = 0.0f;
    }
}

/* Wraps the model root in a null joint that carries the current root pose, so
 * that the root can be transformed without disturbing the model animation. */
void GSmodelAddNull(_GSmodel *model, GSvec *position, GSvec *rotation, GSvec *scale)
{
    GSvec defaultPose;
    GSvec oldTranslation;
    GSvec oldScale;
    f32 oldRotationX;
    f32 oldRotationY;
    f32 oldRotationZ;
    _HSD_JObj *root;
    _HSD_JObj *joint;

    root = model->mJObj;
    if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
        return;
    }
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) != MODEL_FLAG_NULL_JOINT) {
        joint = HSD_JObjAlloc();
        if (joint == NULL) {
            return;
        }
        HSD_JObjGetTranslation(root, (Vec *)&oldTranslation);
        oldRotationX = HSD_JObjGetRotationX(root);
        oldRotationY = HSD_JObjGetRotationY(root);
        oldRotationZ = HSD_JObjGetRotationZ(root);
        HSD_JObjGetScale(root, (Vec *)&oldScale);
        if (position != NULL) {
            HSD_JObjSetTranslation(root, (Vec *)position);
        } else {
            defaultPose.x = 0.0f;
            defaultPose.y = 0.0f;
            defaultPose.z = 0.0f;
            HSD_JObjSetTranslation(root, (Vec *)&defaultPose);
        }
        if (rotation != NULL) {
            HSD_JObjSetRotationX(root, rotation->x);
            HSD_JObjSetRotationY(root, rotation->y);
            HSD_JObjSetRotationZ(root, rotation->z);
        } else {
            HSD_JObjSetRotationX(root, 0.0f);
            HSD_JObjSetRotationY(root, 0.0f);
            HSD_JObjSetRotationZ(root, 0.0f);
        }
        if (scale != NULL) {
            HSD_JObjSetScale(root, (Vec *)scale);
        } else {
            defaultPose.x = 1.0f;
            defaultPose.y = 1.0f;
            defaultPose.z = 1.0f;
            HSD_JObjSetScale(root, (Vec *)&defaultPose);
        }
        HSD_JObjSetupMatrix(root);
        HSD_JObjAddChild(joint, model->mJObj);
        model->mFlags |= MODEL_FLAG_NULL_JOINT;
        model->mJObj = joint;
        /* The root keeps the pose the caller asked for, the joint around it
         * carries the pose the root had before. */
        HSD_JObjSetFlags(joint, HSD_JObjGetFlags(root));
        HSD_JObjSetTranslation(joint, (Vec *)&oldTranslation);
        HSD_JObjSetRotationX(joint, oldRotationX);
        HSD_JObjSetRotationY(joint, oldRotationY);
        HSD_JObjSetRotationZ(joint, oldRotationZ);
        HSD_JObjSetScale(joint, (Vec *)&oldScale);
        HSD_JObjSetMtxDirty(root);
        HSD_JObjSetupMatrix(root);
        HSD_JObjSetupMatrix(joint);
        GSmodelSetAnimIndex(model, model->mAnim.mIndex);
        GSmodelSetAnimType(model, model->mAnim.mType);
        GSmodelStartAnimation(model);
    } else {
        joint = HSD_JObjGetChild(root);
        if (position != NULL) {
            HSD_JObjSetTranslation(joint, (Vec *)position);
        } else {
            defaultPose.x = 0.0f;
            defaultPose.y = 0.0f;
            defaultPose.z = 0.0f;
            HSD_JObjSetTranslation(joint, (Vec *)&defaultPose);
        }
        if (rotation != NULL) {
            HSD_JObjSetRotationX(joint, rotation->x);
            HSD_JObjSetRotationY(joint, rotation->y);
            HSD_JObjSetRotationZ(joint, rotation->z);
        } else {
            HSD_JObjSetRotationX(joint, 0.0f);
            HSD_JObjSetRotationY(joint, 0.0f);
            HSD_JObjSetRotationZ(joint, 0.0f);
        }
        if (scale != NULL) {
            HSD_JObjSetScale(joint, (Vec *)scale);
        } else {
            defaultPose.x = 1.0f;
            defaultPose.y = 1.0f;
            defaultPose.z = 1.0f;
            HSD_JObjSetScale(joint, (Vec *)&defaultPose);
        }
        HSD_JObjSetupMatrix(joint);
        GSmodelSetAnimIndex(model, model->mAnim.mIndex);
        GSmodelSetAnimType(model, model->mAnim.mType);
        GSmodelStartAnimation(model);
    }
}

/* Gives a material back the texture, the PE descriptor and the extra render
 * modes it borrowed during its life. */
void modelMaterialFree(GSmaterial *material)
{
    GSmaterialResetPEdescr(material);
    GSmaterialResetTexture(material);
    GSmaterialDisableExtension(material, GS_MATERIAL_EXT_ALL);
}

/* Puts a material in the state the render layer expects before it is used. */
void modelMaterialInit(GSmaterial *material)
{
    if (material == NULL) {
        return;
    }
    material->mPEDesc = GS_MATERIAL_PEDESC_NONE;
    material->mEnabledExtensions = 0;
    material->mColorChannels[0] = 0;
    material->mColorChannels[1] = 1;
    material->mColorChannels[2] = 2;
    material->mColorChannels[3] = 3;
    material->mEnvMapSource = NULL;
    material->mEnvMapTexture = NULL;
    material->mEnvMapImage = NULL;
    material->mImageDesc = GS_MATERIAL_IMAGEDESC_NONE;
}
