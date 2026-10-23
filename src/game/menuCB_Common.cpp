#include <winMsg.h>
#include <menuCBCommon.h>
#include <menuCBBios.h>
#include <gbaCommand.h>
#include <menu.h>
#include <menuSpriteBios.h>
#include <winSprite.h>

typedef struct MenuCBStackEntry {
    s32 menuID;
    s32 windowID;
} MenuCBStackEntry;

typedef struct MenuCBStack {
    MenuCBStackEntry mStack[8];
    s32 mCount;
} MenuCBStack;

MenuCBStack _CBC;

extern "C" {
s32 winMsgCheck(void);
}

extern "C" s32 menuCB_MessageDispCheck(void)
{
    return winMsgCheck();
}

extern "C" void menuCB_MessageDispOff(void)
{
    winMsgClose(0);
}

/* Show a message window; the ID selects the script the window runs. */
extern "C" void menuCB_MessageDispOn(s32 msgID, s32 windowID)
{
    winMsgOpen(7, msgID, windowID, 0);
}

/* Port of the GBA that is in the battle but not answering, 0 if all of
   them are. */
extern "C" s32 menuCB_GetDisconnectedGBA(void)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        s32 gbaPort = _CB.mTrainerImmediately[i].mControlerID;
        u8 connected;

        if (gbaPort == 0) {
            continue;
        }
        if (menuCBTrainerBios_GetHomePlace(&_CB.mTrainerImmediately[i]) == 0) {
            continue;
        }
        connected = gbaCommandIsConnected(gbaPort);
        if (connected == 0) {
            return gbaPort;
        }
    }
    return 0;
}

extern "C" void menuCBSpriteSetData(tagSPRITE_WORK *sprite, MenuSpriteData *data)
{
    MenuSpriteValue *value = menuSpriteBiosGetValPtr(data);

    sprite->mWidth = value->mWidth;
    sprite->mHeight = value->mHeight;
    sprite->mSpriteValue = value->mDrawValue;
    sprite->mSpriteX = value->mX;
    sprite->mSpriteY = value->mY;
}

/* Pop the current window, then discard entries above menuID. */
extern "C" s32 menuCB_MenuRollback(s32 menuID)
{
    menuCB_MenuPop();
    while (menuID != menuCB_CurrentMenuID()) {
        if (_CBC.mCount == 0) {
            break;
        }
        _CBC.mCount--;
    }
    return menuCB_CurrentMenuID();
}

extern "C" s32 menuCB_MenuPop(void)
{
    if (windowGetActiveID() == menuCB_CurrentMenuID()) {
        menuCloseCustom(menuCB_CurrentMenuID(), 0, 0);
    }
    _CBC.mStack[_CBC.mCount].windowID = 0;
    if (_CBC.mCount == 0) {
        return -1;
    }
    _CBC.mCount--;
    return menuCB_CurrentMenuID();
}

extern "C" void menuCB_MenuPush(s32 menuID)
{
    if ((u32)_CBC.mCount >= 8) {
        return;
    }
    _CBC.mCount++;
    _CBC.mStack[_CBC.mCount].menuID = menuID;
    _CBC.mStack[_CBC.mCount].windowID = 0;
}

extern "C" s32 menuCB_CurrentMenuID(void)
{
    return _CBC.mStack[_CBC.mCount].menuID;
}

extern "C" void menuCB_InitMenu(s32 menuID)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        _CBC.mStack[i].menuID = 0;
        _CBC.mStack[i].windowID = 0;
    }
    _CBC.mStack[0].menuID = menuID;
    _CBC.mCount = 0;
}
