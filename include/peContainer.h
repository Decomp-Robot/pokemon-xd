#ifndef PECONTAINER_H
#define PECONTAINER_H

#include <GSgfx.h>
#include <peNode.h>

class peContainer;

struct peNodeList {
    peEmitterNode *emitter;
    peContainer *nextContainer;
    void *nodeAllocation;
    peNode *freeNodes;
    peNode *nodes;
    u16 nodeCount;
    u16 capacity;
    bool changed;
    bool started;
};

class peContainer : public peNodeList {
public:

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

#endif
