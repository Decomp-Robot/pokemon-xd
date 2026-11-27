#include <GSmaterial.h>

void GSmaterialSetEnvMapBlendValue(GSmaterial *material, f32 blend)
{
    _HSD_TObj *envMapTexture = material->mEnvMapTexture;

    material->mEnvMapBlend = blend;
    if (envMapTexture != NULL) {
        envMapTexture->mBlending = blend;
    }
}

void GSmaterialSetEnvMapParams(GSmaterial *material, s32 mode, s32 type,
                               GStexture *texture, f32 blend)
{
    material->mEnvMapMode = mode;
    material->mEnvMapType = type;
    material->mEnvMapBlend = blend;
    material->mEnvMapSource = texture;
}

void GSmaterialSetModulate(GSmaterial *material, GScolor *color)
{
    material->mModulate.channels.r = color->channels.r;
    material->mModulate.channels.g = color->channels.g;
    material->mModulate.channels.b = color->channels.b;
    material->mModulate.channels.a = color->channels.a;
}

void GSmaterialSetColorChannels(GSmaterial *material, GScolor *color)
{
    material->mColorChannels[0] = color->channels.r;
    material->mColorChannels[1] = color->channels.g;
    material->mColorChannels[2] = color->channels.b;
    material->mColorChannels[3] = color->channels.a;
}
