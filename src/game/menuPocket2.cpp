#include <menuPocket2.h>

// menuPocket2GetHero -- fetch the hero/trainer contexts latched in
// menuPocket2Work for temochi (party) access. The hero pointer at +0x0C
// feeds heroGetStatus (see menuPocket2GetTemochiPokemon) and
// Hero::getHeroStyle; the trainer pointer at +0x08 feeds
// fightTrainer_GetFightPokemonPtr, menuPokemonOpenItemUse and
// fightTrainerIsSelectedItemBall. Groups with the other menuPocket2
// helpers sharing menuPocket2Work (GetCurrentTemochiPokemonSlotID,
// GetTemochiPokemon, Close/OpenPokemonMenu, GetLastSlotID, ...), which
// will join this module as they are recovered.
//
// Note: this translation unit builds with the peephole optimizer disabled
// (menuPocket, GC/1.3.2 -opt nopeephole), matching the sibling pocket
// modules.
void menuPocket2GetHero(Hero **hero, FIGHT_TRAINER **trainer)
{
    *hero = menuPocket2Work.mHero;
    *trainer = menuPocket2Work.mTrainer;
}
