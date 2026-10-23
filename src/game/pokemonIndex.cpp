#include <types.h>

/* Species 251 is Celebi; the 25 slots directly after it are reserved and are
 * skipped when numbering species in the internal index. */
enum {
	CELEBI_SPECIES_NUMBER = 0xfb,
	RESERVED_SPECIES_SLOTS = 0x19,
};

/* Convert the sequential species number used by the debug serial-number entry
 * into an internal species index. Species 1..251 (up to Celebi) are already
 * stored in order; anything past Celebi is shifted past the reserved slots
 * that follow it. */
u16 pokemonindex(s32 index)
{
	index++;
	if (index > CELEBI_SPECIES_NUMBER) {
		index += RESERVED_SPECIES_SLOTS;
	}
	return index;
}
