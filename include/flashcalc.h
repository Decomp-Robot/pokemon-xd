#ifndef FLASHCALC_H
#define FLASHCALC_H

#include <types.h>

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

#ifdef __cplusplus
extern "C" {
#endif

void flcalcCreateCxform(FLASHCXFORM* transform, u32 color);
u32 flcalcMultiTransformColor(const FLASHCXFORM* transform, u32 color);
void flcalcUnitCxform(FLASHCXFORM* transform);
void flcalcMulCxform(FLASHCXFORM* destination, FLASHCXFORM* first, FLASHCXFORM* second);

#ifdef __cplusplus
}
#endif

#endif
