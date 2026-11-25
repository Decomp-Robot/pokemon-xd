#include <GSmem.h>
#include <Pokemon.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightPokemonBios.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <joutai.h>
#include <fightTrainer.h>
#include <itemData.h>
#include <itemParam.h>
#include <itemUse2Pokemon.h>
#include <libc/memory.h>
#include <pokemonBios.h>
#include <pokemonDataBios.h>
#include <pokemonDB.h>
#include <pokemonEvolution.h>
#include <waza.h>

enum ItemUse2PokemonRule {
    ITEMUSE2POKEMON_PPUP_MIN_PP = 5,
    ITEMUSE2POKEMON_PP_ALL_SLOTS = 255,
    ITEMUSE2POKEMON_GUARD_DURATION = 5,
    ITEMUSE2POKEMON_DARK_LEVEL_MIN = 3,
    ITEMUSE2POKEMON_ITEM_HAPPINESS_SCALE = 150,
    ITEMUSE2POKEMON_DARK_HAPPINESS_SCALE = 80,
    ITEMUSE2POKEMON_HAPPINESS_SCALE_MAX = 100
};

enum ITEMUSE2POKEMON_FRIEND_BAND {
    ITEMUSE2POKEMON_FRIEND_THRESHOLD_1 = 100,
    ITEMUSE2POKEMON_FRIEND_THRESHOLD_2 = 200,
    ITEMUSE2POKEMON_FRIEND_THRESHOLD_MAX = 65535
};

enum ITEMUSE2POKEMON_HP_MODE {
    ITEMUSE2POKEMON_HP_SPECIAL = 253,
    ITEMUSE2POKEMON_HP_HALF = 254,
    ITEMUSE2POKEMON_HP_ALL = 255
};

s16 itemUse2PokemonSimulation(ITEMUSE2POKEMON_LOG1 *log, Pokemon *pokemon,
    FightPokemon *fightPokemon, u16 itemID, u8 moveIndex) {
    FIGHT_FLOOR *saved;
    FIGHT_FLOOR *floor;
    s16 result;

    floor = (FIGHT_FLOOR *)fightFloorGetNowPtr();
    if (floor == 0) {
        return 0;
    }
    saved = (FIGHT_FLOOR *)GSmemAlloc(sizeof(FIGHT_FLOOR));
    memcpy(saved, floor, sizeof(FIGHT_FLOOR));
    result = itemUse2Pokemon(log, pokemon, fightPokemon, itemID, moveIndex);
    memcpy(floor, saved, sizeof(FIGHT_FLOOR));
    GSmemFree(saved);
    return result;
}

static inline FightOutPokemon *itemUseGetFightOut(FightPokemon *fightPokemon) {
    FIGHT_TRAINER *trainer;

    if (fightPokemon == 0) {
        return 0;
    }
    trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
    return fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
}

static inline void itemUseNotify(FightPokemon *fightPokemon, s32 buffID) {
    FightOutPokemon *fightOutPokemon;

    fightOutPokemon = itemUseGetFightOut(fightPokemon);
    if (fightOutPokemon != 0) {
        fightSeqCheckSetMotoPokemonToHensinBuff(fightOutPokemon, buffID);
    }
}

static inline void itemUseAddLog(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    ITEMUSE2POKEMON_EFFECT effect, s16 value, s16 value2) {
    s16 index;

    index = *count;
    if (index < ITEMUSE2POKEMON_LOG_MAX) {
        log[index].mEffect = effect;
        log[index].mValue = value;
        log[index].mValue2 = value2;
        *count = index + 1;
    }
}

s16 friendXUp(Pokemon *pokemon, FightPokemon *fightPokemon, s8 change,
    u16 lower, u16 upper) {
    s32 adjusted;
    s32 newFriend;
    s32 oldFriend;
    s32 effective;

    if (pokemonIsDarkPokemon(pokemon) && pokemonGetDarkPokemonLevel(pokemon)
        < ITEMUSE2POKEMON_DARK_LEVEL_MIN) {
        return 0;
    }

    if (pokemonIsDarkPokemon(pokemon)) {
        oldFriend = pokemon_GetPoolFriend(pokemon);
    } else {
        oldFriend = pokemonBiosGetFriend(pokemon);
    }
    if (pokemonIsDarkPokemon(pokemon)) {
        effective = pokemon_GetPoolFriend(pokemon);
    } else {
        effective = 0;
    }
    effective += pokemonBiosGetFriend(pokemon);
    if (effective < lower || effective >= upper) {
        return 0;
    }

    adjusted = change;
    if (adjusted > 0) {
        ItemData *item = itemDataBiosGetPtr(pokemonBiosGetItemDataId(pokemon));
        u16 soubiDataId;
        if (item == 0) {
            soubiDataId = 0;
        } else {
            soubiDataId = itemDataBiosGetItemSoubiDataId(item);
        }
        if (soubiDataId == ITEM_SOUBI_DATA_FRIEND_UP) {
            adjusted = adjusted * ITEMUSE2POKEMON_ITEM_HAPPINESS_SCALE
                / ITEMUSE2POKEMON_HAPPINESS_SCALE_MAX;
        }
        if (pokemonBiosGetCatchBallId(pokemon) == ITEM_LUXURY_BALL) {
            adjusted++;
        }
    }
    if (pokemonIsDarkPokemon(pokemon)) {
        adjusted = adjusted * ITEMUSE2POKEMON_DARK_HAPPINESS_SCALE
            / ITEMUSE2POKEMON_HAPPINESS_SCALE_MAX;
    }

    newFriend = oldFriend + adjusted;
    if (newFriend > POKEMON_FRIEND_LIMIT) {
        newFriend = POKEMON_FRIEND_LIMIT;
    }
    if (newFriend < 0) {
        newFriend = 0;
    }

    if (pokemonIsDarkPokemon(pokemon)) {
        pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POOL_FRIEND, 0,
            newFriend);
        itemUseNotify(fightPokemon, POKEMON_STATUS_FIELD_POOL_FRIEND);
    } else {
        pokemonBiosSetFriend(pokemon, newFriend);
        itemUseNotify(fightPokemon, POKEMON_STATUS_FIELD_FRIEND_LEVEL);
    }
    return newFriend - oldFriend;
}

void hpRecover(ITEMUSE2POKEMON_LOG1 *log, s16 *count, Pokemon *pokemon,
    u8 amount, bool reviveFlag, u16 specialAmount, FightPokemon *fightPokemon) {
    u16 hp;
    u16 maxHp;
    u16 newHp;
    u16 recoverAmount;
    u8 revived;
    u16 value;

    revived = 0;
    hp = pokemonBiosGetHp(pokemon);
    maxHp = pokemonBiosGetMaxHp(pokemon);
    if (hp >= maxHp) {
        return;
    }

    switch (amount) {
    case ITEMUSE2POKEMON_HP_ALL:
        recoverAmount = maxHp;
        break;
    case ITEMUSE2POKEMON_HP_HALF:
        recoverAmount = maxHp / 2;
        if (recoverAmount == 0) {
            recoverAmount = 1;
        }
        break;
    case ITEMUSE2POKEMON_HP_SPECIAL:
        recoverAmount = specialAmount;
        break;
    default:
        recoverAmount = amount;
        break;
    }
    if (recoverAmount == 0) {
        return;
    }

    if (hp == 0) {
        if (reviveFlag == 0) {
            return;
        }
        revived = 1;
    } else if (reviveFlag != 0) {
        return;
    }

    newHp = hp + recoverAmount;
    if (newHp > maxHp) {
        newHp = maxHp;
    }
    pokemonBiosSetHp(pokemon, newHp);

    itemUseNotify(fightPokemon, POKEMON_STATUS_HP);

    if (revived != 0) {
        itemUseAddLog(log, count, ITEMUSE_EFFECT_RECOVERED_FROM_FAINT, 0, 0);
    }
    value = newHp - hp;
    itemUseAddLog(log, count, ITEMUSE_EFFECT_HP_RECOVERED, value, 0);
}

static inline ITEM_PARAM *itemUseGetParam(u16 itemID) {
    ItemData *data;
    u8 effect;

    data = itemDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    effect = itemDataBiosGetItemEffectParam(data);
    if (effect == 0) {
        return 0;
    }
    return itemParamGetPtr(effect);
}

static inline Pokemon *itemUseGetPokemon(FightPokemon *fightPokemon) {
    if (fightPokemon == 0) {
        return 0;
    }
    return fightPokemonGetPokemonPtr(fightPokemon);
}

static inline u8 itemUseStatUp(FightPokemon *fightPokemon, u32 statusID,
    u8 boost) {
    u8 oldStage;
    u8 newStage;
    FightOutPokemon *fightOutPokemon;

    fightOutPokemon = itemUseGetFightOut(fightPokemon);
    if (fightOutPokemon == 0) {
        return 0;
    }
    oldStage = pokemonGetStatus(fightOutPokemon, 0, statusID, 0);
    if (oldStage >= FIGHT_STAT_STAGE_LIMIT) {
        return 0;
    }
    newStage = oldStage + boost;
    if (newStage > FIGHT_STAT_STAGE_LIMIT) {
        newStage = FIGHT_STAT_STAGE_LIMIT;
    }
    pokemonSetStatus(fightOutPokemon, 0, statusID, 0, newStage);
    itemUseNotify(fightPokemon, POKEMON_STATUS_FIELD_FRIEND_LEVEL);
    return newStage - oldStage;
}

static inline void itemUseStatUpLog(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    FightPokemon *fightPokemon, u32 statusID, u8 boost,
    ITEMUSE2POKEMON_EFFECT effect) {
    u8 gained;

    if ((gained = itemUseStatUp(fightPokemon, statusID, boost)) != 0) {
        itemUseAddLog(log, count, effect, gained, 0);
    }
}

static inline u16 itemUseEffortTotal(Pokemon *pokemon) {
    u16 total;

    total = pokemonBiosGetMaxHpEffort(pokemon);
    total += pokemonBiosGetPhyAtkEffort(pokemon);
    total += pokemonBiosGetPhyDefEffort(pokemon);
    total += pokemonBiosGetSpeAtkEffort(pokemon);
    total += pokemonBiosGetSpeDefEffort(pokemon);
    total += pokemonBiosGetNimblenessEffort(pokemon);
    return total;
}

static inline s16 itemUseEffortUp(Pokemon *pokemon, FightPokemon *fightPokemon,
    u32 statusID, u8 boost) {
    u16 total;
    u16 oldEffort;
    u16 newEffort;
    u16 newTotal;

    total = itemUseEffortTotal(pokemon);
    if (total >= POKEMON_TOTAL_EFFORT_LIMIT) {
        return -1;
    }
    oldEffort = pokemonGetStatus(pokemon, 0, statusID, 0);
    if (oldEffort >= POKEMON_EFFORT_LIMIT) {
        return 0;
    }
    newEffort = oldEffort + boost;
    if (newEffort > POKEMON_EFFORT_LIMIT) {
        newEffort = POKEMON_EFFORT_LIMIT;
    }
    newTotal = total - oldEffort;
    newTotal = newTotal + newEffort;
    if (newTotal > POKEMON_TOTAL_EFFORT_LIMIT) {
        newEffort -= newTotal - POKEMON_TOTAL_EFFORT_LIMIT;
    }
    pokemonSetStatus(pokemon, 0, statusID, 0, newEffort);
    pokemonResetBasisStatus(pokemon);
    itemUseNotify(fightPokemon, statusID);
    return (u16)(newEffort - oldEffort);
}

static inline u8 itemUseEffortUpLog(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, FightPokemon *fightPokemon, u32 statusID, u8 boost,
    ITEMUSE2POKEMON_EFFECT effect) {
    s16 gained;

    gained = itemUseEffortUp(pokemon, fightPokemon, statusID, boost);
    if (gained < 0) {
        return 0;
    }
    if (gained == 0) {
        return 1;
    }
    itemUseAddLog(log, count, effect, gained, 0);
    return 1;
}

static inline u8 itemUsePPUp(Pokemon *pokemon, u8 moveIndex, u8 boost) {
    u16 move;
    u8 maxPp;
    u8 oldPp;
    u8 newPp;

    move = pokemonBiosGetPokemonWazaDataId(pokemon, moveIndex);
    if (move == WAZA_SHADOW_SPECIAL) {
        return 0;
    }
    if (wazaCheckDarkWaza(move)) {
        return 0;
    }
    maxPp = pokemonWazaGetMaxPP(pokemon, moveIndex);
    oldPp = pokemonBiosGetPokemonWazaPp(pokemon, moveIndex);
    if (oldPp >= maxPp) {
        return 0;
    }
    newPp = oldPp + boost;
    if (newPp > maxPp) {
        newPp = maxPp;
    }
    pokemonBiosSetPokemonWazaPp(pokemon, moveIndex, newPp);
    return newPp - oldPp;
}

static inline void itemUsePPRecover(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, FightPokemon *fightPokemon, ITEM_PARAM *param,
    u8 moveIndex, u8 boost) {
    Pokemon *target;
    u8 slot;
    u8 i;
    u8 gained;
    s16 index;

    if (itemParamGetPPSelectFlag(param)) {
        slot = moveIndex;
    } else {
        slot = ITEMUSE2POKEMON_PP_ALL_SLOTS;
    }
    target = pokemon;
    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_HENSHIN)
        || pokemonIsJoutaiDataId(pokemon, JOUTAI_HENSHIN_BROKEN)) {
        target = fightPokemonBiosGetMotoPokemonPtr(fightPokemon);
    }
    if (slot != ITEMUSE2POKEMON_PP_ALL_SLOTS) {
        if ((gained = itemUsePPUp(target, slot, boost)) != 0) {
            index = *count;
            if (index < ITEMUSE2POKEMON_LOG_MAX) {
                log[index].mEffect = ITEMUSE_EFFECT_PP_RESTORED;
                log[index].mValue = gained;
                log[index].mValue2 = slot;
                *count = index + 1;
            }
        }
    } else {
        for (i = 0; i < POKEMON_WAZA_COUNT; i++) {
            if ((gained = itemUsePPUp(target, i, boost)) != 0) {
                index = *count;
                if (index < ITEMUSE2POKEMON_LOG_MAX) {
                    log[index].mEffect = ITEMUSE_EFFECT_PP_RESTORED;
                    log[index].mValue = gained;
                    log[index].mValue2 = i;
                    *count = index + 1;
                }
            }
        }
    }
}

static inline void itemUsePPMaxUp(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, u8 moveIndex) {
    u16 move;
    u8 ppUpCount;
    u8 maxPp;
    u8 gained;
    u8 pp;

    move = pokemonBiosGetPokemonWazaDataId(pokemon, moveIndex);
    if (move != WAZA_SHADOW_SPECIAL && !wazaCheckDarkWaza(move)
        && pokemonWazaCheckValid(pokemon, moveIndex)
        && wazaDB_GetPp(move) >= ITEMUSE2POKEMON_PPUP_MIN_PP) {
        ppUpCount = pokemonBiosGetPokemonWazaPpCount(pokemon, moveIndex);
        if (ppUpCount < POKEMON_PP_UP_LIMIT) {
            maxPp = pokemonWazaGetMaxPP(pokemon, moveIndex);
            pokemonBiosSetPokemonWazaPpCount(pokemon, moveIndex, ppUpCount + 1);
            gained = pokemonWazaGetMaxPP(pokemon, moveIndex) - maxPp;
            pp = pokemonBiosGetPokemonWazaPp(pokemon, moveIndex);
            pp += gained;
            pokemonBiosSetPokemonWazaPp(pokemon, moveIndex, pp);
            itemUseAddLog(log, count, ITEMUSE_EFFECT_PP_MAX_UP, gained,
                moveIndex);
        }
    }
}

static inline void itemUsePPMaxFull(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, u8 moveIndex) {
    u16 move;
    u8 maxPp;
    u8 gained;
    u8 pp;

    move = pokemonBiosGetPokemonWazaDataId(pokemon, moveIndex);
    if (move != WAZA_SHADOW_SPECIAL && !wazaCheckDarkWaza(move)
        && wazaDB_GetPp(move) >= ITEMUSE2POKEMON_PPUP_MIN_PP
        && pokemonBiosGetPokemonWazaPpCount(pokemon, moveIndex)
            < POKEMON_PP_UP_LIMIT) {
        maxPp = pokemonWazaGetMaxPP(pokemon, moveIndex);
        pokemonBiosSetPokemonWazaPpCount(pokemon, moveIndex,
            POKEMON_PP_UP_LIMIT);
        gained = pokemonWazaGetMaxPP(pokemon, moveIndex) - maxPp;
        if (gained != 0) {
            pp = pokemonBiosGetPokemonWazaPp(pokemon, moveIndex);
            pp += gained;
            pokemonBiosSetPokemonWazaPp(pokemon, moveIndex, pp);
        }
        itemUseAddLog(log, count, ITEMUSE_EFFECT_PP_MAX_FULL, gained, moveIndex);
    }
}

static inline void itemUseCureSleep(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, FightPokemon *fightPokemon) {
    u8 wokeUp = 0;
    FightOutPokemon *fightOutPokemon;

    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_SLEEP)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_SLEEP);
        fightOutPokemon = itemUseGetFightOut(fightPokemon);
        if (fightOutPokemon != 0) {
            fightOutPokemonInitJoutaiDataId(fightOutPokemon,
                JOUTAI_AKUMU);
            wokeUp = 1;
        }
        itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
        itemUseAddLog(log, count, ITEMUSE_EFFECT_SLEEP_CURED, 0, 0);
        if (wokeUp) {
            itemUseAddLog(log, count, ITEMUSE_EFFECT_SLEEP_CURED_BATTLE, 0,
                0);
        }
    }
}

static inline void itemUseCurePoison(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, FightPokemon *fightPokemon) {
    u8 poisonCleared = 0;
    u8 badPoisonCleared = 0;

    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_POISON)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_POISON);
        poisonCleared = 1;
    }
    if (pokemonIsJoutaiDataId(pokemon, JOUTAI_BAD_POISON)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_BAD_POISON);
        badPoisonCleared = 1;
    }
    itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
    if (poisonCleared) {
        itemUseAddLog(log, count, ITEMUSE_EFFECT_POISON_CURED, 0, 0);
    }
    if (badPoisonCleared) {
        itemUseAddLog(log, count, ITEMUSE_EFFECT_BAD_POISON_CURED, 0, 0);
    }
}

static inline void itemUseGuardSpec(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    FightPokemon *fightPokemon) {
    FightOutPokemon *fightOutPokemon;
    void *side;

    fightOutPokemon = itemUseGetFightOut(fightPokemon);
    if (fightOutPokemon != 0) {
        side = fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, fightOutPokemon);
        if (fightSideCheckWriteJoutaiDataId(side,
                JOUTAI_MIST) == JOUTAI_WRITE_ACCEPT) {
            fightSideWriteJoutaiDataId(side, JOUTAI_MIST, 0);
            itemUseNotify(fightPokemon, POKEMON_STATUS_FIELD_FRIEND_LEVEL);
            itemUseAddLog(log, count, ITEMUSE_EFFECT_GUARD,
                ITEMUSE2POKEMON_GUARD_DURATION, 0);
        }
    }
}

static inline void itemUseEvolve(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, u16 itemID, POKEMON_EVOLUTION_INFO *evolutionMode) {
    u16 evolutionSpecies;
    u16 evolution;

    evolution = pokemonEvolutionCheck(pokemon, EVOLUTION_CHECK_ITEM, itemID,
        &evolutionSpecies, evolutionMode);
    if (evolution != 0 || evolutionSpecies != 0) {
        itemUseAddLog(log, count, ITEMUSE_EFFECT_EVOLUTION, evolution,
            evolutionSpecies);
    }
}

static inline u16 itemUseLevelUp(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon) {
    u8 level;
    u16 oldMaxHp;
    u16 newMaxHp;

    if (pokemonIsDarkPokemon(pokemon)) {
        return 0;
    }
    level = pokemonBiosGetLevel(pokemon);
    if (level >= POKEMON_LEVEL_LIMIT) {
        return 0;
    }
    oldMaxHp = pokemonBiosGetMaxHp(pokemon);
    pokemonBiosSetExp(pokemon, pokemonGrowDataBiosGetExp(
        pokemonGrowDataBiosGetPtr(pokemonDataBiosGetGrowDataId(
            pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)))),
        level + 1));
    pokemonResetBasisStatus(pokemon);
    newMaxHp = pokemonBiosGetMaxHp(pokemon);
    itemUseAddLog(log, count, ITEMUSE_EFFECT_LEVEL_UP, 0, 0);
    return newMaxHp - oldMaxHp;
}

static inline s16 itemUseFriendUp(ITEMUSE2POKEMON_LOG1 *log, s16 *count,
    Pokemon *pokemon, FightPokemon *fightPokemon, s8 change, u16 lower,
    u16 upper) {
    s16 gained;

    if (*count <= 0) {
        return 0;
    }
    gained = friendXUp(pokemon, fightPokemon, change, lower, upper);
    if (gained == 0) {
        return 0;
    }
    itemUseAddLog(log, count, ITEMUSE_EFFECT_FRIEND_UP, gained, 0);
    return gained;
}

s16 itemUse2Pokemon(ITEMUSE2POKEMON_LOG1 *log, Pokemon *pokemon,
    FightPokemon *fightPokemon, u16 itemID, u8 moveIndex) {
    POKEMON_EVOLUTION_INFO evolutionMode;
    s16 count;
    ITEM_PARAM convertedParam;
    ITEM_PARAM *param;
    FightOutPokemon *fightOutPokemon;
    u16 hpAmount;
    u8 boost;
    s8 friendBoost;
    s16 friendGain;
    u8 ok;

    hpAmount = 0;
    count = 0;

    if (itemID != ITEM_ENIGMA_BERRY) {
        param = itemUseGetParam(itemID);
    } else {
        static ITEM_PARAM_ORIG simParam = {{0, 0, 0, 0, 0, 0, 0, 0}};
        itemParamConvertOrigFormat(&convertedParam, &simParam);
        param = &convertedParam;
    }
    if (param == 0) {
        return 0;
    }

    if (fightPokemon != 0) {
        pokemon = itemUseGetPokemon(fightPokemon);
    }

    if (itemParamGetMeromeroFlag(param)) {
        fightOutPokemon = itemUseGetFightOut(fightPokemon);
        if (fightOutPokemon != 0 && fightOutPokemonIsJoutaiDataId(fightOutPokemon,
                JOUTAI_INFATUATION)) {
            fightOutPokemonInitJoutaiDataId(fightOutPokemon,
                JOUTAI_INFATUATION);
            itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
            itemUseAddLog(log, &count, ITEMUSE_EFFECT_CURE_INFATUATION, 0, 0);
        }
    }

    if (itemParamGetCriticalFlag(param)) {
        fightOutPokemon = itemUseGetFightOut(fightPokemon);
        if (fightOutPokemon != 0 && fightOutPokemonCheckWriteJoutaiDataId(
                fightOutPokemon, JOUTAI_FOCUS_ENERGY) == JOUTAI_WRITE_ACCEPT) {
            fightOutPokemonWriteJoutaiDataId(fightOutPokemon,
                JOUTAI_FOCUS_ENERGY, 0);
            itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
            itemUseNotify(fightPokemon, POKEMON_STATUS_FIELD_FRIEND_LEVEL);
            itemUseAddLog(log, &count, ITEMUSE_EFFECT_FOCUS_ENERGY, 0, 0);
        }
    }

    if ((boost = itemParamGetAttackUp(param)) != 0) {
        itemUseStatUpLog(log, &count, fightPokemon,
            POKEMON_STATUS_FIELD_ATTACK_STAGE, boost,
            ITEMUSE_EFFECT_ATTACK_UP);
    }
    if ((boost = itemParamGetDefenceUp(param)) != 0) {
        itemUseStatUpLog(log, &count, fightPokemon,
            POKEMON_STATUS_FIELD_DEFENSE_STAGE, boost,
            ITEMUSE_EFFECT_DEFENCE_UP);
    }
    if ((boost = itemParamGetQuickUp(param)) != 0) {
        itemUseStatUpLog(log, &count, fightPokemon,
            POKEMON_STATUS_FIELD_SPEED_STAGE, boost,
            ITEMUSE_EFFECT_SPEED_UP);
    }
    if ((boost = itemParamGetHitUp(param)) != 0) {
        itemUseStatUpLog(log, &count, fightPokemon,
            POKEMON_STATUS_FIELD_ACCURACY_STAGE, boost,
            ITEMUSE_EFFECT_ACCURACY_UP);
    }
    if ((boost = itemParamGetSpAttackUp(param)) != 0) {
        itemUseStatUpLog(log, &count, fightPokemon,
            POKEMON_STATUS_FIELD_SP_ATTACK_STAGE, boost,
            ITEMUSE_EFFECT_SP_ATTACK_UP);
    }

    if (itemParamGetGuardFlag(param)) {
        itemUseGuardSpec(log, &count, fightPokemon);
    }

    if (itemParamGetLevelUpFlag(param)) {
        hpAmount = itemUseLevelUp(log, &count, pokemon);
    }

    if (itemParamGetSleepFlag(param)) {
        itemUseCureSleep(log, &count, pokemon, fightPokemon);
    }

    if (itemParamGetPoisonFlag(param)) {
        itemUseCurePoison(log, &count, pokemon, fightPokemon);
    }

    if (itemParamGetBurnFlag(param)
        && pokemonIsJoutaiDataId(pokemon, JOUTAI_BURN)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_BURN);
        itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
        itemUseAddLog(log, &count, ITEMUSE_EFFECT_BURN_CURED, 0, 0);
    }
    if (itemParamGetFreezeFlag(param)
        && pokemonIsJoutaiDataId(pokemon, JOUTAI_FREEZE)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_FREEZE);
        itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
        itemUseAddLog(log, &count, ITEMUSE_EFFECT_FREEZE_CURED, 0, 0);
    }
    if (itemParamGetParalyzeFlag(param)
        && pokemonIsJoutaiDataId(pokemon, JOUTAI_PARALYSIS)) {
        pokemonInitJoutaiDataId(pokemon, JOUTAI_PARALYSIS);
        itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
        itemUseAddLog(log, &count, ITEMUSE_EFFECT_PARALYZE_CURED, 0, 0);
    }

    if (itemParamGetConfuseFlag(param)) {
        fightOutPokemon = itemUseGetFightOut(fightPokemon);
        if (fightOutPokemon != 0 && fightOutPokemonIsJoutaiDataId(fightOutPokemon,
                JOUTAI_CONFUSION)) {
            fightOutPokemonInitJoutaiDataId(fightOutPokemon,
                JOUTAI_CONFUSION);
            itemUseNotify(fightPokemon, POKEMON_STATUS_CONDITION);
            itemUseAddLog(log, &count, ITEMUSE_EFFECT_CONFUSE_CURED, 0, 0);
        }
    }

    if (itemParamGetPPMaxUpFlag(param)) {
        itemUsePPMaxUp(log, &count, pokemon, moveIndex);
    }

    if ((boost = itemParamGetHPEffortUp(param)) != 0) {
        if (pokemonBiosGetPokemonDataId(pokemon)
            == POKEMON_SPECIES_SHEDINJA) {
            ok = 1;
        } else {
            ok = itemUseEffortUpLog(log, &count, pokemon, fightPokemon,
                POKEMON_STATUS_FIELD_HP_EFFORT, boost,
                ITEMUSE_EFFECT_EFFORT_MAX_HP_UP);
        }
        if (!ok) {
            return 0;
        }
    }
    if ((boost = itemParamGetAttackEffortUp(param)) != 0) {
        if (!itemUseEffortUpLog(log, &count, pokemon, fightPokemon,
                POKEMON_STATUS_FIELD_ATTACK_EFFORT, boost,
                ITEMUSE_EFFECT_EFFORT_ATTACK_UP)) {
            return 0;
        }
    }

    if ((boost = itemParamGetHPUp(param)) != 0) {
        bool reviveFlag = itemParamGetReviveFlag(param);
        hpRecover(log, &count, pokemon, boost, reviveFlag, hpAmount, fightPokemon);
    }

    if ((boost = itemParamGetPPUp(param)) != 0) {
        itemUsePPRecover(log, &count, pokemon, fightPokemon, param, moveIndex,
            boost);
    }

    if (itemParamGetEvolutionFlag(param) && !pokemonIsDarkPokemon(pokemon)) {
        itemUseEvolve(log, &count, pokemon, itemID, &evolutionMode);
    }

    if ((boost = itemParamGetDefenceEffortUp(param)) != 0) {
        if (!itemUseEffortUpLog(log, &count, pokemon, fightPokemon,
                POKEMON_STATUS_FIELD_DEFENSE_EFFORT, boost,
                ITEMUSE_EFFECT_EFFORT_DEFENCE_UP)) {
            return 0;
        }
    }
    if ((boost = itemParamGetQuickEffortUp(param)) != 0) {
        if (!itemUseEffortUpLog(log, &count, pokemon, fightPokemon,
                POKEMON_STATUS_FIELD_SPEED_EFFORT, boost,
                ITEMUSE_EFFECT_EFFORT_SPEED_UP)) {
            return 0;
        }
    }
    if ((boost = itemParamGetSpDefenceEffortUp(param)) != 0) {
        if (!itemUseEffortUpLog(log, &count, pokemon, fightPokemon,
                POKEMON_STATUS_FIELD_SP_DEFENSE_EFFORT, boost,
                ITEMUSE_EFFECT_EFFORT_SP_DEFENCE_UP)) {
            return 0;
        }
    }
    if ((boost = itemParamGetSpAttackEffortUp(param)) != 0) {
        if (!itemUseEffortUpLog(log, &count, pokemon, fightPokemon,
                POKEMON_STATUS_FIELD_SP_ATTACK_EFFORT, boost,
                ITEMUSE_EFFECT_EFFORT_SP_ATTACK_UP)) {
            return 0;
        }
    }

    if (itemParamGetPPMaxFullFlag(param)) {
        itemUsePPMaxFull(log, &count, pokemon, moveIndex);
    }

    friendGain = 0;
    if ((friendBoost = itemParamGetFriend1Up(param)) != 0) {
        friendGain = itemUseFriendUp(log, &count, pokemon, fightPokemon,
            friendBoost, 0, ITEMUSE2POKEMON_FRIEND_THRESHOLD_1);
    }
    if (friendGain == 0) {
        if ((friendBoost = itemParamGetFriend2Up(param)) != 0) {
            friendGain = itemUseFriendUp(log, &count, pokemon, fightPokemon,
                friendBoost, ITEMUSE2POKEMON_FRIEND_THRESHOLD_1,
                ITEMUSE2POKEMON_FRIEND_THRESHOLD_2);
        }
    }
    if (friendGain == 0) {
        if ((friendBoost = itemParamGetFriend3Up(param)) != 0) {
            itemUseFriendUp(log, &count, pokemon, fightPokemon, friendBoost,
                ITEMUSE2POKEMON_FRIEND_THRESHOLD_2,
                ITEMUSE2POKEMON_FRIEND_THRESHOLD_MAX);
        }
    }

    return count;
}
