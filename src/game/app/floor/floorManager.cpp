#include <floorManager.h>

u32 GSfloorGetCurrent(void)
{
    return floorGetCurrent();
}

u32 GSfloorGetCurrentFloorGroupID(void)
{
    return floorGetCurrentFloorGroupID();
}

u8 GSfloorIsFloorPushed(void)
{
    return floorIsFloorPushed();
}

u8 GSfloorIsPop(void)
{
    return floorIsPop();
}

u8 GSfloorIsPush(void)
{
    return floorIsPush();
}

void GSfloorChange(u32 floorID)
{
    floorChange(floorID);
}

void GSfloorPop(void)
{
    floorPop();
}

void GSfloorPush(u32 floorID)
{
    floorPush(floorID);
}

void GSfloorOpen(u32 floorID)
{
    floorOpen(floorID);
}

void GSfloorStart(void)
{
}
