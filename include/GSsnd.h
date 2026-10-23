#ifndef GSSND_H
#define GSSND_H

#include <GSsndPlayback.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Engine playback state associated with an open sound effect. */
typedef struct GSsndWork GSsndWork;

/* Sound-effect table entry. */
typedef struct {
    unsigned char  reverb;          // 0x00 low nibble: reverb preset the sound plays with
    unsigned char  unk01;         // 0x01: purpose not established
    unsigned char  envSound : 1;    // 0x02 bit 7: played through the environment channel
    unsigned char  stream : 1;      // 0x02 bit 6: streamed from the disc
    unsigned char  soundEffect : 1; // 0x02 bit 5: sound effect, started through sndFXStartEx
    unsigned char  unk4 : 1;        // 0x02 bit 4: unused by the sound API
    unsigned char  open : 1;        // 0x02 bit 3: the engine's work block is bound to this sound
    unsigned char  paused : 1;      // 0x02 bit 2: set by GSsndPause, cleared by GSsndContinue
    unsigned char  unk1 : 1;        // 0x02 bit 1
    unsigned char  unk0 : 1;        // 0x02 bit 0
    unsigned short fxID;            // 0x04 sound-effect id passed to sndFXStartEx
    unsigned short seqID;           // 0x06 sequence index into the BGM table
    GSsndWork* work;               // 0x08 engine playback state while the sound is open
} GSsndSEEntry;

typedef struct {
    unsigned char  unk00;         // 0x00: unused by the sound API
    unsigned char  open;          // 0x01: set once the engine has opened the wave
    unsigned short groupID;       // 0x02 volume group the wave's samples are pushed into
    unsigned long resourceGroup; // 0x04 resource group the pool/project/directory come from
    unsigned long poolResID;     // 0x08 resource id of the wave's sample pool
    unsigned long projectResID;  // 0x0C resource id of the project data
    unsigned long sampDirResID;  // 0x10 resource id of the sample directory
    unsigned long dvdFileID;     // 0x14 DVD file the sample data is read from
} GSsndWaveEntry;

/* Stream buffer descriptor. */
typedef struct {
    unsigned char  unk00[0x2E]; // 0x00: transfer bookkeeping owned by the stream engine
    unsigned char  aramState[2]; // 0x2E: ARAM transfer state, one byte per stream half
    unsigned long halfWriteOffset[2]; // 0x30: bytes filled in each encoded half-buffer
    unsigned long stagingReadOffset;  // 0x38: unconsumed staging-buffer position
    unsigned long state;       // 0x3C: transfer state the engine switches on
} GSsndStreamBuffer;

/* Per-file stream control block. */
typedef struct GSsndStream {
    struct GSsndStream* next;    // 0x00
    struct GSsndStream* chain;   // 0x04
    long state;                            // 0x08
    unsigned long streamID;                // 0x0C MusyX stream handle
    unsigned char  pad10[0x20];
    unsigned long soundID;                 // 0x30
    unsigned short time;                    // 0x34 volume fade time in ms; 0 snaps
    unsigned char  mode;                    // 0x36 fade mode handed to the stream control
    unsigned char  volume;                  // 0x37 volume being ramped
    unsigned char  volumeTarget;            // 0x38 volume the ramp heads for
    unsigned char  fadeStep;                // 0x39 steps the ramp has taken
    unsigned char  volumeStart;             // 0x3A volume the ramp started from
    unsigned char  bufferIndex;             // 0x3B which buffer descriptor is in use
    unsigned char  channelReadComplete;     // 0x3C: channel has finished its file read
    unsigned char  fadeEnable;              // 0x3D
    unsigned char  driveUnavailable;        // 0x3E disc drive cannot supply stream data
    unsigned char  halfIndex;               // 0x3F which of the descriptor's two aramState
                                 // bytes the engine is working on
    unsigned char  prefetch[0x1C];          // 0x40: descriptor the pre-callback hands to the file system
    long loopFlag;                // 0x5C
    unsigned long loopStartNibble;         // 0x60
    unsigned long loopEndNibble;           // 0x64
    unsigned long playbackEndNibble;       // 0x68
    unsigned char  pad06C[5];
    unsigned char  loopReached;             // 0x71 set when the stream wraps to its loop start
    unsigned char  bufferFlag[2];           // 0x72 one per descriptor: transfer running
    unsigned long loopStartSampleAligned;  // 0x74
    unsigned long loopEndSampleAligned;    // 0x78
    unsigned long playbackEndSample;       // 0x7C
    GSsndStreamBuffer buffer[2]; // 0x80
} GSsndStream;

/* Position or direction handed to the emitter and listener calls, and handed
   on to the MusyX 3-D layer as SND_FVECTOR. */
typedef struct {
    float x;
    float y;
    float z;
} GSsndVec3;

/* Emitter and listener blocks of the engine; only the engine fills them in. */
typedef struct GSsndEmitter GSsndEmitter;
typedef struct GSsndListener GSsndListener;

typedef void (*GSsndStreamDataCallback)(unsigned long, unsigned long, GSsndStream*);

/* Set once the stream layer is up. */
extern unsigned char gSoundStreamReady;

extern unsigned long* snd_data_number;
extern GSsndSEEntry* snd_data;
extern unsigned long* snd_wave_data_number;
extern GSsndWaveEntry* snd_wave_data;

void GSsndUpdateSettingsReverbHI(long preset);
void GSsndSetVolume(long volume, long time, unsigned long bgm, unsigned long environment);


void GSsndStreamPostFunc(unsigned long fileNo, unsigned long arg);
void* GSsndStreamPreFunc(unsigned long fileNo);
long _sndStreamGetSndID(unsigned long fileNo);
unsigned char _sndStreamGetValid(unsigned long fileNo);
void _sndStreamVolume(unsigned char volume, unsigned short time, unsigned long fileNo, unsigned char mode);

#ifdef __cplusplus
}
#endif

#define GSSND_INVALID_ID 0xFFFFFFFF

enum GSsndStatus {
    GSSND_CLOSED = 0,
    GSSND_PLAYING = 2,
    GSSND_PAUSED = 3,
    GS_SOUND_STOPPED = GSSND_CLOSED,
    GS_SOUND_PLAYING = GSSND_PLAYING
};


enum GSsndStreamState {
    GSSND_STREAM_FREE = 0,
    GSSND_STREAM_OPEN = 1,
    GSSND_STREAM_READY = 2,
    GSSND_STREAM_START = 3,
    GSSND_STREAM_ACTIVATE = 6,
    GSSND_STREAM_PLAYING = 9,
    GSSND_STREAM_CLOSE = 10,
    GSSND_STREAM_CLOSED = 11,
    GSSND_STREAM_RESTART = 12,
    GSSND_STREAM_DEACTIVATE = 13
};

enum {
    GSSND_VOLUME_STACK_CAPACITY = 3,
    GSSND_PAUSE_EXEMPT_SOUND = 8,
    GSSND_MAX_VOLUME = 127,
    GSSND_REVERB_PRESET_MASK = 15
};

enum GSsndStreamFadeMode {
    GSSND_STREAM_FADE_VOLUME = 0,
    GSSND_STREAM_FADE_PAUSE = 2
};

typedef struct {
    unsigned char count;
    unsigned char volume[GSSND_VOLUME_STACK_CAPACITY];
} GSsndVolumeStack;

struct GSsndWork {
    unsigned char inUse;
    unsigned char volume;
    unsigned char pan;      // 0x02: pan the sound effect is started with
    unsigned char unk03;    // 0x03: unknown purpose; initialized to 64
    unsigned char volumeCount;
    unsigned char volumeStack[GSSND_VOLUME_STACK_CAPACITY];
    unsigned long playbackID;
    unsigned long resourceGroup;
    unsigned long resourceID;
};


#ifdef __cplusplus
extern "C" {
#endif

typedef struct GSdvdFile GSdvdFile;
typedef struct SND_AUX_REVERBHI SND_AUX_REVERBHI;

extern unsigned long _sndDaemonHDL;
extern unsigned long _SampleDataFileOffset;
extern GSdvdFile* _SampleDataFileInfo;
extern void* _SampleDataUploadBuffer;
extern GSsndEmitter* _sndEmitter;
extern unsigned long _maxEmitter;
extern GSsndListener* _sndListener;
extern unsigned long _maxListener;
extern unsigned char* _sndStack;
extern unsigned long _maxStack;
extern GSsndWork* _sndWork;
extern unsigned long _maxSndWork;
extern unsigned long _revH_now_parm_id;
extern SND_AUX_REVERBHI _revH;

extern GSsndStream* _sndStreaming;
extern unsigned long _maxStreaming;
extern unsigned long _maxSoundID;

void _sndStreamDaemon(GSsndStream* stream);
void _sndDaemon(void);
unsigned char GSsndCheckEnv(long soundID);
unsigned char GSsndCheckStream(long soundID);
unsigned char GSsndCheckSE(long soundID);
void GSsndSetPushVolumeID(long soundID, GSsndVolumeStack* stack);
void GSsndGetPushVolumeID(long soundID, GSsndVolumeStack* stack);
long GSsndGetVolume(long soundID);
unsigned long GSsndPopVolumeID(unsigned long soundID, long time);
unsigned long GSsndPushVolumeID(unsigned long soundID, long time, long volume);
unsigned char GSsndCheckEmitter(GSsndEmitter* emitter);
unsigned char GSsndUpdateEmitter(GSsndEmitter* emitter, GSsndVec3* position);
unsigned char GSsndFreeEmitter(GSsndEmitter* emitter);
GSsndEmitter* GSsndCreateEmitter(long soundID, GSsndVec3* position);
GSsndEmitter* GSsndCreateEmitterParam(long soundID, GSsndVec3* position, GSsndVec3* velocity,
                                      float maxDistance, float compression,
                                      unsigned char maxVolume, unsigned char minVolume);
unsigned char GSsndUpdateListenerParam(GSsndListener* listener, GSsndVec3* position,
                                       GSsndVec3* heading, GSsndVec3* up);
unsigned char GSsndFreeListener(GSsndListener* listener);
GSsndListener* GSsndCreateListener(GSsndVec3* position);
GSsndListener* GSsndCreateListenerParam(GSsndVec3* position, GSsndVec3* velocity,
                                        GSsndVec3* heading, GSsndVec3* up, float frontDistance,
                                        float backDistance, float soundSpeed, unsigned char volume);
unsigned long GSsndSetOriginalVolume(long soundID, long time, long volume);
void GSsndOpenWaveDVD(unsigned long waveID, unsigned long dvdFileID, unsigned long sampleOffset,
                      unsigned long resourceGroup, unsigned long poolResID,
                      unsigned long projectResID, unsigned long sampDirResID);

#ifdef __cplusplus
}
#endif

#endif
