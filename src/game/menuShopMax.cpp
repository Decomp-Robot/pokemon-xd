#include <menuShop.h>

// Largest item count the current coupon balance can exchange for. A
// non-positive price returns the cap without reading the balance.
s32 getExchangeMax(u16 itemId, u8 mode)
{
    s32 price;
    s32 max;

    price = getItemCouponPoint(itemId);
    if (price > 0) {
        max = getCouponPoint(mode) / price;
        if (max > 99) {
            max = 99;
        }
    } else {
        max = 99;
    }

    return max;
}

// Largest item count the current money can buy. A non-positive price
// returns the cap without reading the balance.
s32 getBuyMax(u16 itemId)
{
    s32 price;
    s32 max;

    price = getItemPrice(itemId);
    if (price > 0) {
        max = getMoney() / price;
        if (max > 99) {
            max = 99;
        }
    } else {
        max = 99;
    }

    return max;
}
