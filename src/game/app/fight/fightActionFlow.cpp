#include <fightAction.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightSeq.h>

enum { FIGHT_KOUKA_ID_INITIAL_WEATHER = 1 };

extern "C" {
void fightKoukaDoFightKoukaJoukenAndKouka(void *target, u16 fightKoukaId);
u32 tenkouDataBiosGetFightInitMsgId(u16 weatherId);
}

u8 _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr(void *pokemon, u16, void *result)
{
    if (fightOutPokemon_GetSequencePtr((FightOutPokemon *)pokemon) != 0) {
        if (result != 0) {
            *(FightOutPokemon **)result = (FightOutPokemon *)pokemon;
        }
        return 0;
    }
    return 1;
}

extern "C" u8 fightActionFlowTenkouInit(FIGHT_ACTION *action)
{
    fightKoukaDoFightKoukaJoukenAndKouka(0, FIGHT_KOUKA_ID_INITIAL_WEATHER);
    u32 weatherId = fightFloorGetNowTenkouDataId(0, 0);
    FightOutPokemon *pokemon = 0;
    fightFloorLoopValidFightOutPokemon(0,
        _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr, &pokemon, 0);
    if (weatherId != 0) {
        fightFloor_SetAttackPokemonPtr(0, pokemon);
        u32 messageId = tenkouDataBiosGetFightInitMsgId(weatherId);
        fightFloor_SetAppointMsgPtr(0, messageId);
        fightSeqExec(action, (u8 *)fightActionBiosGetBuffDataPtr(action), 0);
    }
    return 1;
}
