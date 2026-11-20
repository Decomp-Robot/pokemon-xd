#ifndef FIGHT_OUT_POKEMON_H
#define FIGHT_OUT_POKEMON_H

#include <types.h>
#include <fightWaza.h>
#include <itemFightItem.h>

// Deployed Pokemon record owned by the battle module.
typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;
struct NullSequence;

// An opponent a deployed Pokemon has fought: the opponent itself, the move the
// opponent last used, the hp it entered the fight with and the damage it took.
struct FightOutPokemonEnemy {
    FightOutPokemon *mTargetFightOutPokemon;
    u16 mOumuWazaDataId;
    u16 mInitHp;
    u16 mDamage;
};

struct FightOutPokemon {
#ifdef __cplusplus
    void setComboFuncId(u8 comboFuncId);
    FightPokemon *getFightPokemonPtr();
#endif
    FightPokemon *mOriginalPokemon;
    FightPokemon *mCurrentPokemon;
    u8 _008[0x640];
    struct FIGHT_WAZA mFightWaza;
    struct FIGHT_WAZA mKeepFightWaza;  // 0x6F4: copy of the live move kept while Bide or Rollout continues across turns.
    FIGHT_ITEM mFightItem;
    struct NullSequence *mSequence;  // 0x7AC: sequence that drives the battle model.
    u8 _7B0[0x68];
    u16 mSketchWazaDataId;  // 0x818: move copied by Sketch.
    u16 mLastSelectWazaDataId;  // 0x81A: most recently selected move.
    u16 mLastUseWazaDataId;  // 0x81C: most recently used move.
    u16 mLastReceiveWazaTargetDataId;  // 0x81E: attacker of the last received move.
    u16 mHitWazaDataId;  // 0x820: move that last landed.
    u16 mHitWazaZokuseiDataId;  // 0x822: type of the move that last landed.
    s16 mGamanDamageValue;  // 0x824: damage accumulated while Bide is charging.
    u16 mGamanDamageTargetId;  // 0x826: target supplying the accumulated Bide damage.
    u16 mOumuWazaDataId;  // 0x828: move remembered by Mirror Move.
    u8 _82A[0x3A];
    struct FightOutPokemonEnemy mEnemyAry[4];
};

#endif
