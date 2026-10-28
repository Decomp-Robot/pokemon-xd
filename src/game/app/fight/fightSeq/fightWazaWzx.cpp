#include <pokemonDB.h>

extern "C" u8 fightWazaWzxVariationFuncOngaesi(u16, FightOutPokemon *pokemon,
                                             FightOutPokemon *, u8)
{
    u16 power = fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(pokemon));
    if (power <= 24) return 0;
    if (power <= 36) return 1;
    if (power <= 80) return 2;
    return 3;
}

extern "C" u8 fightWazaWzxVariationFuncPresent(u16, FightOutPokemon *pokemon,
                                             FightOutPokemon *, u8)
{
    u16 power = fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(pokemon));
    switch (power) {
    case 40:
    case 80:
    case 120:
        return 0;
    default:
        return 1;
    }
}

extern "C" u8 fightWazaWzxVariationFuncYatuatari(u16, FightOutPokemon *pokemon,
                                               FightOutPokemon *, u8)
{
    u16 power = fightWaza_GetIryoku(fightOutPokemon_GetFightWazaPtr(pokemon));
    if (power > 89) return 3;
    if (power >= 62) return 2;
    if (power >= 22) return 1;
    return 0;
}
