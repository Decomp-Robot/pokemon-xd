#include <types.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokemonDB.h>

extern "C" {

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
static const u16 POKEMON_FRIEND_LEVEL_MAX = 255;

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
