#ifndef ITEM_USE2_POKEMON_H
#define ITEM_USE2_POKEMON_H

#include <Pokemon.h>
#include <battlePokemon.h>

/* One entry of the log itemUse2Pokemon fills in with what an item did to a
 * pokemon. An item logs at most 32 entries, the count is kept separately. */
struct ITEMUSE2POKEMON_LOG1 {
    s32 mEffect; // 0x00: the effect the item had.
    s16 mValue;  // 0x04: first value the effect message prints.
    s16 mValue2; // 0x06: second value the effect message prints.
};

extern "C" s16 itemUse2Pokemon(ITEMUSE2POKEMON_LOG1 *log, Pokemon *pokemon,
    FightPokemon *fightPokemon, u16 itemID, u8 moveIndex);

#endif
