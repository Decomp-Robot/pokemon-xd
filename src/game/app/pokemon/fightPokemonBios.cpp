#include <fightPokemonBios.h>
#include <types.h>

extern "C" {

/*
 * A deployed Pokemon keeps the opponents it has fought in the enemy records of
 * its own record: for each fight the record holds the opponent, the move the
 * opponent answered with, the hp the opponent entered the fight with and the
 * damage it has taken since. A record that is not there answers zero, so a
 * caller can address a slot before a fight has filled it.
 */

void fightOutPokemonEnemyBiosSetDamage(FightOutPokemonEnemy *enemy, u16 damage) {
    if (enemy == 0) {
        return;
    }
    enemy->mDamage = damage;
}

void fightOutPokemonEnemyBiosSetInitHp(FightOutPokemonEnemy *enemy, u16 initHp) {
    if (enemy == 0) {
        return;
    }
    enemy->mInitHp = initHp;
}

void fightOutPokemonEnemyBiosSetOumuWazaDataId(FightOutPokemonEnemy *enemy, u16 wazaDataId) {
    if (enemy == 0) {
        return;
    }
    enemy->mOumuWazaDataId = wazaDataId;
}

void fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(FightOutPokemonEnemy *enemy,
                                                        FightOutPokemon *target) {
    if (enemy == 0) {
        return;
    }
    enemy->mTargetFightOutPokemon = target;
}

u16 fightOutPokemonEnemyBiosGetDamage(FightOutPokemonEnemy *enemy) {
    if (enemy == 0) {
        return 0;
    }
    return enemy->mDamage;
}

u16 fightOutPokemonEnemyBiosGetInitHp(FightOutPokemonEnemy *enemy) {
    if (enemy == 0) {
        return 0;
    }
    return enemy->mInitHp;
}

u16 fightOutPokemonEnemyBiosGetOumuWazaDataId(FightOutPokemonEnemy *enemy) {
    if (enemy == 0) {
        return 0;
    }
    return enemy->mOumuWazaDataId;
}

FightOutPokemon *fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(FightOutPokemonEnemy *enemy) {
    if (enemy == 0) {
        return 0;
    }
    return enemy->mTargetFightOutPokemon;
}

// Where the record of one of the four fights of the Pokemon is kept: a slot no
// fight has taken yet is reported as missing.
FightOutPokemonEnemy *fightOutPokemonBiosGetFightOutPokemonEnemyPtr(FightOutPokemon *pokemon,
                                                                   u16 index) {
    if (pokemon == 0) {
        return 0;
    }
    if (index >= 4) {
        return 0;
    }
    return &pokemon->mEnemyAry[index];
}

}
