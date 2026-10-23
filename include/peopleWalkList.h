#ifndef PEOPLE_WALK_LIST_H
#define PEOPLE_WALK_LIST_H

#include <people.h>
#include <GSmemory.h>

enum PeopleMoveType {
    PEOPLE_MOVE_NONE = 0,
    PEOPLE_MOVE_WALK_LIST = 2,
    PEOPLE_MOVE_WALK_LIST_LOOP = 3
};

enum PeopleMoveState {
    PEOPLE_MOVE_STATE_IDLE = 0,
    PEOPLE_MOVE_STATE_START = 1
};

enum PeopleFlags {
    PEOPLE_FLAG_MOVING = 8
};

extern "C" {
void _peopleSetMoveAnimation(tagPeopleWork* people);

bool peopleFreeWalkList(unsigned long groupId, unsigned long peopleId);
bool peopleStartWalkList(unsigned long groupId, unsigned long peopleId, u32 loop);
bool peopleStartWalkListStrict(unsigned long groupId, unsigned long peopleId, u32 loop);
bool peopleAddWalkList(unsigned long groupId, unsigned long peopleId, f32 x, f32 y, f32 z);
bool peopleInitWalkList(unsigned long groupId, unsigned long peopleId, u16 capacity);
}

#endif
