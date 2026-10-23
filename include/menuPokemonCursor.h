#ifndef MENU_POKEMON_CURSOR_H
#define MENU_POKEMON_CURSOR_H

#include <types.h>

// 2D position vector used by the pocket-menu cursor tables.
template <typename T>
struct PocketVec {
    T x;
    T y;
};

// Party-menu cursor, tracking an entry of the current position table.
class CMenuPokemonCursor {
public:
    void setPositionTableDummy();
    void setPositionIndex(long index);
    void setPositionTable(PocketVec<short> *table, long index);

private:
    u8 _00[0x0C];
    long mPositionIndex;
    long mPrevPositionIndex;
    s16 mPositionX;
    s16 mPositionY;
    PocketVec<short> *mPositionTable;
    u8 _1C[0x134];
};

#endif
