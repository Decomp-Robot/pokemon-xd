#include <app/floor/floorModule.h>
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
