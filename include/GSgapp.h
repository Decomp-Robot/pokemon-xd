#ifndef GS_GAPP_H
#define GS_GAPP_H

#include <types.h>

typedef u32 GSgappID;
typedef void (*GSgappUpdateCallback)(GSgappID appID, void *userData);

enum GSgappType { GS_GAPP_FRAME_UPDATE = 1 };

#ifdef __cplusplus
extern "C" {
#endif
GSgappID GSgappCreate(u32 type, u8 priority, void *userData, GSgappUpdateCallback update);
void GSgappTerminate(GSgappID appID);
#ifdef __cplusplus
}
#endif

#endif
