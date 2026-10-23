#ifndef GS_GAPP_H
#define GS_GAPP_H

#include <types.h>

typedef void (*GSgappCallback)(unsigned long appID, unsigned long userData);

#ifdef __cplusplus
extern "C" {
#endif

void GSgappPauseOff(void);
void GSgappPauseOn(void);

s32 GSgappCreate(s32 app, s32 priority, s32 userData, GSgappCallback callback);

#ifdef __cplusplus
}
#endif

#endif
