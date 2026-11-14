#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
void scriptModelSetFractionalFramesFlag(unsigned long resourceID, bool enabled);
void scriptModelPartPaticle(unsigned long modelID, unsigned long particleID, long partIndex);
void scriptModelDeleteParticle(unsigned long particleID, bool killFamily);
void scriptModelSetParticle(unsigned long modelID, unsigned long particleID, u8);
int scriptModelDetach(unsigned long resourceID);
int scriptModelAttach(unsigned long resourceID, unsigned long parentID, long partIndex);
int scriptModelWaitMotion(unsigned long resourceID, long wait);
void scriptModelStopMotion(unsigned long resourceID);
int scriptModelSetEndedMotion(unsigned long resourceID, long motion, bool loop);
void scriptModelSetMotionRate(unsigned long resourceID, long motion, long frame, float rate, long loop);
void scriptModelSetMotion(unsigned long resourceID, long motion, long frame, long loop);
void scriptModelSetDisp(unsigned long resourceID, long display);
void scriptModelClose(unsigned long resourceID);
unsigned long scriptModelOpen(unsigned long resourceID);
void scriptModelInit(void);
#ifdef __cplusplus
}
#endif
#endif
