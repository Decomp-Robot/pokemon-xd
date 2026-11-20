#include <fightPokemonBios.h>
#include <types.h>
#include <waza.h>

extern "C" {

/*
 * A deployed Pokemon remembers the moves used by Mirror Move and Sketch, the
 * damage accumulated while Bide charges and the moves from its latest fights.
 * A missing record has no remembered move; its type reads as Mystery and the
 * other values read as zero.
 */

void fightOutPokemonBiosSetOumuWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mOumuWazaDataId = wazaDataId;
}

void fightOutPokemonBiosSetGamanDamageTargetId(FightOutPokemon *pokemon, u16 targetDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mGamanDamageTargetId = targetDataId;
}

void fightOutPokemonBiosSetGamanDamageValue(FightOutPokemon *pokemon, s16 damage) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mGamanDamageValue = damage;
}

void fightOutPokemonBiosSetHitWazaZokuseiDataId(FightOutPokemon *pokemon, u16 zokuseiDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mHitWazaZokuseiDataId = zokuseiDataId;
}

void fightOutPokemonBiosSetHitWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mHitWazaDataId = wazaDataId;
}

void fightOutPokemonBiosSetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon,
                                                      u16 targetDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mLastReceiveWazaTargetDataId = targetDataId;
}

void fightOutPokemonBiosSetLastUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mLastUseWazaDataId = wazaDataId;
}

void fightOutPokemonBiosSetLastSelectWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mLastSelectWazaDataId = wazaDataId;
}

void fightOutPokemonBiosSetSketchWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->mSketchWazaDataId = wazaDataId;
}

// The move state kept across turns lives with the deployed record; absent means none.
FIGHT_WAZA *fightOutPokemonBiosGetKeepFightWazaPtr(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return 0;
    }
    return &pokemon->mKeepFightWaza;
}

u16 fightOutPokemonBiosGetOumuWazaDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return WAZA_DATA_ID_NONE;
    }
    return pokemon->mOumuWazaDataId;
}

u16 fightOutPokemonBiosGetGamanDamageTargetId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return 0;
    }
    return pokemon->mGamanDamageTargetId;
}

s16 fightOutPokemonBiosGetGamanDamageValue(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return 0;
    }
    return pokemon->mGamanDamageValue;
}

u16 fightOutPokemonBiosGetHitWazaZokuseiDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return ZOKUSEI_DATA_ID_MYSTERY;
    }
    return pokemon->mHitWazaZokuseiDataId;
}

u16 fightOutPokemonBiosGetHitWazaDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return WAZA_DATA_ID_NONE;
    }
    return pokemon->mHitWazaDataId;
}

u16 fightOutPokemonBiosGetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return 0;
    }
    return pokemon->mLastReceiveWazaTargetDataId;
}

u16 fightOutPokemonBiosGetLastUseWazaDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return WAZA_DATA_ID_NONE;
    }
    return pokemon->mLastUseWazaDataId;
}

u16 fightOutPokemonBiosGetLastSelectWazaDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return WAZA_DATA_ID_NONE;
    }
    return pokemon->mLastSelectWazaDataId;
}

u16 fightOutPokemonBiosGetSketchWazaDataId(FightOutPokemon *pokemon) {
    if (pokemon == 0) {
        return WAZA_DATA_ID_NONE;
    }
    return pokemon->mSketchWazaDataId;
}

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
