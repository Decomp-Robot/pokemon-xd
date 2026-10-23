#ifndef FLOOR_BGM_MASK_H
#define FLOOR_BGM_MASK_H

#include <types.h>

/* Background music of the current map area, faded in and out by the floor
 * script layer through this mask. The floor sound module owns the single
 * instance the layer drives. */
class FloorBgmMask {
public:
    void setState(u32 state);
    u8 isActive();

private:
    u32 mState; // 0x00: state the mask is in.
    u8 mActive; // 0x04: mask switched on; set by the constructor and by
                //       finalize, peopleTalkMessageBgmMask clears it while a
                //       talk message masks the BGM.
};

#endif
