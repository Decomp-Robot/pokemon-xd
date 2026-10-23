#ifndef GS_SND_H
#define GS_SND_H
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

extern GSsndSEMaster* gSoundSEMaster;
extern GSsndSEEntry* gSoundSEList;
extern u32* gSoundWaveCountList;
extern GSsndWaveEntry* gSoundWaveList;


#ifdef __cplusplus
extern "C" {
#endif
u8 GSsndCheckStream(s32 sndID);
void GSsndSetOriginalVolume(s32 sndID, s32 time, s32 volume);
void GSsndContinue(s32 sndID, s32 time, s32 mode);
void GSsndPause(s32 sndID, s32 time, s32 mode);
void GSsndOpenWaveDVD(u32 waveID, u32 dvdFileID, u32 sampleOffset, u32 resourceGroup,
                             u32 poolResID, u32 projectResID, u32 sampDirResID);
s32 GSsndCreateListener(GSsndVec3* pos);
s32 GSsndCreateEmitter(s32 sndID, GSsndVec3* pos);

void GSsndPlayVolume(s32 sndID, s32 time, s32 volume, s32 mode);

void GSsndStreamPostFunc(u32 fileNo, u32 arg);
void* GSsndStreamPreFunc(u32 fileNo);
s32 _sndStreamGetSndID(u32 fileNo);
s32 _sndStreamGetValid(u32 fileNo);
void _sndStreamVolume(u8 volume, u16 time, u32 fileNo, u8 mode);

void GSsndPlay(s32 soundID, s32 delay, s32 mode);
s32 GSsndGetStatus(s32 soundID);
s32 GSsndStop(s32 soundID, s32 time, s32 mode);
void GSsndClose(s32 soundID);
s32 GSsndStopNow(s32 soundID);
s32 GSsndPlayNow(s32 soundID);
u8 GSsndCheckSE(s32 soundID);
u8 GSsndCheckEnv(s32 soundID);
s32 GSsndGetVolume(s32 soundID);
void GSsndSetVolume(s32 volume, s32 time, u32 bgm, u32 environment);
#ifdef __cplusplus
}
#endif
enum GSsndStatus { GS_SOUND_STOPPED = 0, GS_SOUND_PLAYING = 2 };
#endif
