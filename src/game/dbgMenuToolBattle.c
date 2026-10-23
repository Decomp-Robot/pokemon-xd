#include <menu.h>
#include <types.h>

/* Tool battle debug menu: coupon exchange state and AGB connection controls. */

#pragma force_active on

#pragma peephole off

extern void menuCBDebug_SkipAGBConnection(u8 skip);

/* Item-run counters. */
static volatile s32 dbgMenuGBAResetCouponCount;
static volatile s32 dbgMenuGBAClearFlag;
static volatile s32 dbgMenuGBAClearCouponCount;
static volatile s32 dbgMenuGBACouponCount;

s32 dbgMenuGBAResetCoupon(void)
{
    dbgMenuGBAResetCouponCount++;
    dbgMenuGBAClearFlag = 0;
    return 0;
}

s32 dbgMenuGBASetClearFlag(void)
{
    dbgMenuGBAClearFlag++;
    return 0;
}

s32 dbgMenuGBAClearCoupon(void)
{
    dbgMenuGBAClearCouponCount++;
    dbgMenuGBACouponCount = 0;
    return 0;
}

s32 dbgMenuGBAAddCoupon(void)
{
    s32 coupon = menuOpen(2, 1);

    menuClose(2);
    if (coupon >= 0) {
        dbgMenuGBACouponCount += coupon;
    }
    return 0;
}

s32 dbgToolBattleDebugSetAGBConnectionMode(s32 menuID, s32 connectionMode)
{
    menuCBDebug_SkipAGBConnection(connectionMode == 0);
    return 0;
}

/* Debug item that does nothing. */
void dbgToolBattleNoOp(void)
{
}

#pragma peephole reset
#pragma force_active reset
