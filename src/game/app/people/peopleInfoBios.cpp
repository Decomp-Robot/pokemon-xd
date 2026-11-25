#include <peopleInfoBios.h>
#include <gimmickBox.h>
#include <relglobal.h>

static const float degreesToRadians = 0.017453292f;

bool peopleInfoBiosGetBoundCheck(PeopleInfo* info)
{
    if (info) return info->boundCheck;
    return false;
}

u8 peopleInfoBiosGetTalkSe(PeopleInfo* info)
{
    if (info) return info->talkSe;
    return 0;
}

void peopleInfoBiosGetMotion(PeopleInfo* info, u8 motionType, int* motionIndex, u8* looping)
{
    if (info) {
        gimmickBoxGetMotion(motionIndex, looping);
        switch (motionType) {
        case PEOPLE_MOTION_WAIT:
            *motionIndex = PEOPLE_ANIM_WAIT;
            *looping = true;
            break;
        case PEOPLE_MOTION_WALK:
            *motionIndex = PEOPLE_ANIM_WALK;
            *looping = true;
            break;
        case PEOPLE_MOTION_RUN:
            *motionIndex = PEOPLE_ANIM_RUN;
            *looping = true;
            break;
        case PEOPLE_MOTION_CHAT_START:
            *motionIndex = PEOPLE_ANIM_CHAT_START;
            *looping = false;
            break;
        case PEOPLE_MOTION_CHAT_LOOP:
            *motionIndex = PEOPLE_ANIM_CHAT_LOOP;
            *looping = true;
            break;
        case PEOPLE_MOTION_CHAT_END:
            *motionIndex = PEOPLE_ANIM_CHAT_END;
            *looping = false;
            break;
        case PEOPLE_MOTION_FALL:
            *motionIndex = PEOPLE_ANIM_FALL;
            *looping = true;
            break;
        case PEOPLE_MOTION_LAND:
            *motionIndex = PEOPLE_ANIM_LAND;
            *looping = false;
            break;
        }
    }
}

float peopleInfoBiosGetRunLength(PeopleInfo* info)
{
    if (info) return info->runLength;
    return 0.0f;
}

float peopleInfoBiosGetWalkLength(PeopleInfo* info)
{
    if (info) return info->walkLength;
    return 0.0f;
}

float peopleInfoBiosGetColBallSize(PeopleInfo* info)
{
    if (info) return info->colBallSize;
    return 0.0f;
}

u8 peopleInfoBiosGetColType(PeopleInfo* info)
{
    if (info) return info->colType;
    return 0;
}

float peopleInfoBiosGetTalkDistance(PeopleInfo* info)
{
    if (info) return info->talkDistance;
    return 3.0f;
}

float peopleInfoBiosGetNeckDownLimit(PeopleInfo* info)
{
    if (info) return degreesToRadians * info->neckDownLimit;
    return 0.0f;
}

float peopleInfoBiosGetNeckUpLimit(PeopleInfo* info)
{
    if (info) return degreesToRadians * info->neckUpLimit;
    return 0.0f;
}

float peopleInfoBiosGetNeckRightLimit(PeopleInfo* info)
{
    if (info) return degreesToRadians * info->neckRightLimit;
    return 0.0f;
}

float peopleInfoBiosGetNeckLeftLimit(PeopleInfo* info)
{
    if (info) return degreesToRadians * info->neckLeftLimit;
    return 0.0f;
}

s8 peopleInfoBiosGetNeckIndex(PeopleInfo* info)
{
    if (info) return info->neckIndex;
    return -1;
}

s8 peopleInfoBiosGetChestIndex(PeopleInfo* info)
{
    if (info) return info->chestIndex;
    return -1;
}

unsigned long peopleInfoBiosGetModelResID(PeopleInfo* info)
{
    if (info) return info->modelResID;
    return 0;
}

PeopleInfo* peopleInfoBiosGetPtrFromIndex(unsigned long index)
{
    if (index >= *peopleInfoData_number) return 0;
    return &peopleInfoData[index];
}

PeopleInfo* peopleInfoBiosGetPtr(unsigned long id)
{
    PeopleInfo* info = peopleInfoData;
    unsigned long count = *peopleInfoData_number;

    while (count != 0) {
        if (info->modelResID == id) return info;
        info++;
        count--;
    }
    return 0;
}
