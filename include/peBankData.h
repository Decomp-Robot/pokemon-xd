#ifndef PE_BANK_DATA_H
#define PE_BANK_DATA_H
#include <peNode.h>
#include <GSgfxLight.h>
#include <HSDScene.h>

struct _peModel {
    u32 mFlags;
    u8 mModelParameters[0x30];
    peSpline *mScaleSplines[3];
    peSpline *mRotationSplines[3];
    union {
        s32 mResourceID;
        u32 mShapeHandle;
        HSD_JObj *mJoint;
    };
    f32 mAnimationLength;
    f32 mAnimationFrame;
};

struct _peDistortion {
    u32 mFlags;
    u8 mDistortionParameters[0x10];
    peSpline *mWidthSpline;
    peSpline *mHeightSpline;
    u8 mTextureParameters[0x14];
    peSpline *mRotationSpline;
    struct peColorSpline *mColorSpline;
    u8 mRenderParameters[0x28];
    union {
        s32 mResourceID;
        GStexture *mTexture;
    };
};

struct _peParticleStrip {
    u32 mFlags;
    u8 mStripParameters[4];
    struct peColorSpline *mColorSpline;
    peColorSpline *mSecondColorSpline;
    peSpline *mAlphaSpline;
    u8 mTextureParameters[8];
    union {
        s32 mResourceID;
        GStexture *mTexture;
        peTextureList *mTextureList;
    };
};

struct _peLight {
    u32 mFlags;
    u8 mLightParameters[0x30];
    peSpline *mPositionSplines[3];
    peSpline *mDirectionSplines[3];
    struct peColorSpline *mColorSpline;
    peSpline *mAlphaSpline;
    u8 mAttenuationParameters[0x18];
    GSgfxColAu8 mColor;
    f32 mShininess;
    union {
        s32 mResourceID;
        void *mResource;
    };
    GSgfxLight *mDiffuseLight;
    GSgfxLight *mSpecularLight;
};

enum peModelFlags {
    PE_MODEL_LOOP = 0x04,
    PE_MODEL_SHAPE_RESOURCE = 0x40,
};
enum peLightFlags {
    PE_LIGHT_DIFFUSE = 0x200,
    PE_LIGHT_SPECULAR = 0x400,
};
enum peStripFlags {
    PE_STRIP_TEXTURE_LIST = 0x400,
    PE_STRIP_DUAL_COLOR = 0x2000,
    PE_STRIP_SECOND_COLOR_SPLINE = 0x40000,
};
enum peBankConversion {
    PE_TEXTURE_COORDINATES_FIXED = 0x08,
    PE_TEXTURE_COORDINATE_SCALE = 1024,
    PE_ANIMATION_FRAMES_PER_SECOND = 60,
    GPD_ARCHIVE_BLOCK_BYTES = 32,
    GPD_ARCHIVE_PADDING_MASK = 0x17,
    PE_BANK_RELOCATED_ADDRESS = 0x80000000,
};
#endif
