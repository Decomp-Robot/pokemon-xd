#include <types.h>
#include <floorManager.h>

/* PokeCoupon exchange menu. menuPokeCouponMain opens the exchange the coupon
 * work refers to, and menuPokeCouponInit checks that the work was set up by
 * the code that opens the menu. */

/* Exchange the coupon work refers to: INVALID until the code that opens the
 * menu sets up one of the two counters, which are shop table entries. */
enum PokeCouponRefer {
    POKECOUPONREFER_INVALID = -1,
    POKECOUPONREFER_SHOP_FIRST = 0,
    POKECOUPONREFER_SHOP_SECOND = 1,
};

/* Shop ids of the two counters and the floor the exchange returns to. */
enum {
    SHOP_ID_POKECOUPON_FIRST = 9,
    SHOP_ID_POKECOUPON_SECOND = 10,
    FLOOR_ID_POKECOUPON = 0x395,
};

/* Coupon work of the exchange: the refer is the exchange the menu acts on. */
struct MenuPokeCouponWork {
    enum PokeCouponRefer m_eRefer; // 0x00: exchange the menu acts on.
};

extern struct MenuPokeCouponWork _menuPokeCouponWork;

extern void menuShopOpen(s32 menuID);
extern void __assert(const char *file, s32 line, const char *expr);

/* No shipped table registers these entry points, so the linker has to be told
 * to keep them. */
#pragma force_active on

void menuPokeCouponExit(void)
{
    _menuPokeCouponWork.m_eRefer = POKECOUPONREFER_INVALID;
}

/* Opens the exchange shop the refer selects, then returns to the floor the
 * exchange runs on. */
void menuPokeCouponMain(void)
{
    switch (_menuPokeCouponWork.m_eRefer) {
    case POKECOUPONREFER_SHOP_FIRST:
        menuShopOpen(SHOP_ID_POKECOUPON_FIRST);
        break;
    case POKECOUPONREFER_SHOP_SECOND:
        menuShopOpen(SHOP_ID_POKECOUPON_SECOND);
        break;
    }
    GSfloorChange(FLOOR_ID_POKECOUPON);
}

void menuPokeCouponInit(void)
{
    if (POKECOUPONREFER_INVALID == _menuPokeCouponWork.m_eRefer) {
        __assert("menuPokeCoupon.c", __LINE__, "POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer");
    }
}

#pragma force_active reset
