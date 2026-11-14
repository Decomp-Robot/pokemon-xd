#include <mail.h>
#include <mailMain.h>
#include <savedata.h>
#include <string.h>

static inline MailMetadata *mailGetData(int mailID)
{
    if (mailID <= 0 || (u32)mailID > *mailData_number) return 0;
    return &mailData[mailID];
}

static inline int mailGetReadFlagByte(int mailID, u8 *mask)
{
    if (mailID < 0 || mailID >= MAILBOX_CAPACITY) return -1;
    *mask = 1 << (7 - mailID % 8);
    return mailID / 8;
}

void mailInitMailbox(MailSaveData *save)
{
    int i;
    memset(save, 0, sizeof(*save));
    save->sortMode = 0;
    for (i = 0; i < MAILBOX_CAPACITY; i++) save->mailIDs[i] = MAIL_ID_EMPTY;
    mailMainInit();
}

s32 mailGetNbMailInMailbox(void)
{
    MailSaveData *save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);
    return save->mailCount;
}

s32 mailGetMailIDInMailbox(s32 index)
{
    MailSaveData *save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);
    if (index < 0 || index >= mailGetNbMailInMailbox()) return -1;
    return save->mailIDs[index];
}

s32 mailGetReceiveNumber(int mailID)
{
    s32 i;
    s32 found = 0;
    i = 0;

    while (i < mailGetNbMailInMailbox() && !found) {
        if (mailGetMailIDInMailbox(i) == mailID) found = 1;
        i++;
    }
    if (!found) return -1;
    return i;
}

s32 mailAddMailbox(int mailID)
{
    MailSaveData *save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);
    s32 index;
    u8 mask;

    if (mailID <= 0 || (u32)mailID > *mailData_number) return 0;
    if (save->mailCount >= MAILBOX_CAPACITY) return 0;
    save->mailIDs[save->mailCount] = mailID;
    save->mailCount++;
    save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);

    index = mailGetReadFlagByte(mailID, &mask);
    if (index >= 0) {
        save->readFlags[index] &= ~mask;
    }
    return 1;
}

s32 mailChkReceiveMail(int mailID)
{
    MailSaveData *save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);
    s32 i;

    if (mailID <= 0 || (u32)mailID > *mailData_number) return 0;
    for (i = 0; i < save->mailCount; i++) {
        if (save->mailIDs[i] == mailID) break;
    }
    if (i >= save->mailCount) return 0;
    return 1;
}

u32 mailGetSubject(int mailID)
{
    MailMetadata *mail = mailGetData(mailID);
    if (mail == 0) return 0;
    return mail->subject;
}

u32 mailGetSenderName(int mailID)
{
    MailMetadata *mail = mailGetData(mailID);
    if (mail == 0) return 0;
    return mail->senderName;
}

u32 mailGetContents(int mailID)
{
    MailMetadata *mail = mailGetData(mailID);
    if (mail == 0) return 0;
    return mail->contents;
}

u32 mailGetNbMailData(void)
{
    return *mailData_number;
}

u16 mailGetSendCnt(void)
{
    MailSaveData *save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);
    return save->sendCount;
}

s32 mailGetSortMode(void)
{
    MailSaveData *save = (MailSaveData *)savedataGetStatus(0, SAVE_DATA_MAIL);
    return save->sortMode;
}

u32 mailGetAttachFileGroup(int mailID)
{
    MailMetadata *mail = mailGetData(mailID);
    if (mail == 0) return 0;
    return mail->attachFileGroup;
}

u16 mailGetSendRate(int) { return 0; }

u8 mailGetSendCondType(int) { return 0; }

u32 mailGetSendCondition(int) { return 0; }

u32 mailGetOpenFlag(int mailID)
{
    MailMetadata *mail = mailGetData(mailID);
    if (mail == 0) return 0;
    return mail->openFlag;
}

u8 mailGetSendCntCond(int) { return 0; }

u16 mailGetSendCntCondValue(int) { return 0; }
