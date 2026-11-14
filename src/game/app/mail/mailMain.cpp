#include <mail.h>
#include <mailMain.h>
#include <heroMove.h>
#include <floor.h>
#include <GSflag.h>
#include <GSsndPlayback.h>
#include <hsdRandom.h>

struct MailReceiveWork {
    s32 active;
    s32 timer;
    unsigned long soundID;
    s32 unknown;
};

MailReceiveWork ReceiveWork;

s32 chkMailSend(int mailID, float randomValue);
void cbStep(long context, FootStepCounter counter, long steps);

enum {
    MAIL_SEND_RATE_DISABLED = 0xffff,
    MAIL_SEND_COUNT_DISABLED = 0xffff,
    MAIL_SEND_CONDITION_DISABLED = 0xff,
    MAIL_RECEIVE_TIMEOUT = 600
};
static const u32 MAIL_SEND_CONDITION_INVALID = 0xffffffff;

enum MailCountCondition {
    MAIL_COUNT_EQUAL = 0,
    MAIL_COUNT_GREATER = 1,
    MAIL_COUNT_LESS = 2,
    MAIL_COUNT_AT_LEAST = 3,
    MAIL_COUNT_AT_MOST = 4,
    MAIL_COUNT_NOT_EQUAL = 5,
    MAIL_COUNT_ALWAYS = 6
};
enum MailSendCondition {
    MAIL_SEND_ALWAYS = 0,
    MAIL_SEND_FLAG = 1,
    MAIL_SEND_SCRIPT = 2
};

void mailMainInit(void)
{
    heroMoveAddStepCallback(cbStep, 0);
    ReceiveWork.active = 0;
    ReceiveWork.soundID = 0;
    ReceiveWork.unknown = 0;
}

void cbStep(long, FootStepCounter, long)
{
    float randomValue = HSD_Randf();
    s32 mailID;
    s32 count = mailGetNbMailData();
    s32 sent = 0;
    for (mailID = 0; mailID < count; mailID++) {
        if (chkMailSend(mailID, randomValue)) {
            mailAddMailbox(mailID);
            sent = 1;
        }
    }
    if (sent) mailMainReceiveStart();
}

s32 chkMailSend(int mailID, float randomValue)
{
    if (mailGetReceiveNumber(mailID) >= 0) return 0;
    u16 rate = mailGetSendRate(mailID);
    if (rate == MAIL_SEND_RATE_DISABLED) return 0;
    if (randomValue >= (0.015625f * rate) / 1000.0f) return 0;
    u8 countCondition = mailGetSendCntCond(mailID);
    if (countCondition == MAIL_SEND_CONDITION_DISABLED) return 0;
    u16 countValue = mailGetSendCntCondValue(mailID);
    if (countValue == MAIL_SEND_COUNT_DISABLED) return 0;
    u16 sendCount = mailGetSendCnt();
    s32 eligible = 0;
    switch (countCondition) {
    case MAIL_COUNT_EQUAL: if (sendCount == countValue) eligible = 1; break;
    case MAIL_COUNT_GREATER: if (sendCount > countValue) eligible = 1; break;
    case MAIL_COUNT_LESS: if (sendCount < countValue) eligible = 1; break;
    case MAIL_COUNT_AT_LEAST: if (sendCount >= countValue) eligible = 1; break;
    case MAIL_COUNT_AT_MOST: if (sendCount <= countValue) eligible = 1; break;
    case MAIL_COUNT_NOT_EQUAL: if (sendCount != countValue) eligible = 1; break;
    case MAIL_COUNT_ALWAYS: eligible = 1; break;
    }
    if (!eligible) return 0;
    u8 condition = mailGetSendCondType(mailID);
    if (condition == MAIL_SEND_CONDITION_DISABLED) return 0;
    s32 send = 0;
    switch (condition) {
    case MAIL_SEND_ALWAYS: send = 1; break;
    case MAIL_SEND_FLAG: {
        u32 flag = mailGetSendCondition(mailID);
        if (flag != MAIL_SEND_CONDITION_INVALID && GSflagGet(flag) != 0) send = 1;
        break;
    }
    case MAIL_SEND_SCRIPT: {
        u32 script = mailGetSendCondition(mailID);
        if (script != MAIL_SEND_CONDITION_INVALID && script != 0) {
            floorExecScriptInteger(script, 0, 0, 0, 0);
            if (floorGetScriptResult()) send = 1;
        }
        break;
    }
    }
    return send;
}

void mailMainReceiveStart(void)
{
    if (ReceiveWork.active == 0) {
        ReceiveWork.active = 1;
        ReceiveWork.timer = 0;
        ReceiveWork.soundID = 0;
        ReceiveWork.unknown = 0;
    }
}

void mailMainReceiveTerminate(void)
{
    ReceiveWork.timer = MAIL_RECEIVE_TIMEOUT;
    if (ReceiveWork.soundID != 0) GSsndStop(ReceiveWork.soundID, 0, 0);
}

void mailMainSendAllMail(void)
{
    s32 mailID;
    s32 count = mailGetNbMailData();
    s32 sent = 0;
    for (mailID = 1; mailID < count; mailID++) {
        if (chkMailSend(mailID, -1.0f)) {
            mailAddMailbox(mailID);
            sent = 1;
        }
    }
    if (sent) mailMainReceiveStart();
}

void mailMainSendByScrpt(u16 mailID)
{
    if (mailGetReceiveNumber(mailID) < 0) {
        mailAddMailbox(mailID);
        mailMainReceiveStart();
    }
}
