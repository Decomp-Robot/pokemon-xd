#ifndef MENU_WAZA_STATUS_H
#define MENU_WAZA_STATUS_H

#include <types.h>
#include <Pokemon.h>

struct MENU_WAZA_ENTRY {
    const u16* moveName;
    const u16* typeName;
    u16 type;
    u8 maxPP;
    u8 currentPP;
};

struct MENU_WAZA_STATUS {
    const u16* pokemonName;
    MENU_WAZA_ENTRY moves[4];
    u8 reserved[0x14];
};

extern "C" void pokemonToMenuWazaStatus(Pokemon* pokemon, MENU_WAZA_STATUS* status);

#endif
