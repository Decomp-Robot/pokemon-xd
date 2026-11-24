#include <fightSeq.h>
#include <fightTrainerAi.h>
#include <waza.h>

s32 _fightTrainerAiWazaDamageNormal(FIGHT_TRAINER *trainer, u16 wazaDataId, FightOutPokemon *attacker,
                                    FightOutPokemon *defender);

s32 fightTrainerAiWazaDamage000(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
                                            u16 wazaDataId, FightOutPokemon *defender) {
    s32 damage = _fightTrainerAiWazaDamageNormal(trainer, wazaDataId, attacker, defender);
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, defender, JOUTAI_DIVING) == 1
        && wazaDataId == WAZA_DATA_ID_SURF) {
        damage *= 2;
    }
    return damage;
}

s32 fightTrainerAiWazaDamageNull(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
                                             u16 wazaDataId, FightOutPokemon *defender) {
    return 0;
}

s32 _fightTrainerAiWazaDamageNormal(FIGHT_TRAINER *trainer, u16 wazaDataId, FightOutPokemon *attacker,
                                    FightOutPokemon *defender) {
    return fightSeqGetNromalWazaDamage(trainer, wazaDataId, attacker, defender, false, false, NULL, NULL);
}
