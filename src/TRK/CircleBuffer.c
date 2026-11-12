#include "CircleBuffer.h"
#include "critical_section.h"
#include <libc/string.h>

s32 CircleBufferReadBytes(CircleBuffer* buffer, u8* data, u32 count) {
    u32 run;

    if (count > buffer->available) {
        return -1;
    }

    MWEnterCriticalSection(&buffer->criticalSection);

    run = buffer->size - (u32)(buffer->readPtr - buffer->buffer);
    if (count < run) {
        memcpy(data, buffer->readPtr, count);
        buffer->readPtr += count;
    } else {
        memcpy(data, buffer->readPtr, run);
        memcpy(data + run, buffer->buffer, count - run);
        buffer->readPtr = buffer->buffer + count - run;
    }

    if (buffer->size == (u32)(buffer->readPtr - buffer->buffer)) {
        buffer->readPtr = buffer->buffer;
    }
    buffer->free += count;
    buffer->available -= count;

    MWExitCriticalSection(&buffer->criticalSection);
    return 0;
}

s32 CircleBufferWriteBytes(CircleBuffer* buffer, const u8* data, u32 count) {
    u32 run;

    if (count > buffer->free) {
        return -1;
    }

    MWEnterCriticalSection(&buffer->criticalSection);

    run = buffer->size - (u32)(buffer->writePtr - buffer->buffer);
    if (run >= count) {
        memcpy(buffer->writePtr, data, count);
        buffer->writePtr += count;
    } else {
        memcpy(buffer->writePtr, data, run);
        memcpy(buffer->buffer, data + run, count - run);
        buffer->writePtr = buffer->buffer + count - run;
    }

    if (buffer->size == (u32)(buffer->writePtr - buffer->buffer)) {
        buffer->writePtr = buffer->buffer;
    }
    buffer->free -= count;
    buffer->available += count;

    MWExitCriticalSection(&buffer->criticalSection);
    return 0;
}

void CircleBufferInitialize(CircleBuffer* buffer, u8* store, u32 size) {
    buffer->buffer = store;
    buffer->size = size;
    buffer->readPtr = buffer->buffer;
    buffer->writePtr = buffer->buffer;
    buffer->available = 0;
    buffer->free = buffer->size;
    MWInitializeCriticalSection(&buffer->criticalSection);
}

u32 CircleBufferGetBytesAvailable(CircleBuffer* buffer) {
    return buffer->available;
}

