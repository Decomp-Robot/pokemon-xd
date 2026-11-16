#include <app/toolentry/toolentry.h>
#include <GSmem.h>
#include <Pokemon.h>
#include <hero.h>
#include <menuCBBios.h>
#include <savedata.h>
#include <toolentryTaisen.h>
#include <libc/memory.h>
#include <types.h>

/*
 * toolentry.cpp
 *
 * Party setup of a link (GBA) battle. Every controller port owns a trainer
 * record that holds the hero the party is prepared in and the order the
 * Pokemon are entered in; the battle itself reads the copies that
 * toolentryTaisenEntryPokemon leaves behind.
 */

/* Size of the battle party work block. */
#define TOOLENTRY_BATTLE_WORK_SIZE 112

/* Empty slot left by the GBA entry order. */
#define TOOLENTRY_ORDER_NONE 0xF

/* Work block of the battle party data; null while none is held. */
static void *_toolentryTaisen;

Pokemon *_toolentryDebugPokemon;

extern "C" {
void toolentryTaisenInit(void);
s32 toolentryTaisenSetPokemon(Pokemon *pokemon, s32 port, s32 index);
s32 toolentryTaisenSetHeroFromSaveData(s32 port);
u8 toolentryTaisenGetPokemonFlag(s32 port, s32 index);
s32 toolentryTaisenGetBattleMode(void);
Hero *toolentryTaisenGetEntryHeroPtr(s32 port);
s32 toolentryTaisenGetEtnryPokemonOrder(s32 port, u32 index);
void toolentryTaisenCreateBattlePokemonData(void);
void toolentryTaisenFreePokemonData(void);

void heroInit(Hero *hero);
}

/* Work block of a requested size, aligned to 32 bytes. */
void *_toolentryAlloc(u32 size)
{
    return GSmemAllocAlign((size + 0x1F) & ~0x1F, 32);
}

void _toolentryFree(void *ptr)
{
    GSmemFree(ptr);
}

_MENUCBWORK *_toolentryGetCBWork(void)
{
    return menuCBBios_WorkContext();
}

/* Hero the party of a port is prepared in. */
Hero *_toolentryTaisenGetHeroPtr(s32 port)
{
    return &menuCBBios_GetTrainerImmediately(port)->mBattleHero;
}

/* Hero a port has entered so far. */
Hero *_toolentryTaisenGetEntryHeroPtr(s32 port)
{
    return &menuCBBios_GetTrainerImmediately(port)->mHero;
}

Pokemon *_toolentryTaisenGetPokemonPtr(s32 port, s32 index)
{
    return heroBiosGetPokemonPtr(&menuCBBios_GetTrainerImmediately(port)->mBattleHero, index);
}

Pokemon *_toolentryTaisenGetEntryPokemonPtr(s32 port, s32 index)
{
    return heroBiosGetPokemonPtr(&menuCBBios_GetTrainerImmediately(port)->mHero, index);
}

/* Whether the party slot of a port holds a pokemon that may be entered. */
u8 _toolentryTaisenGetPokemonFlag(s32 port, s32 index)
{
    u8 valid = pokemonCheckValid(heroBiosGetPokemonPtr(&menuCBBios_GetTrainerImmediately(port)->mBattleHero, index)) != 0;
    return valid;
}

/* Empties the party and the entered party of every port. */
void toolentryTaisenInit(void)
{
    s32 i;
    s32 port;

    for (port = 0; port < 4; port++) {
        heroInit(_toolentryTaisenGetHeroPtr(port));
        for (i = 0; i < 6; i++) {
            pokemonInit(_toolentryTaisenGetPokemonPtr(port, i));
        }
    }
    for (port = 0; port < 4; port++) {
        heroInit(_toolentryTaisenGetEntryHeroPtr(port));
        for (i = 0; i < 6; i++) {
            pokemonInit(_toolentryTaisenGetEntryPokemonPtr(port, i));
        }
    }
}

/* Copies an incoming pokemon into the party of a port. */
s32 toolentryTaisenSetPokemon(Pokemon *pokemon, s32 port, s32 index)
{
    Pokemon *dst = _toolentryTaisenGetPokemonPtr(port, index);

    if (dst != NULL) {
        memcpy(dst, pokemon, sizeof(Pokemon));
        return 1;
    }
    return 0;
}

/* Prepares the party of a port from the hero stored in the save data. */
s32 toolentryTaisenSetHeroFromSaveData(s32 port)
{
    void *status = savedataGetStatus(NULL, SAVE_DATA_HERO);
    Hero *hero = _toolentryTaisenGetHeroPtr(port);

    if (status != NULL) {
        memcpy(hero, status, sizeof(Hero));
        return 1;
    }
    return 0;
}

u8 toolentryTaisenGetPokemonFlag(s32 port, s32 index)
{
    return _toolentryTaisenGetPokemonFlag(port, index);
}

s32 toolentryTaisenGetBattleType(void)
{
    return _toolentryGetCBWork()->battleMode;
}

/* Number of players a battle of the current battle mode is fought with. */
s32 toolentryTaisenGetEntryPlayerNum(void)
{
    TaisenBattleType type = (TaisenBattleType)toolentryTaisenGetBattleType();
    s32 num = 2;
    switch (type) {
    case TAISEN_BATTLE_DOUBLE:
    case TAISEN_BATTLE_SINGLE:
        num = 2;
        break;
    case TAISEN_BATTLE_MULTI:
        num = 4;
        break;
    }
    return num;
}

u16 toolentryTaisenGetBattlePlayerID(s32 port)
{
    return menuCBTrainerBios_GetPlayerID(menuCBBios_GetTrainerImmediately(port));
}

u16 toolentryTaisenGetHomePlace(s32 port)
{
    return menuCBTrainerBios_GetHomePlace(menuCBBios_GetTrainerImmediately(port));
}

s32 toolentryTaisenGetBattleVersus(void)
{
    return _toolentryGetCBWork()->mStartMenuType;
}

s32 toolentryTaisenGetBattleMode(void)
{
    return _toolentryGetCBWork()->connectMode;
}

Pokemon *toolentryTaisenGetPokemonPtr(s32 bank, u32 index)
{
    return _toolentryTaisenGetPokemonPtr(bank, index);
}

Pokemon *toolentryTaisenGetEntryPokemonPtr(s32 port, s32 index)
{
    return _toolentryTaisenGetEntryPokemonPtr(port, index);
}

Hero *toolentryTaisenGetHeroPtr(s32 port)
{
    return _toolentryTaisenGetHeroPtr(port);
}

Hero *toolentryTaisenGetEntryHeroPtr(s32 port)
{
    return _toolentryTaisenGetEntryHeroPtr(port);
}

/* Number of enterable pokemon the party of a port holds. */
u16 toolentryTaisenGetPokemonNum(s32 port)
{
    u16 count = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        if (toolentryTaisenGetPokemonFlag(port, i) != 0) {
            count++;
        }
    }
    return count;
}

/* Number of pokemon a port may take into the battle. */
u16 toolentryTaisenGetEntryPokemonNum(s32 port)
{
    u16 max = menuCBBios_ParticipationPokemonMax();
    u16 num = toolentryTaisenGetPokemonNum(port);

    return num < max ? num : max;
}

/* Carries the party of every port over to the entered party. */
void toolentryCopyHero(void)
{
    Hero *hero;
    Hero *entry;
    s32 port;

    for (port = 0; port < 4; port++) {
        hero = toolentryTaisenGetHeroPtr(port);
        entry = toolentryTaisenGetEntryHeroPtr(port);

        if (hero != NULL && entry != NULL) {
            *entry = *hero;
        }
    }
}

Hero &Hero::operator=(const Hero &other)
{
    memcpy(this, &other, sizeof(Hero));
    return *this;
}

void toolentryTaisenInitPokemonOrder(s32 port)
{
    _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
    s32 i;

    trainer->mEntryCount = 0;
    for (i = 0; i < 6; i++) {
        trainer->mEntryList[i] = -1;
    }
}

/* Appends a party slot to the entry order of a port and returns its position. */
s32 toolentryTaisenSetEtnryPokemonOrder(s32 port, s32 index)
{
    _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
    s32 count = trainer->mEntryCount;
    s32 i;

    if (trainer->mEntryCount >= toolentryTaisenGetEntryPokemonNum(port)) {
        return -1;
    }
    for (i = 0; i < count; i++) {
        if (trainer->mEntryList[i] == index) {
            return -1;
        }
    }
    trainer->mEntryList[count] = index;
    trainer->mEntryCount++;
    return count;
}

/* Takes the whole entry order over from the order a GBA sent. */
s32 toolentryTaisenSetEtnryPokemonOrderGBA(s32 port, s32 num, u32 *pokemon)
{
    _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
    s32 count = 0;
    s32 i;

    for (i = 0; i < num; i++) {
        trainer->mEntryList[i] = pokemon[i];
        count++;
    }
    trainer->mEntryCount = count;
    return count;
}

/* Drops the last entry of a port and returns the remaining count. */
s32 toolentryTaisenDeleteEtnryPokemonOrder(s32 port)
{
    _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
    s32 last = trainer->mEntryCount - 1;

    if (last < 0 || last > 6) {
        return 0;
    }
    trainer->mEntryList[last] = -1;
    trainer->mEntryCount--;
    return trainer->mEntryCount;
}

s32 toolentryTaisengetEtnryPokemonOrderNum(s32 port)
{
    return menuCBBios_GetTrainerImmediately(port)->mEntryCount;
}

s32 toolentryTaisenGetEtnryPokemonOrder(s32 port, u32 index)
{
    _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
    return trainer->mEntryList[index];
}

void toolentryTaisenCreateBattlePokemonData(void)
{
    toolentryTaisenFreePokemonData();
    _toolentryTaisen = _toolentryAlloc(TOOLENTRY_BATTLE_WORK_SIZE);
}

void toolentryTaisenFreePokemonData(void)
{
    if (_toolentryTaisen != NULL) {
        _toolentryFree(_toolentryTaisen);
        _toolentryTaisen = NULL;
    }
}

/* Fills the entered party of a port with the party in the order it was entered
   and drops the entries that no longer have a party slot behind them. */
void toolentryTaisenEntryPokemon(s32 port)
{
    s32 i;
    s32 num = toolentryTaisenGetEntryPokemonNum(port);
    s32 order;
    Hero *entryHero = _toolentryTaisenGetEntryHeroPtr(port);
    Hero *hero = _toolentryTaisenGetHeroPtr(port);
    Pokemon *pokemon;
    Pokemon *entry;

    *entryHero = *hero;
    for (i = 0; i < 6; i++) {
        pokemonInit(_toolentryTaisenGetEntryPokemonPtr(port, i));
        order = toolentryTaisenGetEtnryPokemonOrder(port, i);
        if (order == TOOLENTRY_ORDER_NONE || order == -1) {
            if (num > i) {
                num = i;
            }
        }
        else {
            pokemon = _toolentryTaisenGetPokemonPtr(port, i);
            entry = _toolentryTaisenGetEntryPokemonPtr(port, i);
            if (entry != NULL && pokemon != NULL) {
                *entry = *pokemon;
            }
        }
    }
    for (i = 0; i < num; i++) {
        pokemon = _toolentryTaisenGetPokemonPtr(port, toolentryTaisenGetEtnryPokemonOrder(port, i));
        entry = _toolentryTaisenGetEntryPokemonPtr(port, i);
        if (entry != NULL && pokemon != NULL) {
            *entry = *pokemon;
        }
    }
}

Pokemon &Pokemon::operator=(const Pokemon &other)
{
    memcpy(this, &other, sizeof(Pokemon));
    return *this;
}
