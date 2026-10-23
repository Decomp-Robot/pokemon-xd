#ifndef GSMSG_H
#define GSMSG_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Packed message dimensions: width in the high half, height in the low half.
unsigned long GSmsgGetMessageSize(u32 msgID);
u16 *GScharCpy(u16 *destination, const u16 *source);

void GSmsgPrint2(s32 layer, long x, long y, u32 color, u32 msgID);

s32 GSmsgPrintRect(s32 layer, s32 x, s32 y, s32 width, s32 height,
                     u32 color, s32 msgID);

#ifdef __cplusplus
}
#endif

#endif
