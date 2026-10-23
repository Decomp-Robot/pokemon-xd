#include <menuPokemonCursor.h>

extern PocketVec<short> sDummyPosition;

void CMenuPokemonCursor::setPositionTableDummy()
{
    PocketVec<short> *dummy = &sDummyPosition;
    long zero = 0;
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    *dummy = pos;
    mPositionTable = dummy;
    mPositionIndex = zero;
    mPrevPositionIndex = zero;
}

void CMenuPokemonCursor::setPositionIndex(long index)
{
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    mPositionX = pos.x;
    mPositionY = pos.y;
    mPrevPositionIndex = mPositionIndex;
    mPositionIndex = index;
}
