#include "app/kaisuu/kaisuuBios.h"

s32 kaisuuBiosGetMax(s32 dataId)
{
    if (dataId >= kaisuu_data_number) {
        return 0;
    }
    return kaisuu_data[dataId].max;
}

s32 kaisuuBiosGetMin(s32 dataId)
{
    if (dataId >= kaisuu_data_number) {
        return 0;
    }
    return kaisuu_data[dataId].min;
}
