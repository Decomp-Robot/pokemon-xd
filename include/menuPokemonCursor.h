#ifndef MENU_POKEMON_CURSOR_H
#define MENU_POKEMON_CURSOR_H

#include <CAnt.h>
#include <PocketVec.h>
#include <Pokemon.h>
#include <types.h>

// Party-menu cursor: the pokemon it currently points at, the position
// table entry it sits on and the model state of both records.
class CMenuPokemonCursor {
public:
    void draw();
    bool isCatchPokemon();
    void deletePokemon();
    void setAnimationOff();
    void init();
    void setBeforAnt(CAnt &ant);
    void setPokemon(Pokemon *pokemon);
    void setPositionTableDummy();
    void setPositionIndex(long index);
    void setPositionTable(PocketVec<short> *table, long index);

private:
    enum AnimationState {
        ANIMATION_NONE = 0,
        ANIMATION_CHANGE = 1,
        ANIMATION_DELETE = 2,
        ANIMATION_MOVE = 3
    };
    bool mDisplay;                         // 0x00
    AnimationState mAnimationState;         // 0x04
    float mAnimationRate;                   // 0x08
    long mPositionIndex;                   // 0x0C: current table index.
    long mPrevPositionIndex;               // 0x10: index before the switch.
    s16 mPositionX;                        // 0x14: snapped cursor x.
    s16 mPositionY;                        // 0x16: snapped cursor y.
    PocketVec<short> *mPositionTable;      // 0x18: position table.
    float mFlashTime;                      // 0x1C: elapsed cursor flash time.
    Pokemon mPokemon;                      // 0x20: the pokemon the cursor holds.
    CAnt mAnt;                             // 0xE4: its model.
    CAnt mBeforAnt;                        // 0x118: model kept before the switch.
    bool mHighlight;                      // 0x14C: draw the cursor in yellow.

    static PocketVec<short> defaultPositionTbl;
};

#endif
