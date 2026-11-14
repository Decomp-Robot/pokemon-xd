#include <flashcalc.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static f32 blend(f32 first, f32 second, f32 t);
static f32 blend(s32 first, s32 second, f32 t);

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

extern "C" s32 flcalcBlend(s32 first, s32 second, f32 t)
{
    return (s32)blend(first, second, t);
}

extern "C" u32 flcalcBlendColor(u32 first, u32 second, f32 t)
{
    f32 firstAlpha = (f32)(first & 0xff);
    f32 firstRed = (f32)(first >> 24);
    f32 firstGreen = (f32)((first >> 16) & 0xff);
    f32 firstBlue = (f32)((first >> 8) & 0xff);

    u32 alpha = (u32)(blend(firstAlpha, (f32)(second & 0xff), t));
    u32 red = (u32)(blend(firstRed, (f32)(second >> 24), t));
    u32 green = (u32)(blend(firstGreen, (f32)((second >> 16) & 0xff), t));
    u32 blue = (u32)(blend(firstBlue, (f32)((second >> 8) & 0xff), t));

    return alpha | (red << 24) | (green << 16) | (blue << 8);
}

extern "C" void flcalcConvFlashMatrix(FLASHMATRIXF* matrix, FLASHMATRIX* flashMatrix)
{
    f32 m00 = (f32)flashMatrix->m00;
    f32 m01 = (f32)flashMatrix->m01;
    f32 m10 = (f32)flashMatrix->m10;
    f32 m11 = (f32)flashMatrix->m11;
    f32 tx = (f32)flashMatrix->tx;
    f32 ty = (f32)flashMatrix->ty;

    matrix->m00 = m00 / 65536.0f;
    matrix->m01 = m01 / 65536.0f;
    matrix->m10 = m10 / 65536.0f;
    matrix->m11 = m11 / 65536.0f;
    matrix->tx = tx / 20.0f;
    matrix->ty = ty / 20.0f;
}

extern "C" void flcalcBezier3(GSvec2* point, GSvec2* control, f32 t)
{
    f32 inverseSquared = (lbl_804EF3D4 - t) * (lbl_804EF3D4 - t);
    f32 parameterSquared = t * t;

    point->x = control[0].x * inverseSquared + 2.0f * control[1].x * (lbl_804EF3D4 - t) * t + control[2].x * parameterSquared;
    point->y = control[0].y * inverseSquared + 2.0f * control[1].y * (lbl_804EF3D4 - t) * t + control[2].y * parameterSquared;
}

static f32 blend(f32 first, f32 second, f32 t)
{
    return t * (second - first) + first;
}

static f32 blend(s32 first, s32 second, f32 t)
{
    return t * (f32)(second - first) + (f32)first;
}

extern "C" {
const f32 lbl_804EF3D0 = 0.0f;
const f32 lbl_804EF3D4 = 1.0f;
}
