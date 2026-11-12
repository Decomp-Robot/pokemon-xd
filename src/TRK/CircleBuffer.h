#ifndef TRK_CIRCLEBUFFER_H
#define TRK_CIRCLEBUFFER_H

#include <dolphin/types.h>

typedef struct CircleBuffer {
    u8* readPtr;
    u8* writePtr;
    u8* buffer;
    u32 size;
    u32 available;
    u32 free;
    u32 criticalSection;
} CircleBuffer;

s32 CircleBufferReadBytes(CircleBuffer* buffer, u8* data, u32 count);
s32 CircleBufferWriteBytes(CircleBuffer* buffer, const u8* data, u32 count);
void CircleBufferInitialize(CircleBuffer* buffer, u8* store, u32 size);
u32 CircleBufferGetBytesAvailable(CircleBuffer* buffer);

#endif
