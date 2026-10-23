#ifndef FIGHT_SEQ_H
#define FIGHT_SEQ_H

#include <types.h>
#include <fightPokemon.h>

typedef FightOutPokemon *(*FightPokemonSelectCallback)(FightOutPokemon *pokemon,
    u16 waza, u16 fightType);

#ifdef __cplusplus
extern "C" {
#endif

u8 fightSeqGetEffectAminFlag(void);
void fightSeqSetEffectAminFlag(u8 enabled);
u8 fightSeqCheckYubiwohuruWazaDataId(u16 wazaId);

FightOutPokemon *fightSeqSelectDefensePokemonPtr(FightOutPokemon *pokemon,
    u16 waza, long fightType, FightPokemonSelectCallback selectCallback,
    u8 selectPokemon, u8 checkRedirect, s8 targetType);

#ifdef __cplusplus
}
#endif

#endif
