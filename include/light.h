#ifndef LIGHT_H
#define LIGHT_H
#include <types.h>
#include <dolphin/gx/GXStruct.h>
struct GSlight;
struct GSvec;
#ifdef __cplusplus
extern "C" {
#endif
void scriptLightSetMotion(int, int, int, int);
void scriptLightStopMotion(int);
int scriptLighgtWaitMotion(int, int);
GSlight* lightGetLight(int);
void GSlightSetType(GSlight*, int);
void GSlightSetPosition(GSlight*, GSvec*);
void GSlightSetColor(GSlight*, GXColor);
void GSlightGetPosition(GSlight*, GSvec*);
#ifdef __cplusplus
}
#endif
#endif
