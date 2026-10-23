#ifndef PE_SHAPE_H
#define PE_SHAPE_H

#include <GSmath.h>
#include <peBank.h>
#include <peEmitter.h>
#include <peContainer.h>
#include <peSpline.h>
#include <GSgfx.h>

enum peShapeType {
    PE_SHAPE_CUBE,
    PE_SHAPE_CYLINDER,
    PE_SHAPE_DODECAHEDRON,
    PE_SHAPE_ICOSAHEDRON,
    PE_SHAPE_OCTAHEDRON,
    PE_SHAPE_SPHERE,
    PE_SHAPE_SPHERE_SLICES,
    PE_SHAPE_TORUS
};

enum peShapeFlags {
    PE_SHAPE_TYPE_MASK = 7,
    PE_SHAPE_LAYER_4 = 0x8,
    PE_SHAPE_MIN_FILTER_LINEAR = 0x20,
    PE_SHAPE_MAG_FILTER_LINEAR = 0x40,
    PE_SHAPE_SORT = 0x80,
    PE_SHAPE_ROTATION_SPLINES = 0x100,
    PE_SHAPE_LIGHTING = 0x200,
    PE_SHAPE_SPECULAR = 0x400,
    PE_SHAPE_CULL_BACK = 0x800,
    PE_SHAPE_RANDOM_ROT_X = 0x1000,
    PE_SHAPE_RANDOM_ROT_Y = 0x2000,
    PE_SHAPE_RANDOM_ROT_Z = 0x4000,
    PE_SHAPE_RANDOM_SPEED_X = 0x8000,
    PE_SHAPE_RANDOM_SPEED_Y = 0x10000,
    PE_SHAPE_RANDOM_SPEED_Z = 0x20000,
    PE_SHAPE_REVERSE_ROT_X = 0x40000,
    PE_SHAPE_REVERSE_ROT_Y = 0x80000,
    PE_SHAPE_REVERSE_ROT_Z = 0x100000,
    PE_SHAPE_LAYER_0 = 0x200000,
    PE_SHAPE_LAYER_2 = 0x400000,
    PE_SHAPE_LAYER_1 = 0x800000,
    PE_SHAPE_LAYER_3 = 0x1000000,
    PE_SHAPE_LAYER_5 = 0x2000000,
    PE_SHAPE_LAYER_MASK = 0x3E00008
};

struct _peShape {
    u32 flags;
    GSvec rotation;
    GSvec rotationSpeed;
    GSvec scale;
    GSvec scaleRandom;
    peSpline *rotationSpline[3];
    peSpline *scaleSpline[3];
    peColorSpline *colorSpline;
    peSpline *alphaSpline;
    f32 torusRadius;
    u8 stacks;
    u8 slices;
    u8 reserved5A[2];
    GSvec lightPosition;
    GScolor ambientColor;
    GSgfxColAu8 lightColor;
    f32 shininess;
    void *texture;
    GSgfxLight *diffuseLight;
    GSgfxLight *specularLight;
};

class peShapeContainer : public peContainer {
public:
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);
    virtual void Start();
    virtual ~peShapeContainer();
    peShapeContainer(peEmitterNode *emitter, _peShape *shape, u16 capacity);

    bool CanRender();
    _peShape *shape;
};

/* Per-axis coin-flip result of the random reverse spin: the node negates the
 * matching quaternion component while the shape plays back. */
enum peReverseRotation {
    PE_REVERSE_ROTATION_X = 0x1,
    PE_REVERSE_ROTATION_Y = 0x2,
    PE_REVERSE_ROTATION_Z = 0x4
};

class peShapeNode : public peNode {
public:
    void Start(peEmitterNode *emitter, _peShape *shape, _EmitParams *params);
    bool Update(peEmitterNode *emitter, _peShape *shape, f32 delta);
    void Render(_peShape *shape);

    peNodeTransform renderTransform;
    f32 randomRotation[4];
    f32 rotation[4];
    GSvec scale;
    f32 sortDistance;
    u32 reverseRotation;
    GSvec rotationSpeed;
    GScolor color;
};

struct GSbound;
extern GSbound *g_currentBound;
extern GSmtx *g_CameraMatrix;
extern GSgfxLayer *g_current_layer;
extern GSgfxLayerPass g_render_pass;

#endif
