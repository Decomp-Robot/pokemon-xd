#ifndef GSMOVIE_H
#define GSMOVIE_H

#include <types.h>

struct GStexture;

#ifdef __cplusplus
extern "C" {
#endif
u8 GSmoviePlayCheck(u32 movieID);
void GSmovieStop(u32 movieID);
void GSmoviePlayWait(u32 movieID);
s32 GSmoviePlay(u32 movieID, s32 flags, s32 loopFlag, struct GStexture *texture);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#define THP_STREAM_BLOCK_SIZE 32768

enum GSmovieStreamState {
    GSMOVIE_STREAM_FREE = 0,
    GSMOVIE_STREAM_WAIT_OPEN = 1,
    GSMOVIE_STREAM_PREPARE_PLAYBACK = 2,
    GSMOVIE_STREAM_SETUP_BUFFERS = 3,
    GSMOVIE_STREAM_UNKNOWN_4 = 4,
    GSMOVIE_STREAM_WAIT_PRELOAD = 5,
    GSMOVIE_STREAM_REQUEST_PRELOAD = 6,
    GSMOVIE_STREAM_PRIME_DECODERS = 7,
    GSMOVIE_STREAM_PLAYING = 9,
    GSMOVIE_STREAM_STOP_REQUESTED = 10,
    GSMOVIE_STREAM_STOPPING = 11
};

enum THPPlayerState {
    THP_PLAYER_STOPPED = 0,
    THP_PLAYER_READY = 1,
    THP_PLAYER_PLAYING = 2,
    THP_PLAYER_FINISHED = 3,
    THP_PLAYER_ERROR = 5
};

enum THPStreamReadState {
    THP_STREAM_NEED_READ = 1,
    THP_STREAM_READ_PENDING = 2,
    THP_STREAM_COPY_PENDING = 3
};

enum {
    THP_READ_BUFFER_COUNT = 10,
    THP_TEXTURE_SET_COUNT = 3,
    THP_AUDIO_BUFFER_COUNT = 6,
    THP_SAMPLE_RING_SIZE = 5
};

enum THPAudioSampleStatus {
    THP_AUDIO_REQUEST_FILLED = 0,
    THP_AUDIO_FRAME_CONSUMED = 1,
    THP_AUDIO_NO_DATA = 2
};

enum THPFrameComponent {
    THP_COMPONENT_VIDEO = 0,
    THP_COMPONENT_AUDIO = 1
};

struct __GSmovieStreaming;
struct THPTextureSet;
struct THPAudioBuffer;

struct THPReadBuffer {
    u32 *data;
    u32 frame;
    u8 dataReady;
    u8 videoDecoded;
    u8 audioDecoded;
    u8 pad;
    u32 used;
};

struct THPTextureSet {
    void *yBuffer;
    void *uBuffer;
    void *vBuffer;
    u32 frame;
    u8 ready;
    u8 used;
    u8 pad[2];
};

struct THPAudioBuffer {
    s16 *buffer;
    s16 *currentSample;
    u32 remaining;
    u8 valid;
    u8 pad[3];
};

struct THPPlayer {
    char magic[4];
    u32 version;
    u32 bufferSize;
    u32 audioMaxSamples;
    f32 frameRate;
    u32 numFrames;
    u32 firstFrameSize;
    u32 movieDataSize;
    u32 dataOffsets[4];
    u32 numComponents;
    u8 componentTypes[16];
    u32 width;
    u32 height;
    u32 viFlags;
    u32 numAudioChannels;
    u32 audioFrequency;
    u32 soundSamples;
    u32 soundTracks;
    u8 staging[THP_STREAM_BLOCK_SIZE];
    u32 streamState;
    u32 readIndex;
    u32 pendingOffset;
    u32 readFrameSize;
    u32 frameCounter;
    void *videoWorkBuffer;
    u8 state;
    u8 subState;
    u8 loopFlag;
    u8 hasAudio;
    u32 dvdError;
    u32 videoResult;
    u8 preload;
    u8 startAudioPending;
    u8 paused;
    u8 renderFlag;
    void *movieData;
    u32 movieDataOffset;
    u32 loopFrameSize;
    u32 startFrame;
    u64 retraceCount;
    u32 previousScaled;
    u32 scaled;
    u32 decodedCount;
    u32 audioTrack;
    u32 presentedVideoFrameCount;
    u32 playedAudioFrameCount;
    u16 framesToSkip;
    u16 audioDecodeCount;
    THPTextureSet *currentTextureSet;
    THPAudioBuffer *currentAudioBuffer;
    THPReadBuffer readBuffers[THP_READ_BUFFER_COUNT];
    u32 currentReadIndex;
    u32 readBufferIndex;
    u32 nextReadIndex;
    THPTextureSet textureSets[THP_TEXTURE_SET_COUNT];
    u32 textureSetIndex;
    u32 nextTextureSetIndex;
    THPAudioBuffer audioBuffers[THP_AUDIO_BUFFER_COUNT];
    u32 audioBufferWriteIndex;
    u32 audioBufferReadIndex;
    u32 soundHandle0;
    u32 soundHandle1;
    void *audioBuffer0;
    void *audioBuffer1;
    u64 sampleRing[THP_SAMPLE_RING_SIZE];
    u32 ringReadIndex;
    u32 ringWriteIndex;
    u64 sampleTotal;
    u64 samplePosition;
    u64 sampleAccumulator;
    u32 pad[2];
};

struct __GSmovieStreaming {
    s32 state;
    GStexture *texture;
    u32 id;
    u16 x;
    u16 y;
    void *buffer;
    u8 decodeFlag;
    u8 firstFramePending;
    u8 loopFlag;
    u8 audioTrack;
    u16 token;
    u8 streamFilled;
    u8 pad;
    THPPlayer *player;
};

extern "C" {
s32 _StreamInit(__GSmovieStreaming *stream);
s32 _StreamInitFillStreamBuffer(__GSmovieStreaming *stream);
u8 _StreamPlay(__GSmovieStreaming *stream);
void _StreamQuit(__GSmovieStreaming *stream);
void _movieStreamDecode(__GSmovieStreaming *stream);
void _movieStreamPrepareGetData(__GSmovieStreaming *stream);
}

u32 StreamUpdateCallback(void *buffer1, u32 len1, void *buffer2, u32 len2, u32 user);
u32 GetAudioSample(__GSmovieStreaming *stream, s16 *left, s16 *right, u32 count, long *status);
u32 _VideoDecode(__GSmovieStreaming *stream, THPReadBuffer *buffer, THPTextureSet *textureSet);
void _movieStreamPrepareGetDataCallback_Exec(__GSmovieStreaming *stream, u32 offset, u32 count);
void _movieStreamPrepareGetDataCallback(long fileMode, void *context, long argument);

#endif /* __cplusplus */

#endif
