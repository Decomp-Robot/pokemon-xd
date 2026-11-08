#include <types.h>
#include <GSmemory.h>

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
