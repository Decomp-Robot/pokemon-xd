#ifndef FLASHCALC_H
#define FLASHCALC_H

#include <types.h>
#include <GSmathTypes.h>

typedef struct FLASHCXFORM {
    s16 alphaMultiplier;
    s16 alphaAddend;
    s16 redMultiplier;
    s16 redAddend;
    s16 greenMultiplier;
    s16 greenAddend;
    s16 blueMultiplier;
    s16 blueAddend;
} FLASHCXFORM;

/* Affine transform as stored in a Flash MATRIX record. The linear
 * coefficients are 16.16 fixed point and the translation is in twips. */
typedef struct FLASHMATRIX {
    s32 m00;
    s32 m01;
    s32 m10;
    s32 m11;
    s32 tx;
    s32 ty;
} FLASHMATRIX;

/* The same transform in the float form the renderer transforms points with. */
typedef struct FLASHMATRIXF {
    f32 m00;
    f32 m01;
    f32 m10;
    f32 m11;
    f32 tx;
    f32 ty;
} FLASHMATRIXF;

#ifdef __cplusplus
extern "C" {
#endif

__declspec(section ".sdata2") extern const f32 lbl_804EF3D0;
__declspec(section ".sdata2") extern const f32 lbl_804EF3D4;

void flcalcBezier3(GSvec2* point, GSvec2* control, f32 t);
void flcalcConvFlashMatrix(FLASHMATRIXF* matrix, FLASHMATRIX* flashMatrix);
u32 flcalcBlendColor(u32 first, u32 second, f32 t);
s32 flcalcBlend(s32 first, s32 second, f32 t);
void flcalcCreateCxform(FLASHCXFORM* transform, u32 color);
u32 flcalcMultiTransformColor(const FLASHCXFORM* transform, u32 color);
void flcalcUnitCxform(FLASHCXFORM* transform);
void flcalcUnitMatrix2(FLASHMATRIX* matrix);
void flcalcMulCxform(FLASHCXFORM* destination, FLASHCXFORM* first, FLASHCXFORM* second);

#ifdef __cplusplus
}
#endif

#endif
