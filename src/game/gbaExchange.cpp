#include <types.h>

// _swap32 -- reverse the byte order of a 32-bit word.
//
// The GBA link code shuttles multi-byte fields between the GameCube and
// the Game Boy Advance, so words are byte-swapped on the way in and out
// (see the gbaPokemonFromGC/gbaPokemonConditonToGC family and the
// gbaCommand handlers surrounding this helper). Groups with the other
// byte-order helpers sharing this module (_swap16, which will join here
// as it is recovered).
//
// Note: this translation unit builds with the peephole optimizer disabled
// (gbaExchange, GC/2.6 -opt nopeephole). With peephole enabled the mask /
// shift / or triples collapse into rlwimi insertions; with nopeephole the
// compiler keeps the original rlwinm extract, slwi/srwi and or sequence
// exactly.
unsigned long _swap32(unsigned long value)
{
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
}
