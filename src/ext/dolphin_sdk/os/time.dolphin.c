#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

// UNUSED: the game never calls this; dead-stripped at link.
OSTime __get_clock(void) {
    return __OSGetSystemTime();
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 __get_time(void) {
    return OSTicksToSeconds(OSGetTime()) - 0x43E83E00;
}

// UNUSED: the game never calls this; dead-stripped at link.
int __to_gm_time(void) {
    return 0;
}
