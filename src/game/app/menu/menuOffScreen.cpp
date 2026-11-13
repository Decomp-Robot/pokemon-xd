#include <menuOffScreen.h>
#include <GSgfx.h>
#include <GSmtx.h>
#include <GStexture.h>
#include <GSthread.h>

// Off-screen menu compositing: the captured back framebuffer is put back on
// screen as a full screen quad whose colour follows the fade rate.
static u8 _menuOffScreenPriority = 1;
static u8 _menuOffScreenEnable = 1;

static u8 _menuOffScreenCapture;
static u8 _menuOffScreenDoing;
static u8 _menuOffScreenDisp;
static u8 _menuOffScreenFade;
static GStexture* _menuOffScreenTex;
static f32 _menuOffScreenStartRate;
static f32 _menuOffScreenCurrRate;
static f32 _menuOffScreenEndRate;
static f32 _menuOffScreenTime;
static f32 _menuOffScreenCnt;

// Called by the back framebuffer capture with the number of frames captured so
// far; returning zero ends the capture.
u8 _menuCBOffScreen(GStexture* texture, u32 frames, void* param);

void menuOffScreenDraw(void)
{
    if (!_menuOffScreenEnable) {
        return;
    }
    if (_menuOffScreenFade) {
        _menuOffScreenCnt += GSgfxVideoGetLastRenderTime() / (f32)GSgfxVideoGetVsyncRate();
        if (_menuOffScreenCnt >= _menuOffScreenTime) {
            _menuOffScreenCnt = _menuOffScreenTime;
            _menuOffScreenFade = 0;
        }
    }
    if (!_menuOffScreenDisp) {
        return;
    }
    if (!_menuOffScreenCapture) {
        return;
    }
    f32 rate = _menuOffScreenCnt / _menuOffScreenTime;
    f32 curr = rate * (_menuOffScreenEndRate - _menuOffScreenStartRate) + _menuOffScreenStartRate;
    _menuOffScreenCurrRate = curr;
    u8 bright = (u8)(255.0f - 255.0f * curr / 100.0f);
    GSgfxLayer* layer = GSgfxLayerFindByID(_menuOffScreenPriority ? GSGFX_LAYER_MENU : GSGFX_LAYER_BASE);
    if (layer == 0) {
        return;
    }
    if (layer->mLayerID == GSGFX_LAYER_BASE) {
        layer->blendMode(GSGFX_BLEND_NONE, GSGFX_BLEND_ONE, GSGFX_BLEND_ZERO, true);
    } else {
        layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_ONE, GSGFX_BLEND_ZERO, true);
    }
    f32 mtx[4][4];
    GSmtx44Ortho(mtx, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(mtx);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->pixelMode(true, true, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    enum { OFFSCREEN_VERTEX_FORMAT_POSITION_RGBA_TEXCOORD = 7 };
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(OFFSCREEN_VERTEX_FORMAT_POSITION_RGBA_TEXCOORD));
    layer->texSelect(0, _menuOffScreenTex);
    layer->beginSurface(4);
    // Position, colour and texture coordinate of each corner of the captured
    // screen go straight into the write pipe.
    GSgfxFifo.s16 = 0;
    GSgfxFifo.s16 = 0;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.s16 = 640;
    GSgfxFifo.s16 = 0;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.s16 = 0;
    GSgfxFifo.s16 = 480;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.s16 = 640;
    GSgfxFifo.s16 = 480;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = bright;
    GSgfxFifo.u8 = 0xFF;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 1.0f;
    layer->endSurface();
}

u8 menuOffScreenFadeSync(u8 wait)
{
    while (wait != 0) {
        if (_menuOffScreenFade == 0) {
            break;
        }
        GSthreadSwitch();
    }
    return _menuOffScreenFade != 0;
}

void menuOffScreenFadeSet(f32 endRate, f32 frames)
{
    _menuOffScreenFade = 1;
    _menuOffScreenStartRate = _menuOffScreenCurrRate;
    _menuOffScreenEndRate = endRate;
    _menuOffScreenCurrRate = 0.0f;
    _menuOffScreenTime = frames;
    _menuOffScreenCnt = 0.0f;
    if (frames == 0.0f) {
        _menuOffScreenTime = 1.0f;
        _menuOffScreenCnt = 1.0f;
    }
}

u8 menuOffScreenSetPriority(u8 priority)
{
    u8 old = _menuOffScreenPriority;
    _menuOffScreenPriority = priority;
    return old;
}

u8 menuOffScreenSetDisp(u8 enable)
{
    u8 old = _menuOffScreenDisp;
    _menuOffScreenDisp = enable;
    return old;
}

u8 menuOffScreenIsDoing(void)
{
    return _menuOffScreenDoing;
}

GStexture* menuOffScreenGetPtr(void)
{
    return _menuOffScreenTex;
}

u8 menuOffScreenCheckEnable(u8 wait)
{
    while (wait != 0) {
        if (_menuOffScreenCapture != 0) {
            break;
        }
        GSthreadSwitch();
    }
    return _menuOffScreenCapture;
}

void menuOffScreenRelease(void)
{
    GStexture* tex = _menuOffScreenTex;
    _menuOffScreenFade = 0;
    _menuOffScreenDoing = 0;
    _menuOffScreenCapture = 0;
    _menuOffScreenDisp = 0;
    if (tex != 0) {
        GStextureFree(tex);
        _menuOffScreenTex = 0;
    }
}

void menuOffScreenCreate(u8 wait)
{
    _menuOffScreenFade = 0;
    _menuOffScreenDoing = 1;
    _menuOffScreenCapture = 0;
    _menuOffScreenDisp = 0;
    _menuOffScreenStartRate = 0.0f;
    _menuOffScreenCurrRate = 0.0f;
    _menuOffScreenEndRate = 0.0f;
    _menuOffScreenTime = 0.0f;
    _menuOffScreenCnt = 0.0f;
    if (_menuOffScreenTex == 0) {
        _menuOffScreenTex = GStextureCreate(0, 0, GS_TEXTURE_FORMAT_RGB565, 0, 0);
    }
    GSgfxBeginBackFBCapture(_menuOffScreenTex, _menuCBOffScreen, 0);
    menuOffScreenCheckEnable(wait);
}

void menuOffScreenInit(void)
{
    _menuOffScreenDoing = 0;
    _menuOffScreenCapture = 0;
    _menuOffScreenDisp = 0;
    _menuOffScreenPriority = 1;
    _menuOffScreenFade = 0;
    _menuOffScreenTime = 0.0f;
    _menuOffScreenCnt = 0.0f;
    _menuOffScreenStartRate = 0.0f;
    _menuOffScreenCurrRate = 0.0f;
    _menuOffScreenEndRate = 0.0f;
}

u8 _menuCBOffScreen(GStexture* texture, u32 frames, void* param)
{
    _menuOffScreenCapture = 1;
    return 0;
}
