#include <math.h>

#include <GScamera.h>
#include <GSgfx.h>
#include <GSmem.h>
#include <GSmsg.h>
#include <GSmtx.h>
#include <GSshape.h>
#include <GSshapeRender.h>

#include <dolphin/gx/GXVert.h>
#include <dolphin/mtx.h>

enum GsShapeAnimation {
    GS_SHAPE_ANIM_PLAY,
    GS_SHAPE_ANIM_STEP,
    GS_SHAPE_ANIM_HOLD,
};

static const u32 GS_SHAPE_PERFBAR_EDITOR = 0x00FF00;
static const u8 GS_SHAPE_RENDER_PRIORITY = 80;
static const u8 GS_SHAPE_EDITOR_AXIS_WIDTH = 6;
static const u16 GS_SHAPE_EDITOR_AXIS_VERTICES = 6;

static bool _shapeEditorActive;
static bool _shapeRenderModuleRegistered;
f32 g_primitiveFrameSec;
u16 g_primitiveAnimation;
u16 g_shapeRenderNum;
GsShapeRender **g_shapeRender;
s16 g_primitiveInstanceId = -1;

static inline GSgfxLayerID shapeGetEditorLayerID()
{
    if (!_shapeEditorActive) {
        return GSGFX_LAYER_DEFAULT;
    }
    if (g_shapeRenderNum != 0 && g_shapeRender[0] != 0 && g_shapeRender[0]->mShapeData != 0) {
        return g_shapeRender[0]->GetLayerId();
    }
    return GSGFX_LAYER_DEFAULT;
}

static inline void shapeUpdateAll()
{
    for (u16 i = 0; i < g_shapeRenderNum; i++) {
        if (g_shapeRender[i] == 0) {
            continue;
        }
        s16 instance = g_primitiveInstanceId;
        shapeUpdate(i, instance, g_primitiveFrameSec);
    }
}

static inline void shapeReleaseAll()
{
    for (u16 i = 0; i < g_shapeRenderNum; i++) {
        if (g_shapeRender[i] == 0) {
            continue;
        }
        g_shapeRender[i]->FreeShapeRender();
        GSmemFree(g_shapeRender[i]);
        g_shapeRender[i] = 0;
    }
}

void _shapePreUpdate(f32 delta)
{
    for (u16 i = 0; i < g_shapeRenderNum; i++) {
        if (g_shapeRender[i] != 0 && g_shapeRender[i]->mShapeData != 0) {
            g_shapeRender[i]->SwapBuffers();
        }
    }
}

void _shapeFastRender()
{
    if (!_shapeEditorActive) return;
    GSgfxLayerID layerID = shapeGetEditorLayerID();
    u32 perfbarEntry = GSgfxPerfbarCPUEntry(GS_SHAPE_PERFBAR_EDITOR);
    shapePrimitiveEditorRender(layerID);
    GSgfxPerfbarCPUEntry(perfbarEntry);
}

bool shapeInit(u16 count)
{
    _shapeEditorActive = false;
    shapeReleaseAll();
    if (g_shapeRender != 0) {
        GSmemFree(g_shapeRender);
        g_shapeRender = 0;
    }
    g_shapeRenderNum = 0;

    if (!_shapeRenderModuleRegistered) {
        GSgfxRenderModule module = { 0, _shapePreUpdate, 0, _shapeFastRender, 0, 0,
                                     GSGFX_RENDER_MODULE_SIGNATURE };
        GSgfxRenderModuleRegister(GS_SHAPE_RENDER_PRIORITY, &module, 0);
        _shapeRenderModuleRegistered = true;
    }

    g_shapeRender = (GsShapeRender **)GSmemAllocClr(count * sizeof(GsShapeRender *));
    if (g_shapeRender == 0) {
        g_shapeRenderNum = 0;
        return false;
    }
    g_shapeRenderNum = count;
    return true;
}

bool shapeLoad(u16 shape, void *data)
{
    if (shape >= g_shapeRenderNum) return false;
    if (g_shapeRender[shape] != 0) GSmemFree(g_shapeRender[shape]);
    g_shapeRender[shape] = (GsShapeRender *)GSmemAlloc(sizeof(GsShapeRender));
    if (g_shapeRender[shape] == 0) return false;
    g_shapeRender[shape]->InitShapeRender();
    g_shapeRender[shape]->LoadShapeRender(data);
    return true;
}

void shapeUpdate(u16 shape, s16 instance, f32 time)
{
    if (shape >= g_shapeRenderNum) return;
    if (g_shapeRender[shape] == 0) return;
    g_shapeRender[shape]->Update(time, instance);
}

void shapeRender(u16 shape, s16 instance, GSgfxLayerID layer)
{
    if (shape >= g_shapeRenderNum) return;
    if (g_shapeRender[shape] == 0) return;
    g_shapeRender[shape]->Render(instance, layer);
}

void shapeRelease(u16 shape)
{
    if (shape >= g_shapeRenderNum) return;
    if (g_shapeRender[shape] == 0) return;
    g_shapeRender[shape]->FreeShapeRender();
    GSmemFree(g_shapeRender[shape]);
    g_shapeRender[shape] = 0;
}

s32 shapeGenerateIndex()
{
    for (u16 i = 0; i < g_shapeRenderNum; i++) {
        if (g_shapeRender[i] == 0) {
            return i;
        }
        if (g_shapeRender[i]->mShapeData == 0) {
            return i;
        }
    }
    return -1;
}

s16 shapeCreatePrimitiveInstance(u16 shape)
{
    if (shape >= g_shapeRenderNum) return -1;
    if (g_shapeRender[shape] == 0) return -1;
    return g_shapeRender[shape]->CreateShapeRenderDrawBuffer();
}

void shapeFreePrimitiveInstance(u16 shape, s16 instance)
{
    if (shape >= g_shapeRenderNum) return;
    if (g_shapeRender[shape] == 0) return;
    g_shapeRender[shape]->FreeShapeRenderDrawBuffer(instance);
}

u32 shapeGetUsedMemorySize(u16 shape)
{
    if (shape >= g_shapeRenderNum) return 0;
    if (g_shapeRender[shape] == 0) return 0;
    return g_shapeRender[shape]->GetUsedMemorySizeAll();
}

u32 shapeGetInstancedMemorySizeAll()
{
    return g_instancedMemorySizeAll;
}

void shapePrimitiveEditorRender(GSgfxLayerID layerID)
{
    GSmtx matrix;
    GSgfxLayer *layer;

    if (!_shapeEditorActive) return;
    if (shapeGetEditorLayerID() != layerID) return;

    switch (g_primitiveAnimation) {
    case GS_SHAPE_ANIM_PLAY:
        g_primitiveFrameSec += GSgfxVideoGetLastRenderTime() / GSgfxVideoGetVsyncRate();
        shapeUpdateAll();
        break;
    case GS_SHAPE_ANIM_STEP:
        shapeUpdateAll();
        if (g_shapeRenderNum != 0 && g_shapeRender[0] != 0 && g_shapeRender[0]->mShapeData != 0 &&
            g_shapeRender[0]->mShapeData->mFrameRate >= 1) {
            g_primitiveFrameSec += 1.0f / g_shapeRender[0]->mShapeData->mFrameRate;
        }
        g_primitiveAnimation = GS_SHAPE_ANIM_HOLD;
        break;
    case GS_SHAPE_ANIM_HOLD:
        break;
    }

    layer = GSgfxLayerFindByID(layerID);
    GScameraLoadActiveCameraMatrix(layerID);

    PSMTXRotRad(matrix.sdk(), 'Y', -M_PI / 2);
    PSMTXTransApply(matrix.sdk(), matrix.sdk(), 0.0f, 0.1f, 0.0f);
    layer->mMatrixStack->pushMult(&matrix);

    layer->setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_POSITION_COLOR));

    layer->cullMode(GSGFX_CULL_NONE);
    layer->mPrimitiveType = GSGFX_PRIM_LINES;
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->lineWidth(GS_SHAPE_EDITOR_AXIS_WIDTH, GX_TO_ZERO);
    layer->beginSurface(GS_SHAPE_EDITOR_AXIS_VERTICES);

    f32 origin = 0.0f;
    f32 axisLength = 10.0f;

    GXPosition3f32(origin, origin, origin);
    GXColor4u8(255, 0, 0, 255);
    GXPosition3f32(axisLength, origin, origin);
    GXColor4u8(255, 0, 0, 255);
    GXPosition3f32(origin, origin, origin);
    GXColor4u8(0, 255, 0, 255);
    GXPosition3f32(origin, axisLength, origin);
    GXColor4u8(0, 255, 0, 255);
    GXPosition3f32(origin, origin, origin);
    GXColor4u8(0, 0, 255, 255);
    GXPosition3f32(origin, origin, axisLength);
    GXColor4u8(0, 0, 255, 255);

    layer->endSurface();

    s32 usedMemory = 0;
    s32 instancedMemory = 0;
    for (u16 i = 0; i < g_shapeRenderNum; i++) {
        if (g_shapeRender[i] == 0) {
            continue;
        }
        if (g_shapeRender[i]->mEnable == true && g_shapeRender[i]->mShapeData != 0) {
            g_shapeRender[i]->mShapeData->mPlayMode = GS_SHAPE_PLAY_LOOP;
        }
        shapeRender(i, g_primitiveInstanceId, layerID);
        usedMemory += g_shapeRender[i]->GetUsedMemorySizeAll();
        instancedMemory += g_shapeRender[i]->GetInstancedMemorySizeAll();
    }

    GSprint(shapeGetEditorLayerID(), 550, 30, 0xFFFFFFFF, "%dKB", instancedMemory / 1024);

    layer->mMatrixStack->pop();
}
