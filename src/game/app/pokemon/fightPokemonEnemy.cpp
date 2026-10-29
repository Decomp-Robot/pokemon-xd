#include <fightPokemon.h>
#include <fightPokemonBios.h>
#include <fightPokemonEnemy.h>
#include <types.h>

extern "C" {

/*
 * A deployed Pokemon records the opponents it has fought in a short history:
 * each entry holds the opponent, the move it answered with, the hp it entered
 * the fight with and the damage it has taken since. The history is searched by
 * opponent, filled from the front and cleared when a Pokemon is deployed.
 */

FightOutPokemonEnemy *fightOutPokemonEnemySearchAry(FightOutPokemonEnemy *enemyAry, u16 count,
                                                    FightOutPokemon *target) {
    u16 i;

    if (enemyAry == 0) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        if (fightOutPokemonEnemyCheckValid(&enemyAry[i]) &&
            fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(&enemyAry[i]) == target) {
            return &enemyAry[i];
        }
    }
    return 0;
}

bool fightOutPokemonEnemyCheckValid(FightOutPokemonEnemy *enemy) {
    return fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(enemy) != 0;
}

void fightOutPokemonEnemyCreate(FightOutPokemonEnemy *enemy, FightOutPokemon *target) {
    u16 initHp;

    if (enemy == 0) {
        return;
    }
    if (target == 0) {
        return;
    }
    fightOutPokemonEnemyInit(enemy);
    fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(enemy, target);
    initHp = pokemon_GetHp(fightOutPokemonGetPokemonPtr(target));
    fightOutPokemonEnemyBiosSetInitHp(enemy, initHp);
}

void fightOutPokemonEnemyInitAry(FightOutPokemonEnemy *enemyAry, u16 count) {
    u16 i;

    if (enemyAry == 0) {
        return;
    }
    for (i = 0; i < count; i++) {
        fightOutPokemonEnemyInit(&enemyAry[i]);
    }
}

void fightOutPokemonEnemyInit(FightOutPokemonEnemy *enemy) {
    fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(enemy, 0);
    fightOutPokemonEnemyBiosSetOumuWazaDataId(enemy, 0);
    fightOutPokemonEnemyBiosSetInitHp(enemy, 0);
    fightOutPokemonEnemyBiosSetDamage(enemy, 0);
}

}
