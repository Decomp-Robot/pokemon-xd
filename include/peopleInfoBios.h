#ifndef PEOPLE_INFO_BIOS_H
#define PEOPLE_INFO_BIOS_H

#include <people.h>

typedef tagPeopleInfoBios PeopleInfo;

enum PeopleMotionType {
    PEOPLE_MOTION_WAIT = 1,
    PEOPLE_MOTION_WALK = 2,
    PEOPLE_MOTION_RUN = 3,
    PEOPLE_MOTION_CHAT_START = 4,
    PEOPLE_MOTION_CHAT_LOOP = 5,
    PEOPLE_MOTION_CHAT_END = 6,
    PEOPLE_MOTION_FALL = 7,
    PEOPLE_MOTION_LAND = 8
};

enum PeopleDefaultAnimation {
    PEOPLE_ANIM_WAIT = 3,
    PEOPLE_ANIM_WALK = 5,
    PEOPLE_ANIM_RUN = 8,
    PEOPLE_ANIM_CHAT_START = 11,
    PEOPLE_ANIM_CHAT_LOOP = 12,
    PEOPLE_ANIM_CHAT_END = 13,
    PEOPLE_ANIM_FALL = 39,
    PEOPLE_ANIM_LAND = 40
};

extern "C" {
bool peopleInfoBiosGetBoundCheck(PeopleInfo *info);
u8 peopleInfoBiosGetTalkSe(PeopleInfo *info);
float peopleInfoBiosGetRunLength(PeopleInfo *info);
float peopleInfoBiosGetWalkLength(PeopleInfo *info);
float peopleInfoBiosGetColBallSize(PeopleInfo *info);
u8 peopleInfoBiosGetColType(PeopleInfo *info);
float peopleInfoBiosGetTalkDistance(PeopleInfo *info);
float peopleInfoBiosGetNeckDownLimit(PeopleInfo *info);
float peopleInfoBiosGetNeckUpLimit(PeopleInfo *info);
float peopleInfoBiosGetNeckRightLimit(PeopleInfo *info);
float peopleInfoBiosGetNeckLeftLimit(PeopleInfo *info);
s8 peopleInfoBiosGetChestIndex(PeopleInfo *info);
unsigned long peopleInfoBiosGetModelResID(PeopleInfo *info);
s8 peopleInfoBiosGetNeckIndex(PeopleInfo *info);
PeopleInfo *peopleInfoBiosGetPtr(unsigned long id);
void peopleInfoBiosGetMotion(PeopleInfo *info, u8 motionType, int *motionIndex, u8 *looping);
PeopleInfo *peopleInfoBiosGetPtrFromIndex(u32 index);
}

#endif
