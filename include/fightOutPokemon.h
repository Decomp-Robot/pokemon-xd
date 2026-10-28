#ifndef FIGHT_OUT_POKEMON_H
#define FIGHT_OUT_POKEMON_H

#include <types.h>
#include <fightWaza.h>
#include <itemFightItem.h>

// Deployed Pokemon record owned by the battle module.
typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;

// An opponent a deployed Pokemon has fought: the opponent itself, the move the
// opponent last used, the hp it entered the fight with and the damage it took.
struct FightOutPokemonEnemy {
    FightOutPokemon *mTargetFightOutPokemon;
    u16 mOumuWazaDataId;
    u16 mInitHp;
    u16 mDamage;
};

struct FightOutPokemon {
    FightPokemon *mOriginalPokemon;
    FightPokemon *mCurrentPokemon;
    u8 _008[0x640];
    struct FIGHT_WAZA mFightWaza;
    u8 _6F4[0xAC];
    FIGHT_ITEM mFightItem;
    u8 _7AC[0xB8];
    struct FightOutPokemonEnemy mEnemyAry[4];
};

#endif
