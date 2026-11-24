#ifndef GSSHAPE_RENDER_H
#define GSSHAPE_RENDER_H

#include <GSshapeInternal.h>

extern u32 g_instancedMemorySizeAll;

class GsShapeRender {
public:
    void InitShapeRender();
    void LoadShapeRender(void *data);
    void FreeShapeRender();
    void ExecuteFreeInstance();
    void SwapBuffers();
    void Update(f32 time, s16 instance);
    void Render(s16 instance, GSgfxLayerID layer);
    GSgfxLayerID GetLayerId() const;
    u32 GetInstancedMemorySizeAll();
    u32 GetUsedMemorySizeAll();
    void FreeShapeRenderDrawBuffer(s16 instance);
    s16 CreateShapeRenderDrawBuffer();
    void FreeInstance();

    u8 mEnable;
    u32 mCurrentFrame;
    s32 mCurrentKey;
    GSgfxLayerID mLayerId;
    u32 mShapeFlags;
    tagGsFilePrimShapeData *mShapeData;
    u16 mKeyCount;
    GsKeyObject *mKeys;
    u16 mObjectCount;
    GsShapeObject *mObjects;
    u16 mTextureCount;
    GsShapeTexture *mTextures;
    u16 mDeformParameterUCount;
    u16 mDeformParameterVCount;
    u16 mTwistParameterCount;
    u16 mGradationParameterCount;
    f32 *mDeformParameterU;
    f32 *mDeformParameterV;
    f32 *mTwistParameter;
    tagGScol4ub *mGradationParameter;
    GStexture *mBlendTexture;
    s16 mUsedInstanceCount;
    s16 mNextInstance;
    u8 mInstanceUsed[256];
    u8 mReleasedBuffer;
    u8 mCurrentBuffer;
    u16 mToken[2][256];
    u32 mPolygonCount;
    u32 mUsedMemorySize;
    u32 mInstancedMemorySize;
    s16 mFreeTokenCount[2];
    s16 mFreeToken[2][256];
};

inline void GsShapeRender::FreeInstance() {
    for (s16 i = 0; i < mFreeTokenCount[mCurrentBuffer]; i++) {
        s16 instance = mFreeToken[mCurrentBuffer][i];
        if (mToken[!mCurrentBuffer][instance] != 0) {
            GSgfxTokenWaitForClear(mToken[!mCurrentBuffer][instance]);
        }
        if (mToken[mCurrentBuffer][instance] != 0) {
            GSgfxTokenWaitForClear(mToken[mCurrentBuffer][instance]);
        }
        mToken[0][instance] = 0;
        mToken[1][instance] = 0;
        for (u16 j = 0; j < mObjectCount; j++) {
            if (mObjects[j].FreeShapeDrawBuffer(instance) == 1) {
                tagGsFilePrimObjectEntry *entry = mObjects->mEntry;
                u32 size = entry->mVertexCount
                    * (sizeof(GSvec) + sizeof(tagGScol4ub) + entry->mTextureCount * sizeof(GSvec2))
                    * 2;
                mInstancedMemorySize -= size;
                g_instancedMemorySizeAll -= size;
            }
        }
        mUsedInstanceCount--;
        mInstanceUsed[instance] = 0;
    }
    mFreeTokenCount[mCurrentBuffer] = 0;
}

#endif
