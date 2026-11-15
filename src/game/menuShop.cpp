#include <relglobal.h>
#include <types.h>
#include <GSgfxVideo.h>
#include <GSthread.h>
#include <hero.h>
#include <item.h>
#include <itemDataBios.h>
#include <menuPocketTool.h>
#include <menuIds.h>
#include <msgctrl.h>
#include <stdarg.h>
#include <memcard.h>
#include <menuShop.h>
#include <menuSub.h>
#include <pcbox.h>
#include <savedata.h>
#include <string.h>
#include <winMsg.h>

// Shop menu. The coupon exchange hands items out of a work area the shop entry
// keeps in its stack frame while the exchange window is open. Leaving the
// window writes the PC box back to the memory card, and because the player can
// only be told afterwards whether that write worked, the PC-box save region and
// the coupon balance are kept for the length of the exchange so a failed write
// can be rolled back.

// Work area of the coupon exchange.
class VPC_BOX {
public:
    // Item pages of the exchange work area.
    ItemSlot mItems[2][PCBOX::ItemSlotCount];
    u32 mUnknown758;
    u32 mUnknown75C;
    // Set once the exchange gave an item to the PC box.
    u8 mExchanged;
};

// Size of the PC-box save region the exchange keeps for a rollback.
enum {
    PCBOX_SAVE_SIZE = 48208
};

// Message codes of the coupon exchange prompts.
enum MENU_COUPON_MSG_CODE {
    MENU_COUPON_MSG_NOT_ENOUGH_COUPONS = 4,
    MENU_COUPON_MSG_CANNOT_AFFORD_ITEM = 8,
    MENU_COUPON_MSG_NO_ROOM = 9,
    // Leaving a window that changed nothing.
    MENU_COUPON_MSG_LEAVE = 13,
    // Leaving a window that still has items to write to the card.
    MENU_COUPON_MSG_LEAVE_AND_SAVE = 15
};

enum SHOP_MSG_ID {
    SHOP_MSG_ID_BUY_CONFIRM = 50604,
    SHOP_MSG_ID_SELL_QUANTITY = 50613,
    SHOP_MSG_ID_COUPON_EXCHANGE_CONFIRM = 50620,
    SHOP_MSG_ID_TM_BUY_CONFIRM = 50668,
    SHOP_MSG_ID_TM_SELL_QUANTITY = 50670,
    SHOP_MSG_ID_TM_COUPON_EXCHANGE_CONFIRM = 50671
};

enum SHOP_MODE {
    SHOP_MODE_PCBOX = 2
};

const int SHOP_MAX_COUPON_POINT = 9999999;

void addItem(u16 itemID, int num);
int getItemCouponPoint(u16 itemID);
u16 *getGoodsList(int shopID, int *count);
u32 _msgIdWazaMachineChange(u16 itemID, u32 msgID);

// Buy count entered in the shop's number window.
int NumValue;
// PC-box save region as it was when the exchange started.
void *PPcboxBackup;
// Coupon balance as it was when the exchange started.
u32 CouponBackup;

// Writes the current save data to the memory card and reports the task's success.
int memCardSave(VPC_BOX *box)
{
    if (memcardTaskCall(MEMCARD_TASK_WRITE, WINMSG_WINDOW_MENU, 0) == MEMCARD_RESULT_WRITTEN) {
        return 1;
    }
    return 0;
}

// Records that the exchange gave an item to the PC box, so that leaving the
// window has to save.
void memCardDecCouponAddItem(int point, u16 itemID, int num, VPC_BOX *box)
{
    if (box != NULL) {
        box->mExchanged = 1;
    }
}

// Starts an exchange on a fresh work area: both item pages start empty, and the
// PC-box save region and the coupon balance are kept for a rollback. Returns
// nonzero once the rollback state is in place.
int memCardConnect(VPC_BOX *box)
{
    itemInitAry(box->mItems[0], PCBOX::ItemSlotCount);
    itemInitAry(box->mItems[1], PCBOX::ItemSlotCount);
    box->mUnknown758 = 0;
    box->mUnknown75C = 0;
    box->mExchanged = 0;
    void *status = savedataGetStatus(NULL, SAVE_DATA_PCBOX);
    memcpy(PPcboxBackup, status, PCBOX_SAVE_SIZE);
    CouponBackup = heroGetStatus(NULL, HERO_STATUS_POKECOUPON, 0);
    return 1;
}

// Whether the exchange of this work area left items for the PC box.
bool checkExchangeVpc(VPC_BOX *box)
{
    return box->mExchanged != 0;
}

// Ends an exchange: confirms it with the player, saves the PC box when the
// exchange changed it, and rolls the save region and the coupon balance back
// when that save fails. Returns 0 when the player cancels the confirmation.
int writeBackPcBox(int shopID, u8 mode, VPC_BOX *box)
{
    u8 soundEffect;
    u32 msgID;
    int answer;

    if (!checkExchangeVpc(box)) {
        msgID = menuShopGetCouponMsgID(&soundEffect, shopID, MENU_COUPON_MSG_LEAVE, -1);
        winMsgOpenWithSE(WINMSG_WINDOW_MENU, msgID, true, false, soundEffect);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(true);
        if (answer == 1 || answer == -1) {
            return 0;
        }
        return 1;
    }

    msgID = menuShopGetCouponMsgID(&soundEffect, shopID, MENU_COUPON_MSG_LEAVE_AND_SAVE, -1);
    winMsgOpenWithSE(WINMSG_WINDOW_MENU, msgID, true, false, soundEffect);
    answer = menuSubOpenYesNo(0, -1, -1, 0);
    winMsgClose(true);
    if (answer == 1 || answer == -1) {
        return 0;
    }
    if (!memCardSave(box)) {
        memcpy(savedataGetStatus(NULL, SAVE_DATA_PCBOX), PPcboxBackup, PCBOX_SAVE_SIZE);
        heroSetStatus(NULL, HERO_STATUS_POKECOUPON, CouponBackup);
    }
    return 1;
}

// Wait in render frames, yielding to the other threads between updates.
void waitTime(float seconds)
{
    int frameCount = (u32)(seconds * GSgfxVideoGetVsyncRate());
    int elapsedFrames = 0;

    while (elapsedFrames < frameCount) {
        GSthreadSwitch();
        elapsedFrames += GSgfxVideoGetLastRenderTime();
    }
}

void decCouponAddItem(int point, u16 itemID, int num, u8 mode, VPC_BOX *box)
{
    switch (mode) {
    case SHOP_MODE_PCBOX:
        heroDecPokecoupon(NULL, point);
        pcboxAddItem(NULL, itemID, num);
        memCardDecCouponAddItem(point, itemID, num, box);
        break;
    default:
        heroDecPokecoupon(NULL, point);
        addItem(itemID, num);
        break;
    }
}

int checkAddItem(u16 itemID, u8 mode)
{
    switch (mode) {
    case SHOP_MODE_PCBOX:
        return pcboxGetItemCapacity(NULL, itemID);
    default:
        return heroItemCheckAddItemDataId(NULL, itemID);
    }
}

int getMinPoint(int shopID)
{
    int count;
    u16 *goods;
    int minPoint = SHOP_MAX_COUPON_POINT;
    goods = getGoodsList(shopID, &count);
    while (*goods != 0) {
        int point = getItemCouponPoint(*goods);
        if (point < minPoint) {
            minPoint = point;
        }
        goods++;
    }
    return minPoint;
}

u16 *getGoodsList(int shopID, int *count)
{
    u16 *goods = &shopGoods[shopData[shopID].mGoodsIndex];
    int num = 0;
    u16 *item = goods;
    while (*item != 0) {
        item++;
        num++;
    }
    *count = num;
    return goods;
}

u32 getCouponMsgID(u8 *soundEffect, int shopID, MENU_COUPON_MSG_CODE messageCode,
                   int firstValue, va_list args)
{
    int messageSet;
    u32 *message;
    int value;
    int selector;
    int isSelector;
    u16 itemID;

    messageSet = shopData[shopID].mMessageSet;
    *soundEffect = shopExchanger[messageSet].mSoundEffect;
    message = shopExchanger[messageSet].mMessages;
    message += messageCode;
    value = firstValue;
    itemID = 0;
    isSelector = 1;
    for (; value >= 0; value = va_arg(args, int)) {
        if (isSelector) {
            selector = value;
            isSelector = 0;
        } else {
            isSelector = 1;
            msgctrlSetValue(selector, value);
            if (selector == MSGCTRL_VALUE_ITEM_ID) {
                itemID = value;
            }
        }
    }
    return _msgIdWazaMachineChange(itemID, *message);
}

u32 getMsgID(u8 *soundEffect, int shopID, MENU_SHOP_MSG_CODE messageCode,
             int firstValue, va_list args)
{
    int messageSet;
    u32 *message;
    int value;
    int selector;
    int isSelector;
    u16 itemID;

    messageSet = shopData[shopID].mMessageSet;
    *soundEffect = shopSalesman[messageSet].mSoundEffect;
    message = shopSalesman[messageSet].mMessages;
    message += messageCode;
    value = firstValue;
    itemID = 0;
    isSelector = 1;
    for (; value >= 0; value = va_arg(args, int)) {
        if (isSelector) {
            selector = value;
            isSelector = 0;
        } else {
            isSelector = 1;
            msgctrlSetValue(selector, value);
            if (selector == MSGCTRL_VALUE_ITEM_ID) {
                itemID = value;
            }
        }
    }
    return _msgIdWazaMachineChange(itemID, *message);
}

u32 _msgIdWazaMachineChange(u16 itemID, u32 msgID)
{
    if (itemID != 0) {
        ItemData *item = itemDataBiosGetPtr(itemID);
        if (item != NULL && itemDataBiosGetKind(item) == ITEM_KIND_WAZA_MACHINE) {
            int machineNo = itemDataBiosGetWazaMachineNo(item);
            msgctrlSetValue(MSGCTRL_VALUE_WAZA_ID,
                itemDataBiosGetWazaIDByWazaMachineNo(machineNo));
            switch (msgID) {
            case SHOP_MSG_ID_BUY_CONFIRM:
                msgID = SHOP_MSG_ID_TM_BUY_CONFIRM;
                break;
            case SHOP_MSG_ID_SELL_QUANTITY:
                msgID = SHOP_MSG_ID_TM_SELL_QUANTITY;
                break;
            case SHOP_MSG_ID_COUPON_EXCHANGE_CONFIRM:
                msgID = SHOP_MSG_ID_TM_COUPON_EXCHANGE_CONFIRM;
                break;
            }
        }
    }
    return msgID;
}

u32 menuShopGetCouponMsgID(u8 *soundEffect, s32 shopID, s32 messageCode, s32 valueCode, ...)
{
    va_list args;
    va_start(args, valueCode);
    u32 msgID = getCouponMsgID(soundEffect, shopID, (MENU_COUPON_MSG_CODE)messageCode, valueCode, args);
    va_end(args);
    return msgID;
}

u32 menuShopGetMsgID(u8 *soundEffect, s32 shopID, s32 messageCode, s32 valueCode, ...)
{
    va_list args;
    va_start(args, valueCode);
    u32 msgID = getMsgID(soundEffect, shopID, (MENU_SHOP_MSG_CODE)messageCode, valueCode, args);
    va_end(args);
    return msgID;
}

void openCouponMsgWin(int shopID, MENU_COUPON_MSG_CODE messageCode, int valueCode, ...)
{
    switch (messageCode) {
    case MENU_COUPON_MSG_NOT_ENOUGH_COUPONS:
    case MENU_COUPON_MSG_CANNOT_AFFORD_ITEM:
    case MENU_COUPON_MSG_NO_ROOM:
        menuPtPlaySe(MENU_SE_ERROR);
        break;
    }
    u8 soundEffect;
    va_list args;
    va_start(args, valueCode);
    u32 msgID = getCouponMsgID(&soundEffect, shopID, messageCode, valueCode, args);
    va_end(args);
    winMsgOpenWithSE(WINMSG_WINDOW_MENU, msgID, true, false, soundEffect);
    winMsgClose(true);
}

void openMsgWin(int shopID, MENU_SHOP_MSG_CODE messageCode, int valueCode, ...)
{
    switch (messageCode) {
    case MENU_SHOP_MSG_NOT_ENOUGH_MONEY:
    case MENU_SHOP_MSG_NO_ROOM:
    case MENU_SHOP_MSG_NEED_COLOGNE_CASE:
    case MENU_SHOP_MSG_CANNOT_SELL:
        menuPtPlaySe(MENU_SE_ERROR);
        break;
    }
    u8 soundEffect;
    va_list args;
    va_start(args, valueCode);
    u32 msgID = getMsgID(&soundEffect, shopID, messageCode, valueCode, args);
    va_end(args);
    winMsgOpenWithSE(WINMSG_WINDOW_MENU, msgID, true, false, soundEffect);
    winMsgClose(true);
}
