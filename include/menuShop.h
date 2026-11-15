#ifndef MENU_SHOP_H
#define MENU_SHOP_H

#include <types.h>

enum MENU_SHOP_MSG_CODE {
    MENU_SHOP_MSG_NOT_ENOUGH_MONEY = 5,
    MENU_SHOP_MSG_NO_ROOM = 6,
    MENU_SHOP_MSG_NEED_COLOGNE_CASE = 8,
    MENU_SHOP_MSG_SELL_TOTAL = 9,
    MENU_SHOP_MSG_SELL_COMPLETE = 10,
    MENU_SHOP_MSG_CANNOT_SELL = 11,
    MENU_SHOP_MSG_SELL_QUANTITY = 13
};

#ifdef __cplusplus
extern "C" {
#endif

// Build the shop message and select the sound effect it should play.
u32 menuShopGetMsgID(u8 *soundEffect, s32 shopID, s32 messageCode, s32 valueCode, ...);

// Same for the coupon shop messages.
u32 menuShopGetCouponMsgID(u8 *soundEffect, s32 shopID, s32 messageCode, s32 valueCode, ...);

#ifdef __cplusplus
}
#endif

#endif
