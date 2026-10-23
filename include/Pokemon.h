#ifndef POKEMON_H
#define POKEMON_H

#include <types.h>

// Pokemon -- party/battle pokemon record.
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

void pokemonInit(Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
