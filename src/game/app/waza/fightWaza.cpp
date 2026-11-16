#include <types.h>
#include <fightWaza.h>
#include <fightSeq.h>
#include <waza.h>
#include <GSthread.h>
#include <wazaSequence.h>
#include <NullSequence.h>

extern "C" {

// Reset the statuses of the fighting move and give it the normal move result.
void fightWazaInitJoutai(FIGHT_WAZA *waza)
{
    joutaiInitAry(fightWaza_GetJoutaiPtr(waza, 0), FIGHT_WAZA_JOUTAI_COUNT);
    fightWazaWriteJoutaiDataId(waza, JOUTAI_MOVE_RESULT_NORMAL, 0);
}

void fightWazaInitLoop(FIGHT_WAZA *waza)
{
    fightWazaInitJoutai(waza);
    fightWaza_SetCritical(waza, 1);
    fightWaza_SetDamageValue(waza, 1);
}

// Reset a fighting move to no move played.
void fightWazaInit(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return;
    }
    fightWaza_SetWazaBanme(waza, -1);
    fightWaza_SetMotoWazaDataId(waza, 0);
    fightWaza_SetUseWazaDataId(waza, 0);
    fightWaza_SetTargetDataId(waza, 0);
    fightWazaInitLoop(waza);
    fightWaza_SetDamage(waza, 0);
    fightWaza_SetHitDamage(waza, 0);
    fightWaza_SetIryoku(waza, 0);
    fightWaza_SetZokusei(waza, ZOKUSEI_DATA_ID_MYSTERY);
    fightWaza_SetKaisuu(waza, 0);
    fightWaza_SetAutoMakeFlag(waza, 0);
}

// A fighting move is usable while it names a played move that is neither
// empty nor the end marker of the normal move table, and has a target.
bool fightWazaCheckValid(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    if ((u16)fightWaza_GetMotoWazaDataId(waza) == 0) {
        return 0;
    }
    if ((u16)fightWaza_GetMotoWazaDataId(waza) == WAZA_DATA_ID_NORMAL_END) {
        return 0;
    }
    if ((u16)fightWaza_GetUseWazaDataId(waza) == 0) {
        return 0;
    }
    if ((u16)fightWaza_GetUseWazaDataId(waza) == WAZA_DATA_ID_NORMAL_END) {
        return 0;
    }
    return fightWaza_GetTargetDataId(waza) != 0;
}

// Load the played move's power and type into the fighting move.
void fightWazaSetUseWazaStatus(FIGHT_WAZA *waza, u16 move)
{
    fightWaza_SetUseWazaDataId(waza, move);
    fightWaza_SetIryoku(waza, wazaDB_GetIryoku(move));
    fightWaza_SetZokusei(waza, wazaDB_GetZokuseiDataId(move));
}

// Fill a fighting move for the move played: its stock move slot, the played
// move, its target and the move's power and type.
void fightWazaCreate(FIGHT_WAZA *waza, s8 stockWaza, u16 wazaDataId, u16 targetDataId, u8 autoMakeFlag)
{
    if (waza == 0) {
        return;
    }
    fightWazaInit(waza);
    fightWaza_SetWazaBanme(waza, stockWaza);
    fightWaza_SetTargetDataId(waza, targetDataId);
    fightWaza_SetMotoWazaDataId(waza, wazaDataId);
    fightWazaSetUseWazaStatus(waza, wazaDataId);
    fightWaza_SetAutoMakeFlag(waza, autoMakeFlag);
}

void fightWazaInitJoutaiDataId(FIGHT_WAZA *waza, u16 joutaiDataId)
{
    if (joutaiDataBiosGetStatus(joutaiDataId) == WAZA_FIELD_JOUTAI) {
        joutaiInitJoutaiDataId(waza, joutaiDataId);
    }
}

u8 fightWazaIsJoutaiDataId(FIGHT_WAZA *waza, u16 joutaiDataId)
{
    if (joutaiDataBiosGetStatus(joutaiDataId) != WAZA_FIELD_JOUTAI) {
        return 0;
    }
    return joutaiIsJoutaiDataId(waza, joutaiDataId);
}

// A move result cancels itself out when it reports a super effective and a not
// very effective hit at the same time.
u8 fightWazaIsJoutaiSousai(FIGHT_WAZA *waza)
{
    if (fightWazaIsJoutaiDataId(waza, JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE) == 1) {
        if (fightWazaIsJoutaiDataId(waza, JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE) == 1) {
            return 1;
        }
    }
    return 0;
}

u8 fightWazaCheckWriteJoutaiDataId(FIGHT_WAZA *waza, u16 joutaiDataId)
{
    if (joutaiDataBiosGetStatus(joutaiDataId) != WAZA_FIELD_JOUTAI) {
        return 0;
    }
    return joutaiCheckWriteJoutaiDataId(waza, joutaiDataId);
}

void fightWazaWriteJoutaiDataId(FIGHT_WAZA *waza, u16 joutaiDataId, u16 targetId)
{
    if (joutaiDataBiosGetStatus(joutaiDataId) == WAZA_FIELD_JOUTAI) {
        joutaiWriteJoutaiDataId(waza, joutaiDataId, targetId);
    }
}

// The move hits unless it missed, had no effect, or could not be used at all.
u8 fightWazaIsHit(FIGHT_WAZA *waza)
{
    if (fightWazaIsJoutaiDataId(waza, JOUTAI_MOVE_RESULT_MISSED) == 1) {
        return 0;
    }
    if (fightWazaIsJoutaiDataId(waza, JOUTAI_MOVE_RESULT_NO_EFFECT) == 1) {
        return 0;
    }
    if (fightWazaIsJoutaiDataId(waza, JOUTAI_MOVE_RESULT_FAILED) == 1) {
        return 0;
    }
    return 1;
}

// A move is mixed when multiple move-result statuses are present.
u8 fightWazaIsMix(FIGHT_WAZA *waza)
{
    u16 mixJoutaiDataId[7] = { JOUTAI_MOVE_RESULT_SUPER_EFFECTIVE, JOUTAI_MOVE_RESULT_NOT_VERY_EFFECTIVE,
        JOUTAI_MOVE_RESULT_NO_EFFECT, JOUTAI_MOVE_RESULT_ONE_HIT_KO, JOUTAI_MOVE_RESULT_FAILED,
        JOUTAI_MOVE_RESULT_ENDURED, JOUTAI_MOVE_RESULT_ENDURED_BY_ITEM };
    u16 i;
    u16 count;

    count = 0;
    for (i = 0; i < 7; i++) {
        if (fightWazaIsJoutaiDataId(waza, mixJoutaiDataId[i]) == 1) {
            count++;
        }
    }
    if (count >= 2) {
        return 1;
    }
    return 0;
}

// Run the move's effect callbacks. Moves whose database entry carries none
// fall back to the null callbacks.
void fightWazaDoEffectFunc(u16 move, FightOutPokemon *attackPokemon, FightOutPokemon *defensePokemon,
    u16 invocation)
{
    FightWazaWzxTypeFunc typeFunc;
    FightWazaWzxVariationFunc variationFunc;
    u8 variation;

    typeFunc = wazaDB_GetFightWazaWzxTypeFuncPtr(move);
    if (typeFunc == 0) {
        typeFunc = fightWazaWzxTypeFuncNull;
    }
    variationFunc = wazaDB_GetFightWazaWzxVariationFuncPtr(move);
    if (variationFunc == 0) {
        variationFunc = fightWazaWzxVariationFuncNull;
    }
    variation = variationFunc(move, attackPokemon, defensePokemon, invocation);
    typeFunc(move, attackPokemon, defensePokemon, invocation, variation);
}

// Play the move's effect sequence on the stage of a battler. The move's wzx
// data id selects the sequence; wait 1 holds the thread until it finishes.
void fightWazaDoEffect(NullSequence *stage, u16 move, u16 category, u8 wait)
{
    WazaSequence *sequence;
    u32 sequenceId;

    sequenceId = wazaDB_GetWazawzxdataId(move);
    sequence = Sequence_GetWaza(stage, sequenceId, category);
    if (sequence == 0) {
        return;
    }
    WazaSequence_Start(sequence, 1);
    if (wait == 1) {
        while (WazaSequence_IsActive(sequence)) {
            GSthreadSwitch();
        }
        WazaSequence_Free(sequence);
    }
}

}
