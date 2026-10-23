#ifndef MENU_POCKET2_H
#define MENU_POCKET2_H

#include <types.h>

class Hero;
struct FIGHT_TRAINER;

// Item-menu (pocket2) work (bss menuPocket2Work, 0x54 bytes).
// This header declares only what menuPocket2GetHero observes: the trainer
// context at +0x08 and the hero context at +0x0C. The +0x08 word is the
// FIGHT_TRAINER passed to fightTrainer_GetFightPokemonPtr (see
// menuPocket2GetTemochiPokemon), to menuPokemonOpenItemUse (see
// menuPocket2OpenPokemonMenu) and to fightTrainerIsSelectedItemBall (see
// the menuUseItemBattleBall caller below); the +0x0C word is the Hero
// passed to heroGetStatus and later used with Hero::getHeroStyle.
// Everything else in the 0x54-byte block is unobserved by this
// translation unit, so it stays sized padding; material layout beyond
// these two words remains unknown here.
struct MenuPocket2Work {
    u8 _00[0x08];
    FIGHT_TRAINER *mTrainer; // 0x08: trainer context for temochi access.
    Hero *mHero;             // 0x0C: hero context for temochi access.
    u8 _10[0x44];            // 0x10-0x53: remainder of the block; unknown here.
};

extern MenuPocket2Work menuPocket2Work;

#ifdef __cplusplus
extern "C" {
#endif

void menuPocket2GetHero(Hero **hero, FIGHT_TRAINER **trainer);

#ifdef __cplusplus
}
#endif

#endif
