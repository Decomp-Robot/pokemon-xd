#include <types.h>

/* GSgfx layer surface teardown. */

extern volatile u8 lbl_804EA688;

struct GSgfxLayer {
    void endSurface(void);
};

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

void GSgfxLayer::endSurface(void)
{
    lbl_804EA688 = 0;
}

#pragma force_active reset
