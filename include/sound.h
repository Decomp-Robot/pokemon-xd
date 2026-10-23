#ifndef SOUND_H
#define SOUND_H

#include <types.h>

/* One entry of the sound-effect ("ME") table the game loads at boot. The sound
   ids every entry point here takes index it, and the engine reads the same
   entry back to work out what kind of sound an id names. */
typedef struct {
    u8  reverb;          // 0x00 low nibble: reverb preset the sound plays with
    u8  unk01;           // 0x01: unused by the sound API
    u8  envSound : 1;    // 0x02 bit 7: played through the environment channel
    u8  stream : 1;      // 0x02 bit 6: streamed from the disc
    u8  soundEffect : 1; // 0x02 bit 5: sound effect, started through sndFXStartEx
    u8  unk4 : 1;        // 0x02 bit 4: unused by the sound API
    u8  open : 1;        // 0x02 bit 3: the engine's work block is bound to this sound
    u8  paused : 1;      // 0x02 bit 2: set by GSsndPause, cleared by GSsndContinue
    u8  unk1 : 1;        // 0x02 bit 1
    u8  unk0 : 1;        // 0x02 bit 0
    u16 fxID;            // 0x04 sound-effect id passed to sndFXStartEx
    u16 seqID;           // 0x06 sequence index into the BGM table
    u32 work;            // 0x08 engine work block while the sound is open
} GSsndSEEntry;

/* One entry of the wave (streamed sample) table: 0x18 bytes per wave, holding
   the parameters the engine opens the wave from. */
typedef struct {
    u8  unk00;         // 0x00: unused by the sound API
    u8  open;          // 0x01: set once the engine has opened the wave
    u16 groupID;       // 0x02 volume group the wave's samples are pushed into
    u32 resourceGroup; // 0x04 resource group the pool/project/directory come from
    u32 poolResID;     // 0x08 resource id of the wave's sample pool
    u32 projectResID;  // 0x0C resource id of the project data
    u32 sampDirResID;  // 0x10 resource id of the sample directory
    u32 dvdFileID;     // 0x14 DVD file the sample data is read from
} GSsndWaveEntry;

/* Head of the sound-effect table: the entry count. */
typedef struct {
    u32 count;
} GSsndSEMaster;

/* Stream buffer descriptor. Two of them (0x80 and 0xC0) sit at the end of the
   stream control block and the current one is picked by the stream's buffer
   index. The stream engine fills the payload while a transfer runs; the sound
   module only resets a descriptor when a stream is (re)started. */
typedef struct {
    u8  unk00[0x2E]; // 0x00: transfer bookkeeping owned by the stream engine
    u8  aramState[2]; // 0x2E: ARAM transfer state, one byte per stream half
    u32 unk30;       // 0x30: the three words the transfer engine fills while a
    u32 unk34;       // 0x34 transfer runs; the sound module only clears them
    u32 unk38;       // 0x38 when a stream is (re)started
    u32 state;       // 0x3C: transfer state the engine switches on
} GSsndStreamBuffer;

/* Per-file stream control block owned by the stream engine: 0x100 bytes per
   file, chained through its first two words. Only the fields this module and
   the engine's stream code touch are broken out; the regions between them are
   named for their offset and are otherwise reserved. */
typedef struct GSsndStream {
    struct GSsndStream* next;    // 0x00
    struct GSsndStream* chain;   // 0x04
    u32 state;                   // 0x08
    u8  pad0C[0x24];
    u32 soundID;                 // 0x30
    u16 time;                    // 0x34 volume fade time in ms; 0 snaps
    u8  mode;                    // 0x36 fade mode handed to the stream control
    u8  volume;                  // 0x37 volume being ramped
    u8  volumeTarget;            // 0x38 volume the ramp heads for
    u8  fadeStep;                // 0x39 steps the ramp has taken
    u8  volumeStart;             // 0x3A volume the ramp started from
    u8  bufferIndex;             // 0x3B which buffer descriptor is in use
    u8  unk3C;                   // 0x3C: byte flag the stream engine sets and clears
    u8  fadeEnable;              // 0x3D
    u8  unk3E;                   // 0x3E: purpose unknown
    u8  halfIndex;               // 0x3F which of the descriptor's two aramState
                                 // bytes the engine is working on
    u8  prefetch[0x1C];          // 0x40: descriptor the pre-callback hands to the file system
    s32 loopFlag;                // 0x5C
    u32 loopStart;               // 0x60
    u32 loopEnd;                 // 0x64
    u32 loopPreload;             // 0x68
    u8  pad06C[5];
    u8  loopReached;             // 0x71 set when the stream wraps to its loop start
    u8  bufferFlag[2];           // 0x72 one per descriptor: transfer running
    u32 byteRateStart;           // 0x74
    u32 byteRateEnd;             // 0x78
    u32 byteRatePreload;         // 0x7C
    GSsndStreamBuffer buffer[2]; // 0x80
} GSsndStream;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} GSsndVec3;

/* Raised by GSsndInit once the stream layer is up: every GSnd entry point and
   both stream callbacks do nothing while it is clear. */
extern u8 gSoundStreamReady;

/* Sound currently playing in the environment slot and in the BGM slot. Both
   hold 0 while idle and are cleared when their sound is stopped or replaced. */
extern s32 gSoundCurrentEnvID;
extern s32 gSoundCurrentBGMID;

extern GSsndSEMaster* gSoundSEMaster;
extern GSsndSEEntry* gSoundSEList;
extern u32* gSoundWaveCountList;
extern GSsndWaveEntry* gSoundWaveList;


#ifdef __cplusplus
extern "C" {
#endif

void GSsndClose(s32 soundID);
void soundInitBGM(void);
s32 soundCheckEnv(s32 sndID);
s32 soundGetEnvID(void);
s32 soundGetBGMID(void);
s32 soundPlayME(s32 sndID, s32 time, s32 volume);
void soundPlayWait(s32 sndID);
void soundContinueBGMEX(s32 sndID, s32 time, u8 wait);
s32 soundPauseBGMEX(s32 time, u8 wait);
void soundOpenWave(s32 waveID);
s32 soundOpenPlayME(s32 sndID, s32 time, s32 volume);
u32 soundGetVolume(s32 sndID);
void soundSetVolume(s32 volume, s32 time);
void soundSetOriginalVolume(s32 sndID, s32 time, s32 volume);
s32 soundCreateListener(void);
s32 soundCreateEmitter(s32 sndID, f32 x, f32 y, f32 z);
void soundStopEnv(s32 fadeTime);
s32 soundStop(s32 sndID, s32 fadeTime);
s32 soundOpenPlayEnvFadeOutIn(s32 sndID, s32 fadeTime, s32 time, s32 volume);
s32 soundOpenPlayFadeOutIn(s32 sndID, s32 fadeTime, s32 time, s32 volume);
s32 soundOpenPlayEnv(s32 sndID, s32 time, s32 volume);
s32 soundOpenPlay(s32 sndID, s32 time, s32 volume);
s32 _soundOpenPlay(s32 sndID, s32 time, s32 volume, s32 isBGM);
void _soundFadeout(s32 fadeTime, s32 isBGM, s32 wait);
void _soundCloseFastFadeout(s32 sndID);
s32 _soundCheckActive(s32 sndID);
void _soundWait(u32 msec);
void GSsndPlayNow(long soundID);
void soundClearCurrentIDs(void);
void GSsndStreamPostFunc(u32 fileNo, u32 arg);
void* GSsndStreamPreFunc(u32 fileNo);
s32 _sndStreamGetSndID(u32 fileNo);
s32 _sndStreamGetValid(u32 fileNo);
void _sndStreamVolume(u8 volume, u16 time, u32 fileNo, u8 mode);

#ifdef __cplusplus
}
#endif

#endif
