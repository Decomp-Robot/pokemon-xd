#include <menuPocket2.h>

// menuPocket2GetCurrentTemochiPokemonSlotID -- snapshot the selected
// temochi pokemon slot ID into the current slot field of menuPocket2Work,
// returning the snapshotted ID.
u32 menuPocket2GetCurrentTemochiPokemonSlotID(void)
{
    u32 slot;

    slot = menuPocket2Work.mSelectedSlotID;
    menuPocket2Work.mCurrentSlotID = slot;
    return slot;
}
