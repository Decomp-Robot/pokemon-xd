#include <dolphin/dsp.h>

#include "__dsp.h"

void __DSP_debug_printf(const char* fmt, ...) {}

// UNUSED: the game never calls this; dead-stripped at link.
DSPTaskInfo* __DSPGetCurrentTask(void) {
    return __DSP_curr_task;
}
