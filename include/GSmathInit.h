#ifndef GSMATH_INIT_H
#define GSMATH_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Set up GSmath quantization registers, seed the random generator and
// build the cosine table
void GSmathInit(void);

#ifdef __cplusplus
}
#endif

#endif // GSMATH_INIT_H
