#include "sexData.h"

enum {
    SEX_NAME_MSG_MALE = 3301,
    SEX_NAME_MSG_FEMALE = 3302,
    SEX_NAME_MSG_GENDERLESS = 3303
};

SexData sex_data[] = {
    {SEX_FIXED_RATIO_MALE, SEX_NAME_MSG_MALE},
    {SEX_FIXED_RATIO_FEMALE, SEX_NAME_MSG_FEMALE},
    {SEX_FIXED_RATIO_GENDERLESS, SEX_NAME_MSG_GENDERLESS}
};

u32 sex_data_number = sizeof(sex_data) / sizeof(SexData);
