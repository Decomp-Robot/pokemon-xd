#include <sprite.h>
#include <GSgfx.h>
#include <GSmath.h>
#include <GSmsg.h>
#include <floorLayerConfig.h>
#include <libc/math.h>

struct SpriteWork {
    f32 offsetX;
    f32 offsetY;
    f32 scaleX;
    f32 scaleY;
};

SpriteWork _spriteWork[3] = {
    {0.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f}
};

static inline s16 spriteWorkIndex(s32 layerID)
{
    if (layerID == floorGetMenuLayerID()) return 0;
    if (layerID == GSGFX_LAYER_MENU) return 1;
    return 2;
}

void spriteClearScissor(s32 layerID)
{
    _TargetLayer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    GSgfxLayer *layer = _TargetLayer;
    if (layer) {
        layer->mProjectionState.mScissorLeft = 0;
        layer->mProjectionState.mScissorTop = 0;
        GSgfxVideoGetEFBSize(&layer->mProjectionState.mScissorWidth,
                             &layer->mProjectionState.mScissorHeight);
        layer->mPassState[0].mGroupBFlags |= GS_GFX_RASTER_DIRTY_SCISSOR;
        layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        layer->mPassState[1].mGroupBFlags |= GS_GFX_RASTER_DIRTY_SCISSOR;
        layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    }
}

void spriteSetScissor(s32 layerID, s32 x, s32 y, s32 width, s32 height)
{
    SpriteWork *work = &_spriteWork[spriteWorkIndex(layerID)];
    _TargetLayer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    GSgfxLayer *layer = _TargetLayer;
    if (layer) {
        x = work->offsetX + x;
        y = work->offsetY + y;
        x *= work->scaleX;
        y *= work->scaleY;
        width *= work->scaleX;
        height *= work->scaleY;
        if (x >= 640) x = 640;
        if (y >= 480) y = 480;
        if (width >= 640) width = 640;
        if (height >= 480) height = 480;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (width < 0) width = 0;
        if (height < 0) height = 0;
        layer->mProjectionState.mScissorLeft = x;
        layer->mProjectionState.mScissorTop = y;
        layer->mProjectionState.mScissorWidth = width;
        layer->mProjectionState.mScissorHeight = height;
        layer->mPassState[0].mGroupBFlags |= GS_GFX_RASTER_DIRTY_SCISSOR;
        layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        layer->mPassState[1].mGroupBFlags |= GS_GFX_RASTER_DIRTY_SCISSOR;
        layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    }
}

void spriteClearEnv(s32 layerID)
{
    _TargetLayer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    if (_TargetLayer) {
        _TargetLayer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA,
                               GSGFX_BLEND_INV_SRC_ALPHA, true);
    }
}

void spriteSetEnv(s32 layerID)
{
    SpriteWork *work = &_spriteWork[spriteWorkIndex(layerID)];
    _TargetLayer = GSgfxLayerFindByID((GSgfxLayerID)layerID);
    if (_TargetLayer) {
        f32 tangent;
        GSmtx view;
        f32 projection[4][4];
        GSvec eye, target, up;
        f32 halfWidth, halfHeight;
        f32 width = 640.0f / work->scaleX;
        f32 height = 480.0f / work->scaleY;
        halfWidth = width / 2.0f;
        halfHeight = height / 2.0f;
        f32 angle = 0.017453292f * 30.0f;
        tangent = tan(angle / 2.0f);
        eye.set(halfWidth - work->offsetX, halfHeight - work->offsetY, halfHeight / tangent);
        target.set(halfWidth - work->offsetX, halfHeight - work->offsetY, 0.0f);
        up.set(0.0f, -1.0f, 0.0f);
        C_MTXLookAt(view.m, (Vec *)&eye, (Vec *)&up, (Vec *)&target);
        GSgfxLayer *layer = _TargetLayer;
        GSmtx44Perspective(projection, 30.0f, -(width / height), 0.1f, 30000.0f);
        layer->perspectiveProjection(projection);
        layer->projectionDepthRange(0.1f, 30000.0f);
        _TargetLayer->mMatrixStack->identity();
        _TargetLayer->mMatrixStack->mult(&view);
        _TargetLayer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA,
                               GSGFX_BLEND_INV_SRC_ALPHA, true);
        _TargetLayer->pixelMode(true, false, true);
        _TargetLayer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0,
                                  GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
        _TargetLayer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
        _TargetLayer->cullMode(GSGFX_CULL_NONE);
    }
}

void spriteGetOffset(s32 layerID, s16 *x, s16 *y)
{
    SpriteWork *work = &_spriteWork[spriteWorkIndex(layerID)];
    if (x) *x = work->offsetX;
    if (y) *y = work->offsetY;
}

void spriteSetOffset(s32 layerID, s16 x, s16 y)
{
    SpriteWork *work = &_spriteWork[spriteWorkIndex(layerID)];
    work->offsetX = x;
    work->offsetY = y;
}
