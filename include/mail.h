#ifndef MAIL_H
#define MAIL_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    MAILBOX_CAPACITY = 32,
    MAIL_ID_EMPTY = 255
};

typedef struct MailMetadata {
    u32 senderName;
    u32 subject;
    u32 contents;
    u32 attachFileGroup;
    u32 openFlag;
} MailMetadata;

typedef struct MailSaveData {
    u16 mailIDs[MAILBOX_CAPACITY];
    u16 mailCount;
    u8 readFlags[MAILBOX_CAPACITY / 8];
    u8 sortMode;
    u8 reserved;
    u16 sendCount;
} MailSaveData;

extern u32 *mailData_number;
extern MailMetadata *mailData;

u16 mailGetSendCntCondValue(int mailID);
u8 mailGetSendCntCond(int mailID);
u32 mailGetOpenFlag(int mailID);
u32 mailGetSendCondition(int mailID);
u8 mailGetSendCondType(int mailID);
u16 mailGetSendRate(int mailID);
u32 mailGetAttachFileGroup(int mailID);
s32 mailGetSortMode(void);
u16 mailGetSendCnt(void);
u32 mailGetNbMailData(void);
u32 mailGetContents(int mailID);
u32 mailGetSenderName(int mailID);
u32 mailGetSubject(int mailID);
s32 mailChkReceiveMail(int mailID);
s32 mailAddMailbox(int mailID);
s32 mailGetReceiveNumber(int mailID);
s32 mailGetMailIDInMailbox(s32 index);
s32 mailGetNbMailInMailbox(void);
void mailInitMailbox(MailSaveData *save);

#ifdef __cplusplus
}
#endif

#endif
