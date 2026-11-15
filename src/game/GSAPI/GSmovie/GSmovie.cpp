#include <string.h>
#include <musyx/musyx.h>
#include <dolphin/os.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/vi.h>
#include <GSmovie.h>
#include <GSAPI/GSfsys/GSfsys.h>
#include <GSAPI/GSgapp/GSgapp.h>
#include <GSgfx.h>
#include <GSAPI/GSmovie/THPDraw.h>
#include <GSgfxVideo.h>
#include <GSmem.h>
#include <GSscratch.h>
#include <GSthread.h>

extern "C" {
BOOL THPInit(void);
}

static GSgfxLayerID _movieLayerID = GSGFX_LAYER_INVALID;
static u8 _movieInitialized;
static s32 _movieDaemonHDL;
static u32 _maxStreaming;
static __GSmovieStreaming *_movieStreaming;
static __GSmovieStreaming *_activeMovieStreaming;
static u8 _movieActive;
static GSgfxVideoVSyncCallback OldVIPostCallback;
static u8 _readIdle;
static void *_readAddr;
static u32 _readOffset;
static u32 _readBytes;
static s32 _movieLayerUseCount;

void _movieStreamTHPPlayerPrepareGetDataCallback(long fileMode, void *context, long argument);
BOOL _THPPlayerSetBuffer(__GSmovieStreaming *stream);
void _PlayControl(u32 retraceCount);
void _PlayControlExec(__GSmovieStreaming *stream);

#define THP_VERSION 0x00011000

#define THP_VIDEO_WORK_SIZE 4096

enum MoviePreloadState {
    MOVIE_PRELOAD_READ_NEXT = 0,
    MOVIE_PRELOAD_DONE = 1,
    MOVIE_PRELOAD_BUSY = 2
};

enum MovieRefreshRate {
    MOVIE_PAL_RATE = 5000,
    MOVIE_NTSC_RATE = 5994
};

enum { GSMOVIE_MAX_STREAMING = 4 };
enum { GSMOVIE_RENDER_PRIORITY = 8 };
enum { GSMOVIE_DAEMON_PRIORITY = 30 };
enum { GSMOVIE_LAYER_FIFO_SIZE = 8192, GSMOVIE_LAYER_MATRIX_STACK_CAPACITY = 4 };

void _movieDaemon(void);
void _movieBeforeRender(f32 delta);
void _movieRender(GSgfxLayer *layer, GSgfxLayerPass pass);

static inline __GSmovieStreaming *_movieSearchStreaming(u32 id)
{
    u32 i;
    for (i = 0; i < _maxStreaming; i++) {
        if (_movieStreaming[i].state != GSMOVIE_STREAM_FREE && id == _movieStreaming[i].id) {
            return &_movieStreaming[i];
        }
    }
    return 0;
}

static inline u8 _movieStreamTHPPlayerPrepareGetData(__GSmovieStreaming *stream)
{
    BOOL enabled = OSDisableInterrupts();
    u8 result;

    if (GSfsysStreamGetData(stream->id, 0, _readAddr, THP_STREAM_BLOCK_SIZE, _readOffset,
                            _movieStreamTHPPlayerPrepareGetDataCallback, stream, 0) == TRUE) {
        _readIdle = FALSE;
        result = TRUE;
    } else {
        result = FALSE;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

static inline BOOL _THPPlayerOpen(__GSmovieStreaming *stream)
{
    u32 i;
    THPPlayer *player = stream->player;

    player->hasAudio = FALSE;
    for (i = 0; i < player->numComponents; i++) {
        switch (player->componentTypes[i]) {
        case THP_COMPONENT_VIDEO:
            break;
        case THP_COMPONENT_AUDIO:
            player->hasAudio = TRUE;
            break;
        default:
            return FALSE;
        }
    }
    player->state = player->subState = THP_PLAYER_STOPPED;
    player->loopFlag = FALSE;
    return TRUE;
}

static inline u32 _THPPlayerCalcNeedMemory(__GSmovieStreaming *stream)
{
    u32 size;
    THPPlayer *player = stream->player;

    if (player->preload) {
        size = OSRoundUp32B(player->movieDataSize);
    } else {
        size = 0;
    }
    size += OSRoundUp32B(player->bufferSize) * THP_READ_BUFFER_COUNT;
    size += OSRoundUp32B(player->videoInfo.width * player->videoInfo.height) * THP_TEXTURE_SET_COUNT;
    size += OSRoundUp32B(player->videoInfo.width * player->videoInfo.height / 4) * THP_TEXTURE_SET_COUNT;
    size += OSRoundUp32B(player->videoInfo.width * player->videoInfo.height / 4) * THP_TEXTURE_SET_COUNT;
    if (player->hasAudio) {
        size += OSRoundUp32B(player->audioMaxSamples * 4) * THP_AUDIO_BUFFER_COUNT;
        size += OSRoundUp32B((u32)((f32)(player->audioInfo.audioFrequency * MOVIE_STREAM_BUFFER_MSEC) / 1000.0f + 0.5f)) * 2
              * player->audioInfo.numAudioChannels;
    }
    size += THP_VIDEO_WORK_SIZE;
    return size;
}

static inline u8 _THPPlayerPrepare(__GSmovieStreaming *stream, bool playFlag, s32 audioTrack)
{
    THPPlayer *player = stream->player;

    if (player->state == THP_PLAYER_STOPPED) {
        player->movieDataOffset = player->movieDataStart;
        player->loopFrameSize = player->firstFrameSize;
        player->startFrame = 0;
        player->readFrameSize = player->loopFrameSize;
        if (player->hasAudio) {
            if (audioTrack < 0 || audioTrack >= player->audioInfo.soundTracks) {
                return FALSE;
            }
            player->audioTrack = audioTrack;
        }
        player->loopFlag = playFlag & MOVIE_STREAM_LOOP_FLAG;
        player->decodedCount = 0;
        player->presentedVideoFrameCount = -1;
        player->playedAudioFrameCount = 0;
        return TRUE;
    }
    return FALSE;
}

static inline MoviePreloadState _movieStreamGetPreloadState(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;

    if (!_readIdle) {
        return MOVIE_PRELOAD_BUSY;
    }
    if (_readBytes >= player->movieDataSize) {
        return MOVIE_PRELOAD_DONE;
    }
    return MOVIE_PRELOAD_READ_NEXT;
}

static inline BOOL _THPPlayerPlay(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;

    if (player->state == THP_PLAYER_READY || player->state == THP_PLAYER_PAUSED) {
        player->state = THP_PLAYER_PLAYING;
        player->previousScaled = 0;
        player->scaled = 0;
        player->retraceCount = -1;
        if (OldVIPostCallback == 0) {
            OldVIPostCallback = GSgfxVideoSetVSyncCallback(_PlayControl);
        }
        return TRUE;
    }
    return FALSE;
}

static inline void _movieFreeStreaming(__GSmovieStreaming *stream);

static inline void _movieStop(u32 movieID)
{
    __GSmovieStreaming *stream;
    THPPlayer *player;
    if (!_movieInitialized) {
        return;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream == 0) {
        return;
    }

    GSgfxRenderWaitForDrawComplete();
    if (stream->texture != 0) {
        if (--_movieLayerUseCount == 0) {
            GSgfxLayerConfig::Remove(_movieLayerID);
            _movieLayerID = GSGFX_LAYER_INVALID;
        }
    }
    GSfsysCancel(stream->id);

    player = stream->player;
    if (player->state != THP_PLAYER_STOPPED) {
        player->state = player->subState = THP_PLAYER_STOPPED;
        if (player->hasAudio) {
            _StreamQuit(stream);
        }
        player->dvdError = 0;
        player->videoResult = 0;
    }
    GSmemFree(stream->buffer);
    if (stream == _activeMovieStreaming) {
        _activeMovieStreaming = 0;
    }

    _movieFreeStreaming(stream);

    if (_movieActive == TRUE) {
        u8 streaming = FALSE;
        u32 i;

        for (i = 0; i < _maxStreaming; i++) {
            if (_movieStreaming[i].state != GSMOVIE_STREAM_FREE) {
                streaming = TRUE;
            }
        }
        if (!streaming) {
            GSscratchSetValid();
            _movieActive = FALSE;
            GSgfxVideoSetVSyncCallback(OldVIPostCallback);
            OldVIPostCallback = 0;
        }
    }
}

static inline u8 ProperTimingForStart(__GSmovieStreaming *stream)
{
    if (stream->player->videoInfo.viFlags & THP_VIDEO_ODD_INTERLACE) {
        if (VIGetNextField() == VI_FIELD_ABOVE) {
            return TRUE;
        }
    } else if (stream->player->videoInfo.viFlags & THP_VIDEO_EVEN_INTERLACE) {
        if (VIGetNextField() == VI_FIELD_BELOW) {
            return TRUE;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}

static inline u8 ProperTimingForGettingNextFrame(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;
    s32 frameRate;

    if (player->videoInfo.viFlags & THP_VIDEO_ODD_INTERLACE) {
        if (VIGetNextField() == VI_FIELD_ABOVE) {
            return TRUE;
        }
    } else if (player->videoInfo.viFlags & THP_VIDEO_EVEN_INTERLACE) {
        if (VIGetNextField() == VI_FIELD_BELOW) {
            return TRUE;
        }
    } else {
        frameRate = (s32)(100.0f * player->frameRate);
        if (VIGetTvFormat() == VI_PAL) {
            player->scaled = (s32)((player->retraceCount * frameRate) / MOVIE_PAL_RATE);
        } else {
            player->scaled = (s32)((player->retraceCount * frameRate) / MOVIE_NTSC_RATE);
        }
        if (player->previousScaled != player->scaled) {
            player->previousScaled = player->scaled;
            return TRUE;
        }
    }
    return FALSE;
}

static inline THPTextureSet *PopDecodedTextureSet(__GSmovieStreaming *stream)
{
    THPPlayer *player = stream->player;
    THPTextureSet *textureSet = &player->textureSets[player->nextTextureSetIndex];

    if (!textureSet->ready || textureSet->used == TRUE) {
        return 0;
    }
    player->nextTextureSetIndex++;
    if (player->nextTextureSetIndex >= THP_TEXTURE_SET_COUNT) {
        player->nextTextureSetIndex = 0;
    }
    return textureSet;
}

static inline void _movieReleaseTextureSets(THPPlayer *player)
{
    u32 i;

    for (i = 0; i < THP_TEXTURE_SET_COUNT; i++) {
        if (player->textureSets[i].used == TRUE) {
            player->textureSets[i].used = FALSE;
            player->textureSets[i].ready = FALSE;
        }
    }
}

static inline void _movieContinue(u32 movieID)
{
    __GSmovieStreaming *stream;

    if (!_movieInitialized) {
        return;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream != 0) {
        stream->player->paused = FALSE;
    }
}

static inline void _moviePause(u32 movieID)
{
    __GSmovieStreaming *stream;

    if (!_movieInitialized) {
        return;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream != 0) {
        stream->player->paused = TRUE;
    }
}

static inline __GSmovieStreaming *_movieAllocStreaming(void)
{
    u32 i;
    BOOL enabled = OSDisableInterrupts();
    __GSmovieStreaming *stream = 0;

    for (i = 0; i < _maxStreaming; i++) {
        stream = &_movieStreaming[i];
        if (stream->state == GSMOVIE_STREAM_FREE) {
            stream->player = (THPPlayer *)GSmemAllocAlignFromEnd(sizeof(THPPlayer), 32);
            if (stream->player == 0) {
                stream = 0;
            } else {
                stream->state = GSMOVIE_STREAM_WAIT_OPEN;
            }
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return stream;
}

static inline void _movieFreeStreaming(__GSmovieStreaming *stream)
{
    BOOL enabled = OSDisableInterrupts();

    stream->state = GSMOVIE_STREAM_FREE;
    stream->id = 0;
    if (stream->player != 0) {
        GSmemFree(stream->player);
        stream->player = 0;
    }
    OSRestoreInterrupts(enabled);
}

static inline u8 _movieInitStreaming(void)
{
    __GSmovieStreaming *stream;
    u32 i;

    _maxStreaming = GSMOVIE_MAX_STREAMING;
    _movieStreaming = (__GSmovieStreaming *)GSmemAllocAlignFromEnd(_maxStreaming * sizeof(__GSmovieStreaming), 32);
    if (_movieStreaming == 0) {
        return FALSE;
    }
    for (i = 0; i < _maxStreaming; i++) {
        stream = &_movieStreaming[i];
        stream->id = 0;
        stream->state = GSMOVIE_STREAM_FREE;
        stream->player = 0;
    }
    return TRUE;
}

void GSmovieInit(void)
{
    GSgfxRenderModule module = {0, _movieBeforeRender, _movieRender, 0, 0, 0,
                                GSGFX_RENDER_MODULE_SIGNATURE};

    if (_movieInitialized == TRUE) {
        return;
    }
    if (_movieInitStreaming()) {
        _movieDaemonHDL = GSgappCreate(1, GSMOVIE_DAEMON_PRIORITY, 0, (GSgappCallback)_movieDaemon);
        GSgappSetName(_movieDaemonHDL, "GSmovieDaemon");
        _activeMovieStreaming = 0;
        _movieActive = FALSE;
        GSgfxRenderModuleRegister(GSMOVIE_RENDER_PRIORITY, &module, 0);
        _movieInitialized = TRUE;
    }
}

void GSmoviePlay(u32 movieID, s32 preload, s32 loopFlag, GStexture *texture)
{
    __GSmovieStreaming *stream;
    THPPlayer *player;

    if (!_movieInitialized) {
        return;
    }
    switch (GSmovieGetStatus(movieID)) {
    case GSMOVIE_STATUS_PAUSED:
        _movieContinue(movieID);
        break;
    case GSMOVIE_STATUS_BUSY:
        break;
    default:
        stream = _movieAllocStreaming();
        if (stream == 0) {
            return;
        }
        stream->state = GSMOVIE_STREAM_WAIT_OPEN;
        stream->texture = texture;
        stream->id = movieID;
        stream->buffer = 0;
        stream->decodeFlag = FALSE;
        stream->firstFramePending = TRUE;
        stream->loopFlag = loopFlag;
        stream->audioTrack = 0;
        stream->token = 0;
        stream->streamFilled = FALSE;

        player = stream->player;
        memset(&player->videoInfo, 0, sizeof(THPVideoInfo));
        memset(&player->audioInfo, 0, sizeof(THPAudioInfo));
        player->streamState = THP_STREAM_NEED_READ;
        player->readIndex = 0;
        player->pendingOffset = 0;
        player->readFrameSize = 0;
        player->frameCounter = 0;
        player->videoWorkBuffer = 0;
        player->state = THP_PLAYER_STOPPED;
        player->subState = THP_PLAYER_STOPPED;
        player->loopFlag = FALSE;
        player->hasAudio = FALSE;
        player->dvdError = 0;
        player->videoResult = 0;
        player->preload = preload;
        player->startAudioPending = FALSE;
        player->paused = FALSE;
        player->renderFlag = TRUE;
        player->movieData = 0;
        player->movieDataOffset = 0;
        player->loopFrameSize = 0;
        player->startFrame = 0;
        player->retraceCount = 0;
        player->previousScaled = 0;
        player->scaled = 0;
        player->decodedCount = 0;
        player->audioTrack = 0;
        player->presentedVideoFrameCount = 0;
        player->playedAudioFrameCount = 0;
        player->framesToSkip = 0;
        player->audioDecodeCount = 0;
        player->currentTextureSet = 0;
        player->currentAudioBuffer = 0;
        player->currentReadIndex = 0;
        player->readBufferIndex = 0;
        player->nextReadIndex = 0;
        player->textureSetIndex = 0;
        player->nextTextureSetIndex = 0;
        player->audioBufferWriteIndex = 0;
        player->audioBufferReadIndex = 0;
        player->soundHandle0 = SND_ID_ERROR;
        player->soundHandle1 = SND_ID_ERROR;
        player->audioBuffer0 = 0;
        player->audioBuffer1 = 0;
        memset(&player->audioSampleState, 0, sizeof(THPAudioSampleState));

        if (!GSfsysReadStreaming(movieID)) {
            _movieFreeStreaming(stream);
        }

        if (stream->texture != 0) {
            if (_movieLayerUseCount == 0) {
                _movieLayerID = GSgfxLayerConfig::Add("movie", GSMOVIE_LAYER_FIFO_SIZE, 0,
                                                      GSMOVIE_LAYER_MATRIX_STACK_CAPACITY, GS_GFX_LAYER_NORMAL,
                                                      false, false, false, GSGFX_LAYER_INVALID,
                                                      GS_LAYER_INSERT_FIRST);
            }
            _movieLayerUseCount++;
        }
        break;
    }
}

u8 GSmoviePlayCheck(u32 movieID)
{
    if (!_movieInitialized) {
        return FALSE;
    }
    if (_movieSearchStreaming(movieID) == 0) {
        return FALSE;
    }
    return TRUE;
}

s32 GSmovieGetStatus(u32 movieID)
{
    __GSmovieStreaming *stream;

    if (!_movieInitialized) {
        return GSMOVIE_STATUS_ERROR;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream == 0) {
        return GSMOVIE_STATUS_IDLE;
    }
    if (stream->player->paused == TRUE) {
        return GSMOVIE_STATUS_PAUSED;
    }
    switch (stream->player->state) {
    case THP_PLAYER_ERROR:
        return GSMOVIE_STATUS_ERROR;
    case THP_PLAYER_FINISHED:
        return GSMOVIE_STATUS_DONE;
    }
    return GSMOVIE_STATUS_BUSY;
}

void GSmoviePlayWait(u32 movieID)
{
    __GSmovieStreaming *stream;

    if (!_movieInitialized) {
        return;
    }
    for (;;) {
        stream = _movieSearchStreaming(movieID);
        if (stream == 0) {
            return;
        }
        switch (stream->state) {
        case GSMOVIE_STREAM_FREE:
        case GSMOVIE_STREAM_PLAYING:
        case GSMOVIE_STREAM_STOP_REQUESTED:
        case GSMOVIE_STREAM_STOPPING:
            return;
        }
        GSthreadSwitch();
    }
}

void GSmovieStop(u32 movieID)
{
    __GSmovieStreaming *stream;
    THPPlayer *player;
    if (!_movieInitialized) {
        return;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream == 0) {
        return;
    }

    GSgfxRenderWaitForDrawComplete();
    if (stream->texture != 0) {
        if (--_movieLayerUseCount == 0) {
            GSgfxLayerConfig::Remove(_movieLayerID);
            _movieLayerID = GSGFX_LAYER_INVALID;
        }
    }
    GSfsysCancel(stream->id);

    player = stream->player;
    if (player->state != THP_PLAYER_STOPPED) {
        player->state = player->subState = THP_PLAYER_STOPPED;
        if (player->hasAudio) {
            _StreamQuit(stream);
        }
        player->dvdError = 0;
        player->videoResult = 0;
    }
    GSmemFree(stream->buffer);
    if (stream == _activeMovieStreaming) {
        _activeMovieStreaming = 0;
    }

    _movieFreeStreaming(stream);

    if (_movieActive == TRUE) {
        u8 streaming = FALSE;
        u32 i;

        for (i = 0; i < _maxStreaming; i++) {
            if (_movieStreaming[i].state != GSMOVIE_STREAM_FREE) {
                streaming = TRUE;
            }
        }
        if (!streaming) {
            GSscratchSetValid();
            _movieActive = FALSE;
            GSgfxVideoSetVSyncCallback(OldVIPostCallback);
            OldVIPostCallback = 0;
        }
    }
}

void GSmovieAllPause(void)
{
    u32 i;

    for (i = 0; i < _maxStreaming; i++) {
        if (_movieStreaming[i].state != GSMOVIE_STREAM_FREE) {
            _moviePause(_movieStreaming[i].id);
        }
    }
}

void GSmovieAllContinue(void)
{
    u32 i;

    for (i = 0; i < _maxStreaming; i++) {
        if (_movieStreaming[i].state != GSMOVIE_STREAM_FREE) {
            _movieContinue(_movieStreaming[i].id);
        }
    }
}

void _movieRender(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    __GSmovieStreaming *stream;
    u32 i;
    THPPlayer *player;

    if (pass != GSGFX_LAYER_PASS_OPAQUE) {
        return;
    }
    for (i = 0; i < _maxStreaming; i++) {
        stream = &_movieStreaming[i];
        if (stream->state == GSMOVIE_STREAM_FREE) {
            continue;
        }
        player = stream->player;
        if (!player->renderFlag) {
            continue;
        }
        if (stream->texture == 0) {
            if (layer->mLayerID != GSGFX_LAYER_DEFAULT) {
                continue;
            }
        } else if (_movieLayerID != layer->mLayerID) {
            continue;
        }

        if (player->state != THP_PLAYER_STOPPED && player->currentTextureSet != 0) {
            THPGXSetRenderLayer(layer);
            THPGXYuv2RgbSetup();
            stream->token = THPGXYuv2RgbDraw(player->currentTextureSet->yBuffer, player->currentTextureSet->uBuffer,
                                             player->currentTextureSet->vBuffer, stream->x, stream->y,
                                             player->videoInfo.width, player->videoInfo.height, player->videoInfo.width, player->videoInfo.height);
            THPGXRestore();
        }
        if (stream->texture != 0) {
            layer->textureCopy(pass, stream->texture, false, 0, true, true, true);
        }
    }
}

void _movieBeforeRender(f32 delta)
{
    __GSmovieStreaming *stream;
    u8 playing = FALSE;
    u32 i;
    THPPlayer *player;
    u32 j;

    for (i = 0; i < _maxStreaming; i++) {
        stream = &_movieStreaming[i];
        if (stream->state != GSMOVIE_STREAM_FREE && GSmoviePlayCheck(stream->id)) {
            playing = TRUE;
            switch (stream->player->state) {
            case THP_PLAYER_FINISHED:
            case THP_PLAYER_ERROR:
                stream->state = GSMOVIE_STREAM_STOP_REQUESTED;
                break;
            }
        }
    }

    if (playing == TRUE) {
        for (j = 0; j < _maxStreaming; j++) {
            stream = &_movieStreaming[j];
            if (stream->state != GSMOVIE_STREAM_FREE) {
                player = stream->player;
                GSgfxTokenWaitForClearAndReset(&stream->token);
                _movieReleaseTextureSets(player);
            }
        }
    }
}

void *GSmovieStreamPreFunc(u32 movieID)
{
    __GSmovieStreaming *stream;

    if (!_movieInitialized) {
        return 0;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream == 0) {
        return 0;
    }
    return stream->player;
}

void GSmovieStreamPostFunc(u32 movieID, u32 resourceID)
{
    __GSmovieStreaming *stream;

    if (!_movieInitialized) {
        return;
    }
    stream = _movieSearchStreaming(movieID);
    if (stream != 0) {
        stream->state = GSMOVIE_STREAM_SETUP_BUFFERS;
        GSfsysStreamSetLoopFlag(movieID, resourceID, stream->loopFlag);
    }
}

void _movieDaemon(void)
{
    u32 i;
    __GSmovieStreaming *stream;
    THPPlayer *player;
    u16 efbWidth;
    u16 efbHeight;

    for (i = 0; i < _maxStreaming; i++) {
        stream = &_movieStreaming[i];
        if (stream->state == GSMOVIE_STREAM_FREE) {
            continue;
        }

        player = stream->player;
        if (player->startAudioPending == TRUE) {
            player->startAudioPending = FALSE;
            _StreamPlay(stream);
        }
        _movieStreamPrepareGetData(stream);

        switch (stream->state) {
        case GSMOVIE_STREAM_SETUP_BUFFERS:
            if (player->version != THP_VERSION) {
                _movieStop(stream->id);
                break;
            }
            _THPPlayerOpen(stream);
            stream->buffer = GSmemAllocAlignFromEnd(_THPPlayerCalcNeedMemory(stream), 32);
            if (stream->buffer == 0) {
                _movieStop(stream->id);
                break;
            }
            _THPPlayerSetBuffer(stream);

            if (player->audioInfo.soundTracks != 1) {
                stream->audioTrack = OSGetTick() % player->audioInfo.soundTracks;
            } else {
                stream->audioTrack = 0;
            }

            if (stream->texture == 0) {
                GSgfxVideoGetEFBSize(&efbWidth, &efbHeight);
                stream->x = (efbWidth - player->videoInfo.width) / 2;
                stream->y = (efbHeight - player->videoInfo.height) / 2;
                if (_activeMovieStreaming == 0) {
                    _activeMovieStreaming = stream;
                }
            } else {
                stream->x = 0;
                stream->y = 0;
            }

            if (!_movieActive) {
                GSscratchSetInvalid();
                THPInit();
            }
            _movieActive = TRUE;
            stream->state = GSMOVIE_STREAM_PREPARE_PLAYBACK;
            /* fall through */
        case GSMOVIE_STREAM_PREPARE_PLAYBACK:
            if (!_THPPlayerPrepare(stream, stream->loopFlag == TRUE, stream->audioTrack)) {
                stream->state = GSMOVIE_STREAM_STOP_REQUESTED;
                break;
            }
            if (player->preload) {
                _readIdle = FALSE;
                _readAddr = player->movieData;
                _readOffset = 0;
                _readBytes = 0;
                stream->state = GSMOVIE_STREAM_REQUEST_PRELOAD;
            } else {
                stream->state = GSMOVIE_STREAM_PRIME_DECODERS;
                break;
            }
            /* fall through */
        case GSMOVIE_STREAM_REQUEST_PRELOAD:
            if (_movieStreamTHPPlayerPrepareGetData(stream) != TRUE) {
                break;
            }
            stream->state = GSMOVIE_STREAM_WAIT_PRELOAD;
            /* fall through */
        case GSMOVIE_STREAM_WAIT_PRELOAD:
            switch (_movieStreamGetPreloadState(stream)) {
            case MOVIE_PRELOAD_READ_NEXT:
                stream->state = GSMOVIE_STREAM_REQUEST_PRELOAD;
                break;
            case MOVIE_PRELOAD_DONE:
                stream->state = GSMOVIE_STREAM_PRIME_DECODERS;
                break;
            case MOVIE_PRELOAD_BUSY:
                break;
            }
            break;
        case GSMOVIE_STREAM_PRIME_DECODERS:
            _movieStreamDecode(stream);
            if (stream->decodeFlag) {
                player->state = THP_PLAYER_READY;
                player->subState = THP_PLAYER_STOPPED;
                player->currentTextureSet = 0;
                player->currentAudioBuffer = 0;
                if (player->hasAudio) {
                    _StreamInit(stream);
                    _StreamInitFillStreamBuffer(stream);
                }
                _THPPlayerPlay(stream);
                stream->state = GSMOVIE_STREAM_PLAYING;
            }
            break;
        case GSMOVIE_STREAM_PLAYING:
            _movieStreamDecode(stream);
            break;
        case GSMOVIE_STREAM_STOP_REQUESTED:
            stream->state = GSMOVIE_STREAM_STOPPING;
            _movieStop(stream->id);
            break;
        }
    }
}

void _PlayControlExec(__GSmovieStreaming *stream)
{
    THPTextureSet *textureSet;
    THPPlayer *player = stream->player;
    s32 diff;
    u32 frame;

    if (player->paused == TRUE) {
        return;
    }

    textureSet = (THPTextureSet *)-1;
    if (player->state == THP_PLAYER_PLAYING) {
        if (player->dvdError || player->videoResult) {
            player->state = player->subState = THP_PLAYER_ERROR;
            return;
        }

        player->retraceCount++;
        if (player->retraceCount == 0) {
            if (ProperTimingForStart(stream)) {
                if (player->hasAudio) {
                    diff = player->presentedVideoFrameCount - player->playedAudioFrameCount;
                    if (diff <= 1) {
                        textureSet = PopDecodedTextureSet(stream);
                        player->presentedVideoFrameCount++;
                    } else {
                        if (!_StreamPlay(stream)) {
                            player->state = player->subState = THP_PLAYER_ERROR;
                            return;
                        }
                        player->subState = THP_PLAYER_PLAYING;
                    }
                } else {
                    textureSet = PopDecodedTextureSet(stream);
                }
            } else {
                player->retraceCount = -1;
            }
        } else {
            if (player->hasAudio && player->retraceCount == 1 && player->subState != THP_PLAYER_PLAYING) {
                if (!_StreamPlay(stream)) {
                    player->state = player->subState = THP_PLAYER_ERROR;
                    return;
                }
                player->subState = THP_PLAYER_PLAYING;
            }

            if (ProperTimingForGettingNextFrame(stream)) {
                if (player->hasAudio) {
                    diff = player->presentedVideoFrameCount - player->playedAudioFrameCount;
                    if (diff <= 1) {
                        textureSet = PopDecodedTextureSet(stream);
                        player->presentedVideoFrameCount++;
                    }
                } else {
                    textureSet = PopDecodedTextureSet(stream);
                }
            }
        }

        if (textureSet != 0 && textureSet != (THPTextureSet *)-1) {
            if (player->currentTextureSet != 0) {
                player->currentTextureSet->used = TRUE;
            }
            player->currentTextureSet = textureSet;
        }

        if (!(player->loopFlag & MOVIE_STREAM_LOOP_FLAG)) {
            if (player->hasAudio) {
                frame = player->playedAudioFrameCount + player->startFrame;
                if (frame == player->numFrames && player->currentAudioBuffer == 0) {
                    player->state = player->subState = THP_PLAYER_FINISHED;
                }
            } else {
                if (player->currentTextureSet != 0) {
                    frame = player->currentTextureSet->frame + player->startFrame;
                } else {
                    frame = player->startFrame - 1;
                }
                if (frame == player->numFrames - 1 && textureSet == 0) {
                    player->state = player->subState = THP_PLAYER_FINISHED;
                }
            }
        }
    }
}

void _PlayControl(u32 retraceCount)
{
    if (OldVIPostCallback != 0) {
        OldVIPostCallback(retraceCount);
    }
    for (u32 i = 0; i < _maxStreaming; i++) {
        __GSmovieStreaming *stream = &_movieStreaming[i];
        if (stream->state != GSMOVIE_STREAM_FREE) {
            _PlayControlExec(stream);
        }
    }
}

BOOL _THPPlayerSetBuffer(__GSmovieStreaming *stream)
{
    u8 *ptr;
    THPPlayer *player = stream->player;
    s32 i;
    u32 ySize;
    u32 uvSize;

    if (player->state == THP_PLAYER_STOPPED) {
        ptr = (u8 *)stream->buffer;
        if (player->preload) {
            player->movieData = ptr;
            ptr += player->movieDataSize;
        }

        for (i = 0; i < THP_READ_BUFFER_COUNT; i++) {
            player->readBuffers[i].data = (u32 *)ptr;
            player->readBuffers[i].dataReady = FALSE;
            player->readBuffers[i].videoDecoded = FALSE;
            if (player->hasAudio) {
                player->readBuffers[i].audioDecoded = FALSE;
            } else {
                player->readBuffers[i].audioDecoded = TRUE;
            }
            player->readBuffers[i].used = 0;
            ptr += OSRoundUp32B(player->bufferSize);
        }

        ySize = OSRoundUp32B(player->videoInfo.width * player->videoInfo.height);
        uvSize = OSRoundUp32B(player->videoInfo.width * player->videoInfo.height / 4);
        for (i = 0; i < THP_TEXTURE_SET_COUNT; i++) {
            player->textureSets[i].yBuffer = ptr;
            DCInvalidateRange(ptr, ySize);
            ptr += ySize;
            player->textureSets[i].uBuffer = ptr;
            DCInvalidateRange(ptr, uvSize);
            ptr += uvSize;
            player->textureSets[i].vBuffer = ptr;
            DCInvalidateRange(ptr, uvSize);
            ptr += uvSize;
            player->textureSets[i].ready = FALSE;
            player->textureSets[i].used = FALSE;
        }

        if (player->hasAudio) {
            for (i = 0; i < THP_AUDIO_BUFFER_COUNT; i++) {
                player->audioBuffers[i].buffer = (s16 *)ptr;
                player->audioBuffers[i].currentSample = (s16 *)ptr;
                player->audioBuffers[i].remaining = 0;
                player->audioBuffers[i].valid = FALSE;
                ptr += OSRoundUp32B(player->audioMaxSamples * 4);
            }

            player->audioBuffer0 = ptr;
            ptr += OSRoundUp32B((u32)((f32)(player->audioInfo.audioFrequency * MOVIE_STREAM_BUFFER_MSEC) / 1000.0f + 0.5f)) * 2;
            if (player->audioInfo.numAudioChannels == 2) {
                player->audioBuffer1 = ptr;
                ptr += OSRoundUp32B((u32)((f32)(player->audioInfo.audioFrequency * MOVIE_STREAM_BUFFER_MSEC) / 1000.0f + 0.5f)) * 2;
            }
        }

        player->videoWorkBuffer = ptr;
        return TRUE;
    }
    return FALSE;
}

void _movieStreamTHPPlayerPrepareGetDataCallback(long fileMode, void *context, long argument)
{
    __GSmovieStreaming *stream = (__GSmovieStreaming *)context;
    THPPlayer *player = stream->player;

    _readOffset += THP_STREAM_BLOCK_SIZE;
    if (_readOffset >= player->movieDataSize) {
        _readOffset = 0;
    }
    if (_readOffset >= GSfsysStreamGetCacheSize()) {
        _readOffset = 0;
    }
    _readAddr = (u8 *)_readAddr + THP_STREAM_BLOCK_SIZE;
    _readBytes += THP_STREAM_BLOCK_SIZE;
    if (_readBytes >= player->movieDataSize) {
        _readIdle = TRUE;
    } else if (!_movieStreamTHPPlayerPrepareGetData(stream)) {
        _readIdle = TRUE;
    }
}
