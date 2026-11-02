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
    static PocketVec<short> defaultPositionTbl;
    void init();
    void draw();
    bool isCatchPokemon();
    void deletePokemon();
    void setAnimationOff();
    void setBeforAnt(CAnt &ant);
    void setPokemon(Pokemon *pokemon);
    void setPositionTableDummy();
    void setPositionIndex(long index);
    void setPositionTable(PocketVec<short> *table, long index);

private:
    enum AnimationState { ANIMATION_NONE, ANIMATION_EXCHANGE, ANIMATION_REMOVE, ANIMATION_MOVE };
    bool mDraw;
    u8 _01[3];
    long mAnimationState;
    float mAnimationTime;
    long mPositionIndex;                   // current table index
    long mPrevPositionIndex;               // index before the switch
    s16 mPositionX;                        // snapped cursor x
    s16 mPositionY;                        // snapped cursor y
    PocketVec<short> *mPositionTable;
    float mCursorTime;
    Pokemon mPokemon;                      // the pokemon the cursor holds
    CAnt mAnt;                             // its model
    CAnt mBeforAnt;                        // model kept before the switch
    bool mHighlight;
    u8 _14d[3];
};

#endif
