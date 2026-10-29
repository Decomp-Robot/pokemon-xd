#include <types.h>
#include <memo.h>
#include <PokemonData.h>
#include "menuPdaMemoListXD.h"
#include "menuPdaMemoWaveXD.h"

s8 *pdaMemoNakigoe::GetWaveDispDataAddress(long index) {
    s32 count = memoDataGetCount(NULL);
    if (count != 0) {
        PokemonSpeciesStats *stats = menuPdaMemoListGetPokemonDataPointer(index);
        if (stats != NULL) {
            return pokemonNakigoeDataBiosGetDataAddress(stats);
        }
        return NULL;
    }
    return NULL;
}
