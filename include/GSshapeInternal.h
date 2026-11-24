#ifndef GSSHAPE_INTERNAL_H
#define GSSHAPE_INTERNAL_H

#include <GSgfx.h>
#include <GSmathTypes.h>
#include <GSshape.h>
#include <GStexture.h>

struct tagGScol4ub {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

enum GsShapePlayMode {
    GS_SHAPE_PLAY_ONCE,
    GS_SHAPE_PLAY_LOOP,
    GS_SHAPE_PLAY_CLAMP,
};

struct tagGsFilePrimShapeData {
    u32 mFlags;
    u32 mFrameCount;
    u32 mFrameRate;
    u16 mKeyCount;
    u32 mKeyOffset;
    u8 mPlayMode;
};

struct tagGsFilePrimTexture;

struct tagGsFilePrimObjectEntry {
    u32 mFlags;
    u8 mVisible;
    u8 mColorChannel;
    u8 mAlphaChannel;
    u8 mMaterialMode;
    u8 mStateMode;
    u8 mLightMode;
    u16 mPolygonCount;
    u16 mSubPolygonCount;
    u16 mVertexCount;
    GSvec *mPositions;
    tagGScol4ub *mColors;
    GSvec2 *mTexCoords;
    GSvec *mNormals;
    u32 mVertexFlags;
    u16 mMaterialCount;
    u16 mTextureCount;
    tagGsFilePrimTexture *mTextures[4];
    u16 mDeformType;
    u16 mDeformFlags;
    f32 mDeformAngleX;
    f32 mDeformAngleY;
};

struct GsShapeTexture {
    tagGsFilePrimTexture *mData;
    GStexture *mTexture;
};

class GsKeyObject;

class GsShapeObject {
    friend class GsShapeRender;

public:
    u8 CreateShapeDrawBuffer(s16 index);
    u8 FreeShapeDrawBuffer(s16 index);
    void MakeShapeGfxDL();

private:
    struct GsShapeTextureColor {
        tagGScol4ub mColor;
        tagGScol4ub mLowColor;
    };

    tagGsFilePrimObjectEntry *mEntry;
    GSgfxVF mVertexFormat;
    GSvec *mDrawPositions;
    tagGScol4ub *mDrawColors;
    GSvec2 *mDrawTexCoords[4];
    u8 mLayerCount;
    GsShapeTextureColor mTextureColors[2];
    GSmtx mShapeMatrix;
    GSgfxDL *mDisplayList;
    void *mInstanceBuffer[256];
};

#endif
