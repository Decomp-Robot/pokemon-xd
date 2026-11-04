#include <types.h>

// Price and balance helpers of the shop window: the coupon price and
// balance for exchange, and the ordinary price and money for buying.

s32 getItemCouponPoint(u16 itemId);
s32 getCouponPoint(u8 mode);
s32 getMoney();
s32 getItemPrice(u16 itemId);

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
