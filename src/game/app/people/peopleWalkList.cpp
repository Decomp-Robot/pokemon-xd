#include "peopleWalkList.h"

static inline void clearWalkList(tagPeopleWork* people)
{
    if (people->state.walkList) {
        GSmemFree(people->state.walkList);
        people->state.walkList = 0;
    }
    people->state.moveType = PEOPLE_MOVE_NONE;
    people->state.walkListCapacity = 0;
    people->state.walkListIndex = 0;
    people->state.moveState = PEOPLE_MOVE_STATE_IDLE;
}

bool peopleFreeWalkList(unsigned long groupId, unsigned long peopleId)
{
    tagPeopleWork* people = peopleGet(groupId, peopleId);
    if (!people)
        return false;
    if (people->state.walkList)
        clearWalkList(people);
    return true;
}

bool peopleStartWalkList(unsigned long groupId, unsigned long peopleId, u32 loop)
{
    tagPeopleWork* people = peopleGet(groupId, peopleId);
    if (!people)
        return false;
    peopleSetFlagOn(groupId, peopleId, PEOPLE_FLAG_MOVING);
    people->state.moveState = PEOPLE_MOVE_STATE_IDLE;
    people->state.walkListIndex = 0;
    people->state.moveSpeed = people->state.walkSpeed;
    people->state.moveState = PEOPLE_MOVE_STATE_START;
    if (loop)
        people->state.moveType = PEOPLE_MOVE_WALK_LIST_LOOP;
    else
        people->state.moveType = PEOPLE_MOVE_WALK_LIST;
    people->state.strictWalkList = 0;
    _peopleSetMoveAnimation(people);
    return true;
}

bool peopleStartWalkListStrict(unsigned long groupId, unsigned long peopleId, u32 loop)
{
    tagPeopleWork* people = peopleGet(groupId, peopleId);
    if (!people)
        return false;
    peopleSetFlagOn(groupId, peopleId, PEOPLE_FLAG_MOVING);
    people->state.moveState = PEOPLE_MOVE_STATE_IDLE;
    people->state.walkListIndex = 0;
    people->state.moveSpeed = people->state.walkSpeed;
    people->state.moveState = PEOPLE_MOVE_STATE_START;
    if (loop)
        people->state.moveType = PEOPLE_MOVE_WALK_LIST_LOOP;
    else
        people->state.moveType = PEOPLE_MOVE_WALK_LIST;
    people->state.strictWalkList = 1;
    _peopleSetMoveAnimation(people);
    return true;
}

bool peopleAddWalkList(unsigned long groupId, unsigned long peopleId, f32 x, f32 y, f32 z)
{
    tagPeopleWork* people = peopleGet(groupId, peopleId);
    if (!people)
        return false;
    if (!people->state.walkList)
        return false;
    if (people->state.walkListIndex >= people->state.walkListCapacity)
        return false;
    GSvec* point = &people->state.walkList[people->state.walkListIndex];
    point->x = x;
    point->y = y;
    point->z = z;
    ++people->state.walkListIndex;
    return true;
}

bool peopleInitWalkList(unsigned long groupId, unsigned long peopleId, u16 capacity)
{
    tagPeopleWork* people = peopleGet(groupId, peopleId);
    if (!people)
        return false;
    peopleFreeWalkList(groupId, peopleId);
    people->state.walkList = static_cast<GSvec*>(GSmemAllocClr(sizeof(GSvec) * capacity));
    if (!people->state.walkList)
        return false;
    people->state.walkListCapacity = capacity;
    people->state.walkListIndex = 0;
    people->state.moveState = PEOPLE_MOVE_STATE_IDLE;
    return true;
}
