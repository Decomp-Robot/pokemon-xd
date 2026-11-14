#include <exribbon.h>
#include <pokemonBios.h>
#include <savedata.h>

s32 exribbonSetEarthRibbon(Pokemon *pokemon)
{
    pokemonBiosSetEarthRibbon(pokemon, true);
    u8 *numbers = (u8 *)savedataGetStatus(NULL, SAVE_DATA_EXRIBBON);
    if (numbers[EXRIBBON_EARTH] == 0) {
        numbers[EXRIBBON_EARTH] = RIBBON_NO_EARTH;
    }
    return 1;
}

s32 exribbonSetNarionalRibbon(Pokemon *pokemon)
{
    pokemonBiosSetNationalRibbon(pokemon, true);
    u8 *numbers = (u8 *)savedataGetStatus(NULL, SAVE_DATA_EXRIBBON);
    if (numbers[EXRIBBON_NATIONAL] == 0) {
        numbers[EXRIBBON_NATIONAL] = RIBBON_NO_NATIONAL;
    }
    return 1;
}

void exribbonSetNo(s32 ribbon, u8 no)
{
    u8 *numbers = (u8 *)savedataGetStatus(NULL, SAVE_DATA_EXRIBBON);
    if (numbers[ribbon] == 0 && no != 0) {
        numbers[ribbon] = no;
    }
}

u8 exribbonGetNo(s32 ribbon)
{
    u8 *numbers = (u8 *)savedataGetStatus(NULL, SAVE_DATA_EXRIBBON);
    return numbers[ribbon];
}

void exribbonInit(u8 *numbers)
{
    if (numbers == NULL) {
        numbers = (u8 *)savedataGetStatus(NULL, SAVE_DATA_EXRIBBON);
    }
    for (s32 i = 0; i < EXRIBBON_COUNT; i++) {
        *numbers++ = 0;
    }
}
