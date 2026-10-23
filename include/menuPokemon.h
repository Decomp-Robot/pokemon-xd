#ifndef MENU_POKEMON_H
#define MENU_POKEMON_H

#include <types.h>
#include <fightTrainer.h>

// Party-menu work cleared when the menu opens.
struct MenuPokemonWork {
    u8 _00[0x28];
};

extern MenuPokemonWork menuPokemonWork;

#ifdef __cplusplus
extern "C" {
#endif

void menuPokemonInitWork(void);
u32 menuPokemonOpenItemGive(u8 itemPocket, u8 menuItemIndex, u16 itemID,
                            FIGHT_TRAINER *runtimeTrainer);
void menuPokemonClose(void);

#ifdef __cplusplus
}
#endif

#endif
