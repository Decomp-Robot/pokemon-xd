#ifndef MAILMAIN_H
#define MAILMAIN_H

#include <types.h>
#include <mail.h>

#ifdef __cplusplus
extern "C" {
#endif
void mailMainInit(void);
void mailMainSendByScrpt(u16);
void mailMainReceiveStart(void);
void mailMainReceiveTerminate(void);
void mailMainSendAllMail(void);
#ifdef __cplusplus
}
#endif

#endif
