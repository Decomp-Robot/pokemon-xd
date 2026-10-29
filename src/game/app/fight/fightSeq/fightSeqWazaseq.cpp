#include <Pokemon.h>
#include <fightSeq.h>
#include <fightWaza.h>
#include <fightTarget.h>
#include <fightPokemon.h>
#include <pokemonDB.h>
#include <waza.h>

// Power of Return: it grows with the user's friendship.
extern "C" u16 fightSeqGetOngaesiIryoku(u16 friendship)
{
    return friendship * 10 / 25;
}


// Power of Frustration: it grows as the user's friendship drops.
extern "C" u16 fightSeqGetYatuatariIryoku(u16 friendship)
{
    return (255 - friendship) * 10 / 25;
}


// Return and Frustration work out their power from the attacker's friendship.
extern "C" void WS_NATSUKI(void)
{
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    Pokemon *pokemon;
    u16 sequenceId;
    u16 friendship;
    u16 iryoku;

    (void)fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    sequenceId = wazaDB_GetSeqId(fightOutPokemon_GetUseWazaDataId(attacker));
    friendship = pokemon_GetFriend(pokemon);
    if (sequenceId == WAZA_SEQUENCE_ID_RETURN) {
        iryoku = fightSeqGetOngaesiIryoku(friendship);
    } else {
        iryoku = fightSeqGetYatuatariIryoku(friendship);
    }
    fightWaza_SetIryoku(waza, iryoku);
    fightSeq_NextWazaSeq(1);
}
