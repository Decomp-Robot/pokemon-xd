#include <GSmsg.h>
#include <Pokemon.h>
#include <exribbon.h>
#include <gamedata.h>
#include <hero.h>
#include <heroBios.h>
#include <types.h>

// Layout of the record the link exchange hands over: the header of the hero
// first, the six Pokemon of its party behind it, and the ribbon slots at the
// end of the record.
static const s32 GBA_HERO_NAME_OFFSET = 4;
static const s32 GBA_HERO_NAME_SIZE = 7;
static const s32 GBA_HERO_SEX_OFFSET = 0xC;
static const s32 GBA_HERO_RND_OFFSET = 0x10;
static const s32 GBA_HERO_POKEMON_OFFSET = 0x14;
static const s32 GBA_POKEMON_RECORD_SIZE = 100;
static const s32 GBA_POKEMON_MAX = 6;
static const s32 GBA_HERO_RIBBON_OFFSET = 0x26C;
static const s32 GBA_RIBBON_MAX = 11;
// The game the GBA runs is packed into bits 10 through 13 of the header.
static const s32 GBA_HERO_GAME_SHIFT = 10;
static const u32 GBA_HERO_GAME_MASK = 0xF;

// Games the link contract knows of.
static const u8 GBA_GAME_SAPPHIRE = 1;
static const u8 GBA_GAME_RUBY = 2;
static const u8 GBA_GAME_EMERALD = 3;
static const u8 GBA_GAME_FIRERED = 4;
static const u8 GBA_GAME_LEAFGREEN = 5;

// The GameCube side keeps three home places, so the games share them in
// groups.
static const u8 HERO_HOME_PLACE_SAPPHIRE_RUBY = 1;
static const u8 HERO_HOME_PLACE_EMERALD_FIRERED = 2;
static const u8 HERO_HOME_PLACE_LEAFGREEN = 3;

struct GbaPokemon;

extern "C" {
void gbaPokemonToGC(Pokemon *pokemon, GbaPokemon *gbaRecord);
void pokemon_SetFightTrainerPokemonDataId(Pokemon *pokemon, u16 dataId);
}

static u32 _swap32(u32 value);

// Imports the record a GBA game hands over: the home place, name, sex and RND
// of the hero, then the Pokemon and the ribbons stored behind them.
extern "C" void pokeconv(Hero *hero, u8 *packet)
{
    u16 name[GBA_HERO_NAME_SIZE + 1];
    u32 header;
    u32 gameId;
    s32 i;

    header = (packet[3] << 24) | (packet[2] << 16) | (packet[1] << 8) | packet[0];
    gameId = (header >> GBA_HERO_GAME_SHIFT) & GBA_HERO_GAME_MASK;
    switch (gameId) {
    case GBA_GAME_SAPPHIRE:
        heroBiosSetHomePlace(hero, HERO_HOME_PLACE_SAPPHIRE_RUBY);
        break;
    case GBA_GAME_RUBY:
        heroBiosSetHomePlace(hero, HERO_HOME_PLACE_SAPPHIRE_RUBY);
        break;
    case GBA_GAME_EMERALD:
        heroBiosSetHomePlace(hero, HERO_HOME_PLACE_EMERALD_FIRERED);
        break;
    case GBA_GAME_FIRERED:
        heroBiosSetHomePlace(hero, HERO_HOME_PLACE_EMERALD_FIRERED);
        break;
    case GBA_GAME_LEAFGREEN:
        heroBiosSetHomePlace(hero, HERO_HOME_PLACE_LEAFGREEN);
        break;
    default:
        heroBiosSetHomePlace(hero, HERO_HOME_PLACE_SAPPHIRE_RUBY);
        break;
    }
    GScharConvToGCCode(name, packet + GBA_HERO_NAME_OFFSET, GBA_HERO_NAME_SIZE,
                       (s32)gamedataGetStatus(0, GAMEDATA_STATUS_LANGUAGE));
    heroBiosSetNamePtr(hero, name);
    heroBiosSetSexDataId(hero, packet[GBA_HERO_SEX_OFFSET]);
    heroBiosSetRnd(hero, _swap32(*(u32 *)(packet + GBA_HERO_RND_OFFSET)));
    for (i = 0; i < GBA_POKEMON_MAX; i++) {
        Pokemon *pokemon = heroBiosGetPokemonPtr(hero, i);

        gbaPokemonToGC(pokemon, (GbaPokemon *)(packet + GBA_HERO_POKEMON_OFFSET +
                                                i * GBA_POKEMON_RECORD_SIZE));
        pokemon_SetFightTrainerPokemonDataId(pokemon, i);
    }
    for (i = 0; i < GBA_RIBBON_MAX; i++) {
        exribbonSetNo(i, packet[GBA_HERO_RIBBON_OFFSET + i]);
    }
}

// The GBA side stores its fields little endian, the GameCube side big endian.
static u32 _swap32(u32 value)
{
    return (value << 24) | ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) | (value >> 24);
}
