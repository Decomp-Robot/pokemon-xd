#include <GSmodelInternal.h>

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
