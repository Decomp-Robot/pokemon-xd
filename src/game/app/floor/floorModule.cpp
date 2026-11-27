#include <app/floor/floorModule.h>
#include <floor.h>
#include <GSscript.h>
#include <GSmem.h>

struct FloorPushModuleHeader {
    unsigned long uid;
    unsigned long size;
};

const unsigned long FLOOR_PUSH_MAGIC = 'FLOR';

void* FloorModuleList::makePushState(FloorStateMode mode)
{
    FloorModuleInterface* interface;
    unsigned long alignedSize;
    unsigned char* cursor;
    unsigned char* data;
    unsigned long total;
    void* state;
    FloorModuleEntry* entry;
    unsigned long stateSize;
    unsigned char* buffer;
    unsigned long* words;
    FloorPushModuleHeader* moduleHeader;

    dumpModule();
    total = 0;
    for (entry = mHead; entry; entry = entry->getNext()) {
        interface = entry->getModuleInterface();
        if (!interface || !interface->getStateSize) continue;
        stateSize = interface->getStateSize(mode);
        if (!stateSize) continue;
        alignedSize = (stateSize + 3) & ~3;
        total += alignedSize + sizeof(FloorPushModuleHeader);
    }

    buffer = (unsigned char*)GSmemAlloc(total + sizeof(FloorPushModuleHeader));
    state = buffer;
    if (!buffer) return 0;
    mPushStateSize = total + sizeof(FloorPushModuleHeader);
    words = (unsigned long*)buffer;
    *words++ = FLOOR_PUSH_MAGIC;
    *words++ = total + sizeof(FloorPushModuleHeader);
    cursor = (unsigned char*)words;

    for (entry = mHead; entry; entry = entry->getNext()) {
        interface = entry->getModuleInterface();
        if (!interface || !interface->getStateSize) continue;
        stateSize = interface->getStateSize(mode);
        if (!stateSize) {
            if (mode == FLOOR_STATE_MODE_SAVE) {
                interface->makeStateData(mode, 0, 0);
            }
            continue;
        }
        alignedSize = (stateSize + 3) & ~3;
        if (!interface->getUID) return 0;

        moduleHeader = (FloorPushModuleHeader*)cursor;
        moduleHeader->uid = interface->getUID();
        moduleHeader->size = alignedSize + sizeof(FloorPushModuleHeader);
        data = (unsigned char*)(moduleHeader + 1);
        interface->makeStateData(mode, data, alignedSize);
        cursor = data + alignedSize;
    }
    return state;
}

void FloorModuleList::killModuleGroup(unsigned long groupID)
{
    FloorModuleEntry* entry;

    for (entry = mHead; entry; entry = entry->getNext()) {
        if (entry->getGroupID() == groupID) {
            killModule(entry->getHandle());
        }
    }
    floorGetCommonScript()->deleteThreadGroup(groupID);
    floorGetCurentScript()->deleteThreadGroup(groupID);
}

void FloorModuleList::unblockModuleGroup(unsigned long groupID)
{
    FloorModuleEntry* entry;

    for (entry = mHead; entry; entry = entry->getNext()) {
        if (entry->getGroupID() == groupID) {
            unblockModule(entry->getHandle());
        }
    }
    floorGetCommonScript()->unblockThreadGroup(groupID);
    floorGetCurentScript()->unblockThreadGroup(groupID);
}

void FloorModuleList::blockModuleGroup(unsigned long groupID)
{
    FloorModuleEntry* entry;

    for (entry = mHead; entry; entry = entry->getNext()) {
        if (entry->getGroupID() == groupID) {
            blockModule(entry->getHandle());
        }
    }
    floorGetCommonScript()->blockThreadGroup(groupID);
    floorGetCurentScript()->blockThreadGroup(groupID);
}

void FloorModuleList::unregisterModuleGroup(unsigned long groupID)
{
    FloorModuleEntry* entry;

    for (entry = mEntries; entry != mEntries + mEntryCount; entry++) {
        if (entry->getGroupID() == groupID) {
            unregisterModule(entry->getHandle());
        }
    }
}

void FloorModuleList::killModule(unsigned long moduleHandle)
{
    FloorModuleEntry* entry = findHandle(moduleHandle);

    if (entry) {
        entry->kill();
    }
}

void FloorModuleList::unblockModule(unsigned long moduleHandle)
{
    FloorModuleEntry* entry = findHandle(moduleHandle);

    if (entry) {
        entry->unblock();
    }
}

void FloorModuleList::blockModule(unsigned long moduleHandle)
{
    FloorModuleEntry* entry = findHandle(moduleHandle);

    if (entry) {
        entry->block();
    }
}

void FloorModuleList::unregisterModule(unsigned long moduleHandle)
{
    FloorModuleEntry* entry = findHandle(moduleHandle);

    if (entry) {
        entry->terminate();
        removeFuncFromList(entry);
        entry->init();
    }
}

unsigned long FloorModuleList::registerScriptModule(unsigned char priority, unsigned long groupID,
    unsigned long preID, unsigned long updateID, unsigned long postID)
{
    FloorModuleEntry* entry = findFree();

    if (!entry) {
        return 0;
    }
    entry->init();
    entry->setScriptModule(priority, groupID, preID, updateID, postID);
    insertFuncIntoList(entry);
    entry->setHandle(getNewHandle());
    return entry->getHandle();
}

unsigned long FloorModuleList::registerModule(unsigned char priority, unsigned long groupID, FloorModuleInterface* interface)
{
    FloorModuleEntry* entry;

    if (!interface) {
        return 0;
    }
    entry = findFree();
    if (!entry) {
        return 0;
    }
    entry->init();
    entry->setModule(priority, groupID, interface);
    insertFuncIntoList(entry);
    entry->setHandle(getNewHandle());
    return entry->getHandle();
}
