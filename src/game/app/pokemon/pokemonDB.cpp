#include <types.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokemonDB.h>

static const u16 POKEMON_FRIEND_LEVEL_MAX = 255;

extern "C" {

void fightOutPokemon_SetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId)
{
    if (pokemon == 0) {
        return;
    }
    if (index >= FIGHT_OUT_POKEMON_ZOKUSEI_MAX) {
        return;
    }
    pokemon->mZokuseiDataId[index] = typeId;
}

void fightOutPokemon_SetMeetEnemyFightPokemonEntryId(FightOutPokemon *pokemon, u8 index,
                                                     u16 entryId)
{
    if (pokemon == 0) {
        return;
    }
    if (index >= FIGHT_OUT_POKEMON_MEET_ENEMY_MAX) {
        return;
    }
    pokemon->mMeetEnemyFightPokemonEntryId[index] = entryId;
}

void fightOutPokemon_SetSuccessCnt(FightOutPokemon *pokemon, u8 count)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mSuccessCnt = count;
}

void fightOutPokemon_SetStockItemDataId(FightOutPokemon *pokemon, u16 itemDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mStockItemDataId = itemDataId;
}

void fightOutPokemon_SetUsedItemDataId(FightOutPokemon *pokemon, u16 itemDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mUsedItemDataId = itemDataId;
}

void fightOutPokemon_SetNamakeFlag(FightOutPokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mNamakeFlag = flag;
}

void fightOutPokemon_SetOumuWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mOumuWazaDataId = wazaDataId;
}

void fightOutPokemon_SetGamanDamageTargetId(FightOutPokemon *pokemon, u16 targetId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mGamanDamageTargetId = targetId;
}

void fightOutPokemon_SetGamanDamageValue(FightOutPokemon *pokemon, s16 damage)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mGamanDamageValue = damage;
}

void fightOutPokemon_SetHitWazaZokuseiDataId(FightOutPokemon *pokemon, u16 typeId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mHitWazaZokuseiDataId = typeId;
}

void fightOutPokemon_SetHitWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mHitWazaDataId = wazaDataId;
}

void fightOutPokemon_SetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon, u16 targetDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mLastReceiveWazaTargetDataId = targetDataId;
}

void fightOutPokemon_SetLastUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mLastUseWazaDataId = wazaDataId;
}

void fightOutPokemon_SetLastSelectWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mLastSelectWazaDataId = wazaDataId;
}

void fightOutPokemon_SetSketchWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mSketchWazaDataId = wazaDataId;
}

void fightOutPokemon_SetSequencePtr(FightOutPokemon *pokemon, struct NullSequence *sequence)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mSequence = sequence;
}

void fightOutPokemon_SetFightoutTurnCount(FightOutPokemon *pokemon, u16 count)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mFightoutTurnCount = count;
}

void fightOutPokemon_SetAbicntAverage(FightOutPokemon *pokemon, u8 average)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mAbicntAverage = average;
}

void fightOutPokemon_SetWazaHitDamage(FightOutPokemon *pokemon, s32 hitDamage)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetHitDamage(waza, hitDamage);
}

void fightOutPokemon_SetWazaTargetId(FightOutPokemon *pokemon, u16 targetId)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetTargetDataId(waza, targetId);
}

void fightOutPokemon_SetWazaIryoku(FightOutPokemon *pokemon, u16 iryoku)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetIryoku(waza, iryoku);
}

void fightOutPokemon_SetWazaDamage(FightOutPokemon *pokemon, s32 damage)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetDamage(waza, damage);
}

void fightOutPokemon_SetWazaInitJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    if (joutaiDataId != 0) {
        if (fightWazaIsJoutaiDataId(waza, joutaiDataId) == 1) {
            fightWazaInitJoutaiDataId(waza, joutaiDataId);
        }
    } else {
        fightWazaInitJoutai(waza);
    }
}

void fightOutPokemon_SetWazaJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    if (fightWazaCheckWriteJoutaiDataId(waza, joutaiDataId) == JOUTAI_WRITE_ACCEPT) {
        fightWazaWriteJoutaiDataId(waza, joutaiDataId, 0);
    }
}

void fightOutPokemon_SetWazaCritical(FightOutPokemon *pokemon, u32 critical)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetCritical(waza, critical);
}

void fightOutPokemon_SetWazaDamageValue(FightOutPokemon *pokemon, u8 damageValue)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetDamageValue(waza, damageValue);
}

void fightOutPokemon_SetMotoWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetMotoWazaDataId(waza, wazaDataId);
}

void fightOutPokemon_SetUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId)
{
    FIGHT_WAZA *waza;

    if (pokemon == 0) {
        return;
    }
    waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    fightWaza_SetUseWazaDataId(waza, wazaDataId);
}

void fightOutPokemon_SetFightPokemonPtr(FightOutPokemon *pokemon, FightPokemon *fightPokemon)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mCurrentPokemon = fightPokemon;
}

void fightOutPokemon_SetMotoFightPokemonPtr(FightOutPokemon *pokemon, FightPokemon *fightPokemon)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mOriginalPokemon = fightPokemon;
}

void fightPokemon_SetAiDamage(FightPokemon *pokemon, u16 index, u16 target, s32 damage)
{
    if (pokemon == 0) {
        return;
    }
    if (index >= FIGHT_POKEMON_AI_DAMAGE_MAX) {
        return;
    }
    pokemon->mAiDamage[index][target] = damage;
}

void fightPokemon_SetHokakuFlag(FightPokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mHokakuFlag = flag;
}

void fightPokemon_SetDarkOutFlag(FightPokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mDarkOutFlag = flag;
}

void fightPokemon_SetLevelUpFlag(FightPokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mLevelUpFlag = flag;
}

void fightPokemon_SetCatchEntryFlag(FightPokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mCatchEntryFlag = flag;
}

void fightPokemon_SetEntryId(FightPokemon *pokemon, s16 entryId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mEntryId = entryId;
}

void fightPokemon_SetMotoPokemonPtr(FightPokemon *pokemon, Pokemon *original)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mMotoPokemon = original;
}

void pokemon_SetComboPartnerAll(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mComboPartnerAll = value;
}

void pokemon_SetPoolFriend(Pokemon *pokemon, u16 level)
{
    if (pokemon == 0) {
        return;
    }
    if (level > POKEMON_FRIEND_LEVEL_MAX) {
        level = POKEMON_FRIEND_LEVEL_MAX;
    }
    pokemon->setPoolFriendLevel(level);
}

void pokemon_SetPoolExp(Pokemon *pokemon, u32 exp)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->setPoolExp(exp);
}

void pokemon_SetDarkpokemonDataId(Pokemon *pokemon, u16 dataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mDarkPokemonDataId = dataId;
}

void pokemon_SetFightTrainerPokemonDataId(Pokemon *pokemon, u16 dataId)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mFightTrainerPokemonDataId = dataId;
}

void pokemon_SetAmari(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetAmari(pokemon, value);
}

void pokemon_SetPara1Amari(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPara1Amari(pokemon, value);
}

void pokemon_SetMailId(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mMailId = value;
}

void pokemon_SetPcboxMark(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mPcBoxMark = value;
}

void pokemon_SetFlagAmari(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mFlagAmari = value;
}

void pokemon_SetFuseiFlag(Pokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mFuseiFlag = flag;
}

void pokemon_SetTokuseiFlag(Pokemon *pokemon, u8 flag)
{
    PokemonData *data;

    if (pokemon == 0) {
        return;
    }
    data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon));
    if (pokemonDataBiosGetTokuseiDataId(data, POKEMON_DATA_ABILITY_SECOND) == 0) {
        flag = 0;
    }
    pokemon->mTokuseiFlag = flag;
}

void pokemon_SetTamagoFlag(Pokemon *pokemon, u8 flag)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mTamagoFlag = flag;
}

void pokemon_SetPokerus(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mPokerus = value;
}

// Contest medal setters. The medals of a Pokemon are the second record of its
// expand status, one byte per contest category; a null Pokemon or a status the
// accessor cannot resolve leaves the call without effect.

void pokemon_SetStrongMedal(Pokemon *pokemon, u8 value)
{
    PokemonExpandStatus *status;

    if (pokemon == 0) {
        return;
    }
    status = pokemonBiosGetExpandStatusPtr(pokemon, 1);
    if (status == 0) {
        return;
    }
    status->strong = value;
}

void pokemon_SetCleverMedal(Pokemon *pokemon, u8 value)
{
    PokemonExpandStatus *status;

    if (pokemon == 0) {
        return;
    }
    status = pokemonBiosGetExpandStatusPtr(pokemon, 1);
    if (status == 0) {
        return;
    }
    status->clever = value;
}

void pokemon_SetCuteMedal(Pokemon *pokemon, u8 value)
{
    PokemonExpandStatus *status;

    if (pokemon == 0) {
        return;
    }
    status = pokemonBiosGetExpandStatusPtr(pokemon, 1);
    if (status == 0) {
        return;
    }
    status->cute = value;
}

void pokemon_SetBeautifulMedal(Pokemon *pokemon, u8 value)
{
    PokemonExpandStatus *status;

    if (pokemon == 0) {
        return;
    }
    status = pokemonBiosGetExpandStatusPtr(pokemon, 1);
    if (status == 0) {
        return;
    }
    status->beautiful = value;
}

void pokemon_SetStyleMedal(Pokemon *pokemon, u8 value)
{
    PokemonExpandStatus *status;

    if (pokemon == 0) {
        return;
    }
    status = pokemonBiosGetExpandStatusPtr(pokemon, 1);
    if (status == 0) {
        return;
    }
    status->style = value;
}

// Contest ribbon setters. Each one writes a single field of the ribbon word of
// the Pokemon and leaves the remaining ribbons alone; a null Pokemon or a
// ribbon word the accessor cannot resolve leaves the call without effect.

void pokemon_SetAmariRibbon(Pokemon *pokemon, u8 value)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->amari = value;
}

void pokemon_SetWorldRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->world = flag;
}

void pokemon_SetEarthRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->earth = flag;
}

void pokemon_SetNationalRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->national = flag;
}

void pokemon_SetCountryRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->country = flag;
}

void pokemon_SetSkyRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->sky = flag;
}

void pokemon_SetLandRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->land = flag;
}

void pokemon_SetMarineRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->marine = flag;
}

void pokemon_SetGanbaRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->ganba = flag;
}

void pokemon_SetBromideRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->bromide = flag;
}

void pokemon_SetVictoryRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->victory = flag;
}

void pokemon_SetWinningRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->winning = flag;
}

void pokemon_SetChampRibbon(Pokemon *pokemon, bool flag)
{
    PokemonRibbon *ribbon;

    if (pokemon == 0) {
        return;
    }
    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) {
        return;
    }
    ribbon->champ = flag;
}

// Contest condition of a Pokemon. The fur sheen is its own byte of the record;
// the five contest categories are the first record of the expand status, one
// byte per category.

void pokemon_SetFur(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mFur = value;
}

void pokemon_SetStrong(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    PokemonExpandStatus *status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) {
        return;
    }
    status->strong = value;
}

void pokemon_SetClever(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    PokemonExpandStatus *status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) {
        return;
    }
    status->clever = value;
}

void pokemon_SetCute(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    PokemonExpandStatus *status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) {
        return;
    }
    status->cute = value;
}

void pokemon_SetBeautiful(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    PokemonExpandStatus *status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) {
        return;
    }
    status->beautiful = value;
}

void pokemon_SetStyle(Pokemon *pokemon, u8 value)
{
    if (pokemon == 0) {
        return;
    }
    PokemonExpandStatus *status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) {
        return;
    }
    status->style = value;
}

// Friendship of a Pokemon, capped at its maximum value.

void pokemon_SetFriend(Pokemon *pokemon, u16 value)
{
    if (pokemon == 0) {
        return;
    }
    if (value > POKEMON_FRIEND_LEVEL_MAX) {
        value = POKEMON_FRIEND_LEVEL_MAX;
    }
    pokemon->mFriendLevel = value;
}

// Individual values, effort values and calculated battle stats of a Pokemon.
// Each setter forwards to the pokemonBios setter of the same stat.

void pokemon_SetNimblenessRnd(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetNimblenessRnd(pokemon, value);
}

void pokemon_SetSpeDefRnd(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetSpeDefRnd(pokemon, value);
}

void pokemon_SetSpeAtkRnd(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetSpeAtkRnd(pokemon, value);
}

void pokemon_SetPhyDefRnd(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPhyDefRnd(pokemon, value);
}

void pokemon_SetPhyAtkRnd(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPhyAtkRnd(pokemon, value);
}

void pokemon_SetMaxHpRnd(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetMaxHpRnd(pokemon, value);
}

void pokemon_SetNimblenessEffort(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetNimblenessEffort(pokemon, value);
}

void pokemon_SetSpeDefEffort(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetSpeDefEffort(pokemon, value);
}

void pokemon_SetSpeAtkEffort(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetSpeAtkEffort(pokemon, value);
}

void pokemon_SetPhyDefEffort(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPhyDefEffort(pokemon, value);
}

void pokemon_SetPhyAtkEffort(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPhyAtkEffort(pokemon, value);
}

void pokemon_SetMaxHpEffort(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetMaxHpEffort(pokemon, value);
}

void pokemon_SetNimbleness(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetNimbleness(pokemon, value);
}

void pokemon_SetSpeDef(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetSpeDef(pokemon, value);
}

void pokemon_SetSpeAtk(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetSpeAtk(pokemon, value);
}

void pokemon_SetPhyDef(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPhyDef(pokemon, value);
}

void pokemon_SetPhyAtk(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetPhyAtk(pokemon, value);
}

void pokemon_SetMaxHp(Pokemon *pokemon, u16 value)
{
    pokemonBiosSetMaxHp(pokemon, value);
}

// Current HP of a Pokemon. The stored value may not leave the record above
// the maximum HP the battle stats give it.

void pokemon_SetHp(Pokemon *pokemon, u16 hp)
{
    if (pokemon == 0) {
        return;
    }
    pokemon->mHp = hp;
    u16 maxHp = pokemonBiosGetMaxHp(pokemon);
    if (maxHp < pokemon->mHp) {
        pokemon->mHp = maxHp;
    }
}

}
