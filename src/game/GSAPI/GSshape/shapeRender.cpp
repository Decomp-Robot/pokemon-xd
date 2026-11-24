#include <GSshapeRender.h>

u32 g_instancedMemorySizeAll;

void GsShapeRender::ExecuteFreeInstance() {
    if (mReleasedBuffer == mCurrentBuffer) {
        mCurrentBuffer = !mCurrentBuffer;
        FreeInstance();
        mCurrentBuffer = !mCurrentBuffer;
    }
    FreeInstance();
    mReleasedBuffer = mCurrentBuffer;
}

void GsShapeRender::SwapBuffers() {
    mCurrentBuffer = !mCurrentBuffer;
    ExecuteFreeInstance();
}

GSgfxLayerID GsShapeRender::GetLayerId() const { return mLayerId; }

u32 GsShapeRender::GetInstancedMemorySizeAll() { return mInstancedMemorySize; }

u32 GsShapeRender::GetUsedMemorySizeAll() { return mUsedMemorySize; }

void GsShapeRender::FreeShapeRenderDrawBuffer(s16 instance) {
    if (instance < 0 || instance >= 256) {
        return;
    }

    u8 nextBuffer = !mCurrentBuffer;
    if (mFreeTokenCount[nextBuffer] >= 256) {
        mCurrentBuffer = !mCurrentBuffer;
        FreeInstance();
        mCurrentBuffer = !mCurrentBuffer;
    }
    mFreeToken[nextBuffer][mFreeTokenCount[nextBuffer]] = instance;
    mFreeTokenCount[nextBuffer]++;
}

s16 GsShapeRender::CreateShapeRenderDrawBuffer() {
    FreeInstance();

    u16 slot;
    for (slot = 0; slot < 256; slot++) {
        if (mInstanceUsed[mNextInstance] == 0) {
            mInstanceUsed[mNextInstance] = 1;
            break;
        }
        mNextInstance++;
        if (mNextInstance >= 256) {
            mNextInstance = 0;
        }
    }
    if (slot == 256) {
        return -1;
    }
    if (mUsedInstanceCount >= 256) {
        return -1;
    }
    mUsedInstanceCount++;

    for (u16 object = 0; object < mObjectCount; object++) {
        if (mObjects[object].CreateShapeDrawBuffer(mNextInstance) == 0) {
            for (s32 j = 0; j < object; j++) {
                if (mObjects[j].FreeShapeDrawBuffer(mNextInstance) == 1) {
                    tagGsFilePrimObjectEntry *entry = mObjects->mEntry;
                    u32 size = entry->mVertexCount
                        * (sizeof(GSvec) + sizeof(tagGScol4ub) + entry->mTextureCount * sizeof(GSvec2))
                        * 2;
                    mInstancedMemorySize -= size;
                    g_instancedMemorySizeAll -= size;
                }
            }
            return -1;
        }
        tagGsFilePrimObjectEntry *entry = mObjects->mEntry;
        u32 size = entry->mVertexCount
            * (sizeof(GSvec) + sizeof(tagGScol4ub) + entry->mTextureCount * sizeof(GSvec2)) * 2;
        mInstancedMemorySize += size;
        g_instancedMemorySizeAll += size;
    }
    return mNextInstance;
}
