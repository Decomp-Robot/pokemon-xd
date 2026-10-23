/*
 * Game sound API.
 *
 * Thin, checked layer over the GSnd stream engine: every entry point rejects
 * ids outside the sound-effect table and the two "currently playing" slots
 * (environment effect and BGM) are kept up to date here. The streaming
 * callbacks at the end of the file are the hooks the file system calls while
 * a stream is being decoded.
 */

#include <sound.h>

/* The sound currently playing in each of the two slots this module owns. */
s32 gSoundCurrentBGMID;
s32 gSoundCurrentEnvID;

/* Printed by soundStop when the BGM cannot be stopped - the stream is gone
   and the sound is not one the engine can stop, so the BGM is reopened. */
const char gsBgmStopWarning[] = "soundStop: Warning! BGM cannot be stopped.(snd_id=%d)\n";

/* Sound engine entry points. */
extern s32 GSsndCheckEnv(s32 sndID);
extern u8 GSsndCheckStream(s32 sndID);
extern u8 GSsndCheckSE(s32 sndID);
extern s32 GSsndGetStatus(s32 sndID);
extern u32 GSsndGetVolume(s32 sndID);
extern void GSsndSetOriginalVolume(s32 sndID, s32 time, s32 volume);
extern void GSsndContinue(s32 sndID, s32 time, s32 mode);
extern void GSsndPause(s32 sndID, s32 time, s32 mode);
extern s32 GSsndStop(s32 sndID, s32 time, s32 mode);
extern void GSsndOpenWaveDVD(u32 waveID, u32 dvdFileID, u32 sampleOffset, u32 resourceGroup,
                             u32 poolResID, u32 projectResID, u32 sampDirResID);
extern s32 GSsndCreateListener(GSsndVec3* pos);
extern s32 GSsndCreateEmitter(s32 sndID, GSsndVec3* pos);

/* Stream helpers. */
extern GSsndStream* _sndGetStream(u32 fileNo);
extern void _sndStreamCtrl(GSsndStream* stream, s32 mode);
extern u32 sndFrameToByteRate(u32 frame);

/* Play/close wrappers filling in the engine's default arguments: the play one
   hands the caller's volume straight through, the close one stops the sound
   before the engine releases its work block. */
extern void GSsndPlayDefault(s32 sndID, s32 time, s32 volume, s32 mode);
extern void GSsndClose(s32 sndID);

/* File system. */
extern void GSfsysReadWait(s32 fileNo);
extern s32 GSfsysGetStatus(u32 fileNo);
extern void GSfsysStreamSetLoopFlag(u32 fileNo, u32 arg, u32 loopFlag);
extern void GSfsysStreamSetLoopPoint(u32 fileNo, u32 arg, u32 loopStart, u32 loopEnd);

/* Interrupt masking, as used around the stream volume registers. */
extern s32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(s32 state);

/* GS thread and video timing. */
extern void GSthreadSwitch(void);
extern u8 GSgfxVideoGetVsyncRate(void);
extern f32 GSgfxVideoGetLastRenderTime(void);

/* GSlog's printf-style logging entry point. */
extern void GSlogPrintf(const char* fmt, ...);


/* No stream: the two "currently playing" slots are cleared to 0, and this is
   the value the tests below compare them against. */
#define SND_ID_NONE 0xFFFFFFFF

/* Normalises the engine's non-zero handles to 0/1. */
#define SND_BOOL(v) ((u32)((v) | -(v)) >> 31)

/* Loop points are kept in units of 65536 samples; the file system wants a
   plain sample index. Counts are biased by one, because a loop end marks the
   sample after the last one. */
#define SND_SAMPLE_SHIFT 16
#define SND_SAMPLE_NEXT 0xFFFF

/* Bytes one decoded block occupies, used to turn loop samples into the byte
   rates the stream controller is programmed with. */
#define SND_BLOCK_BYTES 0xE000

/* Milliseconds the current sound is faded over when something takes its place,
   and the longer fade the BGM gets when a sound effect lets it resume. */
#define SND_FAST_FADE_MS 50
#define SND_BGM_RESUME_MS 500

s32 soundCheckEnv(s32 sndID)
{
    return GSsndCheckEnv(sndID);
}

s32 soundGetEnvID(void)
{
    return gSoundCurrentEnvID;
}

s32 soundGetBGMID(void)
{
    return gSoundCurrentBGMID;
}

s32 soundPlayME(s32 sndID, s32 time, s32 volume)
{
    if (sndID == 0 || (u32)sndID >= gSoundSEMaster->count) {
        return 0;
    }
    GSsndPlayDefault(sndID, time, volume, 0);
    soundPlayWait(sndID);
    GSsndClose(sndID);
    return 1;
}

void soundPlayWait(s32 sndID)
{
    for (;;) {
        if (GSsndGetStatus(sndID) != 2) {
            break;
        }
        GSthreadSwitch();
    }
}

void soundContinueBGMEX(s32 sndID, s32 time, u8 wait)
{
    if (sndID == SND_ID_NONE) {
        return;
    }
    GSsndContinue(sndID, time, 0);
    if (wait == 1) {
        _soundWait(time);
    }
}

s32 soundPauseBGMEX(s32 time, u8 wait)
{
    s32 bgmID = soundGetBGMID();

    if (bgmID != SND_ID_NONE) {
        GSsndPause(bgmID, time, 0);
        if (wait == 1) {
            _soundWait(time);
        }
    }
    return bgmID;
}

void soundOpenWave(s32 waveID)
{
    GSsndWaveEntry* wave;

    if (*gSoundWaveCountList <= (u32)waveID) {
        return;
    }
    wave = &gSoundWaveList[waveID];
    GSsndOpenWaveDVD(waveID, wave->dvdFileID, 0, wave->resourceGroup, wave->poolResID,
                     wave->projectResID, wave->sampDirResID);
}

s32 soundOpenPlayME(s32 sndID, s32 time, s32 volume)
{
    s32 bgmID = soundPauseBGMEX(SND_FAST_FADE_MS, 1);
    s32 ret = soundPlayME(sndID, time, volume);

    soundContinueBGMEX(bgmID, SND_BGM_RESUME_MS, 1);
    return ret;
}

u32 soundGetVolume(s32 sndID)
{
    return GSsndGetVolume(sndID);
}

void soundSetVolume(s32 volume, s32 time)
{
    soundSetOriginalVolume(soundGetBGMID(), time, volume);
}

void soundSetOriginalVolume(s32 sndID, s32 time, s32 volume)
{
    GSsndSetOriginalVolume(sndID, time, volume);
}

s32 soundCreateListener(void)
{
    GSsndVec3 pos;
    s32 handle;

    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    handle = GSsndCreateListener(&pos);
    return SND_BOOL(handle);
}

s32 soundCreateEmitter(s32 sndID, f32 x, f32 y, f32 z)
{
    GSsndVec3 pos;
    s32 created;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    created = GSsndCreateEmitter(sndID, &pos);
    return SND_BOOL(created);
}

void soundStopEnv(s32 fadeTime)
{
    soundStop(gSoundCurrentEnvID, fadeTime);
}

s32 soundStop(s32 sndID, s32 fadeTime)
{
    s32 isBGM;

    if (sndID == 0 || (u32)sndID >= gSoundSEMaster->count) {
        return 0;
    }
    isBGM = 0;
    if (gSoundCurrentBGMID == (u32)sndID) {
        gSoundCurrentBGMID = 0;
        isBGM = 1;
    }
    if (gSoundCurrentEnvID == (u32)sndID) {
        gSoundCurrentEnvID = 0;
        isBGM = 0;
    }
    if (GSsndCheckStream(sndID) == 1) {
        if (!GSsndCheckSE(sndID)) {
            _soundFadeout(fadeTime, isBGM, 1);
        }
        GSsndClose(sndID);
    } else if (!GSsndCheckSE(sndID)) {
        _soundFadeout(fadeTime, isBGM, 1);
        if (isBGM) {
            GSlogPrintf(gsBgmStopWarning, sndID);
            _soundOpenPlay(8, 0, 0xFF, isBGM);
        } else {
            GSsndClose(sndID);
        }
    } else {
        GSsndStop(sndID, fadeTime, 0);
        GSsndClose(sndID);
    }
}

s32 soundOpenPlayEnvFadeOutIn(s32 sndID, s32 fadeTime, s32 time, s32 volume)
{
    if (sndID == 0 || (u32)sndID >= gSoundSEMaster->count) {
        return 0;
    }
    _soundFadeout(fadeTime, 0, 1);
    return _soundOpenPlay(sndID, time, volume, 0);
}

s32 soundOpenPlayFadeOutIn(s32 sndID, s32 fadeTime, s32 time, s32 volume)
{
    if (sndID == 0 || (u32)sndID >= gSoundSEMaster->count) {
        return 0;
    }
    _soundFadeout(fadeTime, 1, 1);
    return _soundOpenPlay(sndID, time, volume, 1);
}

s32 soundOpenPlayEnv(s32 sndID, s32 time, s32 volume)
{
    return _soundOpenPlay(sndID, time, volume, 0);
}

s32 soundOpenPlay(s32 sndID, s32 time, s32 volume)
{
    return _soundOpenPlay(sndID, time, volume, 1);
}

s32 _soundOpenPlay(s32 sndID, s32 time, s32 volume, s32 isBGM)
{
    if (sndID == 0 || (u32)sndID >= gSoundSEMaster->count) {
        return 0;
    }
    if (isBGM == 0) {
        if (gSoundCurrentEnvID != SND_ID_NONE) {
            _soundCloseFastFadeout(gSoundCurrentEnvID);
        }
        gSoundCurrentEnvID = sndID;
    } else {
        if (gSoundSEList[sndID].soundEffect != 1) {
            if (gSoundCurrentBGMID != (u32)sndID && gSoundCurrentBGMID != SND_ID_NONE) {
                _soundCloseFastFadeout(gSoundCurrentBGMID);
            }
            gSoundCurrentBGMID = sndID;
        }
    }
    GSsndPlayDefault(sndID, time, volume, 0);
}

void _soundFadeout(s32 fadeTime, s32 isBGM, s32 wait)
{
    s32 sndID;

    if (isBGM) {
        sndID = soundGetBGMID();
    } else {
        sndID = soundGetEnvID();
    }
    if (sndID == SND_ID_NONE) {
        return;
    }
    if ((u8)_soundCheckActive(sndID) == 1) {
        GSsndSetOriginalVolume(sndID, fadeTime, 0);
        if (wait) {
            _soundWait(fadeTime);
        }
    }
}

void _soundCloseFastFadeout(s32 sndID)
{
    if ((u8)_soundCheckActive(sndID) == 1) {
        GSsndSetOriginalVolume(sndID, SND_FAST_FADE_MS, 0);
        _soundWait(SND_FAST_FADE_MS);
        GSsndClose(sndID);
    }
}

s32 _soundCheckActive(s32 sndID)
{
    s32 status = GSsndGetStatus(sndID);

    if (status != 0 && status >= 0 && status < 4) {
        return 1;
    }
    return 0;
}

void _soundWait(u32 msec)
{
    f32 limit;
    f32 elapsed = 0.0f;

    limit = msec / 1000.0f;

    while (elapsed < limit) {
        GSthreadSwitch();
        elapsed += GSgfxVideoGetLastRenderTime() / (f32)GSgfxVideoGetVsyncRate();
    }
}

void soundInitBGM(void)
{
    GSfsysReadWait(0x99);
}

void soundClearCurrentIDs(void)
{
    gSoundCurrentBGMID = 0;
    gSoundCurrentEnvID = 0;
}

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
    u32 enabled = OSDisableInterrupts();

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
