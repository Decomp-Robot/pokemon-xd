#include <Pokemon.h>
#include <PokemonStatus.h>
#include <fightSeq.h>
#include <fightFloor.h>
#include <hsdRandom.h>

#include <fightTarget.h>
#include <fightSide.h>
#include <pokemonDB.h>
#include <fightPokemon.h>
#include <waza.h>
#include <fightMenu.h>
#include <menuFightStatus.h>

extern "C" {
extern u8 fightSeqJibakuIndex;
extern u8 fightSeqJibakuCount;
extern u16 fightSeqGuardOddsTbl[4];
extern u8 *wazaSeqJumpTbl[];
extern u8 seqJibakuDampBlock[];
}

u8 _fightOutPokemonGetFightOutPokemonAllyOumuWazaDataIdAry(FightOutPokemon *pokemon);

enum {
    FIGHT_TARGET_ATTACKER_SIDE = 2,
    FIGHT_TARGET_ATTACKER = 0x11,
    FIGHT_TARGET_DEFENDER = 0x12,
    ZOKUSEI_GRASS = 0x0C,
    TOKUSEI_DAMP = 6,
    WAZA_PROTECT = 0xB6,
    WAZA_DETECT = 0xC5,
    WAZA_ENDURE = 0xCB,
    WAZA_INVALID = 0xFFFF,
    WAZA_SEQ_PROTECT = 0x6F,
    WAZA_SEQ_ENDURE = 0x74,
    WAZA_EFFECT_MIRROR_MOVE = 0x77,
    FIGHT_POKEMON_SLOT_COUNT = 8
};

extern "C" void WS_GUARD_SUCCESS_CHECK(void)
{
    u8 eligible = 1;
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    u16 move = fightOutPokemon_GetUseWazaDataId(attacker);
    u16 sequenceId = wazaDB_GetSeqId(move);
    u16 lastMove = fightOutPokemon_GetLastUseWazaDataId(attacker);
    u8 count = fightOutPokemon_GetSuccessCnt(attacker);
    if (lastMove != WAZA_PROTECT && lastMove != WAZA_DETECT && lastMove != WAZA_ENDURE) {
        count = 0;
        fightOutPokemon_SetSuccessCnt(attacker, 0);
    }
    if (fightFloorIsLastActionFightOutPokemon(0) == 1) {
        eligible = 0;
    }
    u16 random = HSD_Rand();
    u16 threshold = fightSeqGuardOddsTbl[count];
    if (threshold > random && eligible != 0) {
        if (sequenceId == WAZA_SEQ_PROTECT) {
            if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_GUARD) == 2) {
                fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_GUARD, 0);
            }
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
        }
        if (sequenceId == WAZA_SEQ_ENDURE) {
            if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ENDURE) == 2) {
                fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ENDURE, 0);
            }
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 1;
        }
        count++;
        if (count > 3) {
            count = 3;
        }
        fightOutPokemon_SetSuccessCnt(attacker, count);
    } else {
        fightOutPokemon_SetSuccessCnt(attacker, 0);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 2;
        fightFloor_SetAttackPokemonFightWazaJoutaiDataId(0, JOUTAI_MOVE_FAILED);
    }
    fightSeq_NextWazaSeq(1);
}


extern "C" void WS_JIBAKU_INIT(void)
{
    u8 fightType = (u8)fightFloor_GetFightType(0);
    FIGHT_WAZA *waza;
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    u32 menuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(attacker, fightType, 1);
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *defender;
    FightOutPokemon *candidate = 0;
    if (sequence[1] == 1) {
        FightOutPokemon *blocker = fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(0, TOKUSEI_DAMP, 0, 0, 0);
        if (blocker == 0) {
            Pokemon *pokemon = fightOutPokemonGetPokemonPtr(attacker);
            s32 hp = pokemon_GetHp(pokemon);
            fightWaza_SetDamage(waza, hp);
            menuFightStatusSetHP(menuId, 0);
            fightSeqJibakuCount = FIGHT_POKEMON_SLOT_COUNT;
            fightSeqJibakuIndex = 0;
            defender = 0;
            while (fightSeqJibakuIndex < fightSeqJibakuCount) {
                candidate = fightFloor_GetFightOutPokemonFromDefaultSortData(fightSeqJibakuIndex);
                if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 && attacker != candidate) {
                    defender = candidate;
                    break;
                }
                fightSeqJibakuIndex++;
            }
            if (defender != 0) {
                fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
            }
            fightSeq_NextWazaSeq(2);
        } else {
            fightFloor_SetDefensePokemonPtr(0, blocker);
            fightSeq_JumpWazaSeq(seqJibakuDampBlock);
        }
    } else if (sequence[1] == 0) {
        fightSeqJibakuCount = FIGHT_POKEMON_SLOT_COUNT;
        defender = 0;
        fightSeqJibakuIndex = 0;
        while (fightSeqJibakuIndex < fightSeqJibakuCount) {
            candidate = fightFloor_GetFightOutPokemonFromDefaultSortData(fightSeqJibakuIndex);
            if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0) {
                defender = candidate;
                break;
            }
            fightSeqJibakuIndex++;
        }
        if (defender != 0) {
            fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
        }
        fightSeq_NextWazaSeq(2);
    }
}


extern "C" void WS_JIBAKU(void)
{
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    u8 *sequence = fightSeq_GetWazaSeq();
    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(attacker);
    pokemon_SetHp(pokemon, 0);
    fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_CURRENT_HP);
    fightSeq_NextWazaSeq(1);
}


extern "C" void WS_JIBAKU_LOOP(void)
{
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *candidate;
    FightOutPokemon *defender = 0;
    u8 *branch;
    u8 mode = sequence[5];
    branch = *(u8 **)(sequence + 1);
    fightSeqJibakuIndex++;
    while (fightSeqJibakuIndex < fightSeqJibakuCount) {
        candidate = fightFloor_GetFightOutPokemonFromDefaultSortData(fightSeqJibakuIndex);
        if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 &&
            !(mode == 1 && attacker == candidate)) {
            defender = candidate;
            break;
        }
        fightSeqJibakuIndex++;
    }
    if (defender != 0) {
        fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
        fightSeq_JumpWazaSeq(branch);
    } else {
        fightSeq_NextWazaSeq(6);
    }
}


extern "C" void WS_KAIFUKU(void)
{
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    FightOutPokemon *target = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_DEFENDER, 0);
    u8 targetId = sequence[5];
    u8 *branch = *(u8 **)(sequence + 1);
    if (targetId == FIGHT_TARGET_ATTACKER) {
        target = attacker;
        fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, attacker);
    }
    s32 hp = fightOutPokemonMaxHpWaruValue(target, 2);
    fightWaza_SetDamage(waza, -hp);
    if (fightOutPokemonIsHpMantan(target) == 1) {
        fightSeq_JumpWazaSeq(branch);
    } else {
        fightSeq_NextWazaSeq(6);
    }
}


u8 _fightOutPokemonGetFightOutPokemonAllyOumuWazaDataIdAry(FightOutPokemon *pokemon)
{
    u16 move = fightOutPokemon_GetOumuWazaAllyDataId(pokemon);
    if (move != 0 && move != WAZA_SHADOW_SPECIAL) {
        return 1;
    }
    return 0;
}


extern "C" void WS_OUMU_CHECK(void)
{
    u16 moves[4];
    u8 fightType = (u8)fightFloor_GetFightType(0);
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    u16 move = fightOutPokemon_GetOumuWazaDataId(attacker);
    if (move != 0 && move != WAZA_SHADOW_SPECIAL && move != WAZA_INVALID) {
    execute:
        fightOutPokemonLoadWazaEffect(attacker, WAZA_EFFECT_MIRROR_MOVE, 1, 0);
        fightOutPokemonWazaEffect(attacker, WAZA_EFFECT_MIRROR_MOVE, 1, 1,
            fightTargetDataBiosGetBuff(fightTargetDataBiosGetPtr(FIGHT_TARGET_ATTACKER)));
        fightOutPokemonFreeWazaEffect(attacker, WAZA_EFFECT_MIRROR_MOVE, 1);
        ServerStatusFlag &= ~(u32)SEQ_ATTACK_MESSAGE_SHOWN;
        fightWazaSetUseWazaStatus(waza, move);
        FightOutPokemon *defender = fightSeqSelectDefensePokemonPtr(attacker, move, fightType, 0, 1, 1, -1);
        fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
        u16 sequenceId = wazaDB_GetSeqId(move);
        fightSeq_JumpWazaSeq(wazaSeqJumpTbl[sequenceId]);
    } else {
        u8 count = fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(attacker, moves);
        count += _fightOutPokemonGetFightOutPokemonAllyOumuWazaDataIdAry(attacker);
        if (count != 0) {
            move = moves[(u8)((u16)HSD_Rand() % count)];
            if (move != 0 && move != WAZA_SHADOW_SPECIAL) {
                goto execute;
            }
            move = fightOutPokemon_GetOumuWazaAllyDataId(attacker);
            if (move != 0 && move != WAZA_SHADOW_SPECIAL) {
                goto execute;
            }
        }
        fightOutPokemon_SetNoPressureFlag(attacker, 1);
        fightSeq_NextWazaSeq(1);
    }
}


extern "C" void WS_REFLEC(void)
{
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    void *side = fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER_SIDE, attacker);
    u16 validCount = fightFloorGetValidFightOutPokemonCount(0, 1, attacker, 1);
    if (fightSideCheckWriteJoutaiDataId(side, JOUTAI_REFLECT) != 2) {
        fightFloor_SetAttackPokemonFightWazaJoutaiDataId(0, JOUTAI_MOVE_FAILED);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
    } else {
        fightSideWriteJoutaiDataId(side, JOUTAI_REFLECT, 0);
        u16 sideMax = fightFloor_GetFightSideFightOutPokemonSideMax(0);
        if (sideMax >= 2 && validCount >= 2) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 2;
        } else {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 1;
        }
    }
    fightSeq_NextWazaSeq(1);
}


extern "C" void WS_YADORIGI(void)
{
    FightOutPokemon *defender;
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    u8 fightType = (u8)fightFloor_GetFightType(0);
    u32 sourceId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, fightType);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_DEFENDER, 0);
    if (fightWazaIsHit(waza) == 0 ||
        fightOutPokemonCheckWriteJoutaiDataId(defender, JOUTAI_LEECH_SEED) != 2) {
        fightFloor_SetAttackPokemonFightWazaJoutaiDataId(0, JOUTAI_MOVE_FAILED);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 1;
        fightSeq_NextWazaSeq(1);
    } else if (fightOutPokemonIsZokuseiDataId(defender, ZOKUSEI_GRASS) == 1) {
        fightFloor_SetAttackPokemonFightWazaJoutaiDataId(0, JOUTAI_MOVE_FAILED);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 2;
        fightSeq_NextWazaSeq(1);
    } else {
        fightOutPokemonWriteJoutaiDataId(defender, JOUTAI_LEECH_SEED, sourceId);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
        fightSeq_NextWazaSeq(1);
    }
}


extern "C" void WS_DAMAGE_CHG(void)
{
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    s32 damage = fightWaza_GetDamage(waza);
    FightOutPokemon *defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_DEFENDER, 0);
    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(defender);
    s32 maxHp = (u16)pokemon_GetMaxHp(pokemon);
    switch (sequence[1]) {
    case 0:
        damage *= -1;
        break;
    case 1: {
        damage /= 2;
        if (damage == 0) {
            damage = 1;
        }
        s32 halfHp = maxHp / 2;
        if (halfHp < damage) {
            damage = halfHp;
        }
        break;
    }
    case 2:
        damage *= 2;
        break;
    }
    fightWaza_SetDamage(waza, damage);
    fightSeq_NextWazaSeq(2);
}


extern "C" void WS_NEMURU(void)
{
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    FightOutPokemon *defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_DEFENDER, 0);
    fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, attacker);
    s32 hp = fightOutPokemonMaxHpWaruValue(attacker, 1);
    fightWaza_SetDamage(waza, -hp);
    if (fightOutPokemonIsHpMantan(attacker) == 1) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
        return;
    }
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_POISON) == 1 ||
        fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_BAD_POISON) == 1 ||
        fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_PARALYSIS) == 1 ||
        fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_BURN) == 1 ||
        fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_FREEZE) == 1) {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 1;
    } else {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
    }
    fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_SLEEP, 0);
    fightOutPokemonSetKaisuuJoutaiDataId(attacker, JOUTAI_SLEEP, 3);
    fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_CONDITION);
    fightSeq_NextWazaSeq(5);
}


extern "C" void WS_NEKODAMASHI(void)
{
    u8 *sequence = fightSeq_GetWazaSeq();
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ATTACKER, 0);
    u16 turns = fightOutPokemon_GetFightoutTurnCount(attacker);
    if (turns != 0) {
        fightSeq_NextWazaSeq(5);
    } else {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    }
}


extern "C" void WS_SAWAGU(void)
{
    u8 *sequence = fightSeq_GetWazaSeq();
    fightSeq_NextWazaSeq(1);
}
