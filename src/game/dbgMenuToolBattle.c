#include <types.h>

/* Tool battle debug menu: drives the coupon exchange with the GBA side of the
 * link.  lbl_804EA6B8 counts the coupons, lbl_804EA6BC/lbl_804EA6C0 flag a
 * pending clear and lbl_804EA6C4 counts the resets. */

extern volatile u32 lbl_804EA6B8;
extern volatile u32 lbl_804EA6BC;
extern volatile u32 lbl_804EA6C0;
extern volatile u32 lbl_804EA6C4;

s32 menuOpen(s32 menuID, s32 mode);
void menuClose(s32 menuID);
void menuCBDebug_SkipAGBConnection(u8 skip);

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

#pragma peephole off

s32 dbgMenuGBAResetCoupon(void)
{
    lbl_804EA6C4++;
    lbl_804EA6C0 = 0;
    return 0;
}

s32 dbgMenuGBASetClearFlag(void)
{
    lbl_804EA6C0++;
    return 0;
}

s32 zz_000e67c_(void)
{
    lbl_804EA6BC++;
    lbl_804EA6B8 = 0;
    return 0;
}

s32 dbgMenuGBAAddCoupon(void)
{
    s32 coupon = menuOpen(2, 1);

    menuClose(2);
    if (coupon >= 0) {
        lbl_804EA6B8 += coupon;
    }
    return 0;
}

s32 dbgToolBattleDebugSetAGBConnectionMode(s32 menuID, s32 connectionMode)
{
    menuCBDebug_SkipAGBConnection(connectionMode == 0);
    return 0;
}

void zz_000e71c_(void) {}

#pragma peephole reset

#pragma force_active reset
