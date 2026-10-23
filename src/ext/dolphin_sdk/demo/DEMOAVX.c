#include <dolphin.h>
#include <dolphin/gx.h>
#include <dolphin/demo.h>
#include "fake_tgmath.h"

static s16 __AVX_internal_buffer[3200] ATTRIBUTE_ALIGN(32);

static void (*__AVX_save_isr)(void);

static u32 __AVX_num_frames;
static u32 __AVX_num_filled;
static u32 __AVX_curr_frame;

static u16* __AVX_buffer;
static s16* __AVX_left_buffer;
static s16* __AVX_right_buffer;
static u32 __AVX_write_ptr = 0;
static u32 __AVX_buffer_size = 0;

static BOOL flag = FALSE;

// UNUSED: the game never calls this; dead-stripped at link.
static void __DEMOAVX_isr(void) {
    u32 frame_address;

    if (__AVX_save_isr) {
        (*__AVX_save_isr)();

        frame_address = 0x80000000 | AIGetDMAStartAddr();
        ASSERTMSGLINE(83, frame_address, "AVX: frame address is NULL!\n");

        DCInvalidateRange((void*)frame_address, 640);
        memcpy((void *)&__AVX_buffer[__AVX_curr_frame * 320], (void*)frame_address, 640);
        DCFlushRange((void*)&__AVX_buffer[__AVX_curr_frame * 320], 640);

        __AVX_curr_frame = (__AVX_curr_frame + 1) % __AVX_num_frames;
        __AVX_num_filled = (__AVX_num_filled + 1) % 10;
        if (__AVX_curr_frame > 4) {
            flag = TRUE;
        }
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 DEMOAVXGetNumFilled(void) {
    u32 tmp;
    BOOL old;

    old = OSDisableInterrupts();

    tmp = __AVX_num_filled;
    __AVX_num_filled = 0;

    OSRestoreInterrupts(old);
    return tmp;
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 DEMOAVXGetFrameCounter(void) {
    return __AVX_curr_frame;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void DEMOAVXAttach(void* buffer, u32 num_frames) {
    BOOL old;
    u32 i;

    __AVX_buffer = (u16*)buffer;
    __AVX_num_frames = num_frames;
    __AVX_num_filled = 0;
    __AVX_curr_frame = 0;
    
    for (i = 0; i < num_frames * 320; i++) {
        __AVX_buffer[i] = 0;
    }

    DCFlushRange(__AVX_buffer, num_frames * 320);

    old = OSDisableInterrupts();
    __AVX_save_isr = AIRegisterDMACallback(__DEMOAVX_isr);
    OSRestoreInterrupts(old);
}
