#include <types.h>

/* pokemonindex -- convert a zero-based sequential Pokemon number (as used
 * by the debug pocket serial-number entry) into an internal species index.
 * Numbers 1..251 map directly; anything past Celebi (#251) is shifted past
 * the 25 unused slots that follow it. */
u16 pokemonindex(s32 index)
{
	index++;
	if (index > 0xfb) {
		index += 0x19;
	}
	return index;
}
