#ifndef JOUTAI_DATA_H
#define JOUTAI_DATA_H

#include <types.h>

enum {
    JOUTAI_DATA_ID_WEATHER_CLEAR = 78,
    JOUTAI_DATA_ID_WEATHER_SUNLIGHT = 79,
    JOUTAI_DATA_ID_WEATHER_RAIN = 80,
    JOUTAI_DATA_ID_WEATHER_SANDSTORM = 81,
    JOUTAI_DATA_ID_WEATHER_SHADOW_SKY = 82,
    JOUTAI_DATA_ID_WEATHER_HAIL = 83
};

typedef struct JoutaiData JoutaiData;

struct JoutaiData {
    u8 statusKind;
    u8 useBanme;
    u8 writeMode;
    u8 maxCount;
    u8 kaisuuDataId;
    u8 unknown[3];
    u16 status;
    u16 sameJoutai;
    u8 unknown2[4];
    u32 name;
};

#ifdef __cplusplus
extern "C" {
#endif

extern JoutaiData joutai_data[];
extern u32 joutai_data_number;

#ifdef __cplusplus
}
#endif

#endif
