#include <peShape.h>
#include <particleEngine.h>
#include <GSbound.h>
#include <HSD_Rand.h>
#include <math.h>
#include <dolphin/os/OSFastCast.h>

static inline void SetVector(GSvec *out, f32 x, f32 y, f32 z)
{
    out->x = x; out->y = y; out->z = z;
}

static inline f32 ShapeSplineValue(peSpline *spline, f32 time)
{
    if (!spline || (spline->flags & PE_SPLINE_MODE_MASK) == PE_SPLINE_MODE_NONE) return 1.0f;
    f32 distance = spline->time - time;
    distance = distance < 0.0f ? -distance : distance;
    if (distance <= 0.00001f) return spline->value;
    return peSplineGetValue(spline, time);
}

static inline u8 ColorByte(f32 component)
{
    u8 result;
    OSf32tou8(&component, &result);
    return result;
}

static inline f32 ColorComponent(u8 component)
{
    f32 value;
    OSu8tof32(&component, &value);
    return value;
}

static inline void CopyVector(GSvec *out, const GSvec *in)
{
    out->x = in->x; out->y = in->y; out->z = in->z;
}

inline bool peShapeContainer::CanRender()
{
    if (!started) return false;
    u32 flags = shape->flags;
    if (!nodeCount) return false;
    if (g_render_pass != GSGFX_LAYER_PASS_XLU) return false;
    switch (g_current_layer->mLayerID) {
    case 0: if (flags & PE_SHAPE_LAYER_0) return true; break;
    case 2:
        if (flags & PE_SHAPE_LAYER_2) return true;
        if (!(flags & PE_SHAPE_LAYER_MASK)) return true;
        break;
    case 4: if (flags & PE_SHAPE_LAYER_4) return true; break;
    case 1: if (flags & PE_SHAPE_LAYER_1) return true; break;
    case 3: if (flags & PE_SHAPE_LAYER_3) return true; break;
    case 5: if (flags & PE_SHAPE_LAYER_5) return true; break;
    default: return false;
    }
    return false;
}

void peShapeNode::Start(peEmitterNode *emitter, _peShape *shape, _EmitParams *params)
{
    GSvec angles;
    GSvec randomScale;
    u32 flags = shape->flags;
    peNode::Start(emitter, params);
    reverseRotation = 0;
    PSVECScale((const Vec *)&shape->scaleRandom, (Vec *)&randomScale, HSD_Randf());
    f32 scaleX = randomScale.x;
    f32 scaleY = randomScale.y;
    f32 scaleZ = randomScale.z;
    SetVector(&scale, scaleX, scaleY, scaleZ);
    PSVECAdd((const Vec *)&scale, (const Vec *)&shape->scale, (Vec *)&scale);
    randomRotation[0] = 0.0f; randomRotation[1] = 0.0f; randomRotation[2] = 0.0f;
    randomRotation[3] = 1.0f;
    renderTransform.position.x = 0.0f; renderTransform.position.y = 0.0f; renderTransform.position.z = 0.0f;
    renderTransform.rotation[0] = 0.0f; renderTransform.rotation[1] = 0.0f; renderTransform.rotation[2] = 0.0f;
    renderTransform.rotation[3] = 1.0f;
    CopyVector(&renderTransform.scale, &unitScaleVector);
    if (!(emitter->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY)) {
        EulerToQuat(&shape->rotation, rotation);
        if (flags & (PE_SHAPE_RANDOM_ROT_X | PE_SHAPE_RANDOM_ROT_Y | PE_SHAPE_RANDOM_ROT_Z |
                     PE_SHAPE_REVERSE_ROT_X | PE_SHAPE_REVERSE_ROT_Y | PE_SHAPE_REVERSE_ROT_Z)) {
            if (flags & PE_SHAPE_RANDOM_ROT_X) {
                f32 angleX = 6.28318530718f * HSD_Randf();
                angles.x = angleX;
            } else angles.x = 0.0f;
            if (flags & PE_SHAPE_RANDOM_ROT_Y) angles.y = 6.28318530718f * HSD_Randf();
            else angles.y = 0.0f;
            if (flags & PE_SHAPE_RANDOM_ROT_Z) angles.z = 6.28318530718f * HSD_Randf();
            else angles.z = 0.0f;
            if ((flags & PE_SHAPE_REVERSE_ROT_X) && HSD_Randf() < 0.5f) reverseRotation |= PE_REVERSE_ROTATION_X;
            if ((flags & PE_SHAPE_REVERSE_ROT_Y) && HSD_Randf() < 0.5f) reverseRotation |= PE_REVERSE_ROTATION_Y;
            if ((flags & PE_SHAPE_REVERSE_ROT_Z) && HSD_Randf() < 0.5f) reverseRotation |= PE_REVERSE_ROTATION_Z;
            EulerToQuat(&angles, randomRotation);
        }
    }
    rotationSpeed = shape->rotationSpeed;
    if (flags & PE_SHAPE_RANDOM_SPEED_X) rotationSpeed.x *= HSD_Randf();
    if (flags & PE_SHAPE_RANDOM_SPEED_Y) rotationSpeed.y *= HSD_Randf();
    if (flags & PE_SHAPE_RANDOM_SPEED_Z) rotationSpeed.z *= HSD_Randf();
}

bool peShapeNode::Update(peEmitterNode *emitter, _peShape *shape, f32 delta)
{
    peNodeTransform local;
    f32 matrix[3][4];
    GSvec workVector;
    f32 reversed[4];
    GSvec direction;
    f32 increment[4];
    GSvec sortOffset;
    GSvec cameraOffset;
    GSvec step;
    f32 combined[4];
    if (!peNode::Update(emitter, delta)) return false;
    f32 time = 1.0f - remainingLife / life;
    if (emitter->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY) {
        CopyVector(&direction, &velocity);
        if (emitter->emitter->flags & PE_EMITTER_LOCAL_VELOCITY) {
            PSMTXQuat(matrix, (const Quaternion *)transform->rotation);
            PSMTXMultVecSR(matrix, (const Vec *)&direction, (Vec *)&direction);
        }
        workVector.x = atan2(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
        workVector.y = atan2(direction.x, direction.z);
        workVector.z = 0.0f;
        EulerToQuat(&workVector, rotation);
    } else if (shape->flags & PE_SHAPE_ROTATION_SPLINES) {
        workVector.x = shape->rotation.x * ShapeSplineValue(shape->rotationSpline[0], time);
        workVector.y = shape->rotation.y * ShapeSplineValue(shape->rotationSpline[1], time);
        workVector.z = shape->rotation.z * ShapeSplineValue(shape->rotationSpline[2], time);
        EulerToQuat(&workVector, rotation);
    } else {
        PSVECScale((const Vec *)&rotationSpeed, (Vec *)&step, delta);
        CopyVector(&workVector, &step);
        EulerToQuat(&workVector, increment);
        PSQUATMultiply((const Quaternion *)rotation, (const Quaternion *)increment, (Quaternion *)rotation);
    }
    workVector.x = scale.x * ShapeSplineValue(shape->scaleSpline[0], time);
    workVector.y = scale.y * ShapeSplineValue(shape->scaleSpline[1], time);
    workVector.z = scale.z * ShapeSplineValue(shape->scaleSpline[2], time);
    peColorSplineGetValue(&color, shape->colorSpline, time);
    color.channels.r = ColorByte(emitter->colorScale[0] * ColorComponent(color.channels.r));
    color.channels.g = ColorByte(emitter->colorScale[1] * ColorComponent(color.channels.g));
    color.channels.b = ColorByte(emitter->colorScale[2] * ColorComponent(color.channels.b));
    color.channels.a = ColorByte(ShapeSplineValue(shape->alphaSpline, time) *
                                  (255.0f * emitter->colorScale[3]));
    reversed[0] = reverseRotation & PE_REVERSE_ROTATION_X ? -rotation[0] : rotation[0];
    if (reverseRotation & PE_REVERSE_ROTATION_Y) reversed[1] = -rotation[1];
    else reversed[1] = rotation[1];
    if (reverseRotation & PE_REVERSE_ROTATION_Z) reversed[2] = -rotation[2];
    else reversed[2] = rotation[2];
    reversed[3] = rotation[3];
    CopyVector(&local.position, &position);
    PSQUATMultiply((const Quaternion *)randomRotation, (const Quaternion *)reversed, (Quaternion *)combined);
    local.rotation[0] = combined[0]; local.rotation[1] = combined[1];
    local.rotation[2] = combined[2]; local.rotation[3] = combined[3];
    CopyVector(&local.scale, &workVector);
    peNodeTransformsConcat(&renderTransform, transform, &local, false);
    PSVECSubtract((const Vec *)&renderTransform.position, (const Vec *)&g_CameraPosition, (Vec *)&cameraOffset);
    CopyVector(&sortOffset, &cameraOffset);
    sortDistance = PSVECDotProduct((const Vec *)&g_CameraLookDir, (const Vec *)&sortOffset);
    return true;
}

void peShapeNode::Render(_peShape *shape)
{
    u32 flags = shape->flags;
    GScolor black = {0};
    g_current_layer->tunitKColor(0, color);
    if (flags & PE_SHAPE_LIGHTING) {
        g_current_layer->cunitColor(0, shape->ambientColor, color);
        if (flags & PE_SHAPE_SPECULAR)
            g_current_layer->cunitColor(1, black, color);
    }
    f32 matrix[3][4];
    f32 rotationMatrix[3][4];
    PSMTXQuat(rotationMatrix, (const Quaternion *)renderTransform.rotation);
    PSMTXScale(matrix, renderTransform.scale.x, renderTransform.scale.y, renderTransform.scale.z);
    PSMTXConcat(rotationMatrix, matrix, matrix);
    PSMTXTransApply(matrix, matrix, renderTransform.position.x,
                    renderTransform.position.y, renderTransform.position.z);
    g_current_layer->mMatrixStack->pushMult((GSmtx *)matrix);
    switch (flags & PE_SHAPE_TYPE_MASK) {
    case PE_SHAPE_CUBE: g_current_layer->drawCube(); break;
    case PE_SHAPE_CYLINDER: g_current_layer->drawCylinder(shape->slices); break;
    case PE_SHAPE_DODECAHEDRON: g_current_layer->drawDodecaedron(); break;
    case PE_SHAPE_ICOSAHEDRON: g_current_layer->drawIcosahedron(); break;
    case PE_SHAPE_OCTAHEDRON: g_current_layer->drawOctahedron(); break;
    case PE_SHAPE_TORUS: g_current_layer->drawTorus(shape->torusRadius, shape->stacks, shape->slices); break;
    case PE_SHAPE_SPHERE_SLICES: g_current_layer->drawSphere(shape->slices); break;
    case PE_SHAPE_SPHERE:
    default: g_current_layer->drawSphere(shape->stacks, shape->slices); break;
    }
    g_current_layer->mMatrixStack->pop();
}

peShapeContainer::peShapeContainer(peEmitterNode *emitter, _peShape *shape, u16 capacity)
    : peContainer(emitter, sizeof(peShapeNode), capacity), shape(shape)
{
    g_particle_debug_stats.containerBytes += sizeof(peShapeContainer);
    g_particle_debug_stats.totalBytes += sizeof(peShapeContainer);
}

peShapeContainer::~peShapeContainer()
{
    g_particle_debug_stats.containerBytes -= sizeof(peShapeContainer);
    g_particle_debug_stats.totalBytes -= sizeof(peShapeContainer);
}

void peShapeContainer::Start()
{
    if (!started) {
        started = true;
        if (shape->flags & PE_SHAPE_LAYER_1) SetLayerVisibility((GSgfxLayerID)1, true);
        if (shape->flags & PE_SHAPE_LAYER_3) SetLayerVisibility((GSgfxLayerID)3, true);
        if (shape->flags & PE_SHAPE_LAYER_5) SetLayerVisibility((GSgfxLayerID)5, true);
    }
}

u16 peShapeContainer::Emit(u16 count)
{
    if (!started) return 0;
    u16 remaining = CreateNodes(count);
    _EmitParams *params = g_emitParams;
    peShapeNode *node = (peShapeNode *)nodes;
    peShapeNode *next;
    u16 emitted = remaining;
    while (remaining) {
        next = (peShapeNode *)node->next;
        node->Start(emitter, shape, params);
        node = next;
        --remaining;
        ++params;
    }
    return emitted;
}

void peShapeContainer::Update(f32 delta)
{
    if (!started) return;
    peShapeNode *node = (peShapeNode *)nodes;
    peShapeNode *next;
    while (node) {
        next = (peShapeNode *)node->next;
        if (!node->Update(emitter, shape, delta)) RemoveNode(node);
        node = next;
    }
    if (g_currentBound && nodes)
        GSboundAlwaysInView(g_currentBound, true);
    if (started && (shape->flags & PE_SHAPE_SORT) && nodeCount >= 2) {
        node = (peShapeNode *)nodes->next;
        while (node) {
            f32 distance = node->sortDistance;
            peShapeNode *before = node;
            peShapeNode *scan = (peShapeNode *)node->previous;
            while (scan && scan->sortDistance < distance) {
                before = scan;
                scan = (peShapeNode *)scan->previous;
            }
            next = (peShapeNode *)node->next;
            if (before != node) MoveNode(node, before);
            node = next;
        }
    }
}

void GSgfxLayer::tunitKRegSel(u8 stage, GSgfxTunitKReg reg)
{
    switch (reg) {
    case GSGFX_KREG0: tunitKColorSel(stage, GSGFX_KCOLOR_K0); tunitKAlphaSel(stage, GSGFX_KALPHA_K0); break;
    case GSGFX_KREG1: tunitKColorSel(stage, GSGFX_KCOLOR_K1); tunitKAlphaSel(stage, GSGFX_KALPHA_K1); break;
    case GSGFX_KREG2: tunitKColorSel(stage, GSGFX_KCOLOR_K2); tunitKAlphaSel(stage, GSGFX_KALPHA_K2); break;
    case GSGFX_KREG3: tunitKColorSel(stage, GSGFX_KCOLOR_K3); tunitKAlphaSel(stage, GSGFX_KALPHA_K3); break;
    }
}

void peShapeContainer::Render()
{
    if (started) {
        if (CanRender()) {
            u32 flags = shape->flags;
            if (flags & PE_SHAPE_CULL_BACK) g_current_layer->cullMode(GSGFX_CULL_BACK);
            else g_current_layer->cullMode(GSGFX_CULL_NONE);
            particleEngineSetupTexture(0, shape->texture, 0, 0,
                (flags & PE_SHAPE_MIN_FILTER_LINEAR) ? GS_TEXTURE_FILTER_LINEAR : GS_TEXTURE_FILTER_NEAREST,
                (flags & PE_SHAPE_MAG_FILTER_LINEAR) ? GS_TEXTURE_FILTER_LINEAR : GS_TEXTURE_FILTER_NEAREST);
            particleEngineSetupBlendMode(2);
            if (flags & PE_SHAPE_LIGHTING) {
                GSgfxLight *diffuse = shape->diffuseLight;
                GSgfxLight *specular;
                GSgfxLight *lights[2] = { 0, 0 };
                u8 lightCount = 1;
                lights[0] = diffuse;
                g_current_layer->tunitCount(1);
                g_current_layer->cunitCount(1);
                g_current_layer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
                g_current_layer->tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
                g_current_layer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_RASC,
                                                        GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ZERO);
                g_current_layer->tunitKAlphaSel(0, GSGFX_KALPHA_K0);
                g_current_layer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_KONST,
                                                        GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
                GSvec lightPosition;
                GSvec direction;
                GSvec worldPosition;
                PSMTXMultVec((MtxPtr)g_CameraMatrix, (const Vec *)&shape->lightPosition, (Vec *)&worldPosition);
                GSvec *lightPos = &lightPosition;
                *lightPos = worldPosition;
                diffuse->position(lightPos);
                g_current_layer->cunitChannelGen(0, GSGFX_CUNIT_COLOR,
                    GSGFX_CUNIT_REGISTER, GSGFX_CUNIT_REGISTER, 1, &diffuse,
                    GSGFX_LIGHT_POINT, GSGFX_DIFFUSE_CLAMP);
                if (flags & PE_SHAPE_SPECULAR) {
                    specular = shape->specularLight;
                    lights[1] = specular;
                    g_current_layer->tunitCount(2);
                    lightCount = 2;
                    g_current_layer->tunitMapping(1, GSGFX_TUNIT_NO_TEXTURE, GSGFX_TUNIT_NO_TEXTURE, GSGFX_TUNIT_CHANNEL_COLOR1A);
                    g_current_layer->tunitColorOp(1, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
                    g_current_layer->tunitAlphaOp(1, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, false, GSGFX_TUNIT_REG_PREV);
                    g_current_layer->tunitColorArg(1, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_RASC,
                                                            GSGFX_TUNIT_C_ONE, GSGFX_TUNIT_C_PREV);
                    g_current_layer->tunitAlphaArg(1, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO,
                                                            GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_PREV);
                    g_current_layer->cunitCount(2);
                    g_current_layer->cunitChannelGen(1, GSGFX_CUNIT_COLOR,
                        GSGFX_CUNIT_REGISTER, GSGFX_CUNIT_REGISTER, 1, &specular,
                        GSGFX_LIGHT_DIRECTIONAL, GSGFX_DIFFUSE_CLAMP);
                    if (PSVECSquareMag((const Vec *)&shape->lightPosition) < 0.00001f) {
                        direction.x = 0.0f; direction.y = -1.0f; direction.z = 0.0f;
                    } else {
                        GSvec negatedPosition;
                        negatedPosition.x = -shape->lightPosition.x;
                        negatedPosition.y = -shape->lightPosition.y;
                        negatedPosition.z = -shape->lightPosition.z;
                        direction = negatedPosition;
                        PSVECNormalize((const Vec *)&direction, (Vec *)&direction);
                    }
                    PSMTXMultVecSR((MtxPtr)g_CameraMatrix, (const Vec *)&direction, (Vec *)&direction);
                    specular->direction(&direction);
                }
                g_current_layer->cunitLights(lightCount, lights);
            } else {
                g_current_layer->cunitCount(0);
                g_current_layer->tunitCount(1);
                g_current_layer->tunitMapping(0, 0, 0, GSGFX_TUNIT_NO_TEXTURE);
                g_current_layer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
                g_current_layer->tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
                g_current_layer->tunitKRegSel(0, GSGFX_KREG0);
                g_current_layer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_KONST,
                                                        GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ZERO);
                g_current_layer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_KONST,
                                                        GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
            }
            g_current_layer->mVertexFormat = GSgfxVFGetPredef(4);
            for (peShapeNode *node = (peShapeNode *)nodes; node; node = (peShapeNode *)node->next)
                node->Render(shape);
            particleEngineSetDefaultRenderState(true);
        }
    }
}

void peShapeContainer::Stop()
{
    if (started) {
        PreUpdate();
        started = false;
        if (shape->flags & PE_SHAPE_LAYER_1) SetLayerVisibility((GSgfxLayerID)1, false);
        if (shape->flags & PE_SHAPE_LAYER_3) SetLayerVisibility((GSgfxLayerID)3, false);
        if (shape->flags & PE_SHAPE_LAYER_5) SetLayerVisibility((GSgfxLayerID)5, false);
        RemoveAll();
    }
}
