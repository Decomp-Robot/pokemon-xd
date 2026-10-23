#include <types.h>

/*
 * Debug items for the tool battle debug menu. They drive a scratch copy of
 * the GBA coupon exchange state and the AGB connection switch. The debug
 * menu tables that register these callbacks are not part of the final
 * binary, so the items below have no callers; the counters let a debugger
 * confirm that an item ran.
 */

/* Not called by other code; marked so the linker keeps them. */
#pragma force_active on

/* The peephole pass reshuffles the small stores below. */
#pragma peephole off

extern void menuClose(s32 menuID);
extern s32 menuOpen(s32 menuID, s32 mode);
extern void menuCBDebug_SkipAGBConnection(u8 skip);

/* Item-run counters. MWCC lays .sbss out in reverse declaration order, so
 * the declarations below run back to front over the block dbgMenuGBACouponCount,
 * dbgMenuGBAClearCouponCount, dbgMenuGBAClearFlag, dbgMenuGBAResetCouponCount.
 */
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
void zz_000e71c_(void)
{
}

#pragma peephole reset
#pragma force_active reset
