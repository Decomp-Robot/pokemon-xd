#include <deck.h>
#include <deckDataVirtualBios.h>
#include <fightEncountBios.h>
#include <fightSeq.h>
#include <fightTarget.h>
#include <waza.h>
#include <fightTrainerAiCombo.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightTrainerAi.h>
#include <pokemonDB.h>

struct FIGHT_AI_FIX_ACT {
    bool mIsKeepWaza;
    u8 *mScript[2];
};

static const u8 FIGHT_AI_FIX_SCRIPT_SCAN_MAX = 10;

static u8 fixAct_chaseFirst_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_REPEAT
};
static u8 fixAct_freeLead_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_freeLead_2[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_leadThenFirst_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_secondThenFirst_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_OPPONENT_SECOND,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_DONE
};

u8 fixAct_v10_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_REPEAT
};

static u8 fixAct_firstThenFree_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_freeLoop_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_REPEAT
};
static u8 fixAct_freeTriple_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_freeTriple_2[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_freeLoop_2[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_REPEAT
};
static u8 fixAct_freePair_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
static u8 fixAct_freeTripleLoop_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_REPEAT
};
static u8 fixAct_freeTripleLoop_2[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_REPEAT
};

u8 fixAct_oore1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_PARTNER,
    FIGHT_AI_FIX_WAZA_DONE, FIGHT_AI_FIX_DEF_CLEAR
};

namespace {

extern u8 fixAct_sweepFirst_1[];
extern u8 fixAct_mixedKeep_1[];
extern u8 fixAct_mixedKeep_2[];
extern u8 fixAct_slot1Stall_1[];

} // namespace

FIGHT_AI_FIX_ACT fixAiShell[FIGHT_AI_FIX_ACT_ID_COUNT] = {
    /* FIGHT_AI_FIX_ACT_ID_NONE */ {false, {NULL, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_CHASE_FIRST */ {false, {fixAct_chaseFirst_1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_LEAD */ {false, {fixAct_freeLead_1, fixAct_freeLead_2}},
    /* FIGHT_AI_FIX_ACT_ID_SWEEP_FIRST */ {false, {fixAct_sweepFirst_1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_LEAD_THEN_FIRST */ {false, {fixAct_leadThenFirst_1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_FIRST_LOOP */ {false, {fixAct_v10_1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_FIRST_THEN_FREE */ {false, {fixAct_firstThenFree_1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_LOOP */ {true, {fixAct_freeLoop_1, fixAct_freeLoop_1}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_TRIPLE */ {false, {fixAct_freeTriple_1, fixAct_freeTriple_2}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_LOOP_2 */ {true, {fixAct_freeLoop_2, fixAct_freeLoop_2}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_PAIR */ {false, {fixAct_freePair_1, fixAct_freePair_1}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_TRIPLE_LOOP */ {false, {fixAct_freeTripleLoop_1, fixAct_freeTripleLoop_1}},
    /* FIGHT_AI_FIX_ACT_ID_MIXED_KEEP */ {true, {fixAct_mixedKeep_1, fixAct_mixedKeep_2}},
    /* FIGHT_AI_FIX_ACT_ID_SLOT1_STALL */ {false, {fixAct_slot1Stall_1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_FREE_TRIPLE_LOOP_2 */ {false, {fixAct_freeTripleLoop_2, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_OORE */ {false, {fixAct_oore1, NULL}},
    /* FIGHT_AI_FIX_ACT_ID_SECOND_THEN_FIRST */ {false, {fixAct_secondThenFirst_1, NULL}},
};

namespace {

u8 fixAct_sweepFirst_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_4, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_DONE
};
u8 fixAct_mixedKeep_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_4, FIGHT_AI_FIX_DEF_OPPONENT_SECOND,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_4, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_4, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE
};
u8 fixAct_mixedKeep_2[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_OPPONENT_SECOND,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_SLOT_3, FIGHT_AI_FIX_DEF_OPPONENT_FIRST,
    FIGHT_AI_FIX_WAZA_DONE
};
u8 fixAct_slot1Stall_1[] = {
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_1, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_SLOT_2, FIGHT_AI_FIX_DEF_FREE,
    FIGHT_AI_FIX_WAZA_DONE, FIGHT_AI_FIX_DEF_CLEAR
};

} // namespace

void fightAi_FixedActIrekaeInit(FightOutPokemon *pokemon)
{
    FightEncountRecord *encount = fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(0));
    u16 deckDataId = deckDataVirtualDB_GetEnemyDeck(fightEncountDataBiosGetDiskId(encount));
    DeckTrainer *deckTrainer = DeckTrainer::getDeckTrainerObj(deckDataId);
    u8 fixActId = deckTrainer->getFixedActionID();

    if (!fixAiShell[fixActId].mIsKeepWaza) {
        fightOutPokemon_SetFixAiWaza(pokemon, FIGHT_AI_FIX_WAZA_DONE);
    }
}

bool fightAi_FixedActCheck(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 wazaDataId,
    FightOutPokemon *target)
{
    FightTrainerAiWazaHitFunc hitFunc = wazaDB_GetFightTrainerAiWazaHitFuncPtr(wazaDataId);

    if (trainer == NULL) {
        return false;
    }
    if (pokemon == NULL) {
        return false;
    }
    if (target == NULL) {
        return false;
    }
    if (hitFunc(trainer, pokemon, wazaDataId, target) == 0) {
        return false;
    }
    if (fightOutPokemonIsHinsi(target) == 1) {
        return false;
    }
    return fightAi_CanMoveAppointWaza(trainer, pokemon, wazaDataId) != 0;
}

FightOutPokemon *fightAi_GetWazaDataIDFromFixActDef(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u8 fixAiDef)
{
    u16 fightType = fightFloor_GetFightType(0);

    switch (fixAiDef) {
    case FIGHT_AI_FIX_DEF_SELF:
        return pokemon;
    case FIGHT_AI_FIX_DEF_PARTNER:
        return (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE, pokemon,
            fightType);
    case FIGHT_AI_FIX_DEF_OPPONENT_FIRST:
        return (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON, pokemon,
            fightType);
    case FIGHT_AI_FIX_DEF_OPPONENT_SECOND:
        return (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_SECOND_POKEMON, pokemon,
            fightType);
    }
    return NULL;
}

u16 fightAi_GetWazaDataIDFromFixActWaza(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u8 fixAiWaza)
{
    switch (fixAiWaza) {
    case FIGHT_AI_FIX_WAZA_SLOT_1:
    case FIGHT_AI_FIX_WAZA_SLOT_2:
    case FIGHT_AI_FIX_WAZA_SLOT_3:
    case FIGHT_AI_FIX_WAZA_SLOT_4:
        return pokemon_GetPokemonWazaDataId(fightOutPokemonGetPokemonPtr(pokemon), fixAiWaza - 1);
    }
    return 0;
}

void fightAi_FixedAct(FIGHT_TRAINER *trainer)
{
    s32 scriptIndex;
    u8 i;
    u8 j;
    u8 k;
    u8 waza;
    u8 def;
    u8 turnCount = fightFloor_GetTurnCount(0);
    u8 scriptCount[2] = {0, 0};
    FightEncountRecord *encount = fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(0));
    u16 deckDataId;

    if (fightFloorIsNowEncountVirtualBattle(0) == 1) {
        u16 virtualDeckId = deckDataVirtualDB_GetEnemyDeck(fightEncountDataBiosGetDiskId(encount));
        deckDataId = virtualDeckId;
    } else {
        deckDataId = fightEncountDataBiosGetFightTrainerDataId(encount, 1);
    }
    DeckTrainer *deckTrainer = DeckTrainer::getDeckTrainerObj(deckDataId);
    u8 fixActId = deckTrainer->getFixedActionID();
    u8 *scripts[2] = {NULL, NULL};

    scripts[0] = fixAiShell[fixActId].mScript[0];
    scripts[1] = fixAiShell[fixActId].mScript[1];

    for (i = 0; i < 2; i++) {
        if (scripts[i] == NULL) {
            continue;
        }
        for (j = 0; j < FIGHT_AI_FIX_SCRIPT_SCAN_MAX; j++) {
            scriptCount[i]++;
            u8 command = scripts[i][j];
            if (command == FIGHT_AI_FIX_WAZA_DONE) {
                break;
            }
            if (command == FIGHT_AI_FIX_WAZA_REPEAT) {
                break;
            }
        }
    }

    scriptIndex = (turnCount - 1) * 2;

    for (k = 0; k < 2; k++) {
        FightOutPokemon *pokemon = fightTrainer_GetFightoutPokemonPtr(trainer, k);

        if (fightOutPokemon_GetFixAiWaza(pokemon) == FIGHT_AI_FIX_WAZA_DONE) {
            continue;
        }
        fightOutPokemon_SetFixAiDef(pokemon, FIGHT_AI_FIX_DEF_CLEAR);

        u8 *script = scripts[k];

        if (script == NULL) {
            continue;
        }
        s32 commandCount = scriptCount[k] - 1;
        u8 readIndex;

        if (scriptIndex < commandCount) {
            readIndex = scriptIndex;
        } else if (commandCount == scriptIndex) {
            readIndex = scriptIndex;
            u8 command = script[readIndex];
            if (command == FIGHT_AI_FIX_WAZA_DONE) {
                fightOutPokemon_SetFixAiWaza(pokemon, FIGHT_AI_FIX_WAZA_DONE);
                continue;
            }
            if (command == FIGHT_AI_FIX_WAZA_REPEAT) {
                readIndex = scriptIndex % commandCount;
            }
        } else {
            readIndex = scriptIndex % commandCount;
        }

        waza = script[readIndex];
        def = script[readIndex + 1];

        switch (waza) {
        case FIGHT_AI_FIX_WAZA_CLEAR:
            fightOutPokemon_SetFixAiWaza(pokemon, FIGHT_AI_FIX_WAZA_DONE);
            break;
        case FIGHT_AI_FIX_WAZA_SLOT_1:
        case FIGHT_AI_FIX_WAZA_SLOT_2:
        case FIGHT_AI_FIX_WAZA_SLOT_3:
        case FIGHT_AI_FIX_WAZA_SLOT_4:
        case FIGHT_AI_FIX_WAZA_FREE:
            fightOutPokemon_SetFixAiWaza(pokemon, waza);
            break;
        case FIGHT_AI_FIX_WAZA_DONE:
        case FIGHT_AI_FIX_WAZA_REPEAT:
            break;
        }

        switch (def) {
        case FIGHT_AI_FIX_DEF_CLEAR:
            fightOutPokemon_SetFixAiWaza(pokemon, FIGHT_AI_FIX_WAZA_DONE);
            break;
        case FIGHT_AI_FIX_DEF_SELF:
        case FIGHT_AI_FIX_DEF_PARTNER:
        case FIGHT_AI_FIX_DEF_OPPONENT_FIRST:
        case FIGHT_AI_FIX_DEF_OPPONENT_SECOND:
        case FIGHT_AI_FIX_DEF_FREE:
            fightOutPokemon_SetFixAiDef(pokemon, def);
            break;
        }
    }
}

s16 fightAiCombo_GetValidValue(FightOutPokemon **allies, u16 allyCount, FightOutPokemon **enemies, u16 enemyCount)
{
    s16 value = FIGHT_AI_RATE_NEUTRAL;
    FightOutPokemon *allyFirst = NULL;
    FightOutPokemon *allySecond = NULL;
    FightOutPokemon *enemyFirst = NULL;
    FightOutPokemon *enemySecond = NULL;
    u8 i;

    for (i = 0; i < allyCount; i++) {
        if (allies[i] == NULL && allies[fightAi_getAnotherPokeNo(i)] != NULL) {
            allyFirst = allies[fightAi_getAnotherPokeNo(i)];
            allySecond = allies[fightAi_getAnotherPokeNo(i)];
            continue;
        }
        if (allies[i] != NULL && allies[fightAi_getAnotherPokeNo(i)] == NULL) {
            allyFirst = allies[i];
            allySecond = allies[i];
            continue;
        }
        if (fightFloorCmpfightOutPokemonNimbleness(0, allies[i],
                allies[fightAi_getAnotherPokeNo(i)], 0) == 1) {
            allyFirst = allies[i];
            allySecond = allies[fightAi_getAnotherPokeNo(i)];
        } else {
            allyFirst = allies[fightAi_getAnotherPokeNo(i)];
            allySecond = allies[i];
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] == NULL || enemies[fightAi_getAnotherPokeNo(i)] != NULL) {
            enemyFirst = enemies[fightAi_getAnotherPokeNo(i)];
            enemySecond = enemies[fightAi_getAnotherPokeNo(i)];
            continue;
        }
        if (enemies[i] != NULL || enemies[fightAi_getAnotherPokeNo(i)] == NULL) {
            enemyFirst = enemies[i];
            enemySecond = enemies[i];
            continue;
        }
        if (fightFloorCmpfightOutPokemonNimbleness(0, enemies[i],
                enemies[fightAi_getAnotherPokeNo(i)], 0) == 1) {
            enemyFirst = enemies[i];
            enemySecond = enemies[fightAi_getAnotherPokeNo(i)];
        } else {
            enemyFirst = enemies[fightAi_getAnotherPokeNo(i)];
            enemySecond = enemies[i];
        }
    }
    if (fightFloorCmpfightOutPokemonNimbleness(0, enemySecond, allySecond, 0) == 0) {
        value = fightTrainerAiAddValueByAddsubDataId(value,
            fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allySecond),
            AI_ADDSUB_COMBO_ALLY_ACT_FIRST);
    }
    if (fightFloorCmpfightOutPokemonNimbleness(0, enemyFirst, allyFirst, 0) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value,
            fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allySecond),
            AI_ADDSUB_COMBO_ENEMY_ACT_FIRST);
    }
    fightFloorCmpfightOutPokemonNimbleness(0, allySecond, enemyFirst, 0);
    return value;
}
