#ifndef FLOOR_STACK_H
#define FLOOR_STACK_H

#include <types.h>
#include <GSres.h>
#include <app/floor/floorModule.h>

struct GSfloor_dd_;
class AppScript;

class FloorStack {
public:
    FloorStack();
    void unblock();
    void block();
    void setGroupID(unsigned long groupID);
    void setPushState(void* data, unsigned long size);
    void setUpdateState(FloorState state);
    FloorEnterMode getObjective();
    unsigned long getFloorID() const;
    void setFloorID(unsigned long floorID);
    void setPopFlag();
    void setPushFlag();
    void clear();
    void makeScriptModule();

private:
    unsigned long mGroupID;
    unsigned long mFloorID;
    void* mPushState;
    unsigned long mPushStateSize;
    bool mPushFlag;
    bool mPopFlag;
    AppScript* mScriptModule;
    FloorModuleType mType;
    GSthread* mThread;
    GSresID mResources[64];
    FloorState mUpdateState;
};

class FloorStackList {
public:
    FloorStackList(unsigned long capacity);
    ~FloorStackList();
    void unblock();
    void block();
    void setGroupID(unsigned long groupID);
    void setPushState(void* data, unsigned long size);
    void setUpdateState(FloorState state);
    FloorEnterMode getObjective();
    GSfloor_dd_* getGSfloor();
    FloorStateMode getStateMode();
    FloorStack* pop();
    FloorStack* push(unsigned int floorID);
    FloorStack* getCurrentStack();
    unsigned long getGroupID();

private:
    FloorStack* mStack;
    unsigned long mTop;
    unsigned long mCapacity;
};

#endif
