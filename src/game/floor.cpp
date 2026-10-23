#include "floor.h"

extern "C" {
// Floor manager entry points.
void floorOpen(u32 floorID);          // change to another floor.
u32 floorStackGetCurrentRoomID();     // room the floor stack is on.
u32 floorStackGetThreadGroupID();     // thread group of the floor on the stack.
u8 floorStackIsPushd();               // a floor is pushed.
u8 floorStackIsPopd();           // current floor was resumed by a pop.
u8 floorStackIsPushFlag();            // push flag of the floor on the stack.
void floorManagerPop();               // pop the floor.
void floorManagerPush(u32 floorID);   // push a floor.
void floorManagerOpen(u32 floorID);   // open the first floor.
}

// Boot hook after opening the first floor.
extern "C" void floorInitialOpenComplete()
{
}

// Register the room table.
extern "C" void floorSetRoomTable(const FloorRoomRecord *rooms, u32 roomCount)
{
}

// Room the floor stack is on.
u32 floorGetCurrentRoomID()
{
    return floorStackGetCurrentRoomID();
}

// Thread group of the floor on the stack; the floor's threads belong to it.
u32 floorGetGroupID()
{
    return floorStackGetThreadGroupID();
}

// A floor is pushed and running.
u8 floorIsPushd()
{
    return floorStackIsPushd();
}

// The current floor was resumed by popping the floor above it.
u8 floorIsPopd()
{
    return floorStackIsPopd();
}

// Push flag of the floor on the stack.
u8 floorIsPushFlag()
{
    return floorStackIsPushFlag();
}

// Change the running floor to the given one.
void floorChangeFloor(u32 floorID)
{
    floorOpen(floorID);
}

// Pop the floor on top of the stack.
void floorPop()
{
    floorManagerPop();
}

// Push the given floor on the stack.
void floorPush(u32 floorID)
{
    floorManagerPush(floorID);
}

// Open the first floor.
void floorOpenFloor(u32 floorID)
{
    floorManagerOpen(floorID);
}

// Initialization hook for the newly opened floor.
extern "C" void floorSetInitialRoom(u32 roomID)
{
}
