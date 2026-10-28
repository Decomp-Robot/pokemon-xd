#include <GSflag.h>
#include <pokemonDataBios.h>
#include <types.h>

extern "C" {

// Whether the species has been caught, as its zukan caught flag reads.
u32 pokemonDataBiosGetTukamaetaFlag(PokemonData *data)
{
    if (data == 0) {
        return 0;
    }
    return GSflagGet(data->mTukamaetaFlagId);
}

// Whether the species has been seen, as its zukan seen flag reads.
u32 pokemonDataBiosGetMitaFlag(PokemonData *data)
{
    if (data == 0) {
        return 0;
    }
    return GSflagGet(data->mMitaFlagId);
}

}
