#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include <types.h>

extern "C" void fadeEffectInit(void);
extern "C" void fadeEffectHookFunction_Doku_Init(void);

class FADE_EFFECT_BALL {
public:
    FADE_EFFECT_BALL() {}

    f32 positionX;
    f32 positionY;
    f32 positionZ;
    f32 rotation;
    s32 alpha;
};

#endif
