#ifndef PLACE_H
#define PLACE_H

#include <types.h>

/* Place work the radar item and the place-name lookup read. Partial: only the
 * place level flag the radar reads is named here. */
struct PLACE_WORK {
    u8 _00[0x14];
    u32 mPlaceFlag; // 0x14: GSflag id the place level is read through.
};

extern PLACE_WORK *gPlaceWork;

#endif
