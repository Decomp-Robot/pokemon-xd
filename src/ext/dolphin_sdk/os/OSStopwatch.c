#include <dolphin.h>
#include <dolphin/os.h>

// UNUSED: the game never calls this; dead-stripped at link.
void OSInitStopwatch(OSStopwatch* sw, char* name) {
    sw->name = name;
    sw->total = 0;
    sw->hits = 0;
    sw->min = 0x00000000FFFFFFFF;
    sw->max = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
void OSStartStopwatch(OSStopwatch* sw) {
    sw->running = TRUE;
    sw->last = OSGetTime();
}

// UNUSED: the game never calls this; dead-stripped at link.
void OSStopStopwatch(OSStopwatch* sw) {
    OSTime interval;

    if (sw->running) {
        interval = OSGetTime() - sw->last;
        sw->total += interval;
        sw->running = FALSE;
        sw->hits++;
        if (sw->max < interval) {
            sw->max = interval;
        }
        if (interval < sw->min) {
            sw->min = interval;
        }
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
OSTime OSCheckStopwatch(OSStopwatch* sw) {
    OSTime currTotal;

    currTotal = sw->total;
    if (sw->running) {
        currTotal += OSGetTime() - sw->last;
    }
    return currTotal;
}

// UNUSED: the game never calls this; dead-stripped at link.
void OSResetStopwatch(OSStopwatch* sw) {
    OSInitStopwatch(sw, sw->name);
}

// UNUSED: the game never calls this; dead-stripped at link.
void OSDumpStopwatch(OSStopwatch* sw) {
    OSReport("Stopwatch [%s]	:\n", sw->name);
    OSReport("\tTotal= %lld us\n",    OSTicksToMicroseconds(sw->total));
    OSReport("\tHits = %d \n",        sw->hits);
    OSReport("\tMin  = %lld us\n",    OSTicksToMicroseconds(sw->min));
    OSReport("\tMax  = %lld us\n",    OSTicksToMicroseconds(sw->max));
    OSReport("\tMean = %lld us\n",    OSTicksToMicroseconds(sw->total/sw->hits));
}
