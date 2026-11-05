#include <types.h>
#include <bool.h>
#include <menu.h>
#include <menuIds.h>
#include <hero.h>
#include <Pokemon.h>
#include <fight.h>
#include <dbgMenuPokemon.h>

#pragma force_active on

void menuDbgItemCreate(void);
void heroAddPokecoupon(Hero *hero, u32 coupon);
void heroAddPokedoru(Hero *hero, u32 pokedoru);
s16 heroCatchPokemon(Hero *hero, const Pokemon *pokemon, u16 catchFloorID, u8 catchBallID, bool allowStorageFallback);

/* Pokemon being built by the add entry. It is initialized the first
 * time the entry runs and then kept between runs. */
static Pokemon pokemon;

/* One-shot init flags for the entries below; the add entry clears
 * the first one after initializing the Pokemon above. */
static u8 debugPokemonNeedsInit[8] = {1};

s32 dbgMenuHeroCreateItem(void)
{
    menuDbgItemCreate();

    return 0;
}

s32 dbgMenuHeroIncCoupon(void)
{
    s32 coupon = menuOpen(MENU_ID_NUMBER_INPUT, 1);

    if (coupon == -1) {
        return 0;
    }
    heroAddPokecoupon(0, coupon);

    return 0;
}

s32 dbgMenuHeroIncMoney(void)
{
    s32 pokedoru = menuOpen(MENU_ID_NUMBER_INPUT, 1);

    if (pokedoru == -1) {
        return 0;
    }
    heroAddPokedoru(0, pokedoru);

    return 0;
}

s32 dbgMenuHeroPokemonAdd(void)
{
    if (fightIsFight() == 1) {
        return -1;
    }
    if (debugPokemonNeedsInit[0] != 0) {
        pokemonInit(&pokemon);
        debugPokemonNeedsInit[0] = 0;
    }
    if (dbgMenuPokemonEditSub(&pokemon, 0, 0, 0, 0, 0) < 0) {
        return -1;
    }
    heroCatchPokemon(0, &pokemon, 0, POKEMON_CATCH_BALL_POKE_BALL, 1);

    return -1;
}

s32 dbgMenuHeroPokemonSelect5(void)
{
    Pokemon *pokemon;

    if (fightIsFight() == 1) {
        return -1;
    }
    pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 5);
    if (pokemon == 0) {
        return -1;
    }

    return dbgMenuPokemonEditSub(pokemon, 0, 0, 0, 0, 0);
}

s32 dbgMenuHeroPokemonSelect4(void)
{
    Pokemon *pokemon;

    if (fightIsFight() == 1) {
        return -1;
    }
    pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 4);
    if (pokemon == 0) {
        return -1;
    }

    return dbgMenuPokemonEditSub(pokemon, 0, 0, 0, 0, 0);
}

s32 dbgMenuHeroPokemonSelect3(void)
{
    Pokemon *pokemon;

    if (fightIsFight() == 1) {
        return -1;
    }
    pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 3);
    if (pokemon == 0) {
        return -1;
    }

    return dbgMenuPokemonEditSub(pokemon, 0, 0, 0, 0, 0);
}

s32 dbgMenuHeroPokemonSelect2(void)
{
    Pokemon *pokemon;

    if (fightIsFight() == 1) {
        return -1;
    }
    pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 2);
    if (pokemon == 0) {
        return -1;
    }

    return dbgMenuPokemonEditSub(pokemon, 0, 0, 0, 0, 0);
}

s32 dbgMenuHeroPokemonSelect1(void)
{
    Pokemon *pokemon;

    if (fightIsFight() == 1) {
        return -1;
    }
    pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 1);
    if (pokemon == 0) {
        return -1;
    }

    return dbgMenuPokemonEditSub(pokemon, 0, 0, 0, 0, 0);
}

s32 dbgMenuHeroPokemonSelect0(void)
{
    Pokemon *pokemon;

    if (fightIsFight() == 1) {
        return -1;
    }
    pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 0);
    if (pokemon == 0) {
        return -1;
    }

    return dbgMenuPokemonEditSub(pokemon, 0, 0, 0, 0, 0);
}

#pragma force_active reset
