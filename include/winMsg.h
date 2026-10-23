#ifndef WIN_MSG_H
#define WIN_MSG_H

#include <winSprite.h>
#include <types.h>

/* The six values the level-up screen shows, in the order it shows them. The
 * first byte is the flag winMsgOpenLevelUpFiledStatus takes. */
struct MENU_LVUP_STATUS {
    u8 mFlag;       // 0x00: set before the window is opened.
    u8 _01;         // 0x01: padding.
    s16 mHp;        // 0x02
    s16 mAttack;    // 0x04
    s16 mDefense;   // 0x06
    s16 mSpeed;     // 0x08
    s16 mSpAttack;  // 0x0A
    s16 mSpDefense; // 0x0C
};

#ifdef __cplusplus
extern "C" {
s32 winMsgCheck(void);
long winMsgDraw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
void winMsgOpen(signed char mode, unsigned int messageID, bool wait, bool instantText);
void winMsgOpenWithSE(signed char mode, unsigned int messageID, bool wait, bool instantText, unsigned char soundID);
void winMsgClose(bool wait);
void winMsgCloseFight(bool wait);
void winMsgCloseField(bool wait);
void winMsgOpenLevelUpFiledStatus(MENU_LVUP_STATUS *status, bool wait);
void winMsgOpenLevelUpStatus(MENU_LVUP_STATUS *status, bool wait);
void winMsgCloseLevelUpStatus(bool wait);
#else
void winMsgOpen(signed char mode, unsigned int messageID, unsigned char wait, unsigned char instantText);
void winMsgOpenWithSE(signed char mode, unsigned int messageID, unsigned char wait, unsigned char instantText, unsigned char soundID);
void winMsgClose(unsigned char wait);
void winMsgCloseFight(unsigned char wait);
void winMsgCloseField(unsigned char wait);
void winMsgOpenLevelUpFiledStatus(struct MENU_LVUP_STATUS *status, unsigned char wait);
void winMsgOpenLevelUpStatus(struct MENU_LVUP_STATUS *status, unsigned char wait);
void winMsgCloseLevelUpStatus(unsigned char wait);
#endif
#ifdef __cplusplus
}
#endif
#endif
