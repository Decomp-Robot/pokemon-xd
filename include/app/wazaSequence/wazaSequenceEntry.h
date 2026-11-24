#ifndef WAZA_SEQUENCE_ENTRY_H
#define WAZA_SEQUENCE_ENTRY_H

#include <types.h>
#include <GScamera.h>
#include <GSmodel.h>
#include <GSsnd.h>
#include <wazaSequence.h>

enum enumSequenceEntryDataType {
    SEQUENCE_ENTRY_DATA_TYPE_MAIN,
    SEQUENCE_ENTRY_DATA_TYPE_CAMERA,
    SEQUENCE_ENTRY_DATA_TYPE_MODEL,
    SEQUENCE_ENTRY_DATA_TYPE_PARTICLE,
    SEQUENCE_ENTRY_DATA_TYPE_EFFECT,
    SEQUENCE_ENTRY_DATA_TYPE_SOUND,
    SEQUENCE_ENTRY_DATA_TYPE_EVENT,
    SEQUENCE_ENTRY_DATA_TYPE_LENS_FLARE
};

enum SequenceEntryState {
    SEQUENCE_ENTRY_STATE_WAITING,
    SEQUENCE_ENTRY_STATE_PLAYING,
    SEQUENCE_ENTRY_STATE_STOPPED,
    SEQUENCE_ENTRY_STATE_FINISHED
};

enum SequenceEntryFlags {
    SEQUENCE_ENTRY_FLAG_LINK_TRANSFORM = 1
};

enum SequenceEntryLinkID {
    SEQUENCE_ENTRY_LINK_NONE = 255
};

enum SoundEntryFlags {
    SOUND_ENTRY_FLAG_MUSIC_EFFECT = 1
};

class SequenceEntry {
public:
    SequenceEntry(enumSequenceEntryDataType type, WazaSequence *sequence, u8 **data);
    virtual ~SequenceEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void End();
    virtual void Stop(bool force);
    virtual void SetRenderCamera(_GScamera *camera);

    u8 id;
    u8 linkedEntryID;
    u8 startFrameIndex;
    u8 linkedFrameIndex;
    u8 frameIndex;
    u8 refEntryID;
    u16 flags;
    enumSequenceEntryDataType type;
    u8 stageJoint;
    u8 stageJointType;
    s16 linkedJoint;
    u8 *data;
    SequenceEntryState state;
    f32 startTime;
    f32 time;
    SequenceEntry *next;
    SequenceEntry *prev;
    WazaSequence *sequence;
};

class MainSequenceEntry : public SequenceEntry {
public:
    MainSequenceEntry(WazaSequence *sequence, u8 **data);
    virtual ~MainSequenceEntry();
};

class CameraEntry : public SequenceEntry {
public:
    CameraEntry(WazaSequence *sequence, u8 **data);
    virtual ~CameraEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void Stop(bool force);

    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class ModelEntry : public SequenceEntry {
public:
    ModelEntry(WazaSequence *sequence, u8 **data);
    virtual ~ModelEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void Stop(bool force);
    virtual void SetRenderCamera(_GScamera *camera);

    static void Unload(u8 **data);
    static bool Load(u8 **data);

    s32 resourceID;
    s32 animationMode;
    s32 animationID;
    s32 textureMode;
    s32 textureID;
    s32 startFrame;
    u16 jointMask;
    u8 keepAfterEnd;
    _GSmodel *model;
};

class ParticleEntry : public SequenceEntry {
public:
    ParticleEntry(WazaSequence *sequence, u8 **data);
    virtual ~ParticleEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void End();
    virtual void Stop(bool force);
    virtual void SetRenderCamera(_GScamera *camera);

    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class EffectEntry : public SequenceEntry {
public:
    EffectEntry(WazaSequence *sequence, u8 **data);
    virtual ~EffectEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void End();
    virtual void Stop(bool force);

    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class SoundEntry : public SequenceEntry {
public:
    SoundEntry(WazaSequence *sequence, u8 **data);
    virtual ~SoundEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void Stop(bool force);

    static void Unload(u8 **data);
    static bool Load(u8 **data);

    long soundID;
    u16 soundFlags;
    u8 minVolume;
    f32 maxDistance;
    GSsndEmitter *emitter;
};

class EventEntry : public SequenceEntry {
public:
    EventEntry(WazaSequence *sequence, u8 **data);
    virtual ~EventEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);

    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

class LensFlareEntry : public SequenceEntry {
public:
    LensFlareEntry(WazaSequence *sequence, u8 **data);
    virtual ~LensFlareEntry();

    virtual u8 Start();
    virtual bool Update(f32 delta);
    virtual void Stop(bool force);

    static void Unload(u8 **data);
    static bool Load(u8 **data);
};

#endif
