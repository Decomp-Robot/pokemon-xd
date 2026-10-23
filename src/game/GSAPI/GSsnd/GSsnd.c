#include <GSsnd.h>
#include <GSfsys.h>
#include <dolphin/os.h>

/* Stream helpers. */
extern GSsndStream* _sndGetStream(u32 fileNo);
extern void _sndStreamCtrl(GSsndStream* stream, s32 mode);
extern u32 sndFrameToByteRate(u32 frame);

/* File system. */
extern s32 GSfsysGetStatus(u32 fileNo);
extern void GSfsysStreamSetLoopFlag(u32 fileNo, u32 arg, u32 loopFlag);
extern void GSfsysStreamSetLoopPoint(u32 fileNo, u32 arg, u32 loopStart, u32 loopEnd);

/* Loop points are kept in units of 65536 samples; the file system wants a
   plain sample index. Counts are biased by one, because a loop end marks the
   sample after the last one. */
#define SND_SAMPLE_SHIFT 16
#define SND_SAMPLE_NEXT 0xFFFF

/* Bytes one decoded block occupies, used to turn loop samples into the byte
   rates the stream controller is programmed with. */
#define SND_BLOCK_BYTES 0xE000

void GSsndStreamPostFunc(u32 fileNo, u32 arg)
{
    GSsndStream* stream;
    s32 pointStart;
    s32 pointEnd;
    s32 i;

    if (!gSoundStreamReady) {
        return;
    }
    stream = _sndGetStream(fileNo);
    if (stream == 0) {
        return;
    }
    stream->state = 3;
    if (stream->chain != 0) {
        stream->chain->state = 3;
    }
    GSfsysStreamSetLoopFlag(fileNo, arg, stream->loopFlag & 0xFF);

    for (i = 0; i < 2; i++) {
        stream->buffer[i].aramState[0] = 0;
        stream->buffer[i].aramState[1] = 0;
        stream->buffer[i].unk30 = 0;
        stream->buffer[i].unk34 = 0;
        stream->buffer[i].unk38 = 0;
        stream->buffer[i].state = 1;
    }
    stream->loopReached = 0;
    stream->bufferFlag[0] = 0;
    stream->bufferFlag[1] = 0;
    stream->byteRateStart = ((stream->loopStart - 2) >> SND_SAMPLE_SHIFT) * SND_BLOCK_BYTES;
    stream->byteRateEnd = ((stream->loopEnd + SND_SAMPLE_NEXT) >> SND_SAMPLE_SHIFT) * SND_BLOCK_BYTES;
    stream->byteRatePreload = sndFrameToByteRate(stream->loopPreload);

    /* The loop points are handed over as sample counts parked in the high
       half of a word and doubled by the file system. */
    if (stream->loopFlag == 1) {
        pointStart = ((stream->loopStart - 2) >> SND_SAMPLE_SHIFT) << 15;
        pointEnd = ((stream->loopEnd + SND_SAMPLE_NEXT) >> SND_SAMPLE_SHIFT) << 15;
    } else {
        pointStart = 0;
        pointEnd = ((stream->loopPreload + SND_SAMPLE_NEXT) >> SND_SAMPLE_SHIFT) << 15;
    }
    GSfsysStreamSetLoopPoint(fileNo, arg, pointStart * 2, pointEnd * 2);
}

void* GSsndStreamPreFunc(u32 fileNo)
{
    GSsndStream* stream;

    if (!gSoundStreamReady) {
        return 0;
    }
    stream = _sndGetStream(fileNo);
    if (stream == 0) {
        return 0;
    }
    return stream->prefetch;
}

s32 _sndStreamGetSndID(u32 fileNo)
{
    GSsndStream* stream = _sndGetStream(fileNo);

    if (stream == 0) {
        return -1;
    }
    if (stream->next != 0) {
        stream = stream->next;
    }
    return stream->soundID;
}

s32 _sndStreamGetValid(u32 fileNo)
{
    if (_sndGetStream(fileNo) != 0 || GSfsysGetStatus(fileNo) == 1) {
        return 1;
    }
    return 0;
}

void _sndStreamVolume(u8 volume, u16 time, u32 fileNo, u8 mode)
{
    GSsndStream* stream;
    GSsndStream* snd;
    GSsndStream* parent;
    BOOL enabled = OSDisableInterrupts();

    stream = _sndGetStream(fileNo);

    if (stream == 0) {
        OSRestoreInterrupts(enabled);
        return;
    }
    if (stream->next != 0) {
        snd = stream->next;
        parent = stream;
    } else {
        snd = stream;
        parent = stream->chain;
    }
    if (snd->fadeEnable == 1 && !mode) {
        _sndStreamCtrl(snd, 0);
        _sndStreamCtrl(parent, 0);
    }
    snd->volumeStart = snd->volume;
    snd->volumeTarget = volume;
    snd->fadeStep = 0;
    snd->time = time;
    snd->mode = mode;
    parent->volumeStart = snd->volume;
    parent->volumeTarget = volume;
    parent->fadeStep = 0;
    parent->time = time;
    parent->mode = mode;
    OSRestoreInterrupts(enabled);
}
