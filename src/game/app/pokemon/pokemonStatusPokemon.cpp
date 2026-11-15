#include <Pokemon.h>
#include <pokemonDataBios.h>

const u16 POKEMON_POOL_FRIEND_LEVEL_MAX = 255;

const long POKEMON_DARK_POINT_MAX = 6553500;
const unsigned long POKEMON_RARE_PARAM_LIMIT = 8;

bool Pokemon::checkValid() const
{
    return pokemonCheckValid((Pokemon *)this);
}

void Pokemon::setPoolFriendLevel(u16 level)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        level = level < POKEMON_POOL_FRIEND_LEVEL_MAX ? level : POKEMON_POOL_FRIEND_LEVEL_MAX;
        darkPokemon->setPoolFriendLevel(level);
    }
}

void Pokemon::setPoolExp(unsigned long exp)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setPoolExp(exp);
    }
}

void Pokemon::setDarkPoint(float point)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setDarkPoint(point < POKEMON_DARK_POINT_MAX ? point : POKEMON_DARK_POINT_MAX);
    }
}

void Pokemon::setDarkPointDirect(long point)
{
    DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon != 0) {
        darkPokemon->setDarkPointDirect(point < POKEMON_DARK_POINT_MAX ? point : POKEMON_DARK_POINT_MAX);
    }
}

u16 Pokemon::getPoolFriendLevel() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return 0;
    }
    return darkPokemon->getPoolFriendLevel();
}

unsigned long Pokemon::getPoolExp() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return 0;
    }
    return darkPokemon->getPoolExp();
}

float Pokemon::getDarkPoint() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return -1.0f;
    }
    return darkPokemon->getDarkPoint();
}

long Pokemon::getDarkPointDirect() const
{
    const DarkPokemon *darkPokemon = getDarkPokemon();
    if (darkPokemon == 0) {
        return -1;
    }
    return darkPokemon->getDarkPointDirect();
}

const DarkPokemon *Pokemon::getDarkPokemon() const
{
    return darkPokemonGetDarkPokemon(getDarkpokemonDataID());
}

DarkPokemon *Pokemon::getDarkPokemon()
{
    return darkPokemonGetDarkPokemon(getDarkpokemonDataID());
}

bool Pokemon::isDarkPokemon() const
{
    u16 darkID = getDarkpokemonDataID();
    if (darkID == 0) {
        return false;
    }
    return !darkPokemonBiosGetReliveFlag(darkID);
}

u8 Pokemon::getSex() const
{
    return pokemonGetSex((Pokemon *)this);
}

u8 Pokemon::isTokuseiFlag() const
{
    if (pokemonDataBiosGetTokuseiDataId(getPokemonDataPtr(), 1) == 0) {
        return false;
    }
    return mTokuseiFlag != 0;
}

bool Pokemon::isRareParam(unsigned long trainerRnd, unsigned long pokemonRnd)
{
    return ((trainerRnd >> 16) ^ (trainerRnd & 0xFFFF) ^
            (pokemonRnd >> 16) ^ (pokemonRnd & 0xFFFF)) < POKEMON_RARE_PARAM_LIMIT;
}
