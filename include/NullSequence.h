#ifndef NULL_SEQUENCE_H
#define NULL_SEQUENCE_H

#include <GSvec.h>

class WazaSequence;
struct _GScamera;

/*
 * Stage a waza sequence plays back on (wazaSequence.a nullSequence.o): the model
 * of a Pokemon is a stage of this kind, and so are the plain stages of the
 * sequence system.
 *
 * A stage holds the sequences that were loaded onto it as a list, which grows at
 * the front (Add) and is walked by the sequence system to update the sequences
 * (Update) and to hand them the camera they are rendered with. The position,
 * rotation and scale of the stage are what the sequences are placed with, and
 * the Pokemon model overrides how they are read and written; a plain stage keeps
 * them in the fields below.
 *
 * The stage is looked up by the sequence system with the id of the model it was
 * built for (the species of the Pokemon) and the category of the sequence.
 */
class NullSequence {
public:
    NullSequence();
    virtual ~NullSequence();

    void Add(WazaSequence *waza);
    void Remove(WazaSequence *waza);
    WazaSequence *GetWaza(u16 sequence, u16 category) const;
    bool IsActive() const;
    void FreeAllWaza();

    // Advance the sequences loaded on the stage: the sequences that are still
    // running are stepped and handed the camera of the stage.
    virtual void Update(f32 delta);

    // Read and write where the stage stands, how it is turned and how it is
    // scaled: the sequences of the stage are placed with them.
    virtual void GetPosition(GSvec *position);
    virtual void GetRotation(GSvec *rotation);
    virtual void GetScale(GSvec *scale);
    virtual void SetPosition(GSvec *position);
    virtual void SetRotation(GSvec *rotation);
    virtual void SetScale(GSvec *scale);

    // Show or hide the stage.
    virtual void SetVisibility(bool visible);

    u16 modelID;             // 0x04: id the resource of the stage is looked up
                             //       with (the species of the Pokemon model).
    bool visible;            // 0x06: set while the stage is drawn.
    u8 _07;                  // 0x07: unknown.
    u32 isModel;             // 0x08: 1 while the stage is a Pokemon model: the
                             //       sequence code only hands a ModelSequence
                             //       to the resource lookup.
    GSvec position;          // 0x0C: where the stage stands.
    GSvec rotation;          // 0x18: how the stage is turned.
    GSvec scale;             // 0x24: how the stage is scaled.
    WazaSequence *waza;      // 0x30: sequences loaded on the stage, newest first.
    _GScamera *renderCamera; // 0x34: camera the sequences are rendered with.
    NullSequence *next;      // 0x38: next stage of the list of live stages.
};

#endif
