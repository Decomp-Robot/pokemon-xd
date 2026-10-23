#ifndef FIGHTMENU_H
#define FIGHTMENU_H

#include <fightTrainer.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void fightMenuFightOutPokemonRenewStatusMenu(FightOutPokemon *pokemon,
    u16 fightType, u8 update);
void fightMenuFightTrainerRenewStatusMenu(FIGHT_TRAINER *trainer, u16 fightType);

#ifdef __cplusplus
}
#endif

#endif
