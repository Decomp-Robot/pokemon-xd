#include <menu.h>
#include <types.h>

/* Tool battle debug menu: coupon exchange state and AGB connection controls. */

#pragma force_active on

#pragma peephole off

extern void menuCBDebug_SkipAGBConnection(u8 skip);

/* Coupon debug menu state. */
static volatile s32 dbgMenuGBAClearCouponFlag_on;
static volatile s32 dbgMenuGBASetClearFlag_on;
static volatile s32 dbgMenuGBAResetCoupon_on;
static volatile s32 dbgMenuGBAAddCoupon_value;

s32 dbgMenuGBAClearCouponFlag(void)
{
    dbgMenuGBAClearCouponFlag_on++;
    dbgMenuGBASetClearFlag_on = 0;
    return 0;
}

s32 dbgMenuGBASetClearFlag(void)
{
    dbgMenuGBASetClearFlag_on++;
    return 0;
}

s32 dbgMenuGBAResetCoupon(void)
{
    dbgMenuGBAResetCoupon_on++;
    dbgMenuGBAAddCoupon_value = 0;
    return 0;
}

s32 dbgMenuGBAAddCoupon(void)
{
    s32 coupon = menuOpen(2, 1);

    menuClose(2);
    if (coupon >= 0) {
        dbgMenuGBAAddCoupon_value += coupon;
    }
    return 0;
}

s32 dbgToolBattleDebugSetAGBConnectionMode(s32 menuID, s32 connectionMode)
{
    menuCBDebug_SkipAGBConnection(connectionMode == 0);
    return 0;
}

#pragma peephole reset
#pragma force_active reset
