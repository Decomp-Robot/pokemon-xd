#include <flashcalc.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern "C" void flcalcMulCxform(FLASHCXFORM* destination,
                                FLASHCXFORM* first,
                                FLASHCXFORM* second)
{
    destination->redMultiplier = (s16)(((s32)first->redMultiplier * second->redMultiplier) / 256);
    destination->greenMultiplier = (s16)(((s32)first->greenMultiplier * second->greenMultiplier) / 256);
    destination->blueMultiplier = (s16)(((s32)first->blueMultiplier * second->blueMultiplier) / 256);
    destination->alphaMultiplier = (s16)(((s32)first->alphaMultiplier * second->alphaMultiplier) / 256);
    destination->redAddend = (s16)(first->redAddend + second->redAddend);
    destination->greenAddend = (s16)(first->greenAddend + second->greenAddend);
    destination->blueAddend = (s16)(first->blueAddend + second->blueAddend);
    destination->alphaAddend = (s16)(first->alphaAddend + second->alphaAddend);
}

extern "C" void flcalcUnitCxform(FLASHCXFORM* transform)
{
    transform->redMultiplier = 256;
    transform->greenMultiplier = 256;
    transform->blueMultiplier = 256;
    transform->alphaMultiplier = 256;
    transform->redAddend = 0;
    transform->greenAddend = 0;
    transform->blueAddend = 0;
    transform->alphaAddend = 0;
}

extern "C" u32 flcalcMultiTransformColor(const FLASHCXFORM* transform, u32 color)
{
    u32 red = (color >> 24) & 0xff;
    u32 green = (color >> 16) & 0xff;
    u32 blue = (color >> 8) & 0xff;
    u32 alpha = color & 0xff;
    red = MIN(((red * transform->redMultiplier) >> 8) + transform->redAddend, 255);
    green = MIN(((green * transform->greenMultiplier) >> 8) + transform->greenAddend, 255);
    blue = MIN(((blue * transform->blueMultiplier) >> 8) + transform->blueAddend, 255);
    alpha = MIN(((alpha * transform->alphaMultiplier) >> 8) + transform->alphaAddend, 255);
    return alpha | (red << 24) | (green << 16) | (blue << 8);
}

extern "C" void flcalcCreateCxform(FLASHCXFORM* transform, u32 color)
{
    flcalcUnitCxform(transform);
    transform->redMultiplier = (s16)((color >> 24) & 0xff);
    transform->greenMultiplier = (s16)((color >> 16) & 0xff);
    transform->blueMultiplier = (s16)((color >> 8) & 0xff);
    transform->alphaMultiplier = (s16)(color & 0xff);
}
