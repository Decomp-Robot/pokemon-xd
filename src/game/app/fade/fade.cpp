#include <fade.h>
#include <fade_effect.h>
#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GSmemman.h>
#include <GStexture.h>
#include <GSthread.h>
#include <GSsnd.h>
#include <GSmtx.h>
#include <menuOffScreen.h>
#include <time.h>
#include <dolphin/gx.h>

enum FadeState {
    FADE_STATE_IDLE,
    FADE_STATE_IN,
    FADE_STATE_BLACK,
    FADE_STATE_EFFECT_IN,
    FADE_STATE_EFFECT_OUT,
    FADE_STATE_BLACK_AFTER_EFFECT
};
enum FadeSound {
    FADE_SOUND_DOKU = 24
};

typedef u8 (*FadeHookFunction)(GSgfxLayerID, volatile bool, f32, f32, f32, f32, void *);

struct FADE_WORK {
    u8 state;
    bool busy;
    u16 type;
    f32 duration;
    f32 elapsed;
    FadeFunction function;
    GStexture *snapshot;
    f32 effectDuration;
    f32 effectElapsed;
};

volatile FADE_WORK _fadeWork;
u8 _fadeSnapshotDone;

void _fadeSnapshot(void);
u8 myBackFB(GStexture *, u32, void *);

static const GSgfxColAu8 fadeWhite = {255, 255, 255, 255};
static const GSgfxColAu8 fadeBlack = {0, 0, 0, 255};

void fadeEffectDokuStop()
{
    if (_fadeWork.state == FADE_STATE_EFFECT_OUT) {
        _fadeWork.busy = 0;
        _fadeWork.state = FADE_STATE_IDLE;
    }
}

void fadeEffectDokuStart()
{
    if (_fadeWork.state == FADE_STATE_IDLE) {
        fadeSetEX(FADE_OUT_EFFECT, 1.0f, 0.5f, fadeEffectHookFunction_Doku_Init, false);
        GSsndPlayNow(FADE_SOUND_DOKU);
    }
}

int fadeGetType()
{
    return _fadeWork.type;
}

s8 fadeCheck(u8 wait)
{
    if (wait == 0) return _fadeWork.busy;
    while (_fadeWork.busy == 1) GSthreadSwitch();
    GSthreadSwitch();
    return _fadeWork.busy;
}

FadeFunction fadeSetEX(u16 flags, f32 duration, f32 effectTime, FadeFunction function, bool snapshotUse)
{
    FadeFunction previous = fadeSetFunction(function, snapshotUse, flags, effectTime);
    fadeSet(flags, duration);
    return previous;
}

void fadeSetToBlackInIfBlack(f32 time)
{
    if (_fadeWork.state == FADE_STATE_BLACK) fadeSet(FADE_IN_BLACK, time);
}

void fadeSetToBlackOutIfNotBlack(f32 time)
{
    if (_fadeWork.state != FADE_STATE_BLACK) fadeSet(FADE_OUT_BLACK, time);
}

void fadeSet(u16 flags, f32 duration)
{
    _fadeWork.busy = 1;
    u16 type;
    _fadeWork.type = type = flags;
    _fadeWork.duration = duration;
    _fadeWork.elapsed = 0.0f;
    if (type & FADE_FLAG_EFFECT) {
        if (type & FADE_FLAG_OUT) _fadeWork.state = FADE_STATE_EFFECT_OUT;
        else _fadeWork.state = FADE_STATE_EFFECT_IN;
    } else {
        if (type & FADE_FLAG_OUT) _fadeWork.state = FADE_STATE_BLACK;
        else _fadeWork.state = FADE_STATE_IN;
    }
}

FadeFunction fadeSetFunction(FadeFunction function, bool snapshotUse, u16 flags, f32 effectTime)
{
    if (function == 0) return _fadeWork.function;
    _fadeWork.busy = 1;
    u16 type;
    _fadeWork.type = type = flags;
    _fadeWork.effectDuration = effectTime;
    _fadeWork.effectElapsed = 0.0f;
    _fadeWork.state = FADE_STATE_IDLE;
    FadeFunction previous = _fadeWork.function;
    _fadeWork.function = 0;
    if (snapshotUse == true) {
        _fadeSnapshot();
    } else if (_fadeWork.snapshot != 0) {
        if (_fadeWork.snapshot != menuOffScreenGetPtr()) GStextureFree(_fadeWork.snapshot);
        _fadeWork.snapshot = 0;
    }
    fadeEffectInit();
    function();
    return previous;
}

void fadeSetFunctionOnly(FadeFunction function)
{
    _fadeWork.function = function;
}

void _fadeSnapshot()
{
    GStexture *texture = _fadeWork.snapshot;
    _fadeWork.snapshot = 0;
    if (!menuOffScreenIsDoing()) texture = menuOffScreenGetPtr();
    if (texture == 0) {
        long method = GSmemSetAllocMethodHeap(GSmemGetCurrentHeap(), ALLOC_METHOD_LARGEST);
        texture = GStextureCreate(0, 0, GS_TEXTURE_FORMAT_RGB565, GS_TEXTURE_TLUT_FORMAT_NONE, 0);
        GSmemSetAllocMethodHeap(GSmemGetCurrentHeap(), method);
    }
    if (texture != 0) {
        _fadeSnapshotDone = 0;
        GSgfxBeginBackFBCapture(texture, myBackFB, 0);
        while (!_fadeSnapshotDone) GSthreadSwitch();
        _fadeWork.snapshot = texture;
    }
}

u8 myBackFB(GStexture *, u32, void *)
{
    _fadeSnapshotDone = 1;
    return 0;
}

void fadeDaemon()
{
    u8 red, green, blue, alpha;
    GSgfxColAu8 white = fadeWhite;
    GSgfxColAu8 black = fadeBlack;
    GSgfxLayer *layer = GSgfxLayerFindByID(GSGFX_LAYER_MENU);
    if (layer == 0) return;
    u32 fading = _fadeWork.busy;
    u32 wasBusy = _fadeWork.busy;
    if (fading == 1) {
        switch (_fadeWork.state) {
        case FADE_STATE_IDLE:
        case FADE_STATE_EFFECT_OUT:
            break;
        case FADE_STATE_IN:
        case FADE_STATE_BLACK:
        case FADE_STATE_EFFECT_IN:
            _fadeWork.elapsed += timeGetLastFrameTime() / (f32)GSgfxVideoGetVsyncRate();
            if (_fadeWork.elapsed >= _fadeWork.duration) {
                _fadeWork.elapsed = _fadeWork.duration;
                _fadeWork.busy = 0;
                if (_fadeWork.type & FADE_FLAG_OUT) _fadeWork.state = FADE_STATE_BLACK;
                else _fadeWork.state = FADE_STATE_IDLE;
            }
            break;
        }
    }
    if (_fadeWork.function != 0) {
        _fadeWork.effectElapsed += timeGetLastFrameTime() / (f32)GSgfxVideoGetVsyncRate();
        if (_fadeWork.effectElapsed >= _fadeWork.effectDuration)
            _fadeWork.effectElapsed = _fadeWork.effectDuration;
        GStexture *snapshot = _fadeWork.snapshot;
        FadeHookFunction function = (FadeHookFunction)_fadeWork.function;
        if (!function(layer->mLayerID, _fadeWork.busy, _fadeWork.elapsed,
                               _fadeWork.duration, _fadeWork.effectElapsed,
                               _fadeWork.effectDuration, snapshot)) {
            _fadeWork.function = 0;
            if (_fadeWork.snapshot != 0) {
                if (_fadeWork.snapshot != menuOffScreenGetPtr()) GStextureFree(_fadeWork.snapshot);
                _fadeWork.snapshot = 0;
            }
        }
    } else if ((_fadeWork.type & FADE_FLAG_BLACK_AFTER_EFFECT) && wasBusy == 0) {
        _fadeWork.state = FADE_STATE_BLACK_AFTER_EFFECT;
    }
    switch (_fadeWork.state) {
    case FADE_STATE_IDLE:
    case FADE_STATE_EFFECT_IN:
    case FADE_STATE_EFFECT_OUT:
        break;
    case FADE_STATE_IN:
    case FADE_STATE_BLACK:
    case FADE_STATE_BLACK_AFTER_EFFECT:
        f32 rate;
        if (_fadeWork.duration == 0.0f) rate = 1.0f;
        else rate = _fadeWork.elapsed / _fadeWork.duration;
        if (_fadeWork.type & FADE_FLAG_OUT) alpha = (u8)(255.0f * rate);
        else alpha = 255 - (s32)(255.0f * rate);
        if (_fadeWork.type & FADE_FLAG_BLACK) {
            red = black.r; green = black.g; blue = black.b;
        } else {
            red = white.r; green = white.g; blue = white.b;
        }
        if (_fadeWork.state == FADE_STATE_BLACK_AFTER_EFFECT) {
            red = black.r; green = black.g; blue = black.b;
            alpha = (u8)(255.0f * rate);
        }
        f32 projection[4][4];
        GSmtx44Ortho(projection, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
        layer->orthographicProjection(projection);
        layer->mMatrixStack->identity();
        layer->projectionDepthRange(0.0f, 30000.0f);
        layer->SetRenderPass(GSGFX_LAYER_PASS_XLU);
        layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, false);
        layer->pixelMode(true, true, false);
        layer->zCompare(false, GSGFX_Z_COMPARE_LESS);
        layer->cullMode(GSGFX_CULL_NONE);
        layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
        layer->setRenderStateAndSelect(GSgfxVFBuildTemp(GX_POS_XYZ, GX_F32, 0, GX_NRM_XYZ, 1, GX_RGBA8, 0, GX_TEX_S, GX_U8));
        layer->beginSurface(4);
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.u8 = red;
        GSgfxFifo.u8 = green;
        GSgfxFifo.u8 = blue;
        GSgfxFifo.u8 = alpha;
        GSgfxFifo.f32 = 640.0f;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.u8 = red;
        GSgfxFifo.u8 = green;
        GSgfxFifo.u8 = blue;
        GSgfxFifo.u8 = alpha;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.f32 = 480.0f;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.u8 = red;
        GSgfxFifo.u8 = green;
        GSgfxFifo.u8 = blue;
        GSgfxFifo.u8 = alpha;
        GSgfxFifo.f32 = 640.0f;
        GSgfxFifo.f32 = 480.0f;
        GSgfxFifo.f32 = 0.0f;
        GSgfxFifo.u8 = red;
        GSgfxFifo.u8 = green;
        GSgfxFifo.u8 = blue;
        GSgfxFifo.u8 = alpha;
        layer->endSurface();
        break;
    }
}

void fadeInit()
{
    _fadeWork.state = FADE_STATE_IDLE;
    _fadeWork.busy = 0;
    _fadeWork.type = 0;
    _fadeWork.duration = 0.0f;
    _fadeWork.elapsed = 0.0f;
    _fadeWork.function = 0;
    _fadeWork.snapshot = 0;
    _fadeWork.effectDuration = 0.0f;
    _fadeWork.effectElapsed = 0.0f;
}
