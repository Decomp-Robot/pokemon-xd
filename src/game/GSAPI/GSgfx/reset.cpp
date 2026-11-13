#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GSmtx.h>
#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <dolphin/db.h>


enum ResetState { RESET_STATE_IDLE = 0, RESET_STATE_BUTTON_DOWN = 1, RESET_STATE_FADE = 2, RESET_STATE_SHUTDOWN = 3 };
enum ResetType { RESET_TYPE_RESTART = 0, RESET_TYPE_HOTRESET = 1, RESET_TYPE_FORCE_MENU = 2 };
s32 _resetState;
u8 _resetEnable;
s32 _resetType;
typedef u8 (*ResetFadeFunc)(void *);
ResetFadeFunc _resetRenderCBFunc;
void *_resetRenderCBArg;
void (*_resetShutdownCBFunc)(void);
s32 _resetFadeAlpha;

void GSgfxResetUpdate(void)
{
    switch (_resetState) {
    case RESET_STATE_IDLE:
        if (_resetEnable && OSGetResetButtonState() == TRUE) _resetState = RESET_STATE_BUTTON_DOWN;
        break;
    case RESET_STATE_BUTTON_DOWN:
        if (_resetEnable && !OSGetResetButtonState()) {
            _resetState = RESET_STATE_FADE;
            _resetType = RESET_TYPE_RESTART;
        }
        break;
    case RESET_STATE_FADE:
        if (_resetRenderCBFunc(_resetRenderCBArg) == 1) _resetState = RESET_STATE_SHUTDOWN;
        break;
    case RESET_STATE_SHUTDOWN:
        if (DBIsDebuggerPresent() == TRUE) { _resetState = RESET_STATE_IDLE; break; }
        if (_resetShutdownCBFunc) _resetShutdownCBFunc();
        PADRecalibrate(PAD_CHAN0_BIT | PAD_CHAN1_BIT | PAD_CHAN2_BIT | PAD_CHAN3_BIT);
        OSSetIdleFunction(0, 0, 0, 0);
        GSgfxShutdown();
        switch (_resetType) {
        case RESET_TYPE_RESTART: OSResetSystem(OS_RESET_RESTART, 0, FALSE); break;
        case RESET_TYPE_HOTRESET: OSResetSystem(OS_RESET_HOTRESET, 0, FALSE); break;
        case RESET_TYPE_FORCE_MENU: OSResetSystem(OS_RESET_HOTRESET, 0, TRUE); break;
        }
        break;
    }
}

extern "C" void GSgfxReset(s32 type)
{
    if (_resetState != RESET_STATE_IDLE) return;
    _resetType = type;
    _resetState = RESET_STATE_FADE;
}

extern "C" u8 GSgfxResetGetStatus(void) { return _resetEnable; }
extern "C" u8 GSgfxResetRestore(u8 enable) { u8 old = _resetEnable; _resetEnable = enable; return old; }
extern "C" u8 GSgfxResetDisable(void) { u8 old = _resetEnable; _resetEnable = 0; return old; }
extern "C" u8 GSgfxResetEnable(void) { u8 old = _resetEnable; _resetEnable = 1; return old; }
extern "C" void GSgfxResetSetShutdownCallback(void (*callback)(void))
{
    if (_resetState == RESET_STATE_IDLE) _resetShutdownCBFunc = callback;
}

u8 _resetDoFade(void *);
void GSgfxResetInit(void)
{
    _resetEnable = 0;
    _resetState = RESET_STATE_IDLE;
    _resetRenderCBFunc = _resetDoFade;
    _resetRenderCBArg = 0;
    _resetShutdownCBFunc = 0;
}

u8 _resetDoFade(void *argument)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(GSGFX_LAYER_MENU);
    f32 projection[4][4];
    if (layer == 0) return 1;
    layer->pixelMode(true, false, false);
    GSmtx44Ortho(projection, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(projection);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->clipMode(GSGFX_CLIP_DISABLED);
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(0));
    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    layer->beginSurface(4);
    GSgfxFifo.f32 = 0.0f; GSgfxFifo.f32 = 0.0f; GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0;
    s32 alpha = _resetFadeAlpha;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 640.0f; GSgfxFifo.f32 = 0.0f; GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 0.0f; GSgfxFifo.f32 = 480.0f; GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 640.0f; GSgfxFifo.f32 = 480.0f; GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = 0; GSgfxFifo.u8 = alpha;
    layer->endSurface();
    if (_resetFadeAlpha == 255) { _resetFadeAlpha = 0; return 1; }
    _resetFadeAlpha += (256 / GSgfxVideoGetVsyncRate()) * GSgfxVideoGetLastRealRenderTime();
    if (_resetFadeAlpha > 255) _resetFadeAlpha = 255;
    return 0;
}
