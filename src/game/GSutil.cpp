#include <GSutil.h>
#include <math.h>

static f32 GSutilHalfDegrees(f32 halfDegrees) {
    return halfDegrees * 0.5f;
}

f32 gCosTable[181];

void GSmathInitCosTable(void) {
    int i;
    for (i = 0; i < 181; i++) {
        gCosTable[i] = cos(0.017453292f * GSutilHalfDegrees(i));
    }
}
