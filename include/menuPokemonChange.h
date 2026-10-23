#ifndef MENUPOKEMONCHANGE_H
#define MENUPOKEMONCHANGE_H
#include <types.h>

#include <winSprite.h>
struct Pokemon;
extern "C" {
u8 menuPokemonChangeCursor(u8 sex, Pokemon* pokemon);
void menuPokemonChangeWindowCursor(tagWINDOW_WORK* window);
long menuPokemonChangeWindowHook(tagWINDOW_WORK* window);
void menuPokemonChangeItemHook(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
void menuPokemonChangeDraw(tagWINDOW_WORK* window);
void menuPokemonChangeExit();
void menuPokemonChangeMain();
void menuPokemonChangeInit();
void menuPokemonChange();
}
#endif
