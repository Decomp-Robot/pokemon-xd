#ifndef FIGHTMENU_H
#define FIGHTMENU_H

#include <fightTrainer.h>
#include <bool.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
void fightMsgctrlSetValue(u32 selector, unsigned long value);
void fightMenuCloseMsg(void);
void fightMenuAllFightTrainerCloseStatusMenu(u8 waitForClose);
void fightMenuAllFightOutPokemonCloseStatusMenu(u8 waitForClose);
u8 fightMenuOpenMsg(s32 msgId);
void fightMenuFightOutPokemonCloseStatusMenu(FightOutPokemon *pokemon, u16 fightType, bool unused);
u32 fightMenuGetFightOutPokemonPtrToStatusMenuId(FightOutPokemon *pokemon, u16 fightType, u32 lookupMode);

void fightMenuFightOutPokemonRenewStatusMenu(FightOutPokemon *pokemon,
    u16 fightType, u8 update);
void fightMenuFightTrainerRenewStatusMenu(FIGHT_TRAINER *trainer, u16 fightType);

void fightMenuFightOutPokemonOpenStatusMenu(FightOutPokemon *pokemon, u16 fightType, bool showDarkStatus);
void fightMenuFightOutPokemonCloseStatusMenuAnotherMine(u32 statusMenuId, FightOutPokemon *pokemon, u16 fightType, u8 waitForClose, s32 lookupMode);

#ifdef __cplusplus
}
#endif

#endif
