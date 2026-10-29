#include <types.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <fightTrainer.h>
#include <fightTrainerAi.h>
#include <pokemonDB.h>

// Opponents that Perish Song can still be sung on, counted by the fight-out
// traversal below.  The trainer supplies the AI data the eligibility queries
// are answered with.
typedef struct {
    FIGHT_TRAINER *mTrainer;
    u32 mCount;
} HOROBINOUTA_CHECK_WORK;

// Anyone still in the battle on the opposing side who is not counting down
// already and has no soundproof ability can be sung at.  Traversal always
// keeps walking the fight-out list.
extern "C" u8 _fightTrainerAiCheckHorobinoutaSub(void *target, u16 fightType, void *argument)
{
    HOROBINOUTA_CHECK_WORK *work = (HOROBINOUTA_CHECK_WORK *)argument;
    FightOutPokemon *pokemon = (FightOutPokemon *)target;
    FIGHT_TRAINER *trainer = work->mTrainer;

    if (!fightOutPokemonCheckFightOut(pokemon)) {
        return 1;
    }
    if (fightTrainerIsAllyFightTargetPtr(trainer, pokemon, fightType) == 1) {
        return 1;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_ID_PERISH_SONG) != 1
        && fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_SOUNDPROOF) != 1) {
        work->mCount++;
    }
    return 1;
}

u16 fightTrainerAiCheckHorobinouta(FIGHT_TRAINER *trainer)
{
    HOROBINOUTA_CHECK_WORK work;

    work.mTrainer = trainer;
    work.mCount = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightTrainerAiCheckHorobinoutaSub, &work, 0);
    return work.mCount;
}
