#include <types.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <fightTrainerAi.h>

s32 _fightTrainerAiWazaDamageFightPokemonNormal(FIGHT_TRAINER *trainer, u16 wazaDataId,
                                                FightOutPokemon *attacker, FightPokemon *defender);

extern "C" {

s32 fightTrainerAiWazaDamageFightPokemon008(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return _fightTrainerAiWazaDamageFightPokemonNormal(trainer, wazaDataId, attacker, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon007(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return _fightTrainerAiWazaDamageFightPokemonNormal(trainer, wazaDataId, attacker, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon006(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return fightTrainerAiWazaDamageFightPokemon000(trainer, attacker, wazaDataId, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon005(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return fightTrainerAiWazaDamageFightPokemon000(trainer, attacker, wazaDataId, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon004(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return fightTrainerAiWazaDamageFightPokemon000(trainer, attacker, wazaDataId, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon003(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return _fightTrainerAiWazaDamageFightPokemonNormal(trainer, wazaDataId, attacker, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon002(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return fightTrainerAiWazaDamageFightPokemon000(trainer, attacker, wazaDataId, defender);
}

s32 fightTrainerAiWazaDamageFightPokemon001(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return 0;
}

s32 fightTrainerAiWazaDamageFightPokemon000(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return _fightTrainerAiWazaDamageFightPokemonNormal(trainer, wazaDataId, attacker, defender);
}

s32 fightTrainerAiWazaDamageFightPokemonNull(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightPokemon *defender)
{
    return 0;
}

}

s32 _fightTrainerAiWazaDamageFightPokemonNormal(FIGHT_TRAINER *trainer, u16 wazaDataId,
                                                FightOutPokemon *attacker, FightPokemon *defender)
{
    const u8 criticalFlag = false;
    return fightSeqGetWazaDamageHikaePokemon(trainer, wazaDataId, attacker, defender, criticalFlag, NULL, NULL);
}
