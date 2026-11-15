#include <GSgfx.h>
#include <GSmem.h>
#include <dolphin/gx/GXFifo.h>
#include <dolphin/gx/GXManage.h>
#include <dolphin/os/OSInterrupt.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/vi.h>

enum { GPU_IMMEDIATE_BUFFER_SIZE = 65536, GPU_FIFO_COUNT = 2 };

GSgfxGPUMode gsgfx_gpu_mode;
static GSgfxGPUMode _gpuMode;
static void *_gpuImmedBuf;
static GSgfx_GCFifoObj *_gpuImmedFifo;
static GSgfx_GCFifoObj *_gpuBufferFifo;
static volatile s32 _gpuNextFifo;
static u8 _gpuSuppressDrawDoneCallback;
static u8 _gpuSuppressDrawSyncCallback;
static u8 _gpuSuppressBreakpointCallback;
static void (*_gpuDrawDoneUserCB)(void);
static void (*_gpuDrawSyncUserCB)(u16);
static void (*_gpuBreakpointUserCB)(void);
static GSgfx_GCFifoObj _gpuFifo[GPU_FIFO_COUNT];

void _gpuBreakpointCallback(void);
void _gpuDrawSyncCallback(u16 token);
void _gpuDrawDoneCallback(void);

static inline void gpuSetFifoReadPtr(GSgfx_GCFifoObj *fifo, void *readPtr)
{
    void *oldReadPtr, *writePtr;
    BOOL interrupts = OSDisableInterrupts();
    GXGetFifoPtrs((GXFifoObj *)fifo, &oldReadPtr, &writePtr);
    GXInitFifoPtrs((GXFifoObj *)fifo, readPtr, writePtr);
    OSRestoreInterrupts(interrupts);
}

static inline void gpuInitFifoBase(GSgfx_GCFifoObj *fifo, void *base, u32 size)
{
    fifo->base = (u8 *)base;
    fifo->top = (u8 *)base + size - 4;
    fifo->size = size;
    fifo->hiWatermark = size - (size >> 4);
    fifo->loWatermark = size >> 1;
    fifo->rdPtr = base;
    fifo->wrPtr = base;
    fifo->count = 0;
    fifo->bind_cpu = 0;
    fifo->bind_gp = 0;
}

static inline void gpuSetFifoWritePtr(GSgfx_GCFifoObj *fifo, void *writePtr)
{
    void *readPtr, *oldWritePtr;
    BOOL interrupts = OSDisableInterrupts();
    GXGetFifoPtrs((GXFifoObj *)fifo, &readPtr, &oldWritePtr);
    GXInitFifoPtrs((GXFifoObj *)fifo, readPtr, writePtr);
    OSRestoreInterrupts(interrupts);
}

void GSgfxGPUGetBPTick(long *gpTick, long *bpTick)
{
    *gpTick = -1;
    *bpTick = -1;
}

void GSgfxGPUProcessBlock(GSgfx_GCFifoObj *fifo, void *start, void *end)
{
    GSgfx_GCFifoObj *gpuFifo;
    BOOL interrupts = OSDisableInterrupts();
    gpuFifo = &_gpuFifo[_gpuNextFifo++];
    if (_gpuNextFifo > GPU_FIFO_COUNT - 1) {
        _gpuNextFifo = 0;
    }
    u32 size = GXGetFifoSize((GXFifoObj *)fifo);
    void *base = GXGetFifoBase((GXFifoObj *)fifo);
    gpuInitFifoBase(gpuFifo, base, size);
    GXInitFifoPtrs((GXFifoObj *)gpuFifo, start, end);
    gsgfxState->blockFifo = fifo;
    gsgfxState->blockGpuFifo = gpuFifo;
    gsgfxState->blockStart = start;
    gsgfxState->blockEnd = end;
    gsgfxState->blockComplete = 0;
    gsgfxState->blockActive = 1;
    GXEnableBreakPt(end);
    GXSetGPFifo((GXFifoObj *)gpuFifo);
    OSRestoreInterrupts(interrupts);
}

void GSgfxGPUInitFifoBase(GSgfx_GCFifoObj *fifo, void *base, u32 size)
{
    gpuInitFifoBase(fifo, base, size);
}

void GSgfxGPUSetBreakpointCallback(void (*callback)(void))
{
    _gpuBreakpointUserCB = callback;
}

void GSgfxGPUSetDrawSyncCallback(void (*callback)(u16))
{
    _gpuDrawSyncUserCB = callback;
}

void GSgfxGPUSetDrawDoneCallback(void (*callback)(void))
{
    _gpuDrawDoneUserCB = callback;
}

void GSgfxGPUEnterBufferedMode(GSgfx_GCFifoObj *fifo)
{
    GSgfxGPUSetMode(GSGFX_GPU_MODE_BUFFERED);
    GXSetCPUFifo((GXFifoObj *)fifo);
}

s32 GSgfxGPUSetMode(s32 mode)
{
    if (mode != GSGFX_GPU_MODE_NONE) {
        const GSgfxGPUMode oldMode = _gpuMode;
        if (oldMode == mode) {
            return oldMode;
        }
        switch (mode) {
        case GSGFX_GPU_MODE_IMMEDIATE:
            GSgfxRenderAbortFrame();
            _gpuBufferFifo = (GSgfx_GCFifoObj *)GXGetCPUFifo();
            GXSaveCPUFifo((GXFifoObj *)_gpuBufferFifo);
            GXSaveGPFifo(GXGetGPFifo());
            memset(_gpuImmedBuf, 0, GPU_IMMEDIATE_BUFFER_SIZE);
            DCFlushRange(_gpuImmedBuf, GPU_IMMEDIATE_BUFFER_SIZE);
            gpuSetFifoReadPtr(_gpuImmedFifo, _gpuImmedBuf);
            gpuSetFifoWritePtr(_gpuImmedFifo, _gpuImmedBuf);
            GXSetCPUFifo((GXFifoObj *)_gpuImmedFifo);
            GXSetGPFifo((GXFifoObj *)_gpuImmedFifo);
            GXDisableBreakPt();
            GSgfx_GCReset();
            _gpuSuppressDrawDoneCallback = 1;
            GXDrawDone();
            VIWaitForRetrace();
            _gpuSuppressDrawDoneCallback = 0;
            break;
        case GSGFX_GPU_MODE_BUFFERED:
            switch (oldMode) {
            case GSGFX_GPU_MODE_NONE:
                _gpuSuppressDrawDoneCallback = 1;
                GXDrawDone();
                VIWaitForRetrace();
                _gpuSuppressDrawDoneCallback = 0;
                GXSaveCPUFifo((GXFifoObj *)_gpuImmedFifo);
                break;
            case GSGFX_GPU_MODE_IMMEDIATE:
                GSgfxRenderAbortFrame();
                GXSaveCPUFifo((GXFifoObj *)_gpuImmedFifo);
                GXSetCPUFifo((GXFifoObj *)_gpuBufferFifo);
                GSgfx_GCReset();
                break;
            }
            break;
        }
        _gpuMode = (GSgfxGPUMode)mode;
        gsgfx_gpu_mode = (GSgfxGPUMode)mode;
        return oldMode;
    }
    return mode;
}

GSgfx_GCFifoObj *GSgfxGPUGetImmediateFifo(void)
{
    return _gpuImmedFifo;
}

void GSgfxGPUShutdown(void)
{
    GXBool readIdle, unused;
    GXGetGPStatus(&unused, &unused, &readIdle, &unused, &unused);
    if (readIdle != GX_TRUE) {
        GXAbortFrame();
    }
    GXSetDrawDoneCallback(NULL);
    GXSetDrawSyncCallback(NULL);
    GXSetBreakPtCallback(NULL);
}

void GSgfxGPUInit(void)
{
    _gpuImmedBuf = GSmemAlloc(GPU_IMMEDIATE_BUFFER_SIZE);
    DCFlushRange(_gpuImmedBuf, GPU_IMMEDIATE_BUFFER_SIZE);
    _gpuImmedFifo = (GSgfx_GCFifoObj *)GXInit(_gpuImmedBuf, GPU_IMMEDIATE_BUFFER_SIZE);
    _gpuMode = GSGFX_GPU_MODE_NONE;
    gsgfx_gpu_mode = GSGFX_GPU_MODE_NONE;
    gsgfxState->blockFifo = NULL;
    gsgfxState->blockGpuFifo = NULL;
    gsgfxState->blockStart = NULL;
    gsgfxState->blockEnd = NULL;
    gsgfxState->blockComplete = 0;
    gsgfxState->blockActive = 0;
    _gpuSuppressDrawDoneCallback = 0;
    _gpuSuppressDrawSyncCallback = 0;
    _gpuSuppressBreakpointCallback = 0;
    _gpuDrawDoneUserCB = NULL;
    _gpuDrawSyncUserCB = NULL;
    _gpuBreakpointUserCB = NULL;
    GXSetDrawDoneCallback(_gpuDrawDoneCallback);
    GXSetDrawSyncCallback(_gpuDrawSyncCallback);
    GXSetBreakPtCallback(_gpuBreakpointCallback);
}

void _gpuBreakpointCallback(void)
{
    GXFifoObj *fifo = GXGetGPFifo();
    if (fifo != (GXFifoObj *)_gpuImmedFifo) {
        OSEnableInterrupts();
        GXBool readIdle, unused;
        do {
            GXGetGPStatus(&unused, &unused, &readIdle, &unused, &unused);
        } while (readIdle != GX_TRUE);
        OSDisableInterrupts();
        GXSaveGPFifo(fifo);
        gsgfxState->blockComplete = 1;
        gsgfxState->blockActive = 0;
        GSgfx_GCFifoObj *blockFifo = gsgfxState->blockFifo;
        gpuSetFifoReadPtr(blockFifo, gsgfxState->blockEnd);
        blockFifo->blockComplete = 1;
        if (_gpuSuppressBreakpointCallback != 1 && _gpuBreakpointUserCB != NULL) {
            _gpuBreakpointUserCB();
        }
    }
}

bool GSgfxGPUIsIdle(void)
{
    GXBool unused, readIdle;
    GXGetGPStatus(&unused, &unused, &readIdle, &unused, &unused);
    return readIdle == GX_TRUE;
}

void GSgfxGPUSetFifoWritePtr(GSgfx_GCFifoObj *fifo, void *writePtr)
{
    void *readPtr, *oldWritePtr;
    BOOL interrupts = OSDisableInterrupts();
    GXGetFifoPtrs((GXFifoObj *)fifo, &readPtr, &oldWritePtr);
    GXInitFifoPtrs((GXFifoObj *)fifo, readPtr, writePtr);
    OSRestoreInterrupts(interrupts);
}

void _gpuDrawSyncCallback(u16 token)
{
    if (_gpuSuppressDrawSyncCallback != 1 && _gpuDrawSyncUserCB != NULL) {
        _gpuDrawSyncUserCB(token);
    }
}

void _gpuDrawDoneCallback(void)
{
    if (_gpuSuppressDrawDoneCallback != 1 && _gpuDrawDoneUserCB != NULL) {
        _gpuDrawDoneUserCB();
    }
}
