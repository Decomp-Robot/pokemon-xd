#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GSmtx.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>

enum {
    RENDER_FIFO_LIST_MAX = 64,
    RENDER_XFB_COMMAND_SIZE = 96,
    RENDER_XFB_FIFO_SIZE = 65536,
    RENDER_PREPARE_FRAME_COLOR = 0x00808080,
    RENDER_WAIT_COLOR = 0xFFFF00
};
#define RENDER_XFB_COPY_PENDING ((void *)0xFEFEFEFE)

u8 _renderXFBCommandBuf[RENDER_XFB_COMMAND_SIZE] ATTRIBUTE_ALIGN(32);
GSgfx_GCFifoObj _renderXFBCommandFifo;
volatile GSgfxRenderFifoEntry _renderFifoList[RENDER_FIFO_LIST_MAX];

void *_renderPrepareFrameStart;
void *_renderPrepareFrameEnd;
void *_renderLastXFBPtr;
u32 _renderPauseCount;
u16 _renderXFBCopyToken;

void _renderResetGX(void);
void _renderBreakpointCallback(void);
void _renderXFBCopyCallback(void *xfb);
void _renderDrawDoneCallback(void);

void _renderDrawDoneCallback(void)
{
    if (GSgfxVideoGetXFBCount() != 1) {
        _renderLastXFBPtr = gsgfxState->pendingXFB;
        GSgfxVideoSetXFBReady(_renderLastXFBPtr);
        gsgfxState->pendingXFB = 0;
    }
}

void _renderXFBCopyCallback(void *xfb)
{
    gsgfxState->pendingXFB = xfb;
    gsgfxState->renderPhase = GSGFX_RENDER_PHASE_XFB_COPY;
    GSgfxVideoBuildEFB2XFBCopy(_renderXFBCommandBuf, gsgfxState->pendingXFB);
    if (!GSgfxTokenCleared(_renderXFBCopyToken)) {
        u32 startVsync = GSgfxVideoGetVsyncCount();
        while (!GSgfxTokenCleared(_renderXFBCopyToken)) {}
    }
    GSgfxGPUProcessBlock(&_renderXFBCommandFifo, _renderXFBCommandBuf,
                        _renderXFBCommandBuf + RENDER_XFB_COMMAND_SIZE);
}

void _renderBreakpointCallback(void)
{
    BOOL interrupts;
    GSgfxRenderFifoEntry *entry;
    switch (gsgfxState->renderPhase) {
    case GSGFX_RENDER_PHASE_BLOCKS: {
        entry = gsgfxState->renderFifoCursor;
        if (entry->completionCallback != 0) {
            interrupts = OSEnableInterrupts();
            entry->completionCallback(entry->callbackArgument);
            OSRestoreInterrupts(interrupts);
        }
        gsgfxState->renderFifoCursor++;
        GSgfxRenderFifoEntry *nextEntry = gsgfxState->renderFifoCursor;
        if (nextEntry->fifo != 0) {
            GSgfxPerfbarGPUEntry(nextEntry->color);
            GSgfxGPUProcessBlock(nextEntry->fifo, nextEntry->start, nextEntry->end);
        } else {
            GSgfxPerfbarGPUEnd();
            void *xfb = GSgfxVideoFindFreeXFB(false);
            if (xfb == 0) GSgfxVideoSetOneShotPostCallback(_renderXFBCopyCallback);
            else _renderXFBCopyCallback(xfb);
        }
        break;
    }
    case GSGFX_RENDER_PHASE_XFB_COPY:
        _renderLastXFBPtr = gsgfxState->pendingXFB;
        GSgfxVideoSetXFBReady(_renderLastXFBPtr);
        gsgfxState->renderPhase = GSGFX_RENDER_PHASE_BLOCKS;
        gsgfxState->pendingXFB = 0;
        break;
    }
}

void _renderResetGX(void)
{
    Mtx identity_mtx;
    GXColor black = {0, 0, 0, 0};
    int stage;

    PSMTXIdentity(identity_mtx);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD6, GX_TG_MTX2x4, GX_TG_TEX6, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD7, GX_TG_MTX2x4, GX_TG_TEX7, GX_IDENTITY);
    GXSetNumTexGens(1);
    GXClearVtxDesc();
    GXInvalidateVtxCache();

    GXSetLineWidth(6, GX_TO_ZERO);
    GXSetPointSize(6, GX_TO_ZERO);
    GXEnableTexOffsets(GX_TEXCOORD0, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD1, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD2, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD3, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD4, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD5, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD6, 0, 0);
    GXEnableTexOffsets(GX_TEXCOORD7, 0, 0);
    GXLoadPosMtxImm(identity_mtx, GX_PNMTX0);
    GXLoadNrmMtxImm(identity_mtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXLoadTexMtxImm(identity_mtx, GX_IDENTITY, GX_MTX3x4);
    GXLoadTexMtxImm(identity_mtx, GX_PTIDENTITY, GX_MTX3x4);
    GXSetCoPlanar(GX_DISABLE);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetScissorBoxOffset(0, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXInvalidateTexAll();

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD4, GX_TEXMAP4, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD5, GX_TEXMAP5, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD6, GX_TEXMAP6, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD7, GX_TEXMAP7, GX_COLOR0A0);
    GXSetTevOrder(GX_TEVSTAGE8, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE9, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE10, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE11, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE12, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE13, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE14, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevOrder(GX_TEVSTAGE15, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    GXSetNumTevStages(1);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);

    for (stage = GX_TEVSTAGE0; stage < GX_MAX_TEVSTAGE; stage++) {
        GXSetTevKColorSel((GXTevStageID)stage, GX_TEV_KCSEL_1_4);
        GXSetTevKAlphaSel((GXTevStageID)stage, GX_TEV_KASEL_1);
        GXSetTevSwapMode((GXTevStageID)stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);

    for (stage = GX_TEVSTAGE0; stage < GX_MAX_TEVSTAGE; stage++)
        GXSetTevDirect((GXTevStageID)stage);

    GXSetNumIndStages(0);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE1, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE2, GX_ITS_1, GX_ITS_1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE3, GX_ITS_1, GX_ITS_1);

    GXSetFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1.0f, black);
    GXSetFogRangeAdj(GX_DISABLE, 0, 0);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetColorUpdate(GX_ENABLE);
    GXSetAlphaUpdate(GX_ENABLE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZCompLoc(GX_TRUE);
    GXSetDither(GX_ENABLE);
    GXSetDstAlpha(GX_DISABLE, 0);
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GXSetFieldMask(GX_ENABLE, GX_ENABLE);

    GXPokeColorUpdate(GX_TRUE);
    GXPokeAlphaUpdate(GX_TRUE);
    GXPokeDither(GX_FALSE);
    GXPokeBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ONE, GX_LO_SET);
    GXPokeAlphaMode(GX_ALWAYS, 0);
    GXPokeAlphaRead(GX_READ_FF);
    GXPokeDstAlpha(GX_DISABLE, 0);
    GXPokeZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);

    GXSetGPMetric(GX_PERF0_NONE, GX_PERF1_NONE);
    GXClearGPMetric();
}

void GSgfxRenderInit(void)
{
    gsgfxState->pendingXFB = 0;
    gsgfxState->renderPhase = GSGFX_RENDER_PHASE_BLOCKS;
    GSgfxGPUInitFifoBase(&_renderXFBCommandFifo, _renderXFBCommandBuf, RENDER_XFB_FIFO_SIZE);
    GXInitFifoPtrs((GXFifoObj *)&_renderXFBCommandFifo, _renderXFBCommandBuf, _renderXFBCommandBuf);
    GSgfxGPUSetDrawDoneCallback(_renderDrawDoneCallback);
    GSgfxGPUSetBreakpointCallback(_renderBreakpointCallback);
    GXLoadPosMtxImm(mtxIdent[0].m, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GSgfx_GCReset();
    GSgfxRenderModuleInit();
}

void GSgfxRenderAbortFrame(void)
{
    gsgfx_layer_surface_active = 0;
    GSgfxVideoSetOneShotPostCallback(0);
    GXAbortFrame();
    GSgfxTokenAbortFrame();
    if (gsgfxState->pendingXFB != 0 && gsgfxState->pendingXFB != RENDER_XFB_COPY_PENDING)
        GSgfxVideoSetXFBReady(gsgfxState->pendingXFB);
    gsgfxState->renderPhase = GSGFX_RENDER_PHASE_BLOCKS;
    gsgfxState->pendingXFB = 0;
    GSgfxLayerInvalidateAll();
    _renderResetGX();
}

void GSgfxRenderWaitInit(GSgfxRenderWaitData *data)
{
    data->savedPerfColor = GSgfxPerfbarCPUEntry(RENDER_WAIT_COLOR);
    data->startVsync = GSgfxVideoGetVsyncCount();
}

void GSgfxRenderWaitTick(GSgfxRenderWaitData *data)
{
    OSYieldThread();
    if (GSgfxGPUIsIdle() != true) {
        u32 now = GSgfxVideoGetVsyncCount();
        u32 timeout = GSgfxVideoGetVsyncRate() * 3;
        s32 elapsed = now - data->startVsync;
        if (elapsed < 0) data->startVsync = now;
        else if (elapsed > timeout) GSgfxRenderAbortFrame();
    }
}

void GSgfxRenderWaitExit(GSgfxRenderWaitData *data)
{
    GSgfxPerfbarCPUEntry(data->savedPerfColor);
}

void GSgfxRenderWaitForDrawComplete(void)
{
    GSgfxRenderWaitData wait;
    GSgfxRenderWaitInit(&wait);
    while (gsgfxState->pendingXFB != 0)
        GSgfxRenderWaitTick(&wait);
    GSgfxRenderWaitExit(&wait);
}

void GSgfxRenderAddToFifoList(GSgfx_GCFifoObj *fifo, void *start, void *end,
                            u32 color, void (*callback)(void *), void *argument)
{
    if (start == end) return;
    fifo->blockComplete = 0;
    gsgfxState->renderFifoCursor->fifo = fifo;
    gsgfxState->renderFifoCursor->start = start;
    gsgfxState->renderFifoCursor->end = end;
    gsgfxState->renderFifoCursor->color = color;
    gsgfxState->renderFifoCursor->completionCallback = callback;
    gsgfxState->renderFifoCursor->callbackArgument = argument;
    gsgfxState->renderFifoCursor++;
    gsgfxState->renderFifoCursor->fifo = 0;
}

void GSgfxRenderSetupFirstFrame(void)
{
    GSgfx_GCFifoObj *fifo;
    void *readPtr;
    if (gsgfx_gpu_mode == GSGFX_GPU_MODE_BUFFERED) {
        GSgfxLayerPrepareNextFrame();
        GSgfxLayerUpdateActiveFifoStats();
        fifo = GSgfxGPUGetImmediateFifo();
        GXSetCPUFifo((GXFifoObj *)fifo);
        GXGetFifoPtrs((GXFifoObj *)fifo, &readPtr, &_renderPrepareFrameStart);
    }
    GSgfxVideoSetupNextFrame();
    if (gsgfx_gpu_mode == GSGFX_GPU_MODE_BUFFERED) {
        GXSaveCPUFifo((GXFifoObj *)fifo);
        GXGetFifoPtrs((GXFifoObj *)fifo, &readPtr, &_renderPrepareFrameEnd);
        GXSetCPUFifo((GXFifoObj *)&gsgfx_layer_activeFifo->fifo);
    }
    GSgfxTokenMarkFrame();
}

void GSgfxRenderPause(void)
{
    _renderPauseCount++;
}

void GSgfxRenderUnpause(void)
{
    if (_renderPauseCount != 0) _renderPauseCount--;
}

bool GSgfxRenderIsPaused(void)
{
    return _renderPauseCount != 0;
}

extern "C" void GSgfxRender(void)
{
    f32 delta = GSgfxVideoGetLastRenderTime();
    GSgfxRenderModuleDoPreUpdate(delta);

    switch (gsgfx_gpu_mode) {
    case GSGFX_GPU_MODE_IMMEDIATE: {
        GSgfxTokenMark_Sub();
        GSgfxPerfbarCPUEnd();
        GSgfxPerfbarGPUEnd();
        GSgfxPerfbarRender(gsgfxState->activeLayer);
        if (GSgfxVideoGetXFBCount() > 1)
            GSgfxRenderWaitForDrawComplete();
        GSgfxResetUpdate();
        GSgfxSSUpdate();
        GSgfxCaptureUpdate(_renderLastXFBPtr);
        void *xfb = GSgfxVideoCopyEFBToXFB();
        if (GSgfxVideoGetXFBCount() > 1) {
            gsgfxState->pendingXFB = xfb;
            GXSetDrawDone();
        }
        GSgfxPerfbarCPUStart();
        GSgfxPerfbarGPUStart();
        GSgfxLayerInvalidateAll();
        break;
    }
    case GSGFX_GPU_MODE_BUFFERED: {
        GSgfxLayer *layer = GSgfxLayerPrerender();
        GSgfxCaptureRenderFrameNumber(layer);
        GSgfxBackFBDoFrame(layer);
        GSgfxPerfbarCPUEnd();
        GSgfxPerfbarRender(layer);
        GSgfxLayerRenderStats(layer);
        if (GSgfxVideoGetXFBCount() > 1)
            GSgfxRenderWaitForDrawComplete();
        _renderXFBCopyToken = GSgfxTokenMark_Sub();
        GSgfxResetUpdate();
        GSgfxSSUpdate();
        GSgfxCaptureUpdate(_renderLastXFBPtr);
        gsgfxState->pendingXFB = RENDER_XFB_COPY_PENDING;
        _renderFifoList[0].fifo = GSgfxGPUGetImmediateFifo();
        _renderFifoList[0].start = _renderPrepareFrameStart;
        _renderFifoList[0].end = _renderPrepareFrameEnd;
        _renderFifoList[0].color = RENDER_PREPARE_FRAME_COLOR;
        _renderFifoList[1].fifo = 0;
        gsgfxState->renderFifoCursor = (GSgfxRenderFifoEntry *)&_renderFifoList[1];
        GSgfxLayerBuildRenderList();
        GSgfxPerfbarGPUStart();
        gsgfxState->renderFifoCursor = (GSgfxRenderFifoEntry *)_renderFifoList;
        GSgfxRenderFifoEntry *entry = gsgfxState->renderFifoCursor;
        GSgfxPerfbarGPUEntry(entry->color);
        GSgfxGPUProcessBlock(entry->fifo, entry->start, entry->end);
        GSgfxPerfbarCPUStart();
        break;
    }
    }

    GSgfxRenderSetupFirstFrame();

    if (_renderPauseCount)
        delta = 0.0f;
    GSgfxRenderModuleDoPostUpdate(delta);
}
