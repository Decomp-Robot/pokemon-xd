#include <string.h>
#include <dolphin/os.h>

#include <GSsnd.h>
#include <GSAPI/GSsnd/sndStream.h>
#include <GSAPI/GSfsys/GSfsys.h>

#define SND_STREAM_HALF_SIZE 32768
#define SND_STREAM_HALF_NIBBLES (SND_STREAM_HALF_SIZE * 2)
#define SND_STREAM_STAGING_OFFSET (SND_STREAM_HALF_SIZE * 2)
#define SND_STREAM_ADPCM_FRAME_BYTES 8
#define SND_STREAM_ADPCM_HEADER_NIBBLES 2
#define SND_STREAM_DATA_SIZE_MASK 0x3FFFFFFE

void _sndStreamPrepareGetData(GSsndStream* stream)
{
    GSsndStream* target;
    u32 level;

    level = OSDisableInterrupts();
    if (stream->next != 0) {
        target = stream->next;
    } else {
        target = stream;
    }
    if (target->buffer[stream->bufferIndex].aramState[stream->halfIndex] == GSSND_ARAM_EMPTY) {
        _sndStreamPrepareGetData_Exec(stream, target, stream->halfIndex);
    }
    OSRestoreInterrupts(level);
}

void _sndStreamPrepareGetData_Exec(GSsndStream* stream, GSsndStream* target, u8 halfIndex)
{
    switch (target->buffer[stream->bufferIndex].state) {
    case GSSND_BUFFER_READY:
        _sndStreamPrepareGetData_GetNewData(stream, target, halfIndex);
        break;
    case GSSND_BUFFER_STAGED:
        _sndStreamPrepareGetData_GetOldData(stream, target, halfIndex);
        break;
    }
}

void _sndStreamPrepareGetData_GetOldData(GSsndStream* stream, GSsndStream* target, u8 halfIndex)
{
    GSsndStreamBuffer* buffer = &target->buffer[stream->bufferIndex];

    buffer->state = GSSND_BUFFER_COPYING;
    _sndStreamPrepareGetDataCallback_Exec(stream, target, buffer, buffer->stagingReadOffset,
                                          halfIndex);
}

void _sndStreamPrepareGetData_GetNewData(GSsndStream* stream, GSsndStream* target, u8 halfIndex)
{
    GSsndStreamBuffer* buffer = &target->buffer[stream->bufferIndex];
    u8* staging;
    enum GSsndStreamBufferState state;
    u32 offset;

    state = buffer->state;
    buffer->state = GSSND_BUFFER_READING;
    staging = (u8*)stream->sampleBuffer + SND_STREAM_STAGING_OFFSET;
    if (stream->next != 0) {
        offset = target->fileReadOffset + stream->bufferSize / 2;
    } else {
        offset = target->fileReadOffset;
    }
    if (target->loopFlag == 0 && stream->readSamplePosition >= target->prefetch.sampleCount) {
        memset(staging, 0, SND_STREAM_HALF_SIZE);
        _sndStreamPrepareGetDataCallback(0, (void*)halfIndex, (long)stream);
        return;
    }
    if (GSfsysStreamGetData(target->fileNo, target->argument, staging, SND_STREAM_HALF_SIZE, offset,
                            _sndStreamPrepareGetDataCallback, (void*)halfIndex, (long)stream) == 0) {
        buffer->state = state;
    }
}

void _sndStreamPrepareGetDataCallback(long fileMode, void* context, long argument)
{
    GSsndStream* stream = (GSsndStream*)argument;
    GSsndStream* target;
    GSsndStream* partner;
    GSsndStreamBuffer* buffer;
    u32 pos;
    u32 headerPos;
    u32 readEnd;
    u32 loopStart;
    u8 halfIndex;

    halfIndex = (u8)context;
    if (stream->next != 0) {
        target = stream->next;
        partner = stream;
    } else {
        target = stream;
        partner = stream->chain;
    }
    buffer = &target->buffer[stream->bufferIndex];
    pos = _sndStreamNibble_HeaderOFF2ON(stream->readSamplePosition);
    readEnd = pos + SND_STREAM_HALF_NIBBLES;
    if (target->loopFlag == 1 && target->loopReached == 1 &&
        pos <= (loopStart = target->loopStartNibble - SND_STREAM_ADPCM_HEADER_NIBBLES) &&
        loopStart < readEnd) {
        headerPos = _sndStreamNibble_GetHeaderPos((u16)loopStart);
    } else {
        headerPos = 0;
    }
    stream->readSamplePosition += stream->samples / 2;
    if (target->loopFlag == 1 && stream->readSamplePosition >= target->loopEndSampleAligned) {
        stream->readSamplePosition = target->loopStartSampleAligned;
        target->loopReached = 1;
    }
    stream->channelReadComplete = 1;
    if (target->channelReadComplete != 0 && partner->channelReadComplete != 0) {
        target->fileReadOffset += target->bufferSize;
        if (target->fileReadOffset >= (target->prefetch.dataSize & SND_STREAM_DATA_SIZE_MASK)) {
            target->fileReadOffset = 0;
        } else if (target->fileReadOffset >= GSfsysStreamGetCacheSize()) {
            target->fileReadOffset = 0;
        }
        partner->channelReadComplete = 0;
        target->channelReadComplete = 0;
    }
    _sndStreamPrepareGetDataCallback_Exec(stream, target, buffer, headerPos, halfIndex);
}

void _sndStreamPrepareGetDataCallback_Exec(GSsndStream* stream, GSsndStream* target,
                                           GSsndStreamBuffer* buffer, u32 headerPos, u8 halfIndex)
{
    u32 sampleAddress = (u32)stream->sampleBuffer;
    u32 staging = sampleAddress + SND_STREAM_STAGING_OFFSET + headerPos;
    u32 copySize = SND_STREAM_HALF_SIZE - headerPos;
    u32 writeOffset = buffer->halfWriteOffset[halfIndex];
    u32 dest;

    if (halfIndex != 0) {
        dest = sampleAddress + stream->bufferSize / 2 + writeOffset;
    } else {
        dest = sampleAddress + writeOffset;
    }
    if (writeOffset + copySize > SND_STREAM_HALF_SIZE) {
        copySize = SND_STREAM_HALF_SIZE - writeOffset;
        buffer->stagingReadOffset = headerPos + copySize;
        buffer->state = GSSND_BUFFER_STAGED;
    } else {
        buffer->stagingReadOffset = 0;
        buffer->state = GSSND_BUFFER_READY;
    }
    memcpy((void*)dest, (void*)staging, copySize);
    buffer->halfWriteOffset[halfIndex] += copySize;
    if (buffer->halfWriteOffset[halfIndex] >= SND_STREAM_HALF_SIZE) {
        buffer->halfWriteOffset[halfIndex] = 0;
        buffer->aramState[halfIndex] = GSSND_ARAM_FILLED;
    } else {
        buffer->aramState[halfIndex] = GSSND_ARAM_EMPTY;
    }
}

u32 _sndStreamNibble_GetHeaderPos(u32 nibbles)
{
    return (u32)(nibbles / 2.0) & ~(SND_STREAM_ADPCM_FRAME_BYTES - 1);
}

u32 _sndStreamNibble_HeaderOFF2ON(u32 nibbles)
{
    nibbles *= 8.0 / 7.0;
    return nibbles;
}

u32 _sndStreamNibble_HeaderON2OFF(u32 nibbles)
{
    nibbles *= 7.0 / 8.0;
    return nibbles;
}
