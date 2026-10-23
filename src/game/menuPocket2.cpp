#include <menuPocket2.h>

// menuPocket2GetLastSlotID -- return the last selected slot ID kept in
// menuPocket2Work. Groups with the other menuPocket2 helpers sharing
// menuPocket2Work (GetHero, GetCurrentTemochiPokemonSlotID,
// GetTemochiPokemon, Close/OpenPokemonMenu, ...), which will join this
// module as they are recovered.
u8 menuPocket2GetLastSlotID(void)
{
    return menuPocket2Work.mLastSlotID;
}
