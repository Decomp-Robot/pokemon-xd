#ifndef INCLUDE_APP_FLOOR_FLOORMANAGER_H
#define INCLUDE_APP_FLOOR_FLOORMANAGER_H

#include <floorManager.h>
#include <app/floor/floorStack.h>

class FloorModuleList;
class FloorManager {
public:
    unsigned long registerScriptModule(unsigned char priority, unsigned long groupID,
        unsigned long preID, unsigned long updateID, unsigned long postID);
    unsigned long registerModule(unsigned char priority, unsigned long groupID, FloorModuleInterface* interface);
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
