#ifndef WINMSG_H
#define WINMSG_H
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;
#include <types.h>

#ifdef __cplusplus
typedef bool WinMsgBool;
extern "C" {
#else
typedef u8 WinMsgBool;
#endif
void winMsgOpenWithSE(s8 kind, u32 message, WinMsgBool mode, WinMsgBool wait, u8 sound);

s32 winMsgDraw(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);
void winMsgOpen(s8 kind, u32 message, WinMsgBool mode, WinMsgBool wait);
void winMsgClose(WinMsgBool wait);
void winMsgOpenFight(u32 message, WinMsgBool mode, WinMsgBool wait);
void winMsgCloseFight(WinMsgBool wait);
u8 winMsgCloseCheckFight(void);
#ifdef __cplusplus
}
#endif
struct MENU_LVUP_STATUS {
    u8 mFlag;
    u8 _01;
    s16 mHp;
    s16 mAttack;
    s16 mDefense;
    s16 mSpeed;
    s16 mSpAttack;
    s16 mSpDefense;
};
#ifdef __cplusplus
extern "C" {
#endif
s32 winMsgCheck(void);
s32 winMsgCheckField(void);
void winMsgOpenFieldWithSE(u32 message, WinMsgBool wait, WinMsgBool flags, u8 sound);
#ifdef __cplusplus
void winMsgOpenLevelUpFiledStatus(struct MENU_LVUP_STATUS *status, bool wait);
void winMsgCloseLevelUpStatus(bool wait);
#else
void winMsgOpenLevelUpFiledStatus(struct MENU_LVUP_STATUS *status, u8 wait);
void winMsgCloseLevelUpStatus(u8 wait);
#endif
#ifdef __cplusplus
}
#endif
#endif
