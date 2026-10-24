#ifndef FIGHTPOKEMON_H
#define FIGHTPOKEMON_H

#include <Pokemon.h>
#include <fightOutPokemon.h>
#include <fightAction.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;
struct NullSequence;

Pokemon *fightPokemonGetPokemonPtr(FightPokemon *pokemon);
Pokemon *fightOutPokemonGetPokemonPtr(FightOutPokemon *pokemon);
u8 figthPokemonGetLevel(FightPokemon *pokemon);
u16 fightPokemonGetSoubiItemDataId(FightPokemon *pokemon);
u8 fightOutPokemonIsUseHensinBuff(FightOutPokemon *pokemon);
u16 fightOutPokemonGetTokuseiDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonMaxHpWaruValue(FightOutPokemon *pokemon, u16 divisor);
u8 fightOutPokemonIsJoutaiDataId(FightOutPokemon *pokemon, u16 status);
void fightOutPokemonInitOneTurn(FightOutPokemon *pokemon);
void fightOutPokemonInitOneSelfTurn(FightOutPokemon *pokemon);
void fightOutPokemonInitJoutaiKeep(FightOutPokemon *pokemon);
s8 fightOutPokemonGetNowKaisuuJoutaiDataId(FightOutPokemon *pokemon, u16 id);
s8 fightOutPokemonGetKaisuuJoutaiDataId(FightOutPokemon *pokemon, u16 id);
void fightOutPokemonSetNowKaisuuJoutaiDataId(FightOutPokemon *pokemon, u16 id, s8 count);
s32 fightOutPokemonGetJoutaiMigawariHp(FightOutPokemon *pokemon);
u8 fightOutPokemonIsZokuseiDataId(FightOutPokemon *pokemon, u16 type);
u8 fightOutPokemonIsHpMantan(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckFightOut(FightOutPokemon *pokemon);
u16 *fightOutPokemonGetNicknamePtr(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckWriteJoutaiDataId(FightOutPokemon *pokemon, u16 status);
void fightOutPokemonWriteJoutaiDataId(FightOutPokemon *pokemon, u16 status, u16 targetId);
u16 fightOutPokemonGetIdBuffJoutaiDataId(FightOutPokemon *pokemon, u16 id);
void fightOutPokemon_SetHitWazaDataId(FightOutPokemon *pokemon, u16 dataId);
void fightOutPokemon_SetHitWazaZokuseiDataId(FightOutPokemon *pokemon, u16 dataId);
u16 fightOutPokemonGetSoubiItemDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonGetSoubiItemSoubiDataId(FightOutPokemon *pokemon);
s32 figthOutPokemonGetSoubiItemBuff(FightOutPokemon *pokemon);
u8 fightOutPokemonIsJoutaiNormal(FightOutPokemon *pokemon);
void fightOutPokemonResetSeqStatus(FightOutPokemon *pokemon, u16 status);
u8 fightOutPokemonIsNokoriHpFollowing(FightOutPokemon *pokemon, u16 divisor);
s8 fightOutPokemonGetTasteLike(FightOutPokemon *pokemon, u16 taste);
void fightOutPokemonSetKaisuuJoutaiDataId(FightOutPokemon *pokemon, u16 status, u32 count);
void fightOutPokemonLoadWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot, u32 mode);
void fightOutPokemonWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot, u32 mode, s32 statusMenuSelector);
void fightOutPokemonFreeWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot);
u8 fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(FightOutPokemon *pokemon, u16 *moves);

void fightOutPokemonSetTokuseiDataId(FightOutPokemon *pokemon, u16 abilityId);
u16 fightOutPokemonGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);
void fightOutPokemonSetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId);
void fightOutPokemonInitJoutaiDataId(FightOutPokemon *pokemon, u16 statusId);
u32 fightOutPokemonGetBuffJoutaiDataId(FightOutPokemon *pokemon, u16 id);

u16 figthOutPokemonGetPokemonDataId(FightOutPokemon *pokemon);
FIGHT_ACTION *fightOutPokemonCreateFightActionAttackWaza(FightOutPokemon *pokemon, struct FIGHT_ACTION_DATA_dd_ *motoActionData, u16 actionKind, s32 bufferSelector, struct FIGHT_ACTION_DATA_dd_ *wazaActionData, u32 wazaId, u16 targetId, s8 stockWaza, u8 autoMakeFlag);
void fightOutPokemonDarkPokemonKaijoEffect(FightOutPokemon *pokemon, u8 wokeFromSleep, u8 endedHyperMode, u8 raisedAccuracy, s32 msgId, s32 mode);
u16 fightOutPokemon_GetTokuseiDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetAbicntAverage(FightOutPokemon *pokemon);
void fightOutPokemon_SetAbicntAverage(FightOutPokemon *pokemon, u8 abicnt);
u8 fightOutPokemonIsGcHeroFightOutPokemon(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckValid(FightOutPokemon *pokemon);
u8 fightOutPokemonIsAlly(FightOutPokemon *a, FightOutPokemon *b);
u16 fightOutPokemonIsJoutaiKie(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckNoAttackFlag(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetVanishoffFlag(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetTameWazaFlag(FightOutPokemon *pokemon);
void fightOutPokemonInitJoutaiKie(FightOutPokemon *pokemon);
void fightOutPokemon_SetVanishoffFlag(FightOutPokemon *pokemon, u8 flag);
void fightOutPokemon_SetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon, u16 targetId);
void fightOutPokemon_SetLastSelectWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetLastUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetSketchWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemonSetIdBuffJoutaiDataId(FightOutPokemon *pokemon, u16 dataId, u16 wazaDataId);
void fightOutPokemonSetVisibility(FightOutPokemon *pokemon, s32 visibility);
void fightOutPokemonSetOumuWazaDataId(FightOutPokemon *defender, FightOutPokemon *attacker, u16 wazaDataId);
u16 fightOutPokemon_GetStockItemDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetStockItemDataId(FightOutPokemon *pokemon, u16 item);
u16 fightOutPokemonDoItemSoubi(FightOutPokemon *pokemon, u16 item, u8 equip, u8 recordUsedItem);
struct NullSequence *fightOutPokemon_GetSequencePtr(FightOutPokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
