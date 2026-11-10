#ifndef MAILMAIN_H
#define MAILMAIN_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
void mailMainSendByScrpt(u16);
s32 mailChkReceiveMail(u32 mailID);
s32 mailAddMailbox(u32 mailID);
void mailMainReceiveStart(void);
void mailMainSendAllMail(void);
#ifdef __cplusplus
}
#endif

#endif
