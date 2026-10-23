#ifndef PENODE_H
#define PENODE_H

#include <GSmath.h>

class peEmitterNode;
class _EmitParams;

struct peNodeTransform {
    GSvec position;
    f32 rotation[4];
    GSvec scale;
};

class peNode {
public:
    void Start(peEmitterNode *emitter, _EmitParams *params);
    bool Update(peEmitterNode *emitter, f32 delta);

    f32 life;
    f32 remainingLife;
    GSvec position;
    GSvec velocity;
    peNodeTransform *transform;
    f32 forceResponse;
    peNode *next;
    peNode *previous;
};

extern "C" void peNodeTransformsConcat(peNodeTransform *out, peNodeTransform *parent,
                                        peNodeTransform *local, bool localOffset);

#endif
