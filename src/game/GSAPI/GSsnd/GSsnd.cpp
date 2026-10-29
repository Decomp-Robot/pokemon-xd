#include <GSsnd.h>
#include "GSsndInternal.h"
#include <GSfsys.h>
#include <dolphin/dvd.h>
#include <dolphin/os.h>

/* DSP ADPCM positions are nibble counts; each stream block is 0x10000 nibbles. */
#define SND_NIBBLE_BLOCK_SHIFT 16
#define SND_NIBBLE_BLOCK_ROUND_UP 0xFFFF
#define SND_SAMPLES_PER_BLOCK 0xE000
#define SND_BYTE_BLOCK_SHIFT 15

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
    stream->state = GSSND_STREAM_START;
    if (stream->chain != 0) {
        stream->chain->state = GSSND_STREAM_START;
    }
    GSfsysStreamSetLoopFlag(fileNo, arg, stream->loopFlag & 0xFF);

    for (i = 0; i < 2; i++) {
        stream->buffer[i].aramState[0] = 0;
        stream->buffer[i].aramState[1] = 0;
        stream->buffer[i].halfWriteOffset[0] = 0;
        stream->buffer[i].halfWriteOffset[1] = 0;
        stream->buffer[i].stagingReadOffset = 0;
        stream->buffer[i].state = 1;
    }
    stream->loopReached = 0;
    stream->bufferFlag[0] = 0;
    stream->bufferFlag[1] = 0;
    stream->loopStartSampleAligned = ((stream->loopStartNibble - 2) >> SND_NIBBLE_BLOCK_SHIFT) * SND_SAMPLES_PER_BLOCK;
    stream->loopEndSampleAligned = ((stream->loopEndNibble + SND_NIBBLE_BLOCK_ROUND_UP) >> SND_NIBBLE_BLOCK_SHIFT) * SND_SAMPLES_PER_BLOCK;
    stream->playbackEndSample = _sndStreamNibble_HeaderON2OFF(stream->playbackEndNibble);

    /* The file stores a block for each of the two channels. */
    if (stream->loopFlag == 1) {
        pointStart = ((stream->loopStartNibble - 2) >> SND_NIBBLE_BLOCK_SHIFT) << SND_BYTE_BLOCK_SHIFT;
        pointEnd = ((stream->loopEndNibble + SND_NIBBLE_BLOCK_ROUND_UP) >> SND_NIBBLE_BLOCK_SHIFT) << SND_BYTE_BLOCK_SHIFT;
    } else {
        pointStart = 0;
        pointEnd = ((stream->playbackEndNibble + SND_NIBBLE_BLOCK_ROUND_UP) >> SND_NIBBLE_BLOCK_SHIFT) << SND_BYTE_BLOCK_SHIFT;
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

u8 _sndStreamGetValid(u32 fileNo)
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

void _sndStreamDaemon(GSsndStream* stream)
{
}

void _sndDaemon(void)
{
    BOOL enabled;
    GSsndStream* stream;
    u32 driveUnavailable;
    u32 i;
    GSsndStream* first;
    GSsndStream* second;
    switch (GSfsysGetDriveStatus()) {
    case DVD_STATE_FATAL_ERROR:
    case DVD_STATE_COVER_CLOSED:
    case DVD_STATE_NO_DISK:
    case DVD_STATE_COVER_OPEN:
    case DVD_STATE_WRONG_DISK:
    case DVD_STATE_PAUSING:
    case DVD_STATE_RETRY:
        driveUnavailable = 1;
        break;
    default:
        driveUnavailable = 0;
        break;
    }
    for (i = 0; i < _maxStreaming; ++i) {
        stream = &_sndStreaming[i];
        if (stream->next != 0) {
            first = stream->next;
            second = stream;
        } else {
            first = stream;
            second = stream->chain;
        }
        switch (stream->state) {
        case GSSND_STREAM_FREE:
        case GSSND_STREAM_OPEN:
        case GSSND_STREAM_READY:
        case GSSND_STREAM_CLOSE:
        case GSSND_STREAM_CLOSED:
        case GSSND_STREAM_DEACTIVATE:
            break;
        default:
            _sndStreamPrepareGetData(stream);
            break;
        }
        stream->driveUnavailable = driveUnavailable;
        enabled = OSDisableInterrupts();
        switch (stream->state) {
        case GSSND_STREAM_START:
            _sndStreamGetData(stream, _sndStreamGetDataCallback_START);
            break;
        case GSSND_STREAM_ACTIVATE:
            _sndStreamGetData(stream, _sndStreamGetDataCallback_ACTIVATE);
            break;
        case GSSND_STREAM_PLAYING:
            _sndStreamDaemon(stream);
            break;
        case GSSND_STREAM_CLOSE:
            stream->state = GSSND_STREAM_CLOSED;
            if (stream->next == 0) {
                GSsndClose(stream->soundID);
            }
            break;
        case GSSND_STREAM_DEACTIVATE:
            sndStreamDeactivate(stream->streamID);
            if (first->loopFlag == 0) {
                stream->state = GSSND_STREAM_CLOSE;
            } else {
                stream->state = GSSND_STREAM_RESTART;
            }
            break;
        case GSSND_STREAM_RESTART:
            if (first->halfIndex == 0 && second->halfIndex == 0) {
                stream->state = GSSND_STREAM_START;
            } else if (stream->halfIndex != 0) {
                _sndStreamGetData(stream, _sndStreamGetDataCallback_EXEC_UPDATE);
            }
            break;
        }
        OSRestoreInterrupts(enabled);
    }
}

unsigned long GSsndIsPlayingStream(long soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (entry->stream == 1) {
        GSsndWork* work = entry->work;
        if (work == 0) return 0;
        if (work->playbackID == GSSND_INVALID_ID) return 0;
        if (!_sndStreamGetValid(work->playbackID)) return 0;
        GSsndStream* stream = _sndGetStream(work->playbackID);
        if (stream == 0) return 0;
        return stream->state == GSSND_STREAM_PLAYING;
    }
    return soundID;
}

u8 GSsndCheckEnv(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    return snd_data[soundID].envSound;
}

u8 GSsndCheckStream(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    return snd_data[soundID].stream;
}

u8 GSsndCheckSE(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    return snd_data[soundID].soundEffect;
}

void GSsndSetPushVolumeID(s32 soundID, GSsndVolumeStack* stack)
{
    if (!gSoundStreamReady) return;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return;
    GSsndSEEntry* entry = &snd_data[soundID];
    GSsndWork* work = entry->work;
    if (work == 0) return;
    if (!stack) return;
    if (stack->count >= GSSND_VOLUME_STACK_CAPACITY) return;
    work->volumeCount = stack->count;
    for (s32 i = 0; i < stack->count; ++i) work->volumeStack[i] = stack->volume[i];
}

void GSsndGetPushVolumeID(s32 soundID, GSsndVolumeStack* stack)
{
    if (!gSoundStreamReady) return;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return;
    GSsndSEEntry* entry = &snd_data[soundID];
    GSsndWork* work = entry->work;
    if (work == 0) return;
    if (!stack) return;
    if (work->volumeCount >= GSSND_VOLUME_STACK_CAPACITY) return;
    stack->count = work->volumeCount;
    for (s32 i = 0; i < stack->count; ++i) stack->volume[i] = work->volumeStack[i];
}

s32 GSsndGetVolume(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (entry->open != 1) return 0;
    GSsndWork* work = entry->work;
    if (work == 0) return 0;
    return work->volume;
}

void GSsndPopVolumeALL(long time, unsigned long music, unsigned long effects)
{
    s32 i;
    GSsndSEEntry* entry;
    for (i = 0; i < _maxSoundID; ++i) {
        entry = &snd_data[i];
        if (entry->open == 1) {
            if (entry->soundEffect == 1) {
                if (!effects) continue;
            } else if (!music) continue;
            GSsndPopVolumeID(i, time);
        }
    }
}

u32 GSsndPopVolumeID(u32 soundID, s32 time)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    GSsndWork* work = entry->work;
    if (work == 0) return 0;
    if (work->volumeCount == 0) return 0;
    --work->volumeCount;
    return GSsndSetOriginalVolume(soundID, time, work->volumeStack[work->volumeCount]);
}

void GSsndPushVolumeALL(long volume, long time, unsigned long music, unsigned long effects)
{
    s32 i;
    GSsndSEEntry* entry;
    for (i = 0; i < _maxSoundID; ++i) {
        entry = &snd_data[i];
        if (entry->open == 1) {
            if (entry->soundEffect == 1) {
                if (!effects) continue;
            } else if (!music) continue;
            GSsndPushVolumeID(i, time, volume);
        }
    }
}

u32 GSsndPushVolumeID(u32 soundID, s32 time, s32 volume)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    GSsndWork* work = entry->work;
    if (work == 0) return 0;
    if (work->volumeCount >= GSSND_VOLUME_STACK_CAPACITY) return 0;
    s32 newVolume = _sndGetProperVolume(entry, volume);
    if (work->volume < newVolume) newVolume = work->volume;
    work->volumeStack[work->volumeCount] = work->volume;
    if (work->volumeCount < GSSND_VOLUME_STACK_CAPACITY - 1) ++work->volumeCount;
    return GSsndSetOriginalVolume(soundID, time, newVolume);
}

u8 GSsndCheckEmitter(GSsndEmitter* emitter)
{
    if (!gSoundStreamReady) return 0;
    if (emitter == 0) return 0;
    return _sndCheckEmitter(emitter) != 0;
}

u8 GSsndUpdateEmitter(GSsndEmitter* emitter, GSsndVec3* position)
{
    if (!gSoundStreamReady) return 0;
    if (emitter == 0) return 0;
    if (!_sndCheckEmitter(emitter)) return 0;
    GSsndVec3 velocity;
    PSVECSubtract((const Vec*)&emitter->position, (const Vec*)position, (Vec*)&velocity);
    if (sndUpdateEmitter(&emitter->emitter, (SND_FVECTOR*)position, (SND_FVECTOR*)&velocity,
                         emitter->maxVolume, 0)) {
        emitter->position = *position;
        emitter->velocity = velocity;
        return 1;
    }
    return 0;
}

u8 GSsndFreeEmitter(GSsndEmitter* emitter)
{
    if (!gSoundStreamReady) return 0;
    if (emitter == 0) return 0;
    if (!emitter->inUse) return 1;
    SND_VOICEID voiceID = sndEmitterVoiceID(&emitter->emitter);
    if (voiceID != GSSND_INVALID_ID) sndFXKeyOff(voiceID);
    sndRemoveEmitter(&emitter->emitter);
    _sndFreeEmitter(emitter);
    return 1;
}

GSsndEmitter* GSsndCreateEmitter(s32 soundID, GSsndVec3* position)
{
    GSsndVec3 velocity;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.0f;
    return GSsndCreateEmitterParam(soundID, position, &velocity, 10000.0f, 0.0f, GSSND_MAX_VOLUME, 0);
}

GSsndEmitter* GSsndCreateEmitterParam(s32 soundID, GSsndVec3* position, GSsndVec3* velocity,
                                     f32 maxDistance, f32 compression, u8 maxVolume, u8 minVolume)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    GSsndEmitter* emitter = _sndGetNewEmitter();
    if (emitter == 0) return 0;
    sndAddEmitter(&emitter->emitter, (SND_FVECTOR*)position, (SND_FVECTOR*)velocity,
                  maxDistance, compression, SND_EMITTER_CONTINUOUS | SND_EMITTER_RESTARTABLE | SND_EMITTER_DOPPLERFX | SND_EMITTER_PAUSABLE | SND_EMITTER_ITD | GSSND_EMITTER_DISTANCE_FILTER, entry->fxID,
                  maxVolume, minVolume, 0);
    emitter->position = *position;
    emitter->velocity = *velocity;
    emitter->maxVolume = maxVolume;
    emitter->minVolume = minVolume;
    emitter->maxDistance = maxDistance;
    emitter->compression = compression;
    emitter->soundID = soundID;
    if (sndCheckEmitter(&emitter->emitter) == 1) return emitter;
    _sndFreeEmitter(emitter);
    return 0;
}

u8 GSsndUpdateListenerParam(GSsndListener* listener, GSsndVec3* position,
                            GSsndVec3* heading, GSsndVec3* up)
{
    if (!gSoundStreamReady) return 0;
    if (!listener) return 0;
    GSsndVec3 velocity;
    PSVECSubtract((const Vec*)&listener->position, (const Vec*)position, (Vec*)&velocity);
    if (sndUpdateListener(&listener->listener, (SND_FVECTOR*)position, (SND_FVECTOR*)&velocity,
                          (SND_FVECTOR*)heading, (SND_FVECTOR*)up, listener->volume, 0) == 1) {
        listener->position = *position;
        listener->velocity = velocity;
        listener->heading = *heading;
        listener->up = *up;
        return 1;
    }
    return 0;
}

u8 GSsndFreeListener(GSsndListener* listener)
{
    if (!gSoundStreamReady) return 0;
    if (!listener) return 0;
    sndRemoveListener(&listener->listener);
    _sndFreeListener(listener);
    return 1;
}

GSsndListener* GSsndCreateListener(GSsndVec3* position)
{
    GSsndVec3 velocity;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.0f;
    GSsndVec3 heading;
    heading.x = 0.0f;
    heading.y = 0.0f;
    heading.z = -1.0f;
    GSsndVec3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    return GSsndCreateListenerParam(position, &velocity, &heading, &up,
                                    250.0f, 250.0f, 1000.0f, GSSND_MAX_VOLUME);
}

GSsndListener* GSsndCreateListenerParam(GSsndVec3* position, GSsndVec3* velocity,
                                       GSsndVec3* heading, GSsndVec3* up, f32 frontDistance,
                                       f32 backDistance, f32 soundSpeed, u8 volume)
{
    /* Distance attenuation is measured from one metre. */
    static SND_LISTENER_EXPARAMETER expParam = { 1.0f };

    if (!gSoundStreamReady) return 0;
    GSsndListener* listener = _sndGetNewListener();
    if (!listener) return 0;
    if (sndAddListener(&listener->listener, (SND_FVECTOR*)position, (SND_FVECTOR*)velocity,
                       (SND_FVECTOR*)heading, (SND_FVECTOR*)up, frontDistance, backDistance,
                       soundSpeed, SND_LISTENER_DOPPLERFX, volume, &expParam) == 1) {
        listener->position = *position;
        listener->velocity = *velocity;
        listener->heading = *heading;
        listener->up = *up;
        listener->frontDistance = frontDistance;
        listener->backDistance = backDistance;
        listener->soundSpeed = soundSpeed;
        listener->volume = volume;
        return listener;
    }
    _sndFreeListener(listener);
    return 0;
}

u32 GSsndSetOriginalVolume(s32 soundID, s32 time, s32 volume)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (!entry->soundEffect && entry->open != 1) return 0;
    return _sndSetVolume(entry, time, volume);
}

long GSsndGetStatus(long soundID)
{
    unsigned long id = (unsigned long)soundID;
    if (!gSoundStreamReady) return GSSND_CLOSED;
    if (id == GSSND_INVALID_ID || id >= *snd_data_number) return GSSND_CLOSED;
    GSsndSEEntry* entry = &snd_data[id];
    if (entry->open != 1) return GSSND_CLOSED;
    GSsndWork* work = entry->work;
    if (work == 0) return GSSND_CLOSED;
    if (entry->stream == 1) {
        if (entry->paused == 1) return GSSND_PAUSED;
        if (work->playbackID != GSSND_INVALID_ID && _sndStreamGetValid(work->playbackID) == 1)
            return GSSND_PLAYING;
        _sndClose(id, 1);
        return GSSND_CLOSED;
    } else if (entry->soundEffect == 1) {
        if (work->playbackID != GSSND_INVALID_ID && sndFXCheck(work->playbackID) != GSSND_INVALID_ID)
            return GSSND_PLAYING;
        _sndClose(id, 1);
        return GSSND_CLOSED;
    } else {
        if (entry->paused == 1) return GSSND_PAUSED;
        if (work->playbackID != GSSND_INVALID_ID && sndSeqGetValid(work->playbackID) == 1)
            return GSSND_PLAYING;
        _sndClose(id, 1);
        return GSSND_CLOSED;
    }
}

unsigned long GSsndContinue(unsigned long soundID, long time, long mode)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (entry->soundEffect == 1) return 0;
    if (entry->open != 1) return 0;
    GSsndWork* work = entry->work;
    if (work == 0) return 0;
    if (work->playbackID == GSSND_INVALID_ID) return 0;
    if (entry->paused != 1) return 0;
    GSsndUpdateSettingsReverbHI(entry->reverb & GSSND_REVERB_PRESET_MASK);
    if (soundID == GSSND_PAUSE_EXEMPT_SOUND) return 1;
    if (entry->stream == 1) {
        _sndStreamVolume(work->volume, time, work->playbackID, GSSND_STREAM_FADE_VOLUME);
    } else {
        sndSeqContinue(work->playbackID);
        sndSeqVolume(work->volume, time, work->playbackID, SND_SEQVOL_CONTINUE);
    }
    entry->paused = 0;
    return 1;
}

unsigned long GSsndPause(unsigned long soundID, long time, long mode)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (entry->soundEffect == 1) return 0;
    if (entry->open != 1) return 0;
    GSsndWork* work = entry->work;
    if (work == 0) return 0;
    if (work->playbackID == GSSND_INVALID_ID) return 0;
    if (entry->paused == 1) return 0;
    if (soundID == GSSND_PAUSE_EXEMPT_SOUND) return 1;
    if (entry->stream == 1) {
        _sndStreamVolume(0, time, work->playbackID, GSSND_STREAM_FADE_PAUSE);
    } else {
        sndSeqVolume(0, time, work->playbackID, SND_SEQVOL_CONTINUE);
        sndSeqPause(work->playbackID);
    }
    entry->paused = 1;
    return 1;
}

void GSsndStopNow(unsigned long soundID)
{
    GSsndStop(soundID, 0, 0);
}

unsigned long GSsndStop(long soundID, long time, long mode)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (entry->stream == 1) return _sndStopSTREAM(entry, time, mode);
    if (entry->soundEffect == 1) return _sndStopSE(entry, time, mode);
    return _sndStopBGM(entry, time, mode);
}

unsigned long GSsndPlayNow(long soundID)
{
    return _sndPlay(soundID, 0, GSSND_MAX_VOLUME, 0);
}

unsigned long GSsndPlayVolume(long soundID, long time, unsigned char volume, long mode)
{
    return _sndPlay(soundID, time, volume, mode);
}

unsigned long GSsndPlay(long soundID, long time, long mode)
{
    return _sndPlay(soundID, time, GSSND_MAX_VOLUME, mode);
}

unsigned long GSsndClose(long soundID)
{
    return _sndClose(soundID, 1);
}
