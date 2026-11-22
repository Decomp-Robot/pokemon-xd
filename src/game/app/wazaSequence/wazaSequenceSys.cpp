#include <NullSequence.h>
#include <wazaSequence.h>

// Default scale and placement for stages that are not on the active list.
// Every sequence stage that is in use is linked into g_Sequences. The
// Sequence_ functions below read and place the stage that a battle move uses.
// A null stage leaves the result untouched; a non-null stage outside the list
// reads as a neutral placement, and requests to place it are ignored.

NullSequence *g_Sequences;
// Move that was started most recently, read back by the impact-point check.
WazaSequence *g_LastStartedWaza;

static inline bool isStageOnList(NullSequence *sequence)
{
    NullSequence *seq = g_Sequences;
    if (sequence == NULL) {
        return false;
    }
    while (seq != NULL) {
        if (seq == sequence) {
            return true;
        }
        seq = seq->next;
    }
    return false;
}

void Sequence_GetScale(NullSequence *sequence, GSvec *scale)
{
    if (sequence == NULL) {
        return;
    }
    if (!isStageOnList(sequence)) {
        scale->x = scale->y = scale->z = wazaSequenceUnitScale;
    } else {
        sequence->GetScale(scale);
    }
}

void Sequence_GetRotation(NullSequence *sequence, GSvec *rotation)
{
    if (sequence == NULL) {
        return;
    }
    if (!isStageOnList(sequence)) {
        rotation->x = wazaSequenceZeroTransform;
        rotation->y = wazaSequenceZeroTransform;
        rotation->z = wazaSequenceZeroTransform;
    } else {
        sequence->GetRotation(rotation);
    }
}

void Sequence_GetPosition(NullSequence *sequence, GSvec *position)
{
    if (sequence == NULL) {
        return;
    }
    if (!isStageOnList(sequence)) {
        position->x = wazaSequenceZeroTransform;
        position->y = wazaSequenceZeroTransform;
        position->z = wazaSequenceZeroTransform;
    } else {
        sequence->GetPosition(position);
    }
}

void Sequence_SetScale(NullSequence *sequence, GSvec *scale)
{
    if (sequence == NULL) {
        return;
    }
    if (isStageOnList(sequence)) {
        sequence->SetScale(scale);
    }
}

void Sequence_SetRotation(NullSequence *sequence, GSvec *rotation)
{
    if (sequence == NULL) {
        return;
    }
    if (isStageOnList(sequence)) {
        sequence->SetRotation(rotation);
    }
}

void Sequence_SetPosition(NullSequence *sequence, GSvec *position)
{
    if (sequence == NULL) {
        return;
    }
    if (isStageOnList(sequence)) {
        sequence->SetPosition(position);
    }
}

extern "C" {
const f32 wazaSequenceUnitScale = 1.0f;
const f32 wazaSequenceZeroTransform = 0.0f;
}
