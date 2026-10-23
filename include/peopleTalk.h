#ifndef PEOPLE_TALK_H
#define PEOPLE_TALK_H
#include <types.h>
#include <GSgfx.h>
struct tagPeopleWork;
#ifdef __cplusplus
extern "C" {
#endif
void peopleSetTalkDistance(u32 group, u32 id, float distance);
float peopleGetTalkDistance(u32 group, u32 id);
void peopleGetPos(u32 group, u32 id, GSvec* pos);
void peopleTalkLeaveFloor();
void peopleTalkEnterFloor();
void peopleTalkMessageDaemon();
void peopleTalkMessageBgmStop();
void peopleTalkMessageBgmStart();
void peopleTalkMessageBgmSetVolumeRate(float rate);
void peopleTalkMessageBgmSetFadeSpeed(float inTime, float outTime);
void peopleTalkMessageBgmMask(bool on);
void peopleTalkMsgEnd(u32 group, u32 id);
void peopleSetTalkFlagCallback();
void peopleSetTalkKeyWaitCallback();
void peopleTalkMsg(u32 group, u32 id, u32 message);
struct tagPeopleWork* peopleTalkCheck(u32 group, u32 id, float distance, float angle);
#ifdef __cplusplus
}
#endif
#endif
