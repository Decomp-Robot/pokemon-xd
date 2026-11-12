#ifndef INCLUDE_APP_FLOOR_FLOORMANAGER_H
#define INCLUDE_APP_FLOOR_FLOORMANAGER_H

#include <floorManager.h>
#include <app/floor/floorStack.h>

class FloorModuleList;
class FloorManager {
public:
    bool makeSaveData(unsigned char** data, unsigned long* size);
    void freeResource(unsigned long groupID, unsigned long resourceID);
    void addResource(unsigned long groupID, unsigned long resourceID);
    void waitForChange();
    void unblockModule(unsigned long moduleHandle);
    void blockModule(unsigned long moduleHandle);

private:
    FloorModuleList* moduleList;
    FloorStackList* stackList;
    u8 unknown[2528];
};

extern FloorManager* _floorManager;

#endif
