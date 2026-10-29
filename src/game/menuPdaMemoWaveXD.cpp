#include <types.h>
#include <memo.h>
#include <PokemonData.h>
#include <GSsndPlayback.h>
#include "menuPdaMemoListXD.h"
#include "menuPdaMemoWaveXD.h"

void pdaMemoNakigoe::SetNakigoe(long index) {
    s32 voice = GetVocie(index);
    u16 soundID = (u16)voice;

    SetSE(soundID);
    DispRequest(GetWaveDispDataAddress(index), soundID);
}

void pdaMemoNakigoe::SetSE(unsigned long soundID) {
    GSsndPlay(soundID, 0, 0);
}

s32 pdaMemoNakigoe::GetVocie(long index) {
    if (memoDataGetCount(NULL) != 0) {
        PokemonSpeciesStats *stats = menuPdaMemoListGetPokemonDataPointer(index);
        if (stats != NULL) {
            return pokemonDataBiosGetVoice(stats);
        }
        return POKEMON_VOICE_ID_INVALID;
    }
    return POKEMON_VOICE_ID_INVALID;
}
