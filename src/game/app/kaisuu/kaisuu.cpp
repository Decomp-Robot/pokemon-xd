#include "joutai.h"
#include "app/kaisuu/kaisuuBios.h"
#include "hsdRandom.h"

s32 kaisuuGetKaisuu(s32 dataId)
{
    s32 max = kaisuuBiosGetMax(dataId);
    s32 min = kaisuuBiosGetMin(dataId);

    if (max == min) {
        return min;
    } else {
        u16 random = HSD_Rand();
        return min + random % (max - min + 1);
    }
}
