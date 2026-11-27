#ifndef FIGHTPOKEMON_H
#define FIGHTPOKEMON_H

#include <Pokemon.h>
#include <fightOutPokemon.h>
#include <fightAction.h>
#include <joutai.h>
#include <types.h>
#include <bool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;
struct FIGHT_SIDE;
struct NullSequence;
struct MENU_POKEMON_STATUS;

enum {
    FIGHT_POKEMON_AI_DAMAGE_MAX = 64
};

// A Pokemon of a trainer's party: the Pokemon the record was made from, the
// working copy the fight runs on, the status it carries into the fight, the
// fight entry it is booked with and the damage the trainer AI has recorded
// against each of its attacks.
struct FightPokemon {
    Pokemon *mMotoPokemon;    // Pokemon the record was made from.
    Pokemon mPokemonBuff;     // working copy the fight runs on.
    _JOUTAI mFightJoutai[1];  // status carried into the fight.
    s16 mEntryId;             // fight entry the Pokemon is booked with.
    // Flags the fight sets on the Pokemon; all cleared when the record is
    // initialised.
    u8 mCatchEntryFlag;
    u8 mLevelUpFlag;
    u8 mDarkOutFlag;
    u8 mHokakuFlag;
    u8 _0DE[34];
    s32 mAiDamage[FIGHT_POKEMON_AI_DAMAGE_MAX][2];     // damage the AI has recorded per attack.
};

enum FIGHT_AI_FIX_WAZA {
    FIGHT_AI_FIX_WAZA_CLEAR,
    FIGHT_AI_FIX_WAZA_SLOT_1,
    FIGHT_AI_FIX_WAZA_SLOT_2,
    FIGHT_AI_FIX_WAZA_SLOT_3,
    FIGHT_AI_FIX_WAZA_SLOT_4,
    FIGHT_AI_FIX_WAZA_FREE,
    FIGHT_AI_FIX_WAZA_DONE,
    FIGHT_AI_FIX_WAZA_REPEAT
};

enum FIGHT_AI_FIX_DEF {
    FIGHT_AI_FIX_DEF_CLEAR,
    FIGHT_AI_FIX_DEF_SELF,
    FIGHT_AI_FIX_DEF_PARTNER,
    FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_DEF_OPPONENT_SECOND,
    FIGHT_AI_FIX_DEF_FREE
};

Pokemon *fightPokemonGetPokemonPtr(FightPokemon *pokemon);
void fightPokemonSetBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId, u32 buff);
u32 fightPokemonGetBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId);
void fightPokemonCopyJoutaiDataId(FightPokemon *destination, FightPokemon *source, u16 dataId);
void fightPokemonSetIdBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId, u16 idBuff);
u16 fightPokemonGetIdBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId);
u16 fightPokemonGetUserFightTargetIdJoutaiDataId(FightPokemon *pokemon, u16 dataId);
void fightPokemonSetKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId, s8 kaisuu);
void fightPokemonSetNowKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId, s8 nowKaisuu);
s8 fightPokemonGetNowKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId);
s8 fightPokemonGetKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId);
void fightPokemonSetCountAsJoutaiDataId(FightPokemon *pokemon, u16 dataId, s16 count);
s16 fightPokemonGetCountAsJoutaiDataId(FightPokemon *pokemon, u16 dataId);
void fightPokemonWriteJoutaiDataId(FightPokemon *pokemon, u16 dataId, u16 targetId);
u8 fightPokemonCheckWriteJoutaiDataId(FightPokemon *pokemon, u16 dataId);
u8 fightPokemonIsJoutaiDataId(FightPokemon *pokemon, u16 dataId);
void fightPokemonInitJoutaiDataId(FightPokemon *pokemon, u16 dataId);
Pokemon *fightOutPokemonGetPokemonPtr(FightOutPokemon *pokemon);
u8 figthPokemonGetLevel(FightPokemon *pokemon);
u8 fightPokemonCheckValid(FightPokemon *pokemon);
u8 fightPokemonCheckFightOut(FightPokemon *pokemon);
u8 fightPokemonCheckMotoFightPokemon(FightPokemon *pokemon, FightOutPokemon *fightoutPokemon);
u16 fightPokemonGetSoubiItemDataId(FightPokemon *pokemon);
u8 fightOutPokemonIsUseHensinBuff(FightOutPokemon *pokemon);
u16 fightOutPokemonGetTokuseiDataId(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckCanOutOkWazaBanme(FightOutPokemon *pokemon, u16 wazaIndex, u8 checkFlag, u16 *out);
u8 fightOutPokemonCheckFightActionWazaSelect(FightOutPokemon *pokemon, u8 checkFlag);
void fightOutPokemonGetRndStatus(FightOutPokemon *pokemon, u32 *random1, u32 *random2);
u16 fightOutPokemonMaxHpWaruValue(FightOutPokemon *pokemon, u16 divisor);
u16 fightOutPokemonNowHpWaruValue(FightOutPokemon *pokemon, u16 divisor);
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
u8 fightOutPokemonCheckIrekaeReserveFightPokemon(FightOutPokemon *fightoutPokemon,
                                                FightPokemon *pokemon);
u8 fightOutPokemonIsFightActionUseItemKind(FightOutPokemon *pokemon, u8 itemKind,
                                             u8 matchKind, u8 unused);
u16 fightOutPokemonGetFightActionUseItemDataId(FightOutPokemon *pokemon);
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
void fightOutPokemonLoadWazaEffect(FightOutPokemon *pokemon, u16 wazaId, u8 slot, u8 variation);
void fightOutPokemonWazaEffect(FightOutPokemon *pokemon, u16 wazaId, u8 slot, bool wait, s32 statusMenuSelector);
void fightOutPokemonFreeWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot);
u8 fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(FightOutPokemon *pokemon, u16 *moves);
void fightOutPokemonCreateFightPokemonEnemyAryEnemySideAll(FightOutPokemon *pokemon, struct FIGHT_SIDE *side);
void fightOutPokemonInitFightPokemonEnemyAry(FightOutPokemon *pokemon, FightOutPokemon *target);
struct FightOutPokemonEnemy *fightOutPokemonCreateFightPokemonEnemyAry(FightOutPokemon *pokemon,
                                                                       FightOutPokemon *target);

void fightOutPokemonSetTokuseiDataId(FightOutPokemon *pokemon, u16 abilityId);
u16 fightOutPokemonGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);
void fightOutPokemonSetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId);
void fightOutPokemonInitJoutaiDataId(FightOutPokemon *pokemon, u16 statusId);
u32 fightOutPokemonGetBuffJoutaiDataId(FightOutPokemon *pokemon, u16 id);

s16 fightOutPokemonGetCountAsJoutaiDataId(FightOutPokemon *pokemon, u16 statusId);
u8 figthOutPokemonGetLevel(FightOutPokemon *pokemon);
enum { FIGHT_POKEMON_RARE_AUTO = -1 };
struct NullSequence *fightOutPokemonCreateSequence(FightOutPokemon *pokemon, u16 tenkou, s8 rare);
u16 figthOutPokemonGetPokemonDataId(FightOutPokemon *pokemon);
FIGHT_ACTION *fightOutPokemonCreateFightActionAttackWaza(FightOutPokemon *pokemon, struct FIGHT_ACTION_DATA_dd_ *motoActionData, u16 actionKind, s32 bufferSelector, struct FIGHT_ACTION_DATA_dd_ *wazaActionData, u32 wazaId, u16 targetId, s8 stockWaza, u8 autoMakeFlag);
void fightOutPokemonCreate(FightOutPokemon *fightOutPokemon, FightPokemon *pokemon, struct NullSequence *sequence);
u8 fightOutPokemonIsFightActionAttackWazaOut(FightOutPokemon *pokemon, u16 wazaDataId, void *target);
FIGHT_ACTION *fightOutPokemonCreateFightAction(FightOutPokemon *pokemon, FIGHT_ACTION_DATA_dd_ *motoActionData, u16 kind, s32 bufferSelector, FIGHT_ACTION_DATA_dd_ *actionData, u32 buffDataId);
void fightOutPokemonDarkPokemonKaijoEffect(FightOutPokemon *pokemon, u8 wokeFromSleep, u8 endedHyperMode, u8 raisedAccuracy, s32 msgId, s32 mode);
u16 fightOutPokemon_GetTokuseiDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetAbicntAverage(FightOutPokemon *pokemon);
void fightOutPokemon_SetAbicntAverage(FightOutPokemon *pokemon, u8 abicnt);
u8 fightOutPokemonIsGcHeroFightOutPokemon(FightOutPokemon *pokemon);
u8 fightOutPokemonIsHinsi(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetFixAiWaza(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetFixAiDef(FightOutPokemon *pokemon);
void fightOutPokemon_SetFixAiWaza(FightOutPokemon *pokemon, u8 fixAiWaza);
void fightOutPokemon_SetFixAiDef(FightOutPokemon *pokemon, u8 fixAiDef);
u8 fightOutPokemonCheckValid(FightOutPokemon *pokemon);
u8 fightOutPokemonIsAlly(FightOutPokemon *a, FightOutPokemon *b);
u16 fightOutPokemonIsJoutaiKie(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckNoAttackFlag(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetVanishoffFlag(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetKonranMyselfAttackFlag(FightOutPokemon *pokemon);
void fightOutPokemonInitJoutaiKie(FightOutPokemon *pokemon);
void fightOutPokemon_SetVanishoffFlag(FightOutPokemon *pokemon, u8 flag);
void fightOutPokemon_SetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon, u16 targetId);
void fightOutPokemon_SetLastSelectWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetLastUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetSketchWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemonSetIdBuffJoutaiDataId(FightOutPokemon *pokemon, u16 dataId, u16 wazaDataId);
void fightOutPokemonSetVisibility(FightOutPokemon *pokemon, s32 visibility);
void fightOutPokemonSetOumuWazaDataId(FightOutPokemon *defender, FightOutPokemon *attacker, u16 wazaDataId);
void fightPokemonSetNonTrainerPokemonIsActiveFlag(u8 flag);

u16 fightOutPokemon_GetStockItemDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetStockItemDataId(FightOutPokemon *pokemon, u16 item);
u16 fightOutPokemonDoItemSoubi(FightOutPokemon *pokemon, u16 item, u8 equip, u8 recordUsedItem);
struct NullSequence *fightOutPokemon_GetSequencePtr(FightOutPokemon *pokemon);
void fightOutPokemonCopyAllAbiCnt(FightOutPokemon *copy, FightOutPokemon *source);
void fightOutPokemonSetBuffJoutaiDataId(FightOutPokemon *pokemon, u16 dataId, u32 buff);
u16 fightOutPokemonGetUserFightTargetIdJoutaiDataId(FightOutPokemon *pokemon, u16 status);
void fightOutPokemonToMenuPokemonStatus(FightOutPokemon *pokemon,
                                        struct MENU_POKEMON_STATUS *status);

#ifdef __cplusplus
}
#endif

#endif
