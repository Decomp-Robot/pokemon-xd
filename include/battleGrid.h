#ifndef BATTLEGRID_H
#define BATTLEGRID_H

#include <types.h>

struct ModelSequence;

// One position of the battle grid: the sequence that drives the model of the
// trainer standing there and the sequences of the two Pokemon deployed on it.
struct battleGridPosition {
    struct ModelSequence *trainerSequence;      // 0x00
    struct ModelSequence *pokemonSequence[2];   // 0x04, 0x08
    u8 _0C[4];                                  // 0x0C
};

// The positions the battle models stand on, followed by the number of trainers
// that were put on the grid.
struct battleGrid {
    struct battleGridPosition position[4];      // 0x00
    u16 trainerNum;                             // 0x40
    u8 _42[2];                                  // 0x42
};

#ifdef __cplusplus
extern "C" {
#endif

struct battleGrid *battleGridGetPtr(void);
void battleGridReplacePokemon(void *oldSequence, void *newSequence);
void battleGridUpdate(void);
void battleGridReset(void);

#ifdef __cplusplus
}
#endif

#endif
