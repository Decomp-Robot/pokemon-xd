#ifndef PE_PARTICLE_CONTAINER_H
#define PE_PARTICLE_CONTAINER_H

#include <peNode.h>

class peContainer;

struct peNodeList {
    peEmitterNode *mParent;
    peContainer *mNext;
    void *mNodeAllocation;
    peNode *mFreeNodes;
    peNode *mNodes;
    u16 mNodeCount;
    u16 mMaxNodes;
    bool mChanged;
    bool mStarted;
};

class peContainer : public peNodeList {
public:
    peContainer(peEmitterNode *parent, unsigned long nodeSize, u16 maxNodes);
    virtual ~peContainer();
    virtual void Start();
    virtual void PreUpdate();
    virtual u16 Emit(u16 count) = 0;
    virtual void Update(f32 delta) = 0;
    virtual void Render() = 0;
    virtual void Stop() = 0;
    void RemoveNode(peNode *node);
    void RemoveAll();
    u16 CreateNodes(u16 count);
};

class peRenderContainer : public peContainer {
public:
    peRenderContainer(peEmitterNode *parent, unsigned long nodeSize, u16 maxNodes,
                      GSgfxVF *format, GSgfxPrimType primitive, u8 vertices,
                      u8 renderStride, bool doubleBuffer);
    virtual ~peRenderContainer();
    void InitSwap();
    void ResolveMultipleSwap();
    void MarkForSwap();
    void StopSwap();
    virtual void PreUpdate();
    virtual void Start() = 0;

    u8 mSwapState;
    u8 mBufferIndex;
    u16 mReserved;
    u32 mRenderDataSize;
    GSgfxDL mDisplayLists[2];
    char *mRenderData[2];
    u16 mRenderTokens[2];
};

class peParticleContainer : public peRenderContainer {
public:
    peParticleContainer(peEmitterNode *parent, _peParticle *particle, u16 maxNodes);
    virtual ~peParticleContainer();
    void DepthSort();
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);
    virtual void Start();

    int PreviousActive(int index) {
        if (!mSortedNodes)
            return -1;
        while (index > 0) {
            --index;
            if (mSortedNodes[index]->mActive)
                return index;
        }
        return -1;
    }
    bool RenderForLayer();

    peParticleNode **mSortedNodes;
    _peParticle *mParticle;
    f32 mTexCoords[8];
};

#endif
