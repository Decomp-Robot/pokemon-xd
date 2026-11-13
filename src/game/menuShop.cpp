#include <types.h>
#include <hero.h>
#include <item.h>
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
    // Leaving a window that changed nothing.
    MENU_COUPON_MSG_LEAVE = 13,
    // Leaving a window that still has items to write to the card.
    MENU_COUPON_MSG_LEAVE_AND_SAVE = 15
};

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
int writeBackPcBox(int windowID, u8 mode, VPC_BOX *box)
{
    u8 soundEffect;
    u32 msgID;
    int answer;

    if (!checkExchangeVpc(box)) {
        msgID = menuShopGetCouponMsgID(&soundEffect, windowID, MENU_COUPON_MSG_LEAVE, -1);
        winMsgOpenWithSE(WINMSG_WINDOW_MENU, msgID, true, false, soundEffect);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(true);
        if (answer == 1 || answer == -1) {
            return 0;
        }
        return 1;
    }

    msgID = menuShopGetCouponMsgID(&soundEffect, windowID, MENU_COUPON_MSG_LEAVE_AND_SAVE, -1);
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
