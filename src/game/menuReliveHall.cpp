#include <types.h>
#include <GSmemory.h>
#include <menuReliveHall.h>

GSgfxColAu8 relivehallAisyou2ColorTbl[3] = {
    {40, 0, 255, 255},
    {40, 170, 255, 255},
    {200, 240, 255, 255},
};

long pcReliveHallSoundTbl[5] = {1113, 1114, 1115, 20, 21};

namespace {

class CMenuReliveHall;

} // namespace

extern CMenuReliveHall *pMenuReliveHall;

namespace {

#pragma force_active on

void thisFree()
{
    if (pMenuReliveHall != NULL) {
        GSmemFree(pMenuReliveHall);
        pMenuReliveHall = NULL;
    }
}

#pragma force_active reset

} // namespace
