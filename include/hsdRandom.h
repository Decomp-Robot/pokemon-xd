#ifndef HSD_RANDOM_H
#define HSD_RANDOM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
unsigned long HSD_Rand(void);
s32 HSD_Randi(s32 limit);
f32 HSD_Randf(void);
void _HSD_RandForgetMemory(void);
extern u32 *__hsd_rand_seed_hold;
#ifdef __cplusplus
}
#endif

#endif
