#include "trk.h"

const char sTRKNoBufferAvailable[] = "ERROR : No buffer available\n";

static inline DSError TRKReadBuffer1_ui32(TRKBuffer *buffer, u32 *dest)
{
    DSError err;
    u8 *data;
    u8 reversed[4];
    if (gTRKBigEndian) {
        data = (u8 *)dest;
    } else {
        data = reversed;
    }
    err = TRKReadBuffer(buffer, data, sizeof(*dest));
    if (!gTRKBigEndian && err == DS_NoError) {
        ((u8 *)dest)[0] = data[3];
        ((u8 *)dest)[1] = data[2];
        ((u8 *)dest)[2] = data[1];
        ((u8 *)dest)[3] = data[0];
    }
    return err;
}

DSError TRKInitializeMessageBuffers(void)
{
    int i;

    for (i = 0; i < kMessageBufferCount; i++) {
        TRKBuffer *buf = &gTRKMessageBuffers[i];

        TRKLockInit(buf);
        TRKLock(buf);
        buf->isInUse = FALSE;
        TRKUnlock(buf);
    }

    return DS_NoError;
}

DSError TRKGetFreeBuffer(MessageBufferID *bufferID, TRKBuffer **buffer)
{
    s32 result = DS_NoMessageBufferAvailable;
    s32 i;

    *buffer = 0;

    for (i = 0; i < kMessageBufferCount; i++) {
        TRKBuffer *candidate = TRKGetBuffer(i);

        TRKLock(candidate);
        if (!candidate->isInUse) {
            candidate->length = 0;
            candidate->position = 0;
            candidate->isInUse = 1;
            *buffer = candidate;
            *bufferID = i;
            result = DS_NoError;
            // Free buffer found: end the scan.
            i = kMessageBufferCount;
        }
        TRKUnlock(candidate);
    }

    if (result == DS_NoMessageBufferAvailable) {
        usr_puts_serial(sTRKNoBufferAvailable);
    }
    return result;
}

TRKBuffer *TRKGetBuffer(MessageBufferID bufferID)
{
    TRKBuffer *buffer = 0;

    if (bufferID >= 0 && bufferID < kMessageBufferCount) {
        buffer = &gTRKMessageBuffers[bufferID];
    }
    return buffer;
}

void TRKReleaseBuffer(MessageBufferID bufferID)
{
    TRKBuffer *buffer;

    // Events carry -1 when they own no buffer.
    if (bufferID == -1) {
        return;
    }
    if (bufferID < 0) {
        return;
    }
    if (bufferID >= kMessageBufferCount) {
        return;
    }
    buffer = &gTRKMessageBuffers[bufferID];

    TRKLock(buffer);
    buffer->isInUse = 0;
    TRKUnlock(buffer);
}

void TRKResetBuffer(TRKBuffer *buffer, BOOL keepData)
{
    buffer->length = 0;
    buffer->position = 0;

    if (!keepData) {
        TRK_memset(buffer->data, 0, kMessageBufferSize);
    }
}

DSError TRKSetBufferPosition(TRKBuffer *buffer, u32 position)
{
    s32 err = DS_NoError;

    if (position > kMessageBufferSize) {
        err = DS_MessageBufferOverflow;
    } else {
        buffer->position = position;
        if (position > buffer->length) {
            buffer->length = position;
        }
    }
    return err;
}

DSError TRKAppendBuffer(TRKBuffer *buffer, const void *src, u32 size)
{
    s32 err = DS_NoError;
    u32 free;

    if (size == 0) {
        return err;
    }
    free = kMessageBufferSize - buffer->position;
    if (free < size) {
        err = DS_MessageBufferOverflow;
        size = free;
    }
    TRK_STORE_BYTES(buffer, src, size);
    buffer->position += size;
    buffer->length = buffer->position;
    return err;
}

DSError TRKReadBuffer(TRKBuffer *buffer, void *dest, unsigned long size)
{
    s32 err = DS_NoError;
    unsigned int available;

    if (size == 0) {
        return err;
    }
    available = buffer->length - buffer->position;
    if (size > available) {
        err = DS_MessageBufferReadError;
        size = available;
    }
    TRK_memcpy(dest, buffer->data + buffer->position, size);
    buffer->position += size;
    return err;
}

DSError TRKAppendBuffer1_ui64(TRKBuffer *buffer, u64 value)
{
    s32 *bigEndian = &gTRKBigEndian;
    u8 reversed[8];
    u8 *data;
    if (*bigEndian) {
        data = (u8 *)&value;
    } else {
        reversed[0] = ((u8 *)&value)[7];
        reversed[1] = ((u8 *)&value)[6];
        reversed[2] = ((u8 *)&value)[5];
        reversed[3] = ((u8 *)&value)[4];
        reversed[4] = ((u8 *)&value)[3];
        reversed[5] = ((u8 *)&value)[2];
        reversed[6] = ((u8 *)&value)[1];
        reversed[7] = ((u8 *)&value)[0];
        data = reversed;
    }
    return TRKAppendBuffer(buffer, data, sizeof(value));
}

DSError TRKAppendBuffer_ui8(TRKBuffer *buffer, const u8 *src, s32 count)
{
    s32 err;
    s32 errTemp;
    s32 position;
    u32 value;
    s32 i;
    i = 0;
    err = DS_NoError;
    for (; err == DS_NoError && i < count; i++, src++) {
        position = buffer->position;
        value = *src;

        if (position >= kMessageBufferSize) {
            errTemp = DS_MessageBufferOverflow;
        } else {
            buffer->position = position + 1;
            buffer->data[position] = value;
            buffer->length = buffer->length + 1;
            errTemp = DS_NoError;
        }
        err = errTemp;
    }
    return err;
}

DSError TRKAppendBuffer_ui32(TRKBuffer *buffer, const u32 *src, s32 count)
{
    s32 *bigEndian = &gTRKBigEndian;
    const u32 *p = src;
    s32 err, i;
    u8 reversed[4];
    u32 value;
    u8 *data;

    i = 0;
    err = DS_NoError;
    for (; err == DS_NoError && i < count; p++, i++) {
        value = *p;

        if (*bigEndian) {
            data = (u8 *)&value;
        } else {
            reversed[0] = ((u8 *)&value)[3];
            reversed[1] = ((u8 *)&value)[2];
            reversed[2] = ((u8 *)&value)[1];
            reversed[3] = ((u8 *)&value)[0];
            data = reversed;
        }

        err = TRKAppendBuffer(buffer, data, sizeof(value));
    }
    return err;
}

DSError TRKReadBuffer1_ui64(TRKBuffer *buffer, u64 *dest)
{
    u64 value;
    u8 *source;
    u64 *p;
    s32 err;

    p = dest;
    if (gTRKBigEndian) {
        source = (u8 *)p;
    } else {
        source = (u8 *)&value;
    }
    err = TRKReadBuffer(buffer, source, 8);
    if (!gTRKBigEndian && err == DS_NoError) {
        ((u8 *)p)[0] = source[7];
        ((u8 *)p)[1] = source[6];
        ((u8 *)p)[2] = source[5];
        ((u8 *)p)[3] = source[4];
        ((u8 *)p)[4] = source[3];
        ((u8 *)p)[5] = source[2];
        ((u8 *)p)[6] = source[1];
        ((u8 *)p)[7] = source[0];
    }
    return err;
}

DSError TRKReadBuffer_ui8(TRKBuffer *buffer, u8 *dest, s32 count)
{
    s32 err, errTemp, i;

    i = 0;
    err = DS_NoError;
    for (; err == DS_NoError && i < count; i++) {
        errTemp = TRKReadBuffer(buffer, &dest[i], 1);
        err = errTemp;
    }
    return err;
}

DSError TRKReadBuffer_ui32(TRKBuffer *buffer, u32 *dest, s32 count)
{
    DSError err;
    int i;
    for (i = 0, err = DS_NoError; err == DS_NoError && i < count; i++) {
        err = TRKReadBuffer1_ui32(buffer, &dest[i]);
    }
    return err;
}
