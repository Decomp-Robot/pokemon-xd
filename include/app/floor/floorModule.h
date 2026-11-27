#ifndef INCLUDE_APP_FLOOR_FLOORMODULE_H
#define INCLUDE_APP_FLOOR_FLOORMODULE_H

#include <GSAPI/GSthread/GSthread.h>
#include <floorManager.h>

enum FloorStateMode {
    FLOOR_STATE_MODE_NONE = 0,
    FLOOR_STATE_MODE_FIELD = 1,
    FLOOR_STATE_MODE_FIGHT = 2,
    FLOOR_STATE_MODE_SAVE = 3
};
enum FloorState {
    FloorStateUnused = 0,
    FloorStateCreate = 1,
    FloorStatePre = 2,
    FloorStatePreWait = 3,
    FloorStateNormal = 4,
    FloorStateNormalWait = 5,
    FloorStatePost = 6,
    FloorStatePostWait = 7,
    FloorStateFree = 8
};
enum FloorModuleType {
    FLOOR_MODULE_TYPE_NONE = 0,
    FLOOR_MODULE_TYPE_ALL = 255
};
enum FloorEnterMode {
    FLOOR_ENTER_NORMAL = 0,
    FLOOR_ENTER_PUSH = 1,
    FLOOR_ENTER_POP = 2,
    FLOOR_ENTER_CHANGE = 3
};
struct GSthread;
struct FloorModuleInterface {
    void (*create)(unsigned int, FloorEnterMode);
    void (*initialize)(unsigned int, FloorEnterMode);
    void (*update)(unsigned int, FloorEnterMode);
    void (*updateThread)(GSthread*);
    void (*finalize)(unsigned int, FloorEnterMode);
    void (*free)(unsigned int, FloorEnterMode);
    const char* (*getModuleName)();
    void (*makeStateData)(FloorStateMode, void*, unsigned long);
    void (*restoreStateData)(FloorStateMode, void*, unsigned long);
    unsigned long (*getStateSize)(FloorStateMode);
    unsigned long (*getUID)();
};

class FloorModuleEntry {
public:
    FloorModuleEntry* mPrev;
    FloorModuleEntry* mNext;
    unsigned long mHandle;
    FloorState mScriptState;
    FloorModuleType mModuleType;
    unsigned long mGroupID;
    unsigned char mPriority;
    unsigned char mBlocked;
    unsigned char mExecuted;
    unsigned char mReserved;
    FloorModuleInterface mInterface;

    FloorModuleEntry* getNext() const;
    FloorModuleInterface* getModuleInterface();
    unsigned long getGroupID() const;
    unsigned long getHandle() const;
    void setHandle(unsigned long handle);
    void init();
    void kill();
    void unblock();
    void block();
    void terminate();
    void setScriptModule(unsigned char priority, unsigned long groupID,
        unsigned long preID, unsigned long updateID, unsigned long postID);
    void setModule(unsigned char priority, unsigned long groupID, FloorModuleInterface* interface);
};

extern "C" {
void FloorModuleInterface_clear(FloorModuleInterface* interface);
}

class FloorModuleList {
public:
    void killModuleGroup(unsigned long groupID);
    void unblockModuleGroup(unsigned long groupID);
    void blockModuleGroup(unsigned long groupID);
    void unregisterModuleGroup(unsigned long groupID);
    void killModule(unsigned long moduleHandle);
    void unblockModule(unsigned long moduleHandle);
    void blockModule(unsigned long moduleHandle);
    void unregisterModule(unsigned long moduleHandle);
    unsigned long registerScriptModule(unsigned char priority, unsigned long groupID,
        unsigned long preID, unsigned long updateID, unsigned long postID);
    unsigned long registerModule(unsigned char priority, unsigned long groupID, FloorModuleInterface* interface);

private:
    FloorModuleEntry* mEntries;
    unsigned long mEntryCount;
    unsigned long mNextHandle;
    FloorModuleEntry* mHead;
    unsigned long mPushStateSize;

    FloorModuleEntry* findHandle(unsigned long moduleHandle);
    FloorModuleEntry* findFree();
    void insertFuncIntoList(FloorModuleEntry* entry);
    void removeFuncFromList(FloorModuleEntry* entry);
    unsigned long getNewHandle();

public:
    void dumpModule();
    void* makePushState(FloorStateMode mode);
    virtual ~FloorModuleList();
};

#endif
