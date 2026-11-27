#ifndef GSSND_INTERNAL_H
#define GSSND_INTERNAL_H

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <musyx/musyx.h>
#include <GSsnd.h>
#include <GSAPI/GSsnd/sndStream.h>

#ifndef SND_EMITTER_DISTANCEFILTER
#define SND_EMITTER_DISTANCEFILTER 0x00000080
#endif


#ifdef __cplusplus
extern "C" {
#endif

enum { GSSND_EMITTER_DISTANCE_FILTER = 0x80 };

#define GSSND_ALLOC_ALIGNMENT 32
#define GSSND_PAN_CENTER 64
#define GSSND_REVERB_PARM_COUNT 6
#define GSSND_SOUND_GROUP_SHIFT 4
#define GSSND_SOUND_GROUP_MASK 15
#define GSSND_SEQ_SPEED_NORMAL 256

/* GSsnd state paired with the MusyX 3-D emitter and listener records. */
typedef struct GSsndEmitter {
    u8 inUse;
    u8 maxVolume;
    u8 minVolume;
    u8 padding;
    u32 soundID;
    f32 maxDistance;
    f32 compression;
    GSsndVec3 position;
    GSsndVec3 velocity;
    SND_EMITTER emitter;
} GSsndEmitter;

typedef struct GSsndListener {
    u8 inUse;
    u8 volume;
    u8 padding[2];
    f32 frontDistance;
    f32 backDistance;
    f32 soundSpeed;
    GSsndVec3 position;
    GSsndVec3 velocity;
    GSsndVec3 heading;
    GSsndVec3 up;
    SND_LISTENER listener;
} GSsndListener;

GSsndWork* _sndGetNewSndWork(void);
void _sndCheckSndWorkALL(void);
void _sndInitParms(GSsndSEEntry* entry, GSsndWork* work);
u32 _sndOpenWave(u32 waveID, u32 fromDVD, u32 dvdFileID, u32 sampleOffset, u32 resourceGroup,
                 u32 poolResID, u32 projectResID, u32 sampDirResID);
void _sndSetReverbParm(u32 preset);
u32 _sndSetVolumeWork(u32 soundID, u8 volume);
void* _sndDoMalloc(size_t size);
void _sndDoFree(void* ptr);
void _sndInitSndWork(void);
void _sndInitStack(void);
void _sndInitListener(void);
void _sndInitEmitter(void);
u8 _sndStreamInit(u32 count);
void _sndReceiveMessage(SND_VOICEID voiceID, s32 message);

GSsndStream *_sndGetStream(u32 fileNo);
u8 _sndCheckEmitter(GSsndEmitter *emitter);
GSsndEmitter *_sndGetNewEmitter(void);
GSsndListener *_sndGetNewListener(void);
void _sndFreeEmitter(GSsndEmitter *emitter);
void _sndFreeListener(GSsndListener *listener);
u8 _sndGetProperVolume(GSsndSEEntry *entry, u8 volume);
u32 _sndSetVolume(GSsndSEEntry *entry, s32 time, s32 volume);
u32 _sndClose(u32 soundID, u8 stop);
u32 _sndStopSTREAM(GSsndSEEntry *entry, u32 time, u32 mode);
u32 _sndStopSE(GSsndSEEntry *entry, u32 time, u32 mode);
u32 _sndStopBGM(GSsndSEEntry *entry, u32 time, u32 mode);
u32 _sndPlay(u32 soundID, u32 time, u8 volume, u32 mode);
u32 _sndPlaySTREAM(GSsndSEEntry *entry, u32 soundID, u32 time, u8 volume, u32 mode);
u32 _sndPlaySE(GSsndSEEntry *entry, u32 soundID, u8 volume);
u32 _sndPlayBGM(GSsndSEEntry *entry, u32 soundID, u32 time, u8 volume, u32 mode);
u32 _sndDesc2Id(SND_VOICEID voiceID);
GSsndStream *_sndGetNewStream(void);
void _sndFreeStream(GSsndStream *stream);
void _sndFreeSndWork(GSsndWork *work);
void *_sndSetSampleDataUploadCallbackFunction(u32 offset, u32 bytes);

#ifdef __cplusplus
}
#endif

#endif
