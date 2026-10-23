#ifndef MENU_POKEMON_CURSOR_H
#define MENU_POKEMON_CURSOR_H

#include <types.h>

// 2D position vector used by the pocket-menu cursor tables.
// Only the short instantiation is observed: CMenuPokemonCursor indexes
// tables of these vectors with a word-scaled (slwi 2) subscript, and
// setPositionIndex/setPositionTableDummy read x at +0x00 and y at +0x02
// of the selected entry.
template <typename T>
struct PocketVec {
    T x;
    T y;
};

// Partial CMenuPokemonCursor declaration.
//
// Only the position-table state addressed by setPositionIndex (and its
// siblings setPositionTableDummy/setPositionTable/init) is recovered so
// far: the current table index at +0x0C, the previous index at +0x10,
// the snapped cursor position at +0x14/+0x16, and the table pointer at
// +0x18. Everything before +0x0C and from +0x1C on (including the
// Pokemon/CAnt blocks at +0x20, +0xE4 and +0x118 seen in the sibling
// routines, total size 0x150 per init) stays sized padding here;
// material layout beyond the observed prefix is unknown here.
class CMenuPokemonCursor {
public:
    void setPositionIndex(long index);

private:
    u8 _00[0x0C];
    long mPositionIndex;                   // 0x0C: current table index.
    long mPrevPositionIndex;               // 0x10: index before the switch.
    s16 mPositionX;                        // 0x14: snapped cursor x.
    s16 mPositionY;                        // 0x16: snapped cursor y.
    PocketVec<short> *mPositionTable;      // 0x18: position table.
    u8 _1C[0x134];                         // 0x1C-0x14F: remainder; unknown here.
};

#endif
