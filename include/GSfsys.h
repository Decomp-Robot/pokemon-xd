#ifndef GS_FSYS_H
#define GS_FSYS_H

#include <GSAPI/GSfsys/GSfsys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compatibility names retained for existing game callers. */
typedef GSfsysReadCallback GSfsysCompletionFunc;
long GSfsysGetDriveStatus(void);

#ifdef __cplusplus
}
#endif

#endif
