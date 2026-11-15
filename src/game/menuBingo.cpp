#include <types.h>
#include <app/menu/menuBingo.h>

cBingo::PokemonPanel cBingo::mPokemon[BINGO_PANEL_COUNT];
cBingo gBingo;

extern "C" u8 menuBattleDeBingoGetMode(void) { return gBingo.mMode; }

extern "C" u8 menuBattleDeBingoGetMball(void) { return gBingo.mMball; }

extern "C" u8 menuBattleDeBingoGetEpoint(void)
{
    return gBingo.getEpoint();
}
