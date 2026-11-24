#include <types.h>
#include <GSvec.h>
#include <wazaSequence.h>
#include <app/wazaSequence/wazaSequenceEntry.h>
#include <GSsnd.h>
#include <GSthread.h>
#include <GSmodel.h>

GSthread *gSoundThread;

void SoundEntry::Stop(bool force)
{
    if (state != SEQUENCE_ENTRY_STATE_STOPPED && (force || state == SEQUENCE_ENTRY_STATE_PLAYING)) {
        state = SEQUENCE_ENTRY_STATE_STOPPED;
        if (soundFlags & SOUND_ENTRY_FLAG_MUSIC_EFFECT) {
            gSoundThread = NULL;
        } else if (emitter != NULL) {
            GSsndFreeEmitter(emitter);
            emitter = NULL;
        }
        GSsndClose(soundID);
    }
}

bool SoundEntry::Update(f32 delta)
{
    time += delta;
    if (soundFlags & SOUND_ENTRY_FLAG_MUSIC_EFFECT) {
        if (gSoundThread != NULL) {
            return GSthreadIsRunning(gSoundThread);
        }
        return false;
    }
    if (emitter != NULL) {
        GSvec position, rotation;
        NullSequence *stage = sequence->stage;
        stage->GetPosition(&position);
        stage->GetRotation(&rotation);
        if ((flags & SEQUENCE_ENTRY_FLAG_LINK_TRANSFORM) && linkedEntryID != 0 && linkedEntryID != SEQUENCE_ENTRY_LINK_NONE) {
            SequenceEntry *entry = sequence->GetEntryFromID(linkedEntryID);
            if (entry->startTime <= startTime && entry->type == SEQUENCE_ENTRY_DATA_TYPE_MODEL) {
                _GSmodel *model = ((ModelEntry *)entry)->model;
                if (model != NULL) {
                    GSmodelForceAnimTransformUpdate(model);
                    GSmodelGetPosition(model, &position);
                    GSmodelGetRotation(model, &rotation);
                }
            }
        }
        if (GSsndUpdateEmitter(emitter, (GSsndVec3 *)&position)) {
            return GSsndCheckEmitter(emitter);
        }
        return false;
    }
    long status = GSsndGetStatus(soundID);
    return status == GSSND_PLAYING || status == GSSND_PAUSED;
}
