#include "floor.h"
#include "saveData.h"

extern "C" {

u32 GSmemSetCurrentHeap(u32 heap);
u32 floorStackGetThreadGroupID();
void floorStackListFreeResource(FloorStackList *stack, u32 group, u32 res);
void floorStackListAddResource(FloorStackList *stack, u32 group, u32 res);
u32 floorGetLocalHeap();
}

// Whether trainer data contains saved floor state for continuing the game.
u8 floorCheckAnywaySaveData()
{
    AnywaySaveData *saveInfo =
        (AnywaySaveData *)getTrainerDataStartOffset(0, TRAINER_DATA_ANYWAY_SAVE);

    return saveInfo->mSaveDataSize != 0;
}

// Name of the floor script function that continues the floor once the memory
// card save is done.
const char floorSaveCallbackScriptName[] = "anywaysave_callback";

// Tell the floor script that the memory card save finished, so the floor can
// continue where it stopped.
void floorMemoryCardSaveCallback()
{
    floorExecScriptFunctionByName(floorSaveCallbackScriptName, floorGetGroupID(), 0, 0, 0);
}

// Build the save data of the open floor.
u8 floorMakeSaveData(u8 **data, unsigned long *size)
{
    gFloorManager->makeSaveData(data, size);
    return 1;
}

// Release a resource the open floor registered.
void floorStackFreeResource(u32 group, u32 res)
{
    _floorStackFreeResource(gFloorManager, group, res);
}

void _floorStackFreeResource(FloorManager *manager, u32 group, u32 res)
{
    floorStackListFreeResource(manager->mStack, group, res);
}

// Register a resource with the open floor; the floor stack frees it when the
// floor is left.
void floorStackAddResource(u32 group, u32 res)
{
    _floorStackAddResource(gFloorManager, group, res);
}

void _floorStackAddResource(FloorManager *manager, u32 group, u32 res)
{
    floorStackListAddResource(manager->mStack, group, res);
}

// Wait until the floor change is done.
void floorWaitForChange()
{
    gFloorManager->waitForChange();
}

// Start a thread on the floor's own heap.
GSthread *floorCreateThreadLocal(u32 priority, u32 stackSize, u32 flag, GSthreadEntry entry)
{
    u32 heap;
    GSthread *thread;

    heap = GSmemSetCurrentHeap(floorGetLocalHeap());
    thread = floorCreateThread(priority, stackSize, flag, entry);
    GSmemSetCurrentHeap(heap);
    return thread;
}

// Start a thread in the current floor's group. The stack is the floor's
// default when the caller does not pass one.
GSthread *floorCreateThread(u32 priority, u32 stackSize, u32 flag, GSthreadEntry entry)
{
    if (entry == 0) {
        return 0;
    }
    if (stackSize == 0) {
        stackSize = FLOOR_THREAD_STACK_SIZE;
    }
    return GSthreadCreate(priority, floorStackGetThreadGroupID(), stackSize, 1, flag, entry);
}

// Unblock the registered module the handle belongs to.
void floorUnblockModuleHandle(u32 handle)
{
    if (gFloorManager != 0) {
        gFloorManager->unblockModule(handle);
    }
}
