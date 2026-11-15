#ifndef APP_KAISUU_KAISUU_BIOS_H
#define APP_KAISUU_KAISUU_BIOS_H

#include "types.h"

typedef struct KaisuuData {
    s32 min;
    s32 max;
} KaisuuData;

extern KaisuuData kaisuu_data[];
extern u32 kaisuu_data_number;

#ifdef __cplusplus
extern "C" {
#endif
s32 kaisuuBiosGetMax(s32 dataId);
s32 kaisuuBiosGetMin(s32 dataId);
#ifdef __cplusplus
}
#endif

#endif
