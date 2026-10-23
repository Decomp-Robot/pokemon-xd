#include <fade_effect.h>

struct GStexture;


struct FADE_EFFECT_WORK {
    GStexture *texture;
    GStexture *secondaryTexture;
    u16 frameCount;
    u16 unused;
    u16 direction;
    u16 effectState;
    f32 motionIncrement;
    f32 currentMotion;
    f32 motionLimit;
};

FADE_EFFECT_WORK _fadeEffectWork;
FADE_EFFECT_BALL _fadeEffectBall[24];
