#include <string.h>
#include <musyx/musyx.h>
#include <dolphin/os.h>
#include <dolphin/os/OSCache.h>

#include <GSmovie.h>
#include <GSAPI/GSfsys/GSfsys.h>

extern "C" s32 THPVideoDecode(void *src, void *yBuffer, void *uBuffer, void *vBuffer, void *workBuffer);

enum { MOVIE_STREAM_PRIORITY = 255 };

static inline void _movieStreamAdvanceRead(THPPlayer *player, u32 frameSize, __GSmovieStreaming *stream)
{
    player->readIndex += THP_STREAM_BLOCK_SIZE;
    if (player->preload != 0) {
        if (player->readIndex >= player->movieDataSize) {
            player->readIndex = 0;
        }
    } else if (player->readIndex >= player->movieDataSize) {
        player->readIndex = 0;
    } else if (player->readIndex >= GSfsysStreamGetCacheSize()) {
        player->readIndex = 0;
    }
    _movieStreamPrepareGetDataCallback_Exec(stream, 0, frameSize);
}


static inline void _movieStreamReleaseReadBuffer(THPReadBuffer *buffer, __GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;
    if (buffer->videoDecoded != 0 && buffer->audioDecoded != 0) {
        buffer->dataReady = 0;
        buffer->videoDecoded = 0;
        player->readBufferIndex++;
        if (player->readBufferIndex >= THP_READ_BUFFER_COUNT) {
            player->readBufferIndex = 0;
        }
        if (player->hasAudio != 0) {
            buffer->audioDecoded = 0;
            player->nextReadIndex++;
            if (player->nextReadIndex >= THP_READ_BUFFER_COUNT) {
                player->nextReadIndex = 0;
            }
        }
    }
}


static inline void _movieStreamReadBlock(__GSmovieStreaming *stream, s32 state, u32 frameSize)
{
    THPPlayer *player = stream->player;

    switch (state) {
    case THP_STREAM_NEED_READ:
        player->streamState = THP_STREAM_READ_PENDING;
        u8 *staging = player->staging;
        u32 readIndex = player->readIndex;
        if ((player->loopFlag & MOVIE_STREAM_LOOP_FLAG) == 0 && player->frameCounter + player->startFrame >= player->numFrames) {
            memset(staging, 0, THP_STREAM_BLOCK_SIZE);
            _movieStreamPrepareGetDataCallback(0, (void *)frameSize, (long)stream);
            return;
        }
        if (player->preload != 0) {
            memcpy(staging, (u8 *)player->movieData + readIndex, THP_STREAM_BLOCK_SIZE);
            _movieStreamPrepareGetDataCallback(0, (void *)frameSize, (long)stream);
            return;
        }
        {
            u8 ok = GSfsysStreamGetData(stream->id, 0, staging, THP_STREAM_BLOCK_SIZE, readIndex,
                                       _movieStreamPrepareGetDataCallback, (void *)frameSize, (long)stream);
            if (ok == 0) {
                player->streamState = state;
            }
        }
        break;
    case THP_STREAM_READ_PENDING:
        return;
    case THP_STREAM_COPY_PENDING:
        _movieStreamPrepareGetDataCallback_Exec(stream, player->pendingOffset, frameSize);
        break;
    default:
        return;
    }
}


/* The caller disables interrupts; finish the request before restoring them. */
static inline void _movieStreamRequestBlock(__GSmovieStreaming *stream, THPPlayer *player, s32 level)
{
    u32 frameSize;
    s32 streamState;

    switch (stream->state) {
    case GSMOVIE_STREAM_FREE:
    case GSMOVIE_STREAM_WAIT_OPEN:
    case GSMOVIE_STREAM_PREPARE_PLAYBACK:
    case GSMOVIE_STREAM_SETUP_BUFFERS:
    case GSMOVIE_STREAM_UNKNOWN_4:
    case GSMOVIE_STREAM_WAIT_PRELOAD:
    case GSMOVIE_STREAM_REQUEST_PRELOAD:
    case GSMOVIE_STREAM_STOP_REQUESTED:
    case GSMOVIE_STREAM_STOPPING:
        goto done;
    }
    if (player->readBuffers[player->currentReadIndex].dataReady != 0) {
        goto done;
    }
    frameSize = player->readFrameSize;
    if (frameSize == 0) {
        goto done;
    }
    streamState = (s32)stream->player->streamState;
    _movieStreamReadBlock(stream, streamState, frameSize);
done:
    OSRestoreInterrupts(level);
}


void _movieStreamPrepareGetDataCallback_Exec(__GSmovieStreaming *stream, u32 offset, u32 count)
{
    u32 length = THP_STREAM_BLOCK_SIZE - offset;
    THPPlayer *player = stream->player;
    u8 *staging = (u8 *)player->staging + offset;
    u8 *readBuffer = (u8 *)player->readBuffers[player->currentReadIndex].data + player->readBuffers[player->currentReadIndex].used;

    if (player->readBuffers[player->currentReadIndex].used + length > count) {
        length = count - player->readBuffers[player->currentReadIndex].used;
        player->pendingOffset = offset + length;
        player->streamState = THP_STREAM_COPY_PENDING;
    } else {
        player->pendingOffset = 0;
        player->streamState = THP_STREAM_NEED_READ;
    }
    memcpy(readBuffer, staging, length);
    player->readBuffers[player->currentReadIndex].used += length;
    if (player->readBuffers[player->currentReadIndex].used >= count) {
        player->readBuffers[player->currentReadIndex].used = 0;
        player->readBuffers[player->currentReadIndex].dataReady = 1;
        player->readBuffers[player->currentReadIndex].frame = player->frameCounter;
        player->readFrameSize = *(u32 *)player->readBuffers[player->currentReadIndex].data;
        player->currentReadIndex++;
        if (player->currentReadIndex >= THP_READ_BUFFER_COUNT) {
            player->currentReadIndex = 0;
        }
        player->frameCounter++;
        if (player->frameCounter + player->startFrame >= player->numFrames) {
            if ((player->loopFlag & MOVIE_STREAM_LOOP_FLAG) != 0) {
                player->frameCounter = 0;
                player->pendingOffset = 0;
                player->streamState = THP_STREAM_NEED_READ;
                player->readFrameSize = player->loopFrameSize;
            } else {
                player->readFrameSize = 0;
            }
        }
    } else {
        player->readBuffers[player->currentReadIndex].dataReady = 0;
    }
    player = stream->player;
    _movieStreamRequestBlock(stream, player, OSDisableInterrupts());
}


void _movieStreamPrepareGetDataCallback(long fileMode, void *context, long argument)
{
    __GSmovieStreaming *stream = (__GSmovieStreaming *)argument;
    u32 frameSize = (u32)context;
    THPPlayer *player = stream->player;

    _movieStreamAdvanceRead(player, frameSize, stream);
}


extern "C" void _movieStreamPrepareGetData(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;
    s32 level = OSDisableInterrupts();
    _movieStreamRequestBlock(stream, player, level);
}

static inline void _movieStreamDecodeAudio(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;
    THPReadBuffer *buffer;
    THPAudioBuffer *audioBuffer;
    int i;
    u32 *componentSize;
    u8 *end;

    if (player->hasAudio != 0) {
        audioBuffer = &player->audioBuffers[player->audioBufferWriteIndex];
        if (audioBuffer->valid != 1) {
            buffer = &player->readBuffers[player->nextReadIndex];
            if (buffer->dataReady != 0 && buffer->audioDecoded != 1) {
                componentSize = buffer->data + 2;
                end = (u8 *)buffer->data + 8 + player->numComponents * 4;
                for (i = 0; i < player->numComponents; i++) {
                    int flag = player->componentTypes[i];
                    switch (flag) {
                    case THP_COMPONENT_AUDIO:
                        u32 size = *componentSize;
                        u32 samplesDecoded = THPAudioDecode(audioBuffer->buffer, end + size * player->audioTrack, THP_AUDIO_INTERLEAVED);
                        audioBuffer->remaining = samplesDecoded;
                        audioBuffer->currentSample = audioBuffer->buffer;
                        audioBuffer->valid = 1;
                        player->audioBufferWriteIndex++;
                        if (player->audioBufferWriteIndex >= THP_AUDIO_BUFFER_COUNT) {
                            player->audioBufferWriteIndex = 0;
                        }
                        goto loopdone;
                    default:
                        end += *componentSize;
                        componentSize++;
                        break;
                    }
                }
loopdone:
                if (player->audioDecodeCount < 2) {
                    if ((player->loopFlag & MOVIE_STREAM_LOOP_FLAG) == 0) {
                        u32 position = buffer->frame + player->startFrame;
                        if (position == player->numFrames - 1) {
                            stream->decodeFlag = 1;
                        }
                    }
                }
                if (player->audioDecodeCount == 2) {
                    stream->decodeFlag = 1;
                }
                buffer->audioDecoded = 1;
                _movieStreamReleaseReadBuffer(buffer, stream);
                player->audioDecodeCount++;
            }
        }
    }

}

static inline void _movieStreamDecodeVideo(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;

    if (player->hasAudio != 0) {
        while (player->framesToSkip != 0) {
            THPTextureSet *textureSet = &player->textureSets[player->textureSetIndex];
            if (textureSet->ready == 1) {
                return;
            }
            THPReadBuffer *buffer = &player->readBuffers[player->readBufferIndex];
            if (buffer->dataReady == 0) {
                return;
            }
            if (buffer->videoDecoded == 1) {
                return;
            }
            {
                u32 frames = player->numFrames;
                u32 position = buffer->frame + player->startFrame;
                u32 remainder = position % frames;
                if (remainder == frames - 1 && (player->loopFlag & MOVIE_STREAM_LOOP_FLAG) == 0) {
                    _VideoDecode(stream, buffer, textureSet);
                }
            }
            buffer->videoDecoded = 1;
            _movieStreamReleaseReadBuffer(buffer, stream);
            player->framesToSkip--;
            player->decodedCount++;
        }
    }

    {
        THPTextureSet *textureSet = &player->textureSets[player->textureSetIndex];
        if (textureSet->ready == 1) {
            return;
        }
        THPReadBuffer *buffer = &player->readBuffers[player->readBufferIndex];
        if (buffer->dataReady == 0) {
            return;
        }
        if (buffer->videoDecoded == 1) {
            return;
        }
        player->framesToSkip = _VideoDecode(stream, buffer, textureSet);
        buffer->videoDecoded = 1;
        _movieStreamReleaseReadBuffer(buffer, stream);
    }
}

u32 _VideoDecode(__GSmovieStreaming *stream, THPReadBuffer *buffer, THPTextureSet *textureSet)
{
    u32 i;
    u32 *componentSize = buffer->data + 2;
    u8 *dest;
    THPPlayer *player = stream->player;
    int result = 0;

    dest = (u8 *)buffer->data + 8 + player->numComponents * 4;
    for (i = 0; i < player->numComponents; i++) {
        int flag = player->componentTypes[i];
        switch (flag) {
        case THP_COMPONENT_VIDEO:
            result = THPVideoDecode(dest, textureSet->yBuffer, textureSet->uBuffer, textureSet->vBuffer, player->videoWorkBuffer);
            player->videoResult = result;
            if (result != 0) {
                if (stream->firstFramePending == 1) {
                    stream->decodeFlag = 0;
                    stream->firstFramePending = 0;
                }
                player->state = THP_PLAYER_ERROR;
            }
            textureSet->frame = buffer->frame;
            textureSet->ready = 1;
            player->textureSetIndex++;
            if (player->textureSetIndex >= THP_TEXTURE_SET_COUNT) {
                player->textureSetIndex = 0;
            }
            if ((s32)player->presentedVideoFrameCount + 1 > (s32)player->decodedCount) {
                result = (s32)player->presentedVideoFrameCount + 1 - (s32)player->decodedCount;
            } else {
                result = 0;
            }
            player->decodedCount++;
            break;
        }
        dest += *componentSize;
        componentSize++;
    }
    if (stream->firstFramePending == 1) {
        stream->decodeFlag = 1;
        stream->firstFramePending = 0;
    }
    return result;
}


void _movieStreamDecode(__GSmovieStreaming *stream)
{
    _movieStreamDecodeAudio(stream);
    _movieStreamDecodeVideo(stream);
}


u32 GetAudioSample(__GSmovieStreaming *stream, s16 *left, s16 *right, u32 count, long *status)
{
    THPPlayer *player = stream->player;

    if (player->currentAudioBuffer == NULL) {
        u32 index = player->audioBufferReadIndex;
        if (player->audioBuffers[index].valid == 0) {
            *status = THP_AUDIO_NO_DATA;
            return 0;
        }
        player->currentAudioBuffer = &player->audioBuffers[index];
    }
    u32 remaining = player->currentAudioBuffer->remaining;
    u32 samplesCopied;
    if (remaining != 0) {
        if (remaining >= count) {
            samplesCopied = count;
        } else {
            samplesCopied = remaining;
        }
        s16 *source = player->currentAudioBuffer->currentSample;
        if (right == NULL) {
            s16 *leftSample = left;
            for (u32 i = 0; i < samplesCopied; i++) {
                *leftSample++ = source[1];
                source += 2;
            }
        } else {
            s16 *leftSample = left;
            s16 *rightSample = right;
            for (u32 i = 0; i < samplesCopied; i++) {
                *rightSample++ = source[0];
                *leftSample++ = source[1];
                source += 2;
            }
        }
        player->currentAudioBuffer->remaining -= samplesCopied;
        player->currentAudioBuffer->currentSample = source;
        if (player->currentAudioBuffer->remaining == 0) {
            player->currentAudioBuffer->valid = 0;
            player->audioBufferReadIndex++;
            if (player->audioBufferReadIndex >= THP_AUDIO_BUFFER_COUNT) {
                player->audioBufferReadIndex = 0;
            }
            player->currentAudioBuffer = NULL;
            *status = THP_AUDIO_FRAME_CONSUMED;
        } else {
            *status = THP_AUDIO_REQUEST_FILLED;
        }
    }
    return samplesCopied;
}

static inline void _StreamFillBuffer(__GSmovieStreaming *stream, s16 *leftBuffer, s16 *rightBuffer, u32 sampleCount)
{
    u64 position;
    u32 remaining;
    s16 *left;
    s16 *right;
    THPPlayer *player = stream->player;
    remaining = sampleCount;
    left = leftBuffer;
    right = rightBuffer;
    position = player->audioSampleState.sampleAccumulator;

    for (;;) {
        long status;
        u32 samplesRead = GetAudioSample(stream, left, right, remaining, &status);
        position += samplesRead;
        if (status == THP_AUDIO_REQUEST_FILLED) {
            break;
        }
        if (status == THP_AUDIO_FRAME_CONSUMED) {
            remaining -= samplesRead;
            left += samplesRead;
            if (right != NULL) {
                right += samplesRead;
            }
            THPPlayer *ring = stream->player;
            ring->audioSampleState.sampleRing[ring->audioSampleState.ringWriteIndex] = position;
            ring->audioSampleState.ringWriteIndex = ring->audioSampleState.ringWriteIndex + 1;
            if ((s32)ring->audioSampleState.ringWriteIndex >= THP_SAMPLE_RING_SIZE) {
                ring->audioSampleState.ringWriteIndex = 0;
            }
            continue;
        }
        memset(left, 0, remaining * 2);
        if (right != NULL) {
            memset(right, 0, remaining * 2);
        }
        break;
    }
    player->audioSampleState.sampleAccumulator += sampleCount;
}

u32 StreamUpdateCallback(void *buffer1, u32 len1, void *buffer2, u32 len2, u32 user)
{
    __GSmovieStreaming *stream = (__GSmovieStreaming *)user;
    u32 chunk;
    THPPlayer *player;

    if (stream == NULL) {
        return 0;
    }
    player = stream->player;
    if (buffer1 == NULL && buffer2 == NULL && len1 == 0 && len2 == 0) {
        OSReport("--- Drop Stream Voice!! ---\n");
        player->startAudioPending = 1;
        return 0;
    }

    chunk = (((u32)((float)(player->audioInfo.audioFrequency * MOVIE_STREAM_BUFFER_MSEC) / 1000.0f + 0.5f) + 31) & ~31u) / 2;
    player->audioSampleState.samplePosition = player->audioSampleState.sampleTotal + (u64)len1 + len2;
    {
        THPPlayer *dropPlayer = stream->player;
        u64 position = player->audioSampleState.samplePosition;
        while ((s32)dropPlayer->audioSampleState.ringReadIndex != (s32)dropPlayer->audioSampleState.ringWriteIndex) {
            if (position < dropPlayer->audioSampleState.sampleRing[dropPlayer->audioSampleState.ringReadIndex]) {
                break;
            }
            dropPlayer->audioSampleState.ringReadIndex++;
            if ((s32)dropPlayer->audioSampleState.ringReadIndex >= THP_SAMPLE_RING_SIZE) {
                dropPlayer->audioSampleState.ringReadIndex = 0;
            }
            dropPlayer->playedAudioFrameCount++;
        }
    }
    if (len1 + len2 >= chunk) {
        if (buffer1 == player->audioBuffer0) {
            if (player->audioInfo.numAudioChannels == 2) {
                _StreamFillBuffer(stream, (s16 *)player->audioBuffer0,
                                 (s16 *)player->audioBuffer1, chunk);
            } else {
                _StreamFillBuffer(stream, (s16 *)player->audioBuffer0, NULL, chunk);
            }
            DCFlushRange(player->audioBuffer0, chunk * 2);
            sndStreamARAMUpdate(player->soundHandle0, 0, chunk, 0, 0);
            if (player->audioInfo.numAudioChannels == 2) {
                DCFlushRange(player->audioBuffer1, chunk * 2);
                sndStreamARAMUpdate(player->soundHandle1, 0, chunk, 0, 0);
            }
        } else {
            if (player->audioInfo.numAudioChannels == 2) {
                _StreamFillBuffer(stream, (s16 *)player->audioBuffer0 + chunk,
                                 (s16 *)player->audioBuffer1 + chunk, chunk);
            } else {
                _StreamFillBuffer(stream, (s16 *)player->audioBuffer0 + chunk, NULL,
                                 chunk);
            }
            DCFlushRange((u8 *)player->audioBuffer0 + chunk * 2, chunk * 2);
            sndStreamARAMUpdate(player->soundHandle0, chunk, chunk, 0, 0);
            if (player->audioInfo.numAudioChannels == 2) {
                DCFlushRange((u8 *)player->audioBuffer1 + chunk * 2, chunk * 2);
                sndStreamARAMUpdate(player->soundHandle1, chunk, chunk, 0, 0);
            }
        }
        player->audioSampleState.sampleTotal += chunk;
        return chunk;
    }
    return 0;
}


extern "C" s32 _StreamInit(__GSmovieStreaming *stream)
{
    u32 chunk;
    THPPlayer *player = stream->player;
    chunk = (((u32)((float)(player->audioInfo.audioFrequency * MOVIE_STREAM_BUFFER_MSEC) / 1000.0f + 0.5f) + 31) & ~31u);

    player->soundHandle0 = sndStreamAllocEx(MOVIE_STREAM_PRIORITY, player->audioBuffer0, chunk, player->audioInfo.audioFrequency, 127,
                                            (player->audioInfo.numAudioChannels == 2u ? (u8)0 : (u8)64), 0, 0, 0, 0, SND_STREAM_INACTIVE | SND_STREAM_MANUALARAMUPD, StreamUpdateCallback, (u32)stream, NULL);
    if (player->soundHandle0 == SND_ID_ERROR) {
        return 0;
    }
    if (player->audioInfo.numAudioChannels == 2) {
        player->soundHandle1 = sndStreamAllocEx(MOVIE_STREAM_PRIORITY, player->audioBuffer1, chunk, player->audioInfo.audioFrequency, 127,
                                                127, 0, 0, 0, 0, SND_STREAM_INACTIVE | SND_STREAM_MANUALARAMUPD, StreamUpdateCallback, 0, NULL);
        if (player->soundHandle1 == SND_ID_ERROR) {
            sndStreamFree(player->soundHandle0);
            return 0;
        }
    }
    player->audioSampleState.ringReadIndex = 0;
    player->audioSampleState.ringWriteIndex = 0;
    player->audioSampleState.sampleTotal = 0;
    player->audioSampleState.samplePosition = 0;
    player->audioSampleState.sampleAccumulator = 0;
    return 1;
}


extern "C" s32 _StreamInitFillStreamBuffer(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;
    u32 chunk = (((u32)((float)(player->audioInfo.audioFrequency * MOVIE_STREAM_BUFFER_MSEC) / 1000.0f + 0.5f) + 31) & ~31u);

    if (player->audioInfo.numAudioChannels == 2) {
        _StreamFillBuffer(stream, (s16 *)player->audioBuffer0, (s16 *)player->audioBuffer1, chunk);
    } else {
        _StreamFillBuffer(stream, (s16 *)player->audioBuffer0, NULL, chunk);
    }
    DCFlushRange(player->audioBuffer0, chunk * 2);
    sndStreamARAMUpdate(player->soundHandle0, 0, chunk, 0, 0);
    if (player->audioInfo.numAudioChannels == 2) {
        DCFlushRange(player->audioBuffer1, chunk * 2);
        sndStreamARAMUpdate(player->soundHandle1, 0, chunk, 0, 0);
    }
    stream->streamFilled = 1;
    return 1;
}


extern "C" u8 _StreamPlay(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;

    if (player->soundHandle0 != SND_ID_ERROR && sndStreamActivate(player->soundHandle0) != false) {
        if (player->soundHandle1 != SND_ID_ERROR) {
            if (sndStreamActivate(player->soundHandle1) != false) {
                return 1;
            }
            sndStreamDeactivate(player->soundHandle0);
        } else {
            return 1;
        }
    }
    return 0;
}


extern "C" void _StreamQuit(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;

    sndStreamFree(player->soundHandle0);
    player->soundHandle0 = SND_ID_ERROR;
    if (player->soundHandle1 != SND_ID_ERROR) {
        sndStreamFree(player->soundHandle1);
        player->soundHandle1 = SND_ID_ERROR;
    }
}
