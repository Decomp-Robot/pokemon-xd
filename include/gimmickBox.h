#ifndef GIMMICK_BOX_H
#define GIMMICK_BOX_H

#include <types.h>

struct tagPeopleWork;

static const unsigned long GIMMICK_BOX_OBJ_ID_PUSH_BOX = 295961600;

extern "C" {
bool gimmickBoxGetMotion(int* motionIndex, u8* looping);
void gimmickBoxFloorInit();
int gimmickBoxPop();
u8 gimmickBoxIsBox(tagPeopleWork* people);
}

#endif
