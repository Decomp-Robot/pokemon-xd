#ifndef PROGRESSION_DATA_H
#define PROGRESSION_DATA_H

#include <types.h>

struct ProgressionRequirementEntry {
    u8 mPadding00[8];
    u32 mRequiredProgress;
    u8 mPadding0C[0x14];
};

extern ProgressionRequirementEntry* worldMapScooterUnlockData;

#endif
