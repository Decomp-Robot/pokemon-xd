#ifndef GS_SND_H
#define GS_SND_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 GSsndGetStatus(u32 soundID);
void GSsndPlayNow(long soundID);
void GSsndStopNow(unsigned long soundID);

#ifdef __cplusplus
}
#endif

#endif
