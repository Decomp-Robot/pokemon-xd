#ifndef POKEMON_H
#define POKEMON_H

#include <types.h>

// Pokemon -- party/battle pokemon record.
typedef struct Pokemon Pokemon;
struct Pokemon {
    u8 _00[0x02]; // 0x00-0x01: unknown.
    u16 mItemDataId; // 0x02: held item data ID.
    u32 _04[0x08]; // 0x04-0x23: unknown.
    unsigned long mCatchTrainerRnd; // 0x24: controller slot booked as the catch trainer.
    u8 _28[0xC4 - 0x28]; // 0x28-0xC3: unknown.

#ifdef __cplusplus
    u16 getItemDataId() const;
    unsigned long getCatchTrainerRnd() const;
    void setCatchTrainerRnd(unsigned long rnd);
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

/* pokemonGetStatus query IDs. POKEMON_STATUS_SPECIES reads the record's
   species halfword (zero for an empty slot); POKEMON_STATUS_MOVE reads the
   move ID of the slot the fourth argument selects. */
enum PokemonStatusQuery {
    POKEMON_STATUS_SPECIES = 0x6E,
    POKEMON_STATUS_MOVE = 0x7F
};

void pokemonInit(Pokemon *pokemon);
// Gender and nickname of a Pokemon. Nicknames are UTF-16 strings of at most
// ten characters; the name entry mode, the message layer and the link
// exchange each copy them into their own name buffers.
u8 pokemonGetSex(Pokemon *pokemon);
u16 *pokemon_GetNickname(Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
