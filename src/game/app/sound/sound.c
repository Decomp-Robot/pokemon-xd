#include <GSfsys.h>
/*
 * Game sound API.
 *
 * Thin, checked layer over the GSnd stream engine: every entry point rejects
 * ids outside the sound-effect table and the two "currently playing" slots
 * (environment effect and BGM) are kept up to date here.
 */

#include <sound.h>
#include <GSsnd.h>
#include <GSlog.h>

/* The sound currently playing in each of the two slots this module owns. */
s32 soundPlayID;
s32 soundPlayEnvID;

/* Printed by soundStop when the BGM cannot be stopped - the stream is gone
   and the sound is not one the engine can stop, so the BGM is reopened. */
const char gsBgmStopWarning[] = "soundStop: Warning! BGM cannot be stopped.(snd_id=%d)\n";

/* GS thread and video timing. */
extern void GSthreadSwitch(void);
extern u8 GSgfxVideoGetVsyncRate(void);
extern f32 GSgfxVideoGetLastRenderTime(void);


/* No stream: the two "currently playing" slots are cleared to 0, and this is
   the value the tests below compare them against. */
#define SND_ID_NONE 0xFFFFFFFF

/* Normalises the engine's non-zero handles to 0/1. */
#define SND_BOOL(v) ((u32)((v) | -(v)) >> 31)

/* Milliseconds the current sound is faded over when something takes its place,
   and the longer fade the BGM gets when a sound effect lets it resume. */
#define SND_FAST_FADE_MS 50
#define SND_BGM_RESUME_MS 500

u8 soundCheckEnv(s32 sndID)
{
    return GSsndCheckEnv(sndID);
}

u32 soundGetEnvID(void)
{
    return soundPlayEnvID;
}

u32 soundGetBGMID(void)
{
    return soundPlayID;
}

s32 soundPlayME(s32 sndID, s32 time, s32 volume)
{
    if (sndID == 0 || (u32)sndID >= *snd_data_number) {
        return 0;
    }
    GSsndPlayVolume(sndID, time, volume, 0);
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

    if (*snd_wave_data_number <= (u32)waveID) {
        return;
    }
    wave = &snd_wave_data[waveID];
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
    soundSetVolumeID(soundGetBGMID(), time, volume);
}

void soundSetVolumeID(s32 sndID, u32 time, s32 volume)
{
    GSsndSetOriginalVolume(sndID, time, volume);
}

s32 soundCreateListener(void)
{
    GSsndVec3 pos;
    GSsndListener* listener;

    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    listener = GSsndCreateListener(&pos);
    return SND_BOOL((u32)listener);
}

s32 soundCreateEmitter(s32 sndID, f32 x, f32 y, f32 z)
{
    GSsndVec3 pos;
    GSsndEmitter* emitter;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    emitter = GSsndCreateEmitter(sndID, &pos);
    return SND_BOOL((u32)emitter);
}

void soundStopEnv(s32 fadeTime)
{
    soundStop(soundPlayEnvID, fadeTime);
}

s32 soundStop(s32 sndID, s32 fadeTime)
{
    s32 isBGM;

    if (sndID == 0 || (u32)sndID >= *snd_data_number) {
        return 0;
    }
    isBGM = 0;
    if (soundPlayID == (u32)sndID) {
        soundPlayID = 0;
        isBGM = 1;
    }
    if (soundPlayEnvID == (u32)sndID) {
        soundPlayEnvID = 0;
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
            GSlogWrite(gsBgmStopWarning, sndID);
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
    if (sndID == 0 || (u32)sndID >= *snd_data_number) {
        return 0;
    }
    _soundFadeout(fadeTime, 0, 1);
    return _soundOpenPlay(sndID, time, volume, 0);
}

s32 soundOpenPlayFadeOutIn(s32 sndID, s32 fadeTime, s32 time, s32 volume)
{
    if (sndID == 0 || (u32)sndID >= *snd_data_number) {
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
    if (sndID == 0 || (u32)sndID >= *snd_data_number) {
        return 0;
    }
    if (isBGM == 0) {
        if (soundPlayEnvID != SND_ID_NONE) {
            _soundCloseFastFadeout(soundPlayEnvID);
        }
        soundPlayEnvID = sndID;
    } else {
        if (snd_data[sndID].soundEffect != 1) {
            if (soundPlayID != (u32)sndID && soundPlayID != SND_ID_NONE) {
                _soundCloseFastFadeout(soundPlayID);
            }
            soundPlayID = sndID;
        }
    }
    GSsndPlayVolume(sndID, time, volume, 0);
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

void soundInit(void)
{
    soundPlayID = 0;
    soundPlayEnvID = 0;
}

