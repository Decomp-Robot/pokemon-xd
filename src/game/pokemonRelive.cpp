#include <types.h>
#include <GSthread.h>
#include <GSparticle.h>
#include <pokemonRelive.h>

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

    u32 mUnknown0;

    // Selected ceremony pattern; -1 while there is none.
    s32 mPattern;
};

ReliveParameter pParameter;

Pokemon *mParameter;
GSthread *hokoraParticleThread;
u32 keepRelivePatternIsHologram;
_GSparticleGen *hokoraParticleGen;
bool hokoraParticleActive;

extern "C" void preReliveExit(void)
{
    if (hokoraParticleThread != 0 && GSthreadIsRunning(hokoraParticleThread)) {
        GSthreadClose(hokoraParticleThread);
        hokoraParticleThread = 0;
    }

    if (hokoraParticleActive == true) {
        GSparticleDestroyGenerator(hokoraParticleGen, true);
        hokoraParticleGen = 0;
        hokoraParticleActive = false;
    }
}
