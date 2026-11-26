#include <app/effects/glow.h>
#include <GSgfx.h>
#include <GSmtx.h>
#include <GStexture.h>

GSgfxLayerID glowEffect::texture_layer_id = GSGFX_LAYER_INVALID;
u16 glowEffect::token;
u8 glowEffect::refCount;

void glowEffect::_render(GSgfxLayer* layer, GSgfxLayerPass pass)
{
    if (texture_layer_id == GSGFX_LAYER_INVALID) return;
    if (layer->mLayerID != GSGFX_LAYER_FILTER) return;
    if (pass != GSGFX_LAYER_PASS_XLU) return;

    GSgfxLayer* textureLayer = GSgfxLayerFindByID(texture_layer_id);
    if (textureLayer == NULL) return;
    GStexture* texture = textureLayer->GetTextureCopy();
    if (texture == NULL) return;

    f32 mtx[4][4];
    GSmtx44Ortho(mtx, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);

    layer->orthographicProjection(mtx);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA,
                     mAlphaBlend ? GSGFX_BLEND_INV_SRC_ALPHA : GSGFX_BLEND_ONE, true);
    layer->pixelMode(true, true, false);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    layer->cullMode(GSGFX_CULL_BACK);
    layer->texSelect(0, texture);
    layer->mPrimitiveType = GSGFX_QUADS;

    switch (mRenderMode) {
    case GLOW_RENDER_SCALE:
        _renderScale(layer);
        break;
    case GLOW_RENDER_OFFSET:
    default:
        _renderOffset(layer);
        break;
    }

    token = layer->tokenMark();
}
