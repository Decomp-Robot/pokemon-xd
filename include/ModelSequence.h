#ifndef MODEL_SEQUENCE_H
#define MODEL_SEQUENCE_H

#include <NullSequence.h>
#include <GSmodel.h>

/*
 * Pokemon stage of the waza sequence display (wazaSequence.a modelSequence.o):
 * the model of a species that plays back the sequences loaded onto it, which is
 * what the relive ceremony and the evolution demo show the moves of a Pokemon
 * on. The model is built for the species of the Pokemon, and the random number
 * and the shiny flag of the Pokemon record it was built from are what the model
 * is drawn with.
 */
class ModelSequence : public NullSequence {
public:
    ModelSequence(u16 species);
    ModelSequence(u16 species, u32 rnd, bool shiny);
    virtual ~ModelSequence();

    virtual void Update(f32 delta);
    virtual void GetPosition(GSvec *position);
    virtual void GetRotation(GSvec *rotation);
    virtual void GetScale(GSvec *scale);
    virtual void SetPosition(GSvec *position);
    virtual void SetRotation(GSvec *rotation);
    virtual void SetScale(GSvec *scale);
    virtual void SetVisibility(bool visible);

private:
    u32 resourceId;             // 0x3C: loaded model resource ID.
    u32 randomSeed;             // 0x40: seed supplied when creating the Pokemon model.
    u16 resourceKey;            // 0x44: selected model resource key.
    u8 _46[0x11];               // 0x46: other model-sequence state.
    bool shiny;                 // 0x57: shiny state supplied when creating the model.
    u8 _58[0x06];               // 0x58: other model-sequence state.
    bool colorSwapEnabled;      // 0x5E: enable model color swapping.
    bool modulationEnabled;     // 0x5F: enable model modulation.
    bool colorStateApplied;     // 0x60: model color settings are applied.
    u8 _61[0x33];               // 0x61: other model-sequence state.
    _GSmodel *model;            // 0x94: engine model used for transforms.
    u8 _98[0x24];               // 0x98: remaining model-sequence state.
};

#endif
