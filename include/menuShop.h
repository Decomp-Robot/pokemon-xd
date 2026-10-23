#ifndef MENU_SHOP_H
#define MENU_SHOP_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Build the shop message and select the sound effect it should play.
u32 menuShopGetMsgID(u8 *soundEffect, s32 windowID, s32 messageCode, s32 valueCode, ...);

#ifdef __cplusplus
}
#endif

#endif
