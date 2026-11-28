#include <gimmickBox.h>
#include <people.h>
#include <peopleCollision.h>

extern "C" int gimmickBoxPop()
{
    gimmickBoxFloorInit();
    return 1;
}

extern "C" u8 gimmickBoxIsBox(tagPeopleWork* people)
{
    if (!gimmickBoxIsPushBoxFloor()) {
        return 0;
    }
    if (people == 0) {
        return 0;
    }
    return (people->state.infoId == GIMMICK_BOX_OBJ_ID_PUSH_BOX) ? (u8)1 : (u8)0;
}
