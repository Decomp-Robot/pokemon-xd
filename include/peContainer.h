#ifndef PECONTAINER_H
#define PECONTAINER_H

#include <GSgfx.h>
#include <peNode.h>

class peContainer {
public:
    peEmitterNode *emitter;
    peContainer *nextContainer;
    void *nodeAllocation;
    peNode *freeNodes;
    peNode *nodes;
    u16 nodeCount;
    u16 capacity;
    bool changed;
    bool started;

    peContainer(peEmitterNode *emitter, unsigned long nodeSize, u16 capacity);
    virtual ~peContainer();
    virtual void Start();
    virtual void PreUpdate();
    virtual u16 Emit(u16 count) = 0;
    virtual void Update(f32 delta) = 0;
    virtual void Render() = 0;
    virtual void Stop() = 0;

    static void SetLayerVisibility(GSgfxLayerID layer, bool visible);
    void RemoveAll();
    void RemoveNode(peNode *node);
    u16 CreateNodes(u16 count);
    void MoveNode(peNode *node, peNode *before);
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
    void ResetSwap();
    virtual void PreUpdate();
    virtual void Start() = 0;

    bool mSwapState;
    u8 mBufferIndex;
    u16 mReserved;
    u32 mRenderDataSize;
    GSgfxDL mDisplayLists[2];
    char *mRenderData[2];
    u16 mRenderTokens[2];
};

#endif
