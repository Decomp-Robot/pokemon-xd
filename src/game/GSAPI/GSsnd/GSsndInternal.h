#ifndef GSSND_INTERNAL_H
#define GSSND_INTERNAL_H

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <musyx/musyx.h>
#include <GSsnd.h>


#ifdef __cplusplus
extern "C" {
#endif

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

void _sndStreamPrepareGetData(GSsndStream *stream);
u32 _sndStreamGetData(GSsndStream *stream, GSsndStreamDataCallback callback);
void _sndStreamGetDataCallback_START(u32 fileNo, u32 arg, GSsndStream *stream);
void _sndStreamGetDataCallback_ACTIVATE(u32 fileNo, u32 arg, GSsndStream *stream);
void _sndStreamGetDataCallback_EXEC_UPDATE(u32 fileNo, u32 arg, GSsndStream *stream);
GSsndStream *_sndGetStream(u32 fileNo);
u8 _sndCheckEmitter(GSsndEmitter *emitter);
GSsndEmitter *_sndGetNewEmitter(void);
GSsndListener *_sndGetNewListener(void);
void _sndFreeEmitter(GSsndEmitter *emitter);
void _sndFreeListener(GSsndListener *listener);
u8 _sndGetProperVolume(GSsndSEEntry *entry, u8 volume);
u32 _sndSetVolume(GSsndSEEntry *entry, u32 time, u32 volume);
u32 _sndClose(u32 soundID, u8 stop);
u32 _sndStopSTREAM(GSsndSEEntry *entry, u32 time, u32 mode);
u32 _sndStopSE(GSsndSEEntry *entry, u32 time, u32 mode);
u32 _sndStopBGM(GSsndSEEntry *entry, u32 time, u32 mode);
u32 _sndPlay(u32 soundID, u32 time, u32 volume, u32 mode);
void _sndStreamCtrl(GSsndStream *stream, u8 mode);
u32 _sndStreamNibble_HeaderON2OFF(u32 nibbleCount);

#ifdef __cplusplus
}
#endif

#endif
