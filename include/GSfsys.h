#ifndef GS_FSYS_H
#define GS_FSYS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void GSfsysFree(u32 archiveID);
void GSfsysPreRead(long archiveID);
u8 GSfsysWait(long archiveID);

#ifdef __cplusplus
}
#endif

#endif
