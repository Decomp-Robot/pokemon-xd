#ifndef GS_GAPP_H
#define GS_GAPP_H

#include <types.h>

typedef void (*GSgappCallback)(unsigned long appID, unsigned long userData);

#ifdef __cplusplus
extern "C" {
#endif

void GSgappInit(s32 startApp, s32 count);
void GSgappSetName(s32 hdl, char* name);
void GSgappUnblock(s32 hdl);
void GSgappBlock(s32 hdl);
void GSgappSetPauseLevel(s32 hdl, s32 pauseLevel);
void GSgappTerminate(s32 hdl);
void GSgappUpdate(void);
void GSgappPauseOff(void);
void GSgappPauseOn(void);

s32 GSgappCreate(s32 app, s32 priority, s32 userData, GSgappCallback callback);

#ifdef __cplusplus
}
#endif

#endif
