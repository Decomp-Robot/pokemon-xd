#include <GSprim.h>
#include <GSgfx.h>
#include <GSmtx.h>
#include <GScamera.h>

GSprimData primData;

void GSprimSetQuadVertexOrder(s32 order)
{
    primData.quadVertexOrder = order;
    switch (order) {
    case 0:
        primData.primitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
        break;
    case GS_PRIM_VERTEX_ORDER_PERIMETER_QUADS:
        primData.primitiveType = GSGFX_QUADS;
        break;
    }
}

void GSprimSetLineWidth(f32 width)
{
    primData.lineWidth = width;
    primData.linePushMode = 0;
}

void GSprimSetBlendMode(s32 mode)
{
    switch (mode) {
    case GSGFX_BLEND_NONE:
        primData.layer->blendMode(GSGFX_BLEND_NONE, GSGFX_BLEND_ONE, GSGFX_BLEND_ONE, true);
        break;

    case GSGFX_BLEND_BLEND:
        primData.layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA,
                                  true);
        break;

    case GSGFX_BLEND_LOGIC:
        primData.layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_ONE, GSGFX_BLEND_ONE, true);
        break;

    case GSGFX_BLEND_SUBTRACT:
        primData.layer->blendMode(GSGFX_BLEND_SUBTRACT, GSGFX_BLEND_ONE, GSGFX_BLEND_ONE, true);
        break;
    }
}

void GSprimSetRenderLayer(GSgfxLayer *layer)
{
    primData.layer = layer == NULL ? GSgfxLayerFindByID(GSgfxLayerGetActive()) : layer;
}

void GSprimSetup3DRender(GSgfxLayer *layer)
{
    if (layer == NULL) {
        layer = GSgfxLayerFindByID(GSgfxLayerGetActive());
    }
    primData.layer = layer;
    GSgfxLayerSetActive(layer->mLayerID);
    GScameraLoadActiveCameraMatrix(primData.layer->mLayerID);
    primData.layer->pixelMode(true, true, true);
    primData.layer->zCompare(true, GSGFX_Z_COMPARE_LEQUAL);
    primData.layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    primData.layer->cullMode(GSGFX_CULL_NONE);
    GSprimSetBlendMode(GSGFX_BLEND_NONE);
    primData.quadVertexOrder = 0;
    primData.primitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    primData.lineWidth = 1.0f;
    primData.linePushMode = 0;
    GSprimSetTextSize(10.0f, 10.0f);
    GSprimSetTextWidth(1.0f);
}

void GSprimSetup2DRender(GSgfxLayer *layer)
{
    f32 matrix[4][4];
    GSgfxLayer *drawLayer = layer;

    if (layer == NULL) {
        drawLayer = GSgfxLayerFindByID(GSgfxLayerGetActive());
    }
    primData.layer = drawLayer;
    GSmtx44Ortho(matrix, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    drawLayer->orthographicProjection(matrix);
    drawLayer->mMatrixStack->identity();
    drawLayer->projectionDepthRange(0.0f, 30000.0f);
    primData.layer->pixelMode(true, false, false);
    primData.layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    primData.layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    primData.layer->cullMode(GSGFX_CULL_NONE);
    GSprimSetBlendMode(GSGFX_BLEND_NONE);
    primData.quadVertexOrder = 0;
    primData.primitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    primData.lineWidth = 1.0f;
    primData.linePushMode = 0;
    GSprimSetTextSize(10.0f, 10.0f);
    GSprimSetTextWidth(1.0f);
}
