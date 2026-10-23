#ifndef POKEMONEVOLUTION_H
#define POKEMONEVOLUTION_H
#include <types.h>

struct Pokemon;
class Hero;
struct POKEMON_EVOLUTION_INFO {
    u8 mMethod;
    u16 mCondition;
};
extern "C" {
u16 pokemonEvolutionCheck(Pokemon* pokemon, long kind, u16 item, u16* additionalSpecies, POKEMON_EVOLUTION_INFO* info);
long pokemonEvolutionAll(Pokemon* pokemon, u16 species, u16 item, POKEMON_EVOLUTION_INFO* info, Hero* hero, long user, long mode, long wait);
}
#endif
