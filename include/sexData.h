#ifndef INCLUDE_SEXDATA_H
#define INCLUDE_SEXDATA_H

#include <types.h>

typedef struct SexData SexData;

// Fixed-gender entry.
struct SexData {
    u8 fixedRatio;
    u32 nameMessageId;
};

// Ratio bytes follow the usual Pokemon gender convention: 0 is always male,
// 254 is always female and 255 is genderless.
enum SexFixedRatio {
    SEX_FIXED_RATIO_MALE = 0,
    SEX_FIXED_RATIO_FEMALE = 254,
    SEX_FIXED_RATIO_GENDERLESS = 255
};

#ifdef __cplusplus
extern "C" {
#endif

extern SexData sex_data[3];
extern u32 sex_data_number;

u8 sexGetPokemonSexRaitoKotei(u16 index);

#ifdef __cplusplus
}
#endif

#endif
