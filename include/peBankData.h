#ifndef PE_BANK_DATA_H
#define PE_BANK_DATA_H
#include <peParticle.h>
#include <peShape.h>
#include <GSgfxLight.h>
#include <HSDScene.h>

struct _peModel {
    u32 mFlags;
    GSvec mRotation;
    GSvec mRotationVelocity;
    GSvec mScale;
    GSvec mScaleRandom;
    peSpline *mRotationSplines[3];
    peSpline *mScaleSplines[3];
    union {
        s32 mResourceID;
        u32 mShapeHandle;
        HSD_JObj *mJoint;
    };
    f32 mAnimationLength;
    f32 mAnimationFrame;
};

struct peDistortionMotionParameters {
    f32 mWidth;
    f32 mHeight;
    f32 mWidthRandom;
    f32 mHeightRandom;
};

struct peDistortionParameters {
    f32 mOffsetScaleX;
    f32 mOffsetScaleY;
    f32 mRotation;
    f32 mRotationVelocity;
    f32 mRotationVelocityRandom;
};

struct _peDistortion {
    u32 mFlags;
    peDistortionMotionParameters mMotion;
    peSpline *mWidthSpline;
    peSpline *mHeightSpline;
    peDistortionParameters mParameters;
    peSpline *mRotationSpline;
    peSpline *mAlphaSpline;
    u8 mRenderParameters[24];
    f32 mTexCoordLeft;
    f32 mTexCoordTop;
    f32 mTexCoordRight;
    f32 mTexCoordBottom;
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

enum peModelFlags {
    PE_MODEL_SECONDARY_LAYER = 0x2,
    PE_MODEL_LOOP = 0x04,
    PE_MODEL_ANIMATION_LIFETIME = 0x8,
    PE_MODEL_ROTATION_SPLINE = 0x20,
    PE_MODEL_SHAPE_RESOURCE = 0x40,
    PE_MODEL_RANDOM_ROTATION_X = 0x80,
    PE_MODEL_RANDOM_ROTATION_Y = 0x100,
    PE_MODEL_RANDOM_ROTATION_Z = 0x200,
    PE_MODEL_RANDOM_VELOCITY_X = 0x400,
    PE_MODEL_RANDOM_VELOCITY_Y = 0x800,
    PE_MODEL_RANDOM_VELOCITY_Z = 0x1000,
    PE_MODEL_RANDOM_REFLECT_X = 0x2000,
    PE_MODEL_RANDOM_REFLECT_Y = 0x4000,
    PE_MODEL_RANDOM_REFLECT_Z = 0x8000,
    PE_MODEL_DEPTH_SORT = 0x10000,
    PE_MODEL_RANDOM_ORIENTATION_MASK = 0xE380,
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
