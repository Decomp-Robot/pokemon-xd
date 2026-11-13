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

}
