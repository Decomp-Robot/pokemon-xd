#include <types.h>
#include <libc/memory.h>

#include <GSAPI/GSgapp/GSgapp.h>
#include <GSAPI/GSvtr/GSvtr.h>
#include <GSgfx.h>
#include <GSmsg.h>
#include <GSprim.h>
#include <GSres.h>
#include <GSthread.h>

extern "C" void GSprimDraw2DTexQuad(f32 x, f32 y, f32 x1, f32 y1, GStexture *texture, GSgfxColAu8 color);
extern "C" void GSthreadExecuteGroup(unsigned long group);
extern "C" void GSgfxRenderPause(void);
extern "C" void GSgfxRenderUnpause(void);

// Transport state of the overlay, reported by GSvtrGetState.
enum {
    GSVTR_STATE_STOP = 0,
    GSVTR_STATE_PAUSE = 1,
    GSVTR_STATE_PLAY = 2,
    GSVTR_STATE_SSTEP = 3,
    GSVTR_STATE_SLOW = 4,
};

// App block states the update drives.
enum {
    GSVTR_BLOCK_RUNNING = 0, // apps run
    GSVTR_BLOCK_REQUEST = 1, // block requested, done next update
    GSVTR_BLOCK_HELD = 2,    // apps blocked; selected thread groups still execute each update
    GSVTR_BLOCK_RELEASE = 3, // release requested, done next update
};

// Handles of the game apps that follow the transport state. The update
// blocks them while the transport is held and unblocks them when it runs.
u32 _vtrGSgappHdlList[4];
enum { GSVTR_GSAPP_HDL_MAX = sizeof(_vtrGSgappHdlList) / sizeof(_vtrGSgappHdlList[0]) };

// Whether the overlay draws at all; GSvtrEnable/GSvtrDisable set it.
u8 _vtrEnabled;
// Transport state, reported by GSvtrGetState.
s32 _vtrState;
// App block state the update drives.
s32 _vtrBlockState;
// Number of occupied slots of _vtrGSgappHdlList.
u32 _vtrGSgappCount;
// Frames counted in slow play; every fifth frame releases the apps for one step.
u32 _vtrSlowPlayFrame;
// Whether the overlay draws its info display: strip, frame and transport
// state text.
u8 _vtrInfoDisp;
// Whether the frame around the strip is drawn (on while a capture runs).
u8 _vtrInfoFrame;
// Overlay strip texture, set by GSvtrLoadTexture.
GStexture *_vtrTex;
// Mirror of the renderer pause state the update keeps in step.
u8 _vtrRenderPaused;

static inline void _vtrBlockGapps(void)
{
    u32 i;
    u32 *hdl;

    i = 0;
    hdl = _vtrGSgappHdlList;
    while (i < _vtrGSgappCount) {
        if (*hdl != 0) {
            GSgappBlock(*hdl);
        }
        i++;
        hdl++;
    }
}

static inline void _vtrUnblockGapps(void)
{
    u32 i;
    u32 *hdl;

    i = 0;
    hdl = _vtrGSgappHdlList;
    while (i < _vtrGSgappCount) {
        if (*hdl != 0) {
            GSgappUnblock(*hdl);
        }
        i++;
        hdl++;
    }
}

void _vtrTexDispFunc(void)
{
    GSgfxLayer *layer;
    GSgfxColAu8 frameColor = { 255, 0, 0, 255 };
    GSgfxColAu8 textureColor = { 255, 255, 255, 255 };

    if (_vtrEnabled == 0 || _vtrInfoDisp == 0) {
        return;
    }
    if (_vtrTex == 0) {
        return;
    }
    layer = GSgfxLayerFindByID(GSGFX_LAYER_MENU);
    if (layer == 0) {
        return;
    }
    GSprimSetup2DRender(layer);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_ONE, GSGFX_BLEND_ZERO, true);
    if (_vtrInfoFrame != 0) {
        GSprimDraw2DQuad(frameColor, 6.0f, 24.0f, 634.0f, 68.0f);
    }
    GSprimDraw2DTexQuad(12.0f, 28.0f, 628.0f, 64.0f, _vtrTex, textureColor);
    switch (_vtrState) {
    case GSVTR_STATE_STOP:
        GSprint(layer->mLayerID, 560, 44, 0xFFFFFFFF, "???");
        break;
    case GSVTR_STATE_PAUSE:
        GSprint(layer->mLayerID, 560, 44, 0xFFFFFFFF, "Pause");
        break;
    case GSVTR_STATE_PLAY:
        GSprint(layer->mLayerID, 560, 44, 0xFFFFFFFF, "Play");
        break;
    case GSVTR_STATE_SSTEP:
        switch (_vtrBlockState) {
        case GSVTR_BLOCK_RUNNING:
        case GSVTR_BLOCK_REQUEST:
            GSprint(layer->mLayerID, 560, 44, 0xFFFFFFFF, "SStep");
            break;
        case GSVTR_BLOCK_HELD:
        case GSVTR_BLOCK_RELEASE:
            GSprint(layer->mLayerID, 560, 44, 0xFFFFFFFF, "Pause");
            break;
        }
        break;
    case GSVTR_STATE_SLOW:
        GSprint(layer->mLayerID, 560, 44, 0xFFFFFFFF, "Slow");
        break;
    }
}

void _vtrUpdateFunc(u32 appID, u32 parameter)
{
    u8 paused;

    if (_vtrEnabled == 0) {
        return;
    }
    _vtrTexDispFunc();
    switch (_vtrBlockState) {
    case GSVTR_BLOCK_RUNNING:
        paused = 0;
        break;
    case GSVTR_BLOCK_REQUEST:
        _vtrBlockState = GSVTR_BLOCK_HELD;
        _vtrBlockGapps();
        paused = 1;
        break;
    case GSVTR_BLOCK_HELD:
        paused = 1;
        break;
    case GSVTR_BLOCK_RELEASE:
        _vtrBlockState = GSVTR_BLOCK_RUNNING;
        _vtrUnblockGapps();
        paused = 0;
        break;
    }
    if (paused != _vtrRenderPaused) {
        if (paused == 1) {
            GSgfxRenderPause();
        } else {
            GSgfxRenderUnpause();
        }
        _vtrRenderPaused = paused;
    }
    switch (_vtrState) {
    case GSVTR_STATE_PAUSE:
        break;
    case GSVTR_STATE_PLAY:
        break;
    case GSVTR_STATE_SSTEP:
        if (_vtrBlockState != GSVTR_BLOCK_HELD) {
            _vtrBlockState = GSVTR_BLOCK_REQUEST;
        }
        break;
    case GSVTR_STATE_SLOW:
        _vtrBlockState = (_vtrBlockState == GSVTR_BLOCK_RUNNING) ? GSVTR_BLOCK_REQUEST : GSVTR_BLOCK_HELD;
        if (++_vtrSlowPlayFrame >= 5) {
            _vtrBlockState = GSVTR_BLOCK_RELEASE;
            _vtrSlowPlayFrame = 0;
        }
        break;
    }
    if (_vtrBlockState == GSVTR_BLOCK_HELD) {
        GSthreadExecuteGroup(0xE38F910B);
        GSthreadExecuteGroup(0x175F4322);
    }
}

void GSvtrInit(void)
{
    s32 appID;

    _vtrEnabled = 0;
    _vtrState = GSVTR_STATE_STOP;
    _vtrGSgappCount = 0;
    _vtrTex = 0;
    _vtrInfoDisp = 1;
    memset(_vtrGSgappHdlList, 0, sizeof(_vtrGSgappHdlList));
    appID = GSgappCreate(1, 253, 10, _vtrUpdateFunc);
    GSgappSetName(appID, "GSvtr");
}

s32 GSvtrRegisterGSgapp(s32 hdl)
{
    u32 i;
    u32 *slot;

    if (_vtrGSgappCount + 1 >= GSVTR_GSAPP_HDL_MAX) {
        return 0;
    }
    slot = _vtrGSgappHdlList;
    for (i = 0; i < GSVTR_GSAPP_HDL_MAX; i++, slot++) {
        if (*slot == 0) {
            *slot = hdl;
            _vtrGSgappCount++;
            return 1;
        }
    }
    return 0;
}

void GSvtrLoadTexture(void)
{
    _vtrTex = (GStexture *)GSresGetResourceAllGroup(0x0AB21200);
}

void GSvtrEnable(void)
{
    _vtrEnabled = 1;
    _vtrState = GSVTR_STATE_PLAY;
    _vtrBlockState = GSVTR_BLOCK_RELEASE;
}

void GSvtrDisable(void)
{
    _vtrState = GSVTR_STATE_STOP;
    _vtrEnabled = 0;
    _vtrUnblockGapps();
    if (_vtrRenderPaused == 1) {
        GSgfxRenderUnpause();
        _vtrRenderPaused = 0;
    }
}

u8 GSvtrIsActive(void)
{
    return _vtrEnabled;
}

s32 GSvtrGetState(void)
{
    return _vtrState;
}

void GSvtrSetInfoDisp(u8 disp, u8 frame)
{
    _vtrInfoDisp = disp;
    _vtrInfoFrame = frame;
}

u8 GSvtrGetInfoDisp(void)
{
    return _vtrInfoDisp;
}

void GSvtrPause(void)
{
    _vtrState = GSVTR_STATE_PAUSE;
    if (_vtrBlockState == GSVTR_BLOCK_HELD) {
        return;
    }
    _vtrBlockState = GSVTR_BLOCK_REQUEST;
}

void GSvtrPlay(void)
{
    _vtrState = GSVTR_STATE_PLAY;
    _vtrBlockState = GSVTR_BLOCK_RELEASE;
}

void GSvtrSingleStep(void)
{
    _vtrState = GSVTR_STATE_SSTEP;
    _vtrBlockState = GSVTR_BLOCK_RELEASE;
}

void GSvtrSlowPlay(void)
{
    _vtrState = GSVTR_STATE_SLOW;
    _vtrBlockState = GSVTR_BLOCK_RELEASE;
    _vtrSlowPlayFrame = 0;
}
