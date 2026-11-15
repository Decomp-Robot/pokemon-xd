#include <GSgfx.h>
#include <dolphin/gx/GXLighting.h>
#include <GSAPI/GSvec/GSvec.h>

u32 GSgfxLight::Load(u8 index)
{
    if (mFlags & GSGFX_LIGHT_DIRTY_COLOR)
        GXInitLightColor(&mLightObj, *(GXColor *)&mColor);

    switch (mType) {
    case GSGFX_LIGHT_SPECULAR:
        if (mFlags & GSGFX_LIGHT_DIRTY_HALF_ANGLE) {
            GXInitSpecularDirHA(&mLightObj, mDirection.x, mDirection.y, mDirection.z,
                               mParameters.specular.halfAngleX,
                               mParameters.specular.halfAngleY,
                               mParameters.specular.halfAngleZ);
        } else if (mFlags & GSGFX_LIGHT_DIRTY_DIRECTION) {
            GXInitSpecularDir(&mLightObj, mDirection.x, mDirection.y, mDirection.z);
        }
        if (mFlags & GSGFX_LIGHT_DIRTY_SHININESS) {
            f32 halfShininess = mParameters.specular.shininess / 2.0f;
            GXInitLightAttn(&mLightObj, 0.0f, 0.0f, 1.0f,
                            halfShininess, 0.0f, 1.0f - halfShininess);
        }
        break;
    case GSGFX_LIGHT_SPOT:
        if (mFlags & GSGFX_LIGHT_DIRTY_DIRECTION)
            GXInitLightDir(&mLightObj, mDirection.x, mDirection.y, mDirection.z);
        if (mFlags & GSGFX_LIGHT_DIRTY_SPOT) {
            GXInitLightSpot(&mLightObj, mParameters.spot.cutoff, mParameters.spot.function);
            GXInitLightDistAttn(&mLightObj, mParameters.spot.referenceDistance,
                               mParameters.spot.referenceBrightness,
                               mParameters.spot.distanceFunction);
        } else if (mFlags & GSGFX_LIGHT_DIRTY_ATTENUATION) {
            GXInitLightAttn(&mLightObj, mParameters.attenuation.a0, mParameters.attenuation.a1,
                           mParameters.attenuation.a2, mParameters.attenuation.k0,
                           mParameters.attenuation.k1, mParameters.attenuation.k2);
        }
    case GSGFX_LIGHT_DIFFUSE:
        if (mFlags & GSGFX_LIGHT_DIRTY_POSITION)
            GXInitLightPos(&mLightObj, mPosition.x, mPosition.y, mPosition.z);
        break;
    }

    mFlags = 0;
    GXLightID mask = (GXLightID)(1 << index);
    mSlot = index;
    GXLoadLightObjImm(&mLightObj, mask);
    return mask;
}

void GSgfxLight::MakeSpec(GSgfxColAu8 *color, GSvec *direction, f32 shininess, GSvec *halfAngle)
{
    mFlags = 0;
    mType = GSGFX_LIGHT_SPECULAR;
    mColor = *color;
    mFlags |= GSGFX_LIGHT_DIRTY_COLOR;
    mDirection = *direction;
    mFlags |= GSGFX_LIGHT_DIRTY_DIRECTION;
    mParameters.specular.shininess = shininess;
    mFlags |= GSGFX_LIGHT_DIRTY_SHININESS;
    if (halfAngle) {
        mParameters.specular.halfAngleX = halfAngle->x;
        mParameters.specular.halfAngleY = halfAngle->y;
        mParameters.specular.halfAngleZ = halfAngle->z;
        mFlags |= GSGFX_LIGHT_DIRTY_HALF_ANGLE;
    }
}

void GSgfxLight::MakeDiff(GSgfxColAu8 *color, GSvec *position, bool directional)
{
    mFlags = 0;
    mType = GSGFX_LIGHT_DIFFUSE;
    mColor = *color;
    mFlags |= GSGFX_LIGHT_DIRTY_COLOR;
    if (directional) {
        mPosition = GSvecNormalized(*position);
        PSVECScale(&mPosition, &mPosition, -1e18f);
    } else {
        mPosition = *position;
    }
    mFlags |= GSGFX_LIGHT_DIRTY_POSITION;
}

GSgfxLight::GSgfxLight()
{
    mColor.r = 255;
    mColor.g = 255;
    mColor.b = 255;
    mColor.a = 255;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    GXInitLightColor(&mLightObj, *(GXColor *)&mColor);
    GXInitLightPos(&mLightObj, 0.0f, 0.0f, 0.0f);
    mType = GSGFX_LIGHT_DIFFUSE;
    mFlags = 0;
}
