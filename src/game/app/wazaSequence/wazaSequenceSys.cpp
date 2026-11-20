#include <NullSequence.h>
#include <wazaSequence.h>

// Default scale and placement for stages that are not on the active list.
// Every sequence stage that is in use is linked into g_Sequences. The
// Sequence_ functions below read and place the stage that a battle move uses.
// A null stage leaves the result untouched; a non-null stage outside the list
// reads as a neutral placement, and requests to place it are ignored.

NullSequence *g_Sequences;

extern "C" inline bool wazaSequenceSysIsValidSequence(NullSequence *sequence)
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
    if (!wazaSequenceSysIsValidSequence(sequence)) {
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
    if (!wazaSequenceSysIsValidSequence(sequence)) {
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
    if (!wazaSequenceSysIsValidSequence(sequence)) {
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
    if (wazaSequenceSysIsValidSequence(sequence)) {
        sequence->SetScale(scale);
    }
}

void Sequence_SetRotation(NullSequence *sequence, GSvec *rotation)
{
    if (sequence == NULL) {
        return;
    }
    if (wazaSequenceSysIsValidSequence(sequence)) {
        sequence->SetRotation(rotation);
    }
}

void Sequence_SetPosition(NullSequence *sequence, GSvec *position)
{
    if (sequence == NULL) {
        return;
    }
    if (wazaSequenceSysIsValidSequence(sequence)) {
        sequence->SetPosition(position);
    }
}

extern "C" {
const f32 wazaSequenceUnitScale = 1.0f;
const f32 wazaSequenceZeroTransform = 0.0f;
}
