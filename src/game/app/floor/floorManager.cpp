#include <app/floor/floorManager.h>
#include <app/floor/floorModule.h>
#include <floor.h>
#include <GSmem.h>
#include <savedata.h>

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

bool floorCheckAnywaySaveData(void)
{
    AnywaySaveHeader* save = (AnywaySaveHeader*)savedataGetStatus(0, SAVE_DATA_ANYWAY);
    return save->size != 0;
}

void floorMemoryCardSaveCallback(void)
{
    floorExecScriptIntegerByName("anywaysave_callback", GSfloorGetCurrentFloorGroupID(), 0, 0, 0);
}

bool floorMakeSaveData(unsigned char** data, unsigned long* size)
{
    _floorManager->makeSaveData(data, size);
    return true;
}

void floorFreeResource(unsigned long groupID, unsigned long resourceID)
{
    _floorManager->freeResource(groupID, resourceID);
}

void FloorManager::freeResource(unsigned long groupID, unsigned long resourceID)
{
    stackList->freeResource(groupID, resourceID);
}

void floorAddResource(unsigned long groupID, unsigned long resourceID)
{
    _floorManager->addResource(groupID, resourceID);
}

void FloorManager::addResource(unsigned long groupID, unsigned long resourceID)
{
    stackList->addResource(groupID, resourceID);
}

void floorWaitForChange(void)
{
    _floorManager->waitForChange();
}

GSthread* floorCreateThreadLocal(s32 priority, u32 stackSize, s32 terminateOnExit, GSthreadEntry entry)
{
    s32 previousHeap = GSmemSetCurrentHeap(floorGetLocalHeapHandle());
    GSthread* thread = floorCreateThread(priority, stackSize, terminateOnExit, entry);
    GSmemSetCurrentHeap(previousHeap);
    return thread;
}

GSthread* floorCreateThread(s32 priority, u32 stackSize, s32 terminateOnExit, GSthreadEntry entry)
{
    if (!entry) {
        return 0;
    }
    if (!stackSize) {
        stackSize = 16384;
    }
    return GSthreadCreate(priority, floorGetCurrentFloorGroupID(), stackSize, 1, terminateOnExit, entry);
}

void floorUnblockModule(unsigned long moduleHandle)
{
    if (_floorManager) {
        _floorManager->unblockModule(moduleHandle);
    }
}

void FloorManager::unblockModule(unsigned long moduleHandle)
{
    if (moduleList) {
        moduleList->unblockModule(moduleHandle);
    }
}

void floorBlockModule(unsigned long moduleHandle)
{
    if (_floorManager) {
        _floorManager->blockModule(moduleHandle);
    }
}

void FloorManager::blockModule(unsigned long moduleHandle)
{
    if (moduleList) {
        moduleList->blockModule(moduleHandle);
    }
}
