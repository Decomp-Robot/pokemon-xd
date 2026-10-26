#ifndef FIGHT_OUT_POKEMON_H
#define FIGHT_OUT_POKEMON_H

#include <types.h>
#include <fightWaza.h>
#include <itemFightItem.h>

// Deployed Pokemon record owned by the battle module.
typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;

struct FightOutPokemon {
    FightPokemon *mOriginalPokemon;
    FightPokemon *mCurrentPokemon;
    u8 _008[0x640];
    struct FIGHT_WAZA mFightWaza;
    u8 _6F4[0xAC];
    FIGHT_ITEM mFightItem;
    u8 _7AC[0xE8];
};

#endif
