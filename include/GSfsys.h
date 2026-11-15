#ifndef GS_FSYS_H
#define GS_FSYS_H

#include <GSAPI/GSfsys/GSfsys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compatibility names retained for existing game callers. */
typedef GSfsysReadCallback GSfsysCompletionFunc;
long GSfsysGetDriveStatus(void);
u8 GSfsysInit(u32, u32, u32, u32);

#ifdef __cplusplus
}
#endif

#endif
