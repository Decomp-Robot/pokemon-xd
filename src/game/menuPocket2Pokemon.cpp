#include <menuPocket2.h>

// menuPocket2GetHero -- fetch the hero and battle-trainer contexts the
// pocket menu latched in menuPocket2Work.
void menuPocket2GetHero(Hero **hero, FIGHT_TRAINER **trainer)
{
    *hero = menuPocket2Work.mHero;
    *trainer = menuPocket2Work.mTrainer;
}

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
