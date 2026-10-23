#ifndef WINMSG_H
#define WINMSG_H
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
s32 winMsgDraw(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);
void winMsgOpen(s32 mode, u32 messageID, s32 windowID, s32 wait);
void winMsgClose(s32 wait);
void winMsgOpenFight(s32 status, s32 speaker, s32 wait);
void winMsgCloseFight(s32 wait);
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
