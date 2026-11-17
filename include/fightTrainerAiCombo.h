#ifndef FIGHT_TRAINER_AI_COMBO_H
#define FIGHT_TRAINER_AI_COMBO_H

#include <fightTrainerAi.h>

#ifdef __cplusplus
extern "C" {
#endif

s16 fightAiCombo_GetValidValue(FightOutPokemon **allies, u16 allyCount, FightOutPokemon **enemies, u16 enemyCount);
s16 fightAiCombo_GetAllWazaMaxValueTable(AI_ALLWAZA_VALID_TBL *records, u8 count, s16 value,
    FIGHT_TRAINER *trainer, FightOutPokemon **allies, FightOutPokemon **enemies,
    AI_ALLWAZA_VALID_TBL *output, s8 weatherDataId);

u8 fightAiCombo_DarkWeather_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Arare_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Sunaarasi_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Amagoi_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Nihonbare_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Amagoi_Mizu_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Amagoi_Kaminari_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Nihonbare_Honoo_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_Nihonbare_Sooraabiimu_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
u8 fightAiCombo_null(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation);
s8 fightAi_getAnotherPokeNo(u8 pokeNo);

u8 fightAiCombo_Batontatti_Zikoanzi_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount);
u8 fightAiCombo_Batontatti_Nekodamasi_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount);
u8 fightAiCombo_Sukirusuwappu_Irekae_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount);
u8 fightAiCombo_Amaikaori_Hubuki_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount);
u8 fightAiCombo_null_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, FightOutPokemon **enemies,
    u16 enemyCount);

s16 fightTrainerAiSelectIrekaeDasuFightPokemon(FIGHT_TRAINER *trainer, u8 fightType, u8 selectType, FightOutPokemon *pokemon);

u8 fightAi_CanMoveAppointWaza(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 wazaDataId);
u8 fightAi_CreateFightActionAttckWaza(FightOutPokemon *pokemon, FightOutPokemon *target,
    u16 wazaDataId, s16 wazaBanme);
u8 fightAi_CreateFightActionAttckIrekae(FightOutPokemon *pokemon, s16 pokemonIndex);
FightOutPokemon *fightAi_SelectDefPoke(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount, u16 wazaDataId, s8 wazaBanme);


#ifdef __cplusplus
}
#endif

s8 _getWazaMaxValueIndex(u8 *valueAry, s16 *banmeAry, u16 wazaCount, u8 comparisonValue);

#endif
