#include <GSsnd.h>
#include "GSsndInternal.h"
#include <GSfsys.h>
#include <GSAPI/GSgapp/GSgapp.h>
#include <GSdvd.h>
#include <GSlog.h>
#include <GSmem.h>
#include <GSres.h>
#include <dolphin/ar.h>
#include <dolphin/dvd.h>
#include <dolphin/os.h>

/* DSP ADPCM positions are nibble counts; each stream block is 0x10000 nibbles. */
#define SND_NIBBLE_BLOCK_SHIFT 16
#define SND_NIBBLE_BLOCK_ROUND_UP 0xFFFF
#define SND_SAMPLES_PER_BLOCK 57344
#define SND_BYTE_BLOCK_SHIFT 15

unsigned long _SampleDataFileOffset;
GSdvdFile* _SampleDataFileInfo;
void* _SampleDataUploadBuffer;

void* _sndDoMalloc(size_t size)
{
    return GSmemAllocAlign(size, GSSND_ALLOC_ALIGNMENT);
}

void _sndDoFree(void* ptr)
{
    GSmemFree(ptr);
}

void _sndInitParms(GSsndSEEntry* entry, GSsndWork* work)
{
    u32 level;
    level = OSDisableInterrupts();
    entry->open = 1;
    entry->paused = 0;
    entry->work = work;
    work->playbackID = GSSND_INVALID_ID;
    work->volumeCount = 0;
    work->volume = GSSND_MAX_VOLUME;
    work->pan = GSSND_PAN_CENTER;
    work->unk03 = GSSND_PAN_CENTER;
    OSRestoreInterrupts(level);
}

void _sndSetReverbParm(u32 preset)
{
    f32* parm = &snd_rev_data[preset * GSSND_REVERB_PARM_COUNT];

    _revH.tempDisableFX = FALSE;
    _revH.coloration = parm[0];
    _revH.mix = parm[1];
    _revH.time = parm[2];
    _revH.damping = parm[3];
    _revH.preDelay = parm[4];
    _revH.crosstalk = parm[5];
    _revH_now_parm_id = preset;
}

void _sndInitSndWork(void)
{
    u32 i;

    for (i = 0; i < _maxSndWork; i++) {
        _sndWork[i].inUse = 0;
    }
}

void _sndInitStack(void)
{
    u32 i;

    for (i = 0; i < _maxStack; i++) {
        _sndStack[i] = 0;
    }
}

void _sndInitListener(void)
{
    u32 i;

    for (i = 0; i < _maxListener; i++) {
        _sndListener[i].inUse = 0;
    }
}

void _sndInitEmitter(void)
{
    u32 i;

    for (i = 0; i < _maxEmitter; i++) {
        _sndEmitter[i].inUse = 0;
    }
}

GSsndWork* _sndGetNewSndWork(void)
{
    u32 level;
    GSsndWork* work;
    u32 i;

    level = OSDisableInterrupts();
    work = 0;
    for (i = 0; i < _maxSndWork; i++) {
        if (_sndWork[i].inUse != 1) {
            _sndWork[i].inUse = 1;
            work = &_sndWork[i];
            break;
        }
    }
    OSRestoreInterrupts(level);
    return work;
}

GSsndListener* _sndGetNewListener(void)
{
    u32 level;
    GSsndListener* listener;
    u32 i;

    level = OSDisableInterrupts();
    listener = 0;
    for (i = 0; i < _maxListener; i++) {
        if (_sndListener[i].inUse != 1) {
            _sndListener[i].inUse = 1;
            listener = &_sndListener[i];
            break;
        }
    }
    OSRestoreInterrupts(level);
    return listener;
}

GSsndEmitter* _sndGetNewEmitter(void)
{
    u32 level;
    GSsndEmitter* emitter;
    u32 i;

    level = OSDisableInterrupts();
    emitter = 0;
    for (i = 0; i < _maxEmitter; i++) {
        if (_sndEmitter[i].inUse != 1) {
            _sndEmitter[i].inUse = 1;
            emitter = &_sndEmitter[i];
            break;
        }
    }
    OSRestoreInterrupts(level);
    return emitter;
}

GSsndStream* _sndGetNewStream(void)
{
    u32 level;
    GSsndStream* stream;
    u32 i;

    level = OSDisableInterrupts();
    stream = 0;
    for (i = 0; i < _maxStreaming; i++) {
        if (_sndStreaming[i].state == GSSND_STREAM_FREE) {
            _sndStreaming[i].state = GSSND_STREAM_OPEN;
            stream = &_sndStreaming[i];
            break;
        }
    }
    OSRestoreInterrupts(level);
    return stream;
}

void _sndFreeSndWork(GSsndWork* work)
{
    u32 level;
    level = OSDisableInterrupts();
    work->inUse = 0;
    OSRestoreInterrupts(level);
}

void _sndFreeListener(GSsndListener* listener)
{
    u32 level;
    level = OSDisableInterrupts();
    listener->inUse = 0;
    OSRestoreInterrupts(level);
}

void _sndFreeEmitter(GSsndEmitter* emitter)
{
    u32 level;
    level = OSDisableInterrupts();
    emitter->inUse = 0;
    OSRestoreInterrupts(level);
}

void _sndFreeStream(GSsndStream* stream)
{
    u32 level;
    level = OSDisableInterrupts();
    stream->state = GSSND_STREAM_FREE;
    stream->fileNo = 0;
    OSRestoreInterrupts(level);
}

u32 _sndDesc2Id(SND_VOICEID voiceID)
{
    GSsndWork* work;
    s32 i;

    for (i = 0; i < _maxSoundID; i++) {
        work = snd_data[i].work;
        if (work != 0) {
            if (!snd_data[i].stream) {
                if (work->playbackID == voiceID) return i;
            }
        }
    }
    return 0;
}

u32 _sndPlayBGM(GSsndSEEntry* entry, u32 soundID, u32 time, u8 volume, u32 mode)
{
    void* sequence;
    GSsndWork* work;
    SND_PLAYPARA para;

    if (entry->open != 1) {
        if (entry->seqID >= *snd_file_number) return 0;
        GSsndOpen(soundID, snd_file[entry->seqID].fileNo, snd_file[entry->seqID].argument);
    }
    work = entry->work;
    if (work == 0) return 0;
    if (entry->open != 1) return 0;
    if (work->playbackID != GSSND_INVALID_ID) return 0;
    sequence = GSresGetResource(work->resourceGroup, work->resourceID);
    if (sequence == 0) return 0;
    GSsndUpdateSettingsReverbHI(entry->reverb & GSSND_REVERB_PRESET_MASK);
    para.flags = SND_PLAYPARA_VOLUME;
    para.trackMute[0] = 0;
    para.trackMute[1] = 0;
    para.speed = GSSND_SEQ_SPEED_NORMAL;
    para.volume.time = 0;
    para.volume.target = 0;
    para.numSeqVolDef = 0;
    para.seqVolDef = 0;
    para.numFaded = 0;
    para.faded = 0;
    work->volume = volume;
    work->playbackID = sndSeqPlayEx((entry->reverb >> GSSND_SOUND_GROUP_SHIFT) & GSSND_SOUND_GROUP_MASK,
                                    entry->fxID,
                                    sequence, &para, SND_STUDIO_DEFAULT);
    sndSeqVolume(volume, time, work->playbackID, SND_SEQVOL_CONTINUE);
    return 1;
}

u32 _sndPlaySE(GSsndSEEntry* entry, u32 soundID, u8 volume)
{
    GSsndWork* work;

    if (entry->open != 1) {
        GSsndOpen(soundID, 0, 0);
    }
    work = entry->work;
    if (work == 0) return 0;
    work->volume = volume;
    work->playbackID = sndFXStartEx(entry->fxID, volume, work->pan, SND_STUDIO_DEFAULT);
    sndFXCtrl(work->playbackID, SND_MIDICTRL_VOLUME, volume);
    return 1;
}

GSsndStream* _sndGetStream(u32 fileNo)
{
    u32 i;

    for (i = 0; i < _maxStreaming; i++) {
        if (_sndStreaming[i].state == GSSND_STREAM_FREE) continue;
        if (_sndStreaming[i].fileNo == fileNo) return &_sndStreaming[i];
    }
    return 0;
}

u32 _sndPlaySTREAM(GSsndSEEntry* entry, u32 soundID, u32 time, u8 volume, u32 mode)
{
    GSsndWork* work;
    GSsndStream* first;
    GSsndStream* second;
    u32 fileNo;
    u32 resourceID;
    u32 argument;

    if (entry->open != 1) {
        GSsndOpen(soundID, 0, 0);
    }
    work = entry->work;
    if (work == 0) return 0;
    if (!entry->soundEffect && work->playbackID != GSSND_INVALID_ID) return 0;
    fileNo = snd_file[entry->seqID].fileNo;
    resourceID = snd_file[entry->seqID].resourceID;
    argument = snd_file[entry->seqID].argument;
    if (_sndStreamGetValid(fileNo) == 1) {
        GSfsysWait(fileNo);
        GSsndClose(_sndStreamGetSndID(fileNo));
        GSfsysWait(fileNo);
        GSsndOpen(soundID, 0, 0);
        work = entry->work;
        if (work == 0) return 0;
    } else {
        GSfsysCancel(fileNo);
        GSfsysWait(fileNo);
    }
    first = _sndGetNewStream();
    if (first == 0) {
        _sndClose(soundID, 0);
        return 0;
    }
    first->next = 0;
    first->chain = 0;
    first->state = GSSND_STREAM_READY;
    first->fileNo = fileNo;
    first->argument = argument;
    first->volume = 0;
    first->volumeStart = 0;
    first->volumeTarget = volume;
    first->fadeStep = 0;
    first->bufferIndex = 0;
    first->halfIndex = 0;
    first->readSamplePosition = 0;
    first->playbackSamplePosition = 0;
    first->fileReadOffset = 0;
    first->channelReadComplete = 0;
    first->fadeEnable = 0;
    first->driveUnavailable = 0;
    first->soundID = soundID;
    first->time = time;
    first->mode = GSSND_STREAM_FADE_VOLUME;
    second = _sndGetNewStream();
    if (second == 0) {
        _sndClose(soundID, 0);
        _sndFreeStream(first);
        return 0;
    }
    second->next = 0;
    second->chain = 0;
    second->state = GSSND_STREAM_READY;
    second->fileNo = 0;
    second->argument = 0;
    second->volume = 0;
    second->volumeStart = 0;
    second->volumeTarget = volume;
    second->fadeStep = 0;
    second->bufferIndex = 1;
    second->halfIndex = 0;
    second->readSamplePosition = 0;
    second->playbackSamplePosition = 0;
    second->fileReadOffset = 0;
    second->channelReadComplete = 0;
    second->fadeEnable = 0;
    second->driveUnavailable = 0;
    second->soundID = soundID;
    second->time = time;
    second->mode = GSSND_STREAM_FADE_VOLUME;
    first->chain = second;
    second->next = first;
    if (resourceID == 0) {
        resourceID = GSfsysGetInvalidID();
    }
    if (GSfsysReadStreamingEX(fileNo, resourceID, argument) == 0) {
        _sndClose(soundID, 0);
        _sndFreeStream(first);
        _sndFreeStream(second);
        return 0;
    }
    work->volume = volume;
    work->playbackID = fileNo;
    return 1;
}

u8 _sndGetProperVolume(GSsndSEEntry* entry, u8 volume)
{
    if (volume == SND_FX_DEFVOL) {
        if (entry->soundEffect == 1) {
            volume = GSSND_DEFAULT_SE_VOLUME;
        } else {
            volume = GSSND_MAX_VOLUME;
        }
    }
    if (volume > GSSND_MAX_VOLUME) {
        volume = GSSND_MAX_VOLUME;
    }
    return volume;
}

u32 _sndPlay(u32 soundID, u32 time, u8 volume, u32 mode)
{
    GSsndSEEntry* entry;

    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    entry = &snd_data[soundID];
    volume = _sndGetProperVolume(entry, volume);
    if (entry->stream == 1) {
        return _sndPlaySTREAM(entry, soundID, time, volume, mode);
    }
    if (entry->soundEffect == 1) {
        return _sndPlaySE(entry, soundID, volume);
    }
    return _sndPlayBGM(entry, soundID, time, volume, mode);
}

u32 _sndStopBGM(GSsndSEEntry* entry, u32 time, u32 mode)
{
    GSsndWork* work = entry->work;

    if (work == 0) return 0;
    if (entry->open != 1) return 0;
    if (work->playbackID == GSSND_INVALID_ID) return 0;
    sndSeqVolume(0, time, work->playbackID, SND_SEQVOL_STOP);
    work->playbackID = GSSND_INVALID_ID;
    return 1;
}

u32 _sndStopSE(GSsndSEEntry* entry, u32 time, u32 mode)
{
    GSsndWork* work = entry->work;
    u32 result;

    if (work == 0) return 0;
    if (work->playbackID == GSSND_INVALID_ID) return 0;
    if (sndFXCheck(work->playbackID) != GSSND_INVALID_ID) {
        result = sndFXKeyOff(work->playbackID);
    } else {
        result = 0;
    }
    work->playbackID = GSSND_INVALID_ID;
    return result;
}

u32 _sndStopSTREAM(GSsndSEEntry* entry, u32 time, u32 mode)
{
    GSsndWork* work = entry->work;
    GSsndStream* stream;
    GSsndStream* first;
    GSsndStream* second;

    if (work == 0) return 0;
    if (work->playbackID == GSSND_INVALID_ID) return 0;
    GSfsysCancel(work->playbackID);
    work->playbackID = GSSND_INVALID_ID;
    stream = _sndGetStream(snd_file[entry->seqID].fileNo);
    if (stream == 0) return 0;
    if (stream->next != 0) {
        first = stream->next;
        second = stream;
    } else {
        first = stream;
        second = stream->chain;
    }
    sndStreamDeactivate(first->streamID);
    sndStreamDeactivate(second->streamID);
    _sndFreeStream(first);
    _sndFreeStream(second);
    return 1;
}

u32 _sndSetVolumeWork(u32 soundID, u8 volume)
{
    GSsndSEEntry* entry;
    GSsndWork* work;

    entry = &snd_data[soundID];
    if (entry->open != 1) {
        if (entry->soundEffect == 1) {
            GSsndOpen(soundID, 0, 0);
        } else {
            return 0;
        }
    }
    work = entry->work;
    if (work != 0) {
        work->volume = volume & GSSND_MAX_VOLUME;
    }
    return 1;
}

u32 _sndSetVolume(GSsndSEEntry* entry, s32 time, s32 volume)
{
    GSsndWork* work = entry->work;

    if (work == 0) return 0;
    work->volume = _sndGetProperVolume(entry, volume);
    if (work->playbackID != GSSND_INVALID_ID) {
        if (entry->stream == 1) {
            _sndStreamVolume(work->volume, time, work->playbackID, GSSND_STREAM_FADE_VOLUME);
        } else if (entry->soundEffect == 1) {
            sndFXCtrl(work->playbackID, SND_MIDICTRL_VOLUME, work->volume);
        } else {
            sndSeqVolume(work->volume, time, work->playbackID, SND_SEQVOL_CONTINUE);
        }
        return 1;
    }
    return 0;
}

void _sndCheckSndWorkALL(void)
{
    s32 i;

    GSlogWrite("_sndCheckSndWorkALL:Start\n");
    for (i = 0; i < _maxSoundID; i++) {
        GSsndGetStatus((unsigned long)i);
    }
    GSlogWrite("_sndCheckSndWorkALL:End\n");
}

void* _sndSetSampleDataUploadCallbackFunction(u32 offset, u32 bytes)
{
    s32 result;

    while (ARGetDMAStatus()) {
    }
    while (true) {
        result = GSdvdRead(_SampleDataFileInfo, _SampleDataUploadBuffer, bytes, offset + _SampleDataFileOffset);
        switch (result) {
        case DVD_RESULT_CANCELED:
        case DVD_RESULT_FATAL_ERROR:
            continue;
        }
        break;
    }
    return _SampleDataUploadBuffer;
}

u32 _sndOpenWave(u32 waveID, u32 fromDVD, u32 dvdFileID, u32 sampleOffset, u32 resourceGroup,
                 u32 poolResID, u32 projectResID, u32 sampDirResID)
{
    GSsndWaveEntry* entry;
    void* project;
    void* samples;
    void* sampDir;
    void* pool;

    if (!gSoundStreamReady) return 0;
    if (waveID >= *snd_wave_data_number) return 0;
    entry = &snd_wave_data[waveID];
    if (entry->open == 1) return 0;
    project = GSresGetResource(resourceGroup, projectResID);
    if (project == 0) return 0;
    sampDir = GSresGetResource(resourceGroup, sampDirResID);
    if (sampDir == 0) return 0;
    pool = GSresGetResource(resourceGroup, poolResID);
    if (pool == 0) return 0;
    if (fromDVD == 1) {
        samples = 0;
        sndSetSampleDataUploadCallback(_sndSetSampleDataUploadCallbackFunction, ARQGetChunkSize());
        _SampleDataFileInfo = GSdvdOpen((const char*)dvdFileID);
        if (_SampleDataFileInfo == 0) return 0;
        _SampleDataFileOffset = sampleOffset;
        _SampleDataUploadBuffer = _sndDoMalloc(ARQGetChunkSize());
        if (_SampleDataUploadBuffer == 0) {
            GSdvdClose(_SampleDataFileInfo);
            return 0;
        }
    } else {
        samples = (void*)dvdFileID;
        sndSetSampleDataUploadCallback(0, 0);
    }
    if (sndPushGroup(project, entry->groupID, samples, sampDir, pool) == FALSE) {
        if (fromDVD == 1) {
            GSdvdClose(_SampleDataFileInfo);
        }
        return 0;
    }
    if (fromDVD == 1) {
        _sndDoFree(_SampleDataUploadBuffer);
        GSdvdClose(_SampleDataFileInfo);
    }
    entry->open = 1;
    return 1;
}

u32 _sndClose(u32 soundID, u8 stop)
{
    GSsndSEEntry* entry;
    GSsndWork* work;

    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    entry = &snd_data[soundID];
    if (entry->open != 1) return 0;
    work = entry->work;
    if (work == 0) return 0;
    if (stop == 1) {
        GSsndStop(soundID, 0, 0);
    }
    _sndInitParms(entry, work);
    _sndFreeSndWork(work);
    entry->work = 0;
    entry->open = 0;
    return 1;
}

void _sndReceiveMessage(SND_VOICEID voiceID, s32 message)
{
    u32 soundID = _sndDesc2Id(voiceID);

    if (soundID != 0) {
        _sndClose(soundID, 0);
    }
}

u8 _sndStreamInit(u32 count)
{
    GSsndStream* stream;
    u32 i;

    _maxStreaming = count;
    _sndStreaming = (GSsndStream*)_sndDoMalloc(count * sizeof(GSsndStream));
    if (_sndStreaming == 0) return 0;
    for (i = 0; i < _maxStreaming; i++) {
        stream = &_sndStreaming[i];
        stream->next = 0;
        stream->chain = 0;
        stream->state = GSSND_STREAM_FREE;
        stream->volume = 0;
        stream->volumeStart = 0;
        stream->volumeTarget = 0;
        stream->bufferIndex = 0;
        stream->bufferSize = 65536;
        stream->samples = (stream->bufferSize / SND_STREAM_ADPCM_BLKBYTES) * SND_STREAM_ADPCM_BLKSIZE;
        stream->sampleBuffer = _sndDoMalloc(98304);
        stream->halfIndex = 0;
        stream->readSamplePosition = 0;
        stream->playbackSamplePosition = 0;
        stream->fileReadOffset = 0;
        stream->fileNo = 0;
        stream->soundID = 0;
        stream->time = 0;
        stream->mode = 0;
        stream->channelReadComplete = 0;
        stream->fadeEnable = 0;
        stream->driveUnavailable = 0;
        stream->streamID = sndStreamAllocEx(255, stream->sampleBuffer, stream->samples, 32000,
                                            stream->volume, 64, 0, 0, 0, SND_STUDIO_DEFAULT,
                                            SND_STREAM_ADPCM | SND_STREAM_INACTIVE | SND_STREAM_MANUALARAMUPD,
                                            _sndStreamCallback, (u32)stream, 0);
    }
    return 1;
}

u8 _sndCheckEmitter(GSsndEmitter* emitter)
{
    SND_VOICEID voiceID;

    if (!emitter->inUse) return 0;
    voiceID = sndEmitterVoiceID(&emitter->emitter);
    if (voiceID == SND_ID_ERROR) return 0;
    if (!sndCheckEmitter(&emitter->emitter)) {
        sndFXKeyOff(voiceID);
        return 0;
    }
    if (sndFXCheck(voiceID) == SND_ID_ERROR) {
        sndFXKeyOff(voiceID);
        return 0;
    }
    return 1;
}

u8 GSsndInit(u32 musicCount, u32 stackSize, u32 effectCount, u32 listenerCount, u32 emitterCount)
{
    SND_HOOKS hooks = { _sndDoMalloc, _sndDoFree };
    u32 workCount;

    if (gSoundStreamReady == 1) return 0;
    workCount = musicCount + effectCount;
    if (workCount >= SYNTH_MAX_VOICES) return 0;
    _maxSoundID = *snd_data_number;
    _revH_now_parm_id = GSSND_INVALID_ID;
    _maxSndWork = workCount;
    _sndWork = (GSsndWork*)_sndDoMalloc(workCount * sizeof(GSsndWork));
    if (_sndWork == 0) return 0;
    _sndInitSndWork();
    _maxStack = stackSize;
    _sndStack = (u8*)_sndDoMalloc(stackSize);
    if (_sndStack == 0) return 0;
    _sndInitStack();
    _maxListener = listenerCount;
    _sndListener = (GSsndListener*)_sndDoMalloc(listenerCount * sizeof(GSsndListener));
    if (_sndListener == 0) return 0;
    _sndInitListener();
    _maxEmitter = emitterCount;
    _sndEmitter = (GSsndEmitter*)_sndDoMalloc(emitterCount * sizeof(GSsndEmitter));
    if (_sndEmitter == 0) return 0;
    _sndInitEmitter();
    sndSetHooks(&hooks);
    if (sndInit(SYNTH_MAX_VOICES, 48, 16, 1, SND_FLAGS_EMITTERGROUPSUSEMAXVOICES, 6275072) != 0) return 0;
    _sndSetReverbParm(0);
    sndAuxCallbackPrepareReverbHI(&_revH);
    sndSetAuxProcessingCallbacks(SND_STUDIO_DEFAULT, sndAuxCallbackReverbHI, &_revH, SND_MIDI_NONE, 0, 0, 0,
                                 SND_MIDI_NONE, 0);
    sndSetReceiveMessageCallback(_sndReceiveMessage);
    _sndDaemonHDL = GSgappCreate(1, 30, 0, (GSgappCallback)_sndDaemon);
    GSgappSetName(_sndDaemonHDL, "GSmovieDaemon");
    if (!_sndStreamInit(4)) return 0;
    gSoundStreamReady = 1;
    GSsndSetVolume(127, 0, 1, 0);
    GSsndSetVolume(100, 0, 0, 1);
    GSsndSetOutputMode(GSsndGetOutputMode());
    return 1;
}

void GSsndQuit(void)
{
    if (gSoundStreamReady && sndIsInstalled() == 1) {
        GSsndSetVolume(0, 0, 1, 1);
        sndQuit();
        GSgappTerminate(_sndDaemonHDL);
        _sndDaemonHDL = 0;
    }
}

void GSsndSetVolume(long volume, long time, unsigned long music, unsigned long effects)
{
    s32 volGroup;
    GSsndSEEntry* entry;
    s32 soundID;

    if (!gSoundStreamReady) return;
    if (music == 1 && effects == 1) {
        volGroup = SND_ALL_VOLGROUPS;
    } else if (music == 1 && effects != 1) {
        volGroup = SND_MUSIC_VOLGROUPS;
    } else if (music != 1 && effects == 1) {
        volGroup = SND_FX_VOLGROUPS;
    } else {
        return;
    }
    for (soundID = 0; soundID < _maxSoundID; soundID++) {
        entry = &snd_data[soundID];
        if (entry->open == 1) {
            switch (volGroup) {
            case SND_ALL_VOLGROUPS:
                break;
            case SND_MUSIC_VOLGROUPS:
                if (entry->soundEffect == 1) continue;
                break;
            case SND_FX_VOLGROUPS:
                if (!entry->soundEffect) continue;
                break;
            }
            _sndSetVolumeWork(soundID, volume);
            if (entry->stream == 1) _sndSetVolume(entry, time, volume);
        }
    }
    sndVolume(volume, time, volGroup);
}

void GSsndSetMasterVolume(long volume, long time, unsigned long music, unsigned long effects)
{
    if (gSoundStreamReady) {
        sndMasterVolume(volume, time, music, effects);
    }
}

void GSsndSetOutputMode(long mode)
{
    if (!gSoundStreamReady) return;
    sndOutputMode((SND_OUTPUTMODE)mode);
    switch (mode) {
    case GSSND_OUTPUTMODE_MONO:
        OSSetSoundMode(OS_SOUND_MODE_MONO);
        break;
    case GSSND_OUTPUTMODE_STEREO:
    case GSSND_OUTPUTMODE_SURROUND:
        OSSetSoundMode(OS_SOUND_MODE_STEREO);
        break;
    }
}

long GSsndGetOutputMode(void)
{
    switch (OSGetSoundMode()) {
    case OS_SOUND_MODE_MONO:
        return GSSND_OUTPUTMODE_MONO;
    case OS_SOUND_MODE_STEREO:
        return GSSND_OUTPUTMODE_STEREO;
    default:
        return GSSND_OUTPUTMODE_MONO;
    }
}

void GSsndUpdateSettingsReverbHI(unsigned long preset)
{
    if (gSoundStreamReady && preset < *snd_rev_data_number && preset != 0 &&
        _revH_now_parm_id != preset) {
        _sndSetReverbParm(preset);
        sndAuxCallbackUpdateSettingsReverbHI(&_revH);
    }
}

void GSsndOpenWaveDVD(unsigned long waveID, unsigned long dvdFileID, unsigned long sampleOffset,
                      unsigned long resourceGroup, unsigned long poolResID,
                      unsigned long projectResID, unsigned long sampDirResID)
{
    _sndOpenWave(waveID, 1, dvdFileID, sampleOffset, resourceGroup, poolResID, projectResID,
                 sampDirResID);
}

u32 GSsndOpen(u32 soundID, u32 resourceGroup, u32 resourceID)
{
    GSsndSEEntry* entry;
    GSsndWork* work;

    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    entry = &snd_data[soundID];
    if (entry->open == 1) return 0;
    work = _sndGetNewSndWork();
    if (work == 0) {
        _sndCheckSndWorkALL();
        work = _sndGetNewSndWork();
        if (work == 0) return 0;
    }
    _sndInitParms(entry, work);
    work->resourceGroup = resourceGroup;
    work->resourceID = resourceID;
    return 1;
}

unsigned long GSsndClose(long soundID)
{
    return _sndClose(soundID, 1);
}

unsigned long GSsndPlay(long soundID, long time, long mode)
{
    return _sndPlay(soundID, time, GSSND_MAX_VOLUME, mode);
}

unsigned long GSsndPlayVolume(long soundID, long time, unsigned char volume, long mode)
{
    return _sndPlay(soundID, time, volume, mode);
}

unsigned long GSsndPlayNow(long soundID)
{
    return _sndPlay(soundID, 0, GSSND_MAX_VOLUME, 0);
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

void GSsndStopNow(unsigned long soundID)
{
    GSsndStop(soundID, 0, 0);
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

u32 GSsndSetOriginalVolume(s32 soundID, s32 time, s32 volume)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    GSsndSEEntry* entry = &snd_data[soundID];
    if (!entry->soundEffect && entry->open != 1) return 0;
    return _sndSetVolume(entry, time, volume);
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

u8 GSsndFreeListener(GSsndListener* listener)
{
    if (!gSoundStreamReady) return 0;
    if (!listener) return 0;
    sndRemoveListener(&listener->listener);
    _sndFreeListener(listener);
    return 1;
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

GSsndEmitter* GSsndCreateEmitter(s32 soundID, GSsndVec3* position)
{
    GSsndVec3 velocity;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.0f;
    return GSsndCreateEmitterParam(soundID, position, &velocity, 10000.0f, 0.0f, GSSND_MAX_VOLUME, 0);
}

bool GSsndFreeEmitter(GSsndEmitter* emitter)
{
    if (!gSoundStreamReady) return false;
    if (emitter == 0) return false;
    if (!emitter->inUse) return true;
    SND_VOICEID voiceID = sndEmitterVoiceID(&emitter->emitter);
    if (voiceID != GSSND_INVALID_ID) sndFXKeyOff(voiceID);
    sndRemoveEmitter(&emitter->emitter);
    _sndFreeEmitter(emitter);
    return true;
}

bool GSsndUpdateEmitter(GSsndEmitter* emitter, GSsndVec3* position)
{
    if (!gSoundStreamReady) return false;
    if (emitter == 0) return false;
    if (!_sndCheckEmitter(emitter)) return false;
    GSsndVec3 velocity;
    PSVECSubtract((const Vec*)&emitter->position, (const Vec*)position, (Vec*)&velocity);
    if (sndUpdateEmitter(&emitter->emitter, (SND_FVECTOR*)position, (SND_FVECTOR*)&velocity,
                         emitter->maxVolume, 0)) {
        emitter->position = *position;
        emitter->velocity = velocity;
        return true;
    }
    return false;
}

bool GSsndCheckEmitter(GSsndEmitter* emitter)
{
    if (!gSoundStreamReady) return false;
    if (emitter == 0) return false;
    return _sndCheckEmitter(emitter) != 0;
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

u8 GSsndCheckSE(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    return snd_data[soundID].soundEffect;
}

u8 GSsndCheckStream(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    return snd_data[soundID].stream;
}

u8 GSsndCheckEnv(s32 soundID)
{
    if (!gSoundStreamReady) return 0;
    if (soundID == GSSND_INVALID_ID || soundID >= *snd_data_number) return 0;
    return snd_data[soundID].envSound;
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

void _sndStreamDaemon(GSsndStream* stream)
{
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

u8 _sndStreamGetValid(u32 fileNo)
{
    if (_sndGetStream(fileNo) != 0 || GSfsysGetStatus(fileNo) == 1) {
        return 1;
    }
    return 0;
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
