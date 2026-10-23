#include <dolphin.h>
#include <dolphin/ax.h>
#include <dolphin/mix.h>
#include <dolphin/syn.h>
#include "fake_tgmath.h"

#include "__syn.h"

#define AX_MAX_VOICES 64

SYNVOICE __SYNVoice[64];

// UNUSED: the game never calls this; dead-stripped at link.
void __SYNClearVoiceReferences(void* p) {
    AXVPB* axvpb;
    SYNSYNTH* synth;
    SYNVOICE* voice;

    ASSERTLINE(33, p);

    axvpb = p;
    synth = (void*)axvpb->userContext;
    voice = &__SYNVoice[axvpb->index];

    ASSERTLINE(38, synth);
    ASSERTLINE(39, axvpb->index < AX_MAX_VOICES);
    MIXReleaseChannel(axvpb);

    if (voice->keyGroup) {
        synth->keyGroup[voice->midiChannel][voice->keyGroup] = 0;
    }
    if (synth->voice[voice->midiChannel][voice->keyNum] == voice) {
        synth->voice[voice->midiChannel][voice->keyNum] = 0;
    }

    voice->synth = 0;
    synth->notes--;
}

// UNUSED: the game never calls this; dead-stripped at link.
void __SYNSetVoiceToRelease(SYNVOICE* voice, u32 priority) {
    ASSERTLINE(63, voice);
    voice->veState = 3;
    voice->peState = 3;
    AXSetVoicePriority(voice->axvpb, priority);
}

// UNUSED: the game never calls this; dead-stripped at link.
