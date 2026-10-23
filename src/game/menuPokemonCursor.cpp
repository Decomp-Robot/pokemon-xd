#include <menuPokemonCursor.h>

// CMenuPokemonCursor::setPositionIndex -- switch the cursor to another
// entry of the current position table.
// The entry selected by the outgoing index is snapped into the cursor
// position first, the outgoing index is preserved as the previous index,
// and the incoming index is then stored as current. Groups with the other
// position-table helpers sharing this state (setPositionTableDummy,
// setPositionTable, init), which will join this module as they are
// recovered.
void CMenuPokemonCursor::setPositionIndex(long index)
{
    PocketVec<short> pos = mPositionTable[mPositionIndex];
    mPositionX = pos.x;
    mPositionY = pos.y;
    mPrevPositionIndex = mPositionIndex;
    mPositionIndex = index;
}
