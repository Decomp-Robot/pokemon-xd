#ifndef PLACE_H
#define PLACE_H

#include <types.h>

// One of the places the roaming Miror B can be sent to.
typedef struct PLACE_POINT {
    u16 mWeight;  // Weight the place is rolled with, summed over the table.
    u16 mPlace;   // Floor the place stands for.
    u32 mMessage; // Place name message the radar prints for it.
} PLACE_POINT;

// Slots of the place table in the order the roaming uses.
enum PlacePointID {
    PLACE_POINT_PYRITE_TOWN = 0,
    PLACE_POINT_REALGAM_TOWER = 1,
    PLACE_POINT_FIRST_SPOT = 2,
    PLACE_POINT_SECOND_SPOT = 3,
    PLACE_POINT_THIRD_SPOT = 4,
    PLACE_POINT_OUTSKIRT_STAND = 5,
    PLACE_POINT_GATEON_PORT = 6,
    PLACE_POINT_MAX = 7
};

// Work of the roaming Miror B: the settings of the appearance roll and the GS
// flags his state is kept in.
struct PLACE_WORK {
    u16 mStepMax;   // Steps walked before an appearance is rolled.
    u16 mUpRate;    // Roll below which the place level goes up.
    u16 mDownRate;  // Width of the downward band after mUpRate.
    u16 mRateRange; // Size of the appearance roll.
    u16 mLevelMax;  // He appears at one level below this threshold.
    u16 mStayMax;   // Steps he stays before he leaves.
    u32 mStartFlag; // Set while the roaming is enabled.
    u32 mStepFlag;  // Step counter.
    u32 mPlaceFlag; // Place level, the radar measures his distance with it.
    u32 mMiraboPlaceFlag; // Place he is on, 0 for none.
    u32 mStayFlag;  // Stay counter.
    u32 mStopFlag;  // Set while the roaming is suppressed.
    u32 mIconFlag;  // Shows his icon on the field.
    u32 mProgressEntryMin; // First flag_etc entry of the progress window.
    u32 mProgressEntryMax; // Last flag_etc entry of the progress window.
    u32 mRandomFlag; // Random appearance variant, 0 or 1.
    PLACE_POINT mPoint[PLACE_POINT_MAX]; // Places he can be sent to.
};

extern PLACE_WORK *gPlaceWork;

#endif
