#include <dolphin/axart.h>
#include <dolphin/os.h>
#include <dolphin/mix.h>

static AXART_SOUND* __AXARTSoundList;

// UNUSED: the game never calls this; dead-stripped at link.
void AXARTInit(void) {
    __AXARTSoundList = 0;
    AXARTSet3DDistanceScale(40.0f);
    AXARTSet3DDopplerScale(20.0f);
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void AXARTInitLfo(AXART_LFO* lfo, f32* samples, u32 length, f32 delta) {
    ASSERTLINE(417, samples);
    ASSERTLINE(418, length);

    lfo->lfo = samples;
    lfo->length = length;
    lfo->delta = delta;
    lfo->sampleIndex = 0;
    lfo->counter = lfo->sample1 = lfo->sample = lfo->output = 0.0f;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void AXARTInitArtPitch(AXART_PITCH* articulator) {
    ASSERTLINE(547, articulator);

    articulator->art.type = AXART_TYPE_PITCH;
    articulator->cents = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void AXARTInitArtVolume(AXART_VOLUME* articulator) {
    ASSERTLINE(617, articulator);

    articulator->art.type = AXART_TYPE_VOLUME;
    articulator->attenuation = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
void AXARTInitArtAuxAVolume(AXART_AUXA_VOLUME* articulator) {
    ASSERTLINE(639, articulator);

    articulator->art.type = AXART_TYPE_AUX_A_VOLUME;
    articulator->attenuation = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
void AXARTInitArtAuxBVolume(AXART_AUXB_VOLUME* articulator) {
    ASSERTLINE(661, articulator);

    articulator->art.type = AXART_TYPE_AUX_B_VOLUME;
    articulator->attenuation = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
