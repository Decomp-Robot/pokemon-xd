#include <types.h>

/*
 * pokemonRelive.cpp
 *
 * Relive (purification) part of the Pokemon menu: the ceremony that plays
 * while a Dark Pokemon is purified in the Relive hall. The relive commands
 * (preReliveSetParameter) store the ceremony pattern they were handed in
 * pParameter; -1 means that no pattern has been selected.
 */

// Parameters of the running relive ceremony.
class ReliveParameter {
public:
    ReliveParameter()
    {
    }

    // Leading word of the parameter block.
    u32 mUnknown0;

    // Selected ceremony pattern; -1 while there is none.
    s32 mPattern;
};

ReliveParameter pParameter;
