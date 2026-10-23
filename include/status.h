#ifndef STATUS_H
#define STATUS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
s32 statusGetStatus(u8 statusKind, void* status, u16 dataId, u16 fieldId, u16 index);
#ifdef __cplusplus
}
#endif

#endif
