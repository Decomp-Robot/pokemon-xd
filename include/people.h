#ifndef PEOPLE_H
#define PEOPLE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void peopleSetTresureSearchDistance(u16 treasureId, float distance);

void peopleTalkMessageBgmMask(bool);
void peopleTalkMessageBgmSetFadeSpeed(float, float);
void peopleTalkMessageBgmSetVolumeRate(float);
#ifdef __cplusplus
}
#endif

#endif
