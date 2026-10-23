#ifndef MENU_POKEMON_CURSOR_H
#define MENU_POKEMON_CURSOR_H

#include <CAnt.h>
#include <Pokemon.h>
#include <types.h>

// 2D position vector used by the pocket-menu cursor tables.
template <typename T>
struct PocketVec {
    T x;
    T y;
};

// Party-menu cursor: the pokemon it currently points at, the position
// table entry it sits on and the model state of both records.
class CMenuPokemonCursor {
public:
    void setBeforAnt(CAnt &ant);
    void setPokemon(Pokemon *pokemon);
    void setPositionTableDummy();
    void setPositionIndex(long index);
    void setPositionTable(PocketVec<short> *table, long index);

private:
    u8 _00[0x0C];
    long mPositionIndex;                   // 0x0C: current table index.
    long mPrevPositionIndex;               // 0x10: index before the switch.
    s16 mPositionX;                        // 0x14: snapped cursor x.
    s16 mPositionY;                        // 0x16: snapped cursor y.
    PocketVec<short> *mPositionTable;      // 0x18: position table.
    u8 _1C[0x04];                          // 0x1C-0x1F: remainder; unknown here.
    Pokemon mPokemon;                      // 0x20: the pokemon the cursor holds.
    CAnt mAnt;                             // 0xE4: its model.
    CAnt mBeforAnt;                        // 0x118: model kept before the switch.
    u8 _14C[0x04];                         // 0x14C-0x14F: remainder; unknown here.
};

#endif
