#ifndef SOUND_H
#define SOUND_H

#include <types.h>

#include <GSsnd.h>

/* Sound currently playing in the environment slot and in the BGM slot. Both
   hold 0 while idle and are cleared when their sound is stopped or replaced. */
extern s32 soundPlayEnvID;
extern s32 soundPlayID;

#ifdef __cplusplus
extern "C" {
#endif

void soundInitBGM(void);
u8 soundCheckEnv(s32 sndID);
u32 soundGetEnvID(void);
u32 soundGetBGMID(void);
s32 soundPlayME(s32 sndID, s32 time, u8 volume);
void soundPlayWait(s32 sndID);
void soundContinueBGMEX(s32 sndID, s32 time, u8 wait);
s32 soundPauseBGMEX(s32 time, u8 wait);
void soundOpenWave(s32 waveID);
s32 soundOpenPlayME(s32 sndID, s32 time, u8 volume);
u32 soundGetVolume(s32 sndID);
s32 soundSetVolume(s32 volume, s32 time);
s32 soundSetVolumeID(s32 sndID, u32 time, s32 volume);
s32 soundCreateListener(void);
s32 soundCreateEmitter(s32 sndID, f32 x, f32 y, f32 z);
s32 soundStopEnv(s32 fadeTime);
s32 soundStop(s32 sndID, s32 fadeTime);
s32 soundOpenPlayEnvFadeOutIn(s32 sndID, s32 fadeTime, s32 time, u8 volume);
s32 soundOpenPlayFadeOutIn(s32 sndID, s32 fadeTime, s32 time, u8 volume);
s32 soundOpenPlayEnv(s32 sndID, s32 time, u8 volume);
s32 soundOpenPlay(s32 sndID, s32 time, u8 volume);
s32 _soundOpenPlay(s32 sndID, s32 time, u8 volume, s32 isBGM);
void _soundFadeout(s32 fadeTime, s32 isBGM, s32 wait);
void _soundCloseFastFadeout(s32 sndID);
s32 _soundCheckActive(s32 sndID);
void _soundWait(u32 msec);
void soundInit(void);
#ifdef __cplusplus
}
#endif

#endif
