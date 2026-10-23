#include <types.h>
#include <floorManager.h>

/* PokeCoupon exchange menu: opened by the shop code after menuPokeCouponInit.
 * The coupon work is read by the GBA side of the exchange while the menu runs. */

/* Coupon work of the exchange, set up by the shop code that opens the menu.
 * m_eRefer holds POKECOUPONREFER_INVALID while the work is not set up yet. */
enum {
    POKECOUPONREFER_INVALID = -1,
};

struct MenuPokeCouponWork {
    s32 m_eRefer; // 0x00: coupon exchange state the menu acts on.
    s32 _04;      // 0x04: not used by this unit.
};

extern struct MenuPokeCouponWork _menuPokeCouponWork;

void menuShopOpen(s32 menuID);
void __assert(const char *file, s32 line, const char *cond);

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

/* Puts the coupon work back into the invalid state, so the next
 * menuPokeCouponInit call re-reads it. */
void menuPokeCouponExit(void)
{
    _menuPokeCouponWork.m_eRefer = POKECOUPONREFER_INVALID;
}

void menuPokeCouponMain(void)
{
    switch (_menuPokeCouponWork.m_eRefer) {
    case 0:
        menuShopOpen(9);
        break;
    case 1:
        menuShopOpen(10);
        break;
    }
    GSfloorChange(0x395);
}

void menuPokeCouponInit(void)
{
    /* assert(POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer); */
    if (POKECOUPONREFER_INVALID == _menuPokeCouponWork.m_eRefer) {
        __assert("menuPokeCoupon.c", 60, "POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer");
    }
}

#pragma force_active reset
