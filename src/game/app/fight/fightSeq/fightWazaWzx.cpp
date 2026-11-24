#include <pokemonDB.h>
#include <fightPokemon.h>
#include <fightPokemonBios.h>
#include <fightTarget.h>
#include <fightFloor.h>
#include <fightSeq.h>
#include <waza.h>
#include <battleGrid.h>
#include <Pokemon.h>
#include <sequence.h>

extern "C" u8 fightWazaWzxOiuchiVariationFlag;

enum {
    WAZA_EFFECT_SLOT_ATK = 1,
    WAZA_EFFECT_SLOT_DEF = 2,
    WAZA_EFFECT_SLOT_SPE = 3
};

enum {
    FIGHT_EFFECT_DRAIN_RECOVERY = 50
};

enum {
    WAZA_EFFECT_INVOCATION_INITIAL = 0,
    WAZA_EFFECT_INVOCATION_FOLLOWUP = 1
};

void _fightWazaWxzfightOutPokemonWazaEffectAtk(FightOutPokemon *pokemon, u16 wazaId, bool wait)
{
    fightOutPokemonWazaEffect(pokemon, wazaId, WAZA_EFFECT_SLOT_ATK, wait,
        fightTargetDataBiosGetBuff(fightTargetDataBiosGetPtr(FIGHT_TARGET_ID_ATTACKER)));
}

void _fightWazaWxzfightOutPokemonWazaEffectDef(FightOutPokemon *pokemon, u16 wazaId, bool wait)
{
    fightOutPokemonWazaEffect(pokemon, wazaId, WAZA_EFFECT_SLOT_DEF, wait,
        fightTargetDataBiosGetBuff(fightTargetDataBiosGetPtr(FIGHT_TARGET_ID_DEFENDER)));
}

void _fightWazaWxzfightOutPokemonWazaEffectSpe(FightOutPokemon *pokemon, u16 wazaId, bool wait)
{
    fightOutPokemonWazaEffect(pokemon, wazaId, WAZA_EFFECT_SLOT_SPE, wait,
        fightTargetDataBiosGetBuff(fightTargetDataBiosGetPtr(FIGHT_TARGET_ID_ATTACKER)));
}

extern "C" u8 fightWazaWzxVariationFuncNull(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    return 0;
}

extern "C" void fightWazaWzxTypeFuncNull(u16, FightOutPokemon *, FightOutPokemon *, u16, u8)
{
}

extern "C" u8 fightWazaWzxVariationFuncNormal(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    return 0;
}

extern "C" void fightWazaWzxTypeFuncNormal(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                           u16 invocation, u8 variation)
{
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    } else {
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncNormal2(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                            u16 invocation, u8 variation)
{
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, 0);
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    } else {
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncTame(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                         u16 invocation, u8 variation)
{
    if (UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION] != 1) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
        _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
    } else {
        fightWazaWzxTypeFuncNormal(wazaId, attacker, defender, invocation, variation);
    }
}

extern "C" void fightWazaWzxTypeFuncSpc(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                        u16, u8 variation)
{
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
    _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
}

extern "C" void fightWazaWzxTypeFuncVampire(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                            u16 invocation, u8 variation)
{
    s32 damage = fightWaza_GetDamage(fightOutPokemon_GetFightWazaPtr(attacker));
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    } else if (invocation == WAZA_EFFECT_INVOCATION_FOLLOWUP) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
        if (damage > 0) {
            fightSeqFightOutPokemonEffect(FIGHT_EFFECT_DRAIN_RECOVERY, attacker, false, true);
        } else {
            _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
        }
    }
}

extern "C" void fightWazaWzxTypeFuncAttDefSpc(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                              u16 invocation, u8 variation)
{
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    } else if (invocation == WAZA_EFFECT_INVOCATION_FOLLOWUP) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
        _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncAllEnemy(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                             u16, u8 variation)
{
    FightOutPokemon *other = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_SECOND_POKEMON,
                                                                 attacker, fightFloor_GetFightType(0));
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
    fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
    fightOutPokemonLoadWazaEffect(other, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
    if (fightOutPokemonCheckFightOut(other) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(other, wazaId, false);
    } else {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncAllAlly(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                            u16, u8 variation)
{
    FightOutPokemon *ally = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE,
                                                                attacker, fightFloor_GetFightType(0));
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
    fightOutPokemonLoadWazaEffect(ally, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
    if (fightOutPokemonCheckFightOut(ally) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectSpe(ally, wazaId, false);
    } else {
        _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncAttDef(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                           u16 invocation, u8 variation)
{
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, false);
    } else {
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncItamiwake(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                              u16 invocation, u8 variation)
{
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, false);
    } else {
        fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncAtt(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                        u16 invocation, u8 variation)
{
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
    if (invocation == WAZA_EFFECT_INVOCATION_INITIAL) {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncTedasuke(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                             u16, u8 variation)
{
    u16 fightType = fightFloor_GetFightType(0);
    void *target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
        fightWaza_GetTargetDataId(fightOutPokemon_GetFightWazaPtr(attacker)), fightType);
    FightOutPokemon *ally = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE,
                                                                attacker, fightType);
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
    fightOutPokemonLoadWazaEffect(ally, wazaId, WAZA_EFFECT_SLOT_SPE, variation);
    _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
    if (fightOutPokemonCheckFightOut(ally) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectSpe(ally, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncNegaigoto(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                              u16, u8 variation)
{
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
    _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, false);
}

extern "C" void fightWazaWzxTypeFuncMagiccort(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                              u16, u8 variation)
{
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
    if (fightOutPokemonIsJoutaiDataId(defender, JOUTAI_MAGIC_COAT) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, false);
    }
}

extern "C" void fightWazaWzxTypeFuncKawarawari(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                               u16, u8 variation)
{
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, 0);
    fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
    _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
    _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
}

extern "C" void fightWazaWzxTypeFuncHensin(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                           u16, u8)
{
    NullSequence *oldSequence;
    FightOutPokemon work;
    u16 tenkou = fightFloorGetNowTenkouDataId(0, 1);
    u16 wazawzxDataId = wazaDB_GetWazawzxdataId(wazaId);
    oldSequence = fightOutPokemon_GetSequencePtr(attacker);
    u8 rare = pokemonCheckRare2(fightPokemonGetPokemonPtr(fightOutPokemon_GetMotoFightPokemonPtr(attacker)));
    NullSequence *newSequence = fightOutPokemonCreateSequence(attacker, tenkou, rare);
    fightOutPokemonBiosCopy(&work, attacker);
    fightOutPokemon_SetSequencePtr(&work, newSequence);
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_SPE, 0);
    fightOutPokemonLoadWazaEffect(&work, wazaId, WAZA_EFFECT_SLOT_SPE, 1);
    _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, true);
    battleGridReplacePokemon(oldSequence, newSequence);
    battleGridUpdate();
    fightOutPokemon_SetSequencePtr(attacker, newSequence);
    _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
    Sequence_Free(oldSequence);
}

extern "C" void fightWazaWzxTypeFuncMigawari(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *,
                                             u16, u8)
{
    NullSequence *oldSequence;
    u16 tenkou = fightFloorGetNowTenkouDataId(0, 1);
    NullSequence *newSequence;
    FightOutPokemon work;
    u16 wazawzxDataId = wazaDB_GetWazawzxdataId(wazaId);
    oldSequence = fightOutPokemon_GetSequencePtr(attacker);
    newSequence = fightOutPokemonCreateSequence(attacker, tenkou, FIGHT_POKEMON_RARE_AUTO);
    fightOutPokemonBiosCopy(&work, attacker);
    fightOutPokemon_SetSequencePtr(&work, newSequence);
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, 0);
    fightOutPokemonLoadWazaEffect(&work, wazaId, WAZA_EFFECT_SLOT_SPE, 0);
    _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
    battleGridReplacePokemon(oldSequence, newSequence);
    battleGridUpdate();
    fightOutPokemon_SetSequencePtr(attacker, newSequence);
    _fightWazaWxzfightOutPokemonWazaEffectSpe(attacker, wazaId, false);
    Sequence_Free(oldSequence);
}

extern "C" void fightWazaWzxTypeFuncHuuin(u16 wazaId, FightOutPokemon *attacker, FightOutPokemon *defender,
                                          u16, u8 variation)
{
    u16 fightType = fightFloor_GetFightType(0);
    defender = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON,
                                                                  attacker, fightType);
    FightOutPokemon *otherDefender = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_SECOND_POKEMON,
                                                                  attacker, fightType);
    fightOutPokemonLoadWazaEffect(attacker, wazaId, WAZA_EFFECT_SLOT_ATK, variation);
    fightOutPokemonLoadWazaEffect(defender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
    fightOutPokemonLoadWazaEffect(otherDefender, wazaId, WAZA_EFFECT_SLOT_DEF, variation);
    if (fightOutPokemonCheckFightOut(defender) == 1 && fightOutPokemonCheckFightOut(otherDefender) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(otherDefender, wazaId, false);
    } else if (fightOutPokemonCheckFightOut(otherDefender) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(otherDefender, wazaId, false);
    } else if (fightOutPokemonCheckFightOut(defender) == 1) {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
        _fightWazaWxzfightOutPokemonWazaEffectDef(defender, wazaId, false);
    } else {
        _fightWazaWxzfightOutPokemonWazaEffectAtk(attacker, wazaId, true);
    }
}

extern "C" u8 fightWazaWzxVariationFuncKetaguri(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    switch (fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(attacker))) {
    case 20: return 0;
    case 40: return 1;
    case 60: return 2;
    case 80: return 3;
    case 100: return 4;
    case 120: return 5;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncTikyuunage(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    u8 level = figthOutPokemonGetLevel(attacker);
    if (level < 33) return 0;
    if (level < 66) return 1;
    return 2;
}

extern "C" u8 fightWazaWzxVariationFuncTripleKick(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    switch (fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(attacker))) {
    case 10: return 0;
    case 20: return 1;
    case 30: return 2;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncKorogaru(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    switch (fightOutPokemonGetCountAsJoutaiDataId(attacker, JOUTAI_ROLLOUT)) {
    case 1: return 0;
    case 2: return 1;
    case 3: return 2;
    case 4: return 3;
    case -1: return 4;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncRenzokugiri(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    switch (fightOutPokemonGetCountAsJoutaiDataId(attacker, JOUTAI_FURY_CUTTER)) {
    case 1: return 0;
    case 2: return 1;
    case 3: return 2;
    case 4: return 3;
    case 5: return 4;
    }
    return 0;
}


extern "C" u8 fightWazaWzxVariationFuncOngaesi(u16, FightOutPokemon *pokemon,
                                             FightOutPokemon *, u16)
{
    u16 power = fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(pokemon));
    if (power <= 24) return 0;
    if (power <= 36) return 1;
    if (power <= 80) return 2;
    return 3;
}

extern "C" u8 fightWazaWzxVariationFuncPresent(u16, FightOutPokemon *pokemon,
                                             FightOutPokemon *, u16)
{
    u16 power = fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(pokemon));
    switch (power) {
    case 40:
    case 80:
    case 120:
        return 0;
    default:
        return 1;
    }
}

extern "C" u8 fightWazaWzxVariationFuncYatuatari(u16, FightOutPokemon *pokemon,
                                               FightOutPokemon *, u16)
{
    u16 power = fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(pokemon));
    if (power > 89) return 3;
    if (power >= 62) return 2;
    if (power >= 22) return 1;
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncMagnitude(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    switch (fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(attacker))) {
    case 10:
    case 30: return 0;
    case 50:
    case 70:
    case 90: return 1;
    case 110:
    case 150: return 2;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncWeatherHP(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    switch (fightFloorGetNowTenkouDataId(0, 1)) {
    case FIGHT_WEATHER_SUNLIGHT: return 0;
    case FIGHT_WEATHER_NONE: return 1;
    case FIGHT_WEATHER_RAIN:
    case FIGHT_WEATHER_SANDSTORM:
    case FIGHT_WEATHER_HAIL:
    case FIGHT_WEATHER_SHADOW_SKY: return 2;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncHakidasu(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    switch (UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION]) {
    case 1:
    case 2: return 0;
    case 3: return 1;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncNomikomu(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    switch (UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION]) {
    case 1:
    case 2: return 0;
    case 3: return 1;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncKawarawari(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    switch (UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION]) {
    case 0: return 3;
    case 1: return 2;
    case 2: return 1;
    case 3: return 0;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncIceball(u16, FightOutPokemon *attacker, FightOutPokemon *, u16)
{
    switch (fightOutPokemonGetCountAsJoutaiDataId(attacker, JOUTAI_ROLLOUT)) {
    case 1: return 0;
    case 2: return 1;
    case 3: return 2;
    case 4: return 3;
    case -1: return 4;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncWeatherBall(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    switch (fightFloorGetNowTenkouDataId(0, 1)) {
    case FIGHT_WEATHER_NONE: return 0;
    case FIGHT_WEATHER_RAIN: return 1;
    case FIGHT_WEATHER_HAIL: return 2;
    case FIGHT_WEATHER_SUNLIGHT: return 3;
    case FIGHT_WEATHER_SANDSTORM: return 4;
    case FIGHT_WEATHER_SHADOW_SKY: return 5;
    }
    return 0;
}

extern "C" u8 fightWazaWzxVariationFuncOiuchi(u16, FightOutPokemon *, FightOutPokemon *, u16)
{
    return fightWazaWzxOiuchiVariationFlag == 1;
}
