#include "menuPdaMemoListXD.h"

#include <types.h>

// Default model angle of each component of a memo-list entry.
f32 _defaultAngle[3] = { 0.0f, -0.3f, 0.0f };

extern "C" f32 menuPdaMemoListGetDefaultAngle(s16 index) {
    return _defaultAngle[index];
}
