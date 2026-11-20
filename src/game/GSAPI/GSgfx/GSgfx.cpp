#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GSlog.h>
#include <GSmathInit.h>
#include <GSscratch.h>
#include <string.h>

extern "C" void GSgfxRelocToScratch(void);
extern "C" void GSgfx_GCFirstInit(void);
extern "C" void GSgfx_GCInitState(void);
extern "C" void HSD_Init(void);

void GSgfxVideoShutdown(void);
void GSgfxVideoInit(u8 xfbCount, u16 efbHeight, GSgfxVideoMode mode);
void GSgfxVideoSecondInit(void);
void GSgfxHaltInit(void);
void GSgfxVFInit(void);
void GSgfxLayerInit(void);
void GSgfxResetInit(void);
void _gfxScratchNotify(GSscratchNotify notify, void* ptr, u8);

struct _GSgfxInitParms {
    u8 displayEnabled;
    u8 pad_01[3];
    u8 field_04;
    u8 pad_05;
    u16 field_06;
    s32 region;
    u32 field_0C;
    u32 field_10;
    u32 field_14;
};

GSgfxState _gfxMemoryState;
GSgfxState *gsgfxState;

void GSgfxRelocToScratch(void)
{
    GSgfxState* scratch = static_cast<GSgfxState*>(GSscratchAlloc(12, _gfxScratchNotify));
    memcpy(scratch, gsgfxState, sizeof(GSgfxState));
    gsgfxState = scratch;
}

void GSgfxShutdown(void)
{
    GSgfxVideoShutdown();
    GSgfxGPUShutdown();
}

void GSgfxInit(_GSgfxInitParms* parms)
{
    gsgfxState = &_gfxMemoryState;
    memset(&_gfxMemoryState, 0, sizeof(GSgfxState));

    GSgfx_GCFirstInit();
    GSgfx_GCInitState();
    GSgfxVideoInit(parms->field_04, parms->field_06, static_cast<GSgfxVideoMode>(parms->region));
    GSgfxGPUInit();
    GSgfxVideoSecondInit();

    if (parms->displayEnabled == 1) {
        GSgfxHaltInit();
    }

    GSmathInit();
    GSgfxRenderInit();
    GSgfxVFInit();
    GSgfxLayerInit();
    GSgfxTokenInit();
    GSgfxPerfbarInit(4);
    GSgfxBackFBInit();
    GSgfxResetInit();
    HSD_Init();

    new GSgfxLayer(GSGFX_LAYER_DEFAULT, "main", parms->field_0C, parms->field_10,
                   parms->field_14, GS_GFX_LAYER_NORMAL);

    GSgfxRenderSetupFirstFrame();
    GSlogWrite("GSgfx: Init OK, state located at %08Xh (size=%d)\n",
               (u32)gsgfxState, sizeof(GSgfxState));
}

void _gfxScratchNotify(GSscratchNotify notify, void* ptr, u8)
{
    switch (notify) {
    case GS_SCRATCH_INVALID:
        memcpy(&_gfxMemoryState, ptr, sizeof(GSgfxState));
        gsgfxState = &_gfxMemoryState;
        break;
    case GS_SCRATCH_VALID:
        memcpy(ptr, &_gfxMemoryState, sizeof(GSgfxState));
        gsgfxState = static_cast<GSgfxState*>(ptr);
        break;
    default:
        break;
    }
}
