#include <dolphin.h>
#include <dolphin/os.h>
#include <dolphin/ax.h>
#include <dolphin/mix.h>
#include <dolphin/syn.h>
#include "fake_tgmath.h"

#include "__syn.h"

SYNSYNTH* __SYNSynthList;

// prototypes
static void __SYNAddSynthToList(SYNSYNTH* synth);
static void __SYNRemoveSynthFromList(SYNSYNTH* synth);

// UNUSED: the game never calls this; dead-stripped at link.
static void __SYNAddSynthToList(SYNSYNTH* synth) {
    BOOL old = OSDisableInterrupts();

    if (__SYNSynthList) {
        synth->next = __SYNSynthList;
    } else {
        synth->next = NULL;
    }
    __SYNSynthList = synth;
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __SYNRemoveSynthFromList(SYNSYNTH* synth) {
    SYNSYNTH* tempHead;
    SYNSYNTH* tempTail;
    SYNSYNTH* tempSynth;
    BOOL old;

    old = OSDisableInterrupts();
    tempHead = NULL;
    tempTail = NULL;
    
    for (tempSynth = __SYNSynthList; tempSynth; tempSynth = tempSynth->next) {
        if (tempSynth != synth) {
            if (tempHead) {
                tempTail->next = tempSynth;
                tempTail = tempSynth;
            } else {
                tempHead = tempTail = tempSynth;
            }
        }
    }

    if (tempTail) {
        tempTail->next = NULL;
    }

    __SYNSynthList = tempHead;
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
void SYNInit(void) {
    int i;

    for (i = 0; i < 64; i++) {
        __SYNVoice[i].synth = 0;
    }
    __SYNSynthList = NULL;
}

// UNUSED: the game never calls this; dead-stripped at link.
void SYNQuit(void) {
    SYNInit();
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void SYNQuitSynth(SYNSYNTH* synth) {
    int i;
    BOOL old;
    SYNVOICE* voice;

    old = OSDisableInterrupts();
    if (synth->notes) {
        for (i = 0; i < 64; i++) {
            voice = &__SYNVoice[i];
            if (voice->synth == synth) {
                MIXReleaseChannel(voice->axvpb);
                AXFreeVoice(voice->axvpb);
                voice->synth = 0;
            }
        }
    }

    __SYNRemoveSynthFromList(synth);
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
void SYNMidiInput(SYNSYNTH* synth, u8* input) {
    u8* src;

    ASSERTLINE(244, synth);
    ASSERTLINE(245, input);

    src = input;
    *(synth->inputPosition) = *(src); (synth->inputPosition) += 1; (src) += 1;
    *(synth->inputPosition) = *(src); (synth->inputPosition) += 1; (src) += 1;
    *(synth->inputPosition) = *(src); (synth->inputPosition) += 1; (src) += 1;
    synth->inputCounter++;

    if (synth->inputCounter >= SYN_INPUT_BUFFER_SIZE) {
        ASSERTMSGLINE(258, FALSE, "synth input buffer exceeded, increase SYN_INPUT_BUFFER_SIZE");
    }
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
