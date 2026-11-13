#include <floorStack.h>
#include <floorBios.h>
#include <floorDataBios.h>
#include <GSthread.h>
#include <GSmem.h>
#include <AppScript.h>
#include <string.h>

void FloorStackList::unblock()
{
    GSthreadUnblockGroup(getGroupID());
    getCurrentStack()->unblock();
}

void FloorStackList::block()
{
    GSthreadBlockGroup(getGroupID());
    getCurrentStack()->block();
}

void FloorStackList::setGroupID(unsigned long groupID)
{
    FloorStack* stack = getCurrentStack();
    stack->setGroupID(groupID);
    stack->unblock();
}

void FloorStackList::setPushState(void* data, unsigned long size)
{
    getCurrentStack()->setPushState(data, size);
}

void FloorStackList::setUpdateState(FloorState state)
{
    getCurrentStack()->setUpdateState(state);
}

FloorEnterMode FloorStackList::getObjective()
{
    return getCurrentStack()->getObjective();
}

GSfloor_dd_* FloorStackList::getGSfloor()
{
    return floorDataBiosGetPtr(getCurrentStack()->getFloorID());
}

FloorStateMode FloorStackList::getStateMode()
{
    enum { FLOOR_KIND_FIELD = 1, FLOOR_KIND_FIGHT = 2 };
    u8 kind = floorDataBiosGetFloorKind(floorDataBiosGetPtr(getCurrentStack()->getFloorID()));
    if (kind == FLOOR_KIND_FIELD) {
        return FLOOR_STATE_MODE_FIELD;
    } else if (kind == FLOOR_KIND_FIGHT) {
        return FLOOR_STATE_MODE_FIGHT;
    }
    return FLOOR_STATE_MODE_NONE;
}

FloorStack* FloorStackList::pop()
{
    --mTop;
    FloorStack* stack = getCurrentStack();
    stack->setPopFlag();
    return stack;
}

FloorStack* FloorStackList::push(unsigned int floorID)
{
    ++mTop;
    FloorStack* stack = getCurrentStack();
    stack->clear();
    stack->setPushFlag();
    stack->setFloorID(floorID);
    return stack;
}

FloorStackList::~FloorStackList()
{
    if (mStack) {
        delete mStack;
        mStack = NULL;
    }
}

FloorStackList::FloorStackList(unsigned long capacity)
{
    mStack = new FloorStack[capacity];
    mTop = 0;
    mCapacity = capacity;
    for (unsigned long i = 0; i < capacity; ++i) {
        mStack[i].makeScriptModule();
    }
}

void FloorStack::addResource(unsigned long groupID, unsigned long resourceID)
{
    GSresID* resource;

    for (resource = mResources; resource != &mResources[64]; ++resource) {
        if (resource->groupId == groupID && resource->resourceId == resourceID) {
            GSresFreeResource(resource->groupId, resource->resourceId);
            return;
        }
    }
    for (resource = mResources; resource != &mResources[64]; ++resource) {
        if (resource->groupId == 0) {
            resource->set(groupID, resourceID);
            return;
        }
    }
}

void FloorStack::freeAllResource()
{
    GSresID* resource;

    for (resource = mResources; resource != &mResources[64]; ++resource) {
        if (resource->groupId != 0) {
            GSresFreeResource(resource->groupId, resource->resourceId);
        }
    }
    memset(mResources, 0, sizeof(mResources));
}

void FloorStack::freeResource(unsigned long groupID, unsigned long resourceID)
{
    GSresID* resource;

    for (resource = mResources; resource != &mResources[64]; ++resource) {
        if (resource->groupId == groupID && resource->resourceId == resourceID) {
            GSresFreeResource(resource->groupId, resource->resourceId);
            resource->clear();
            return;
        }
    }
}

void FloorStack::unblock()
{
    if (mThread) {
        GSthreadUnblock(mThread);
    }
}

void FloorStack::block()
{
    if (mThread) {
        GSthreadBlock(mThread);
    }
}

void FloorStack::reallocPushData()
{
    void* data = GSmemAlloc(mPushStateSize);

    if (data) {
        memcpy(data, mPushState, mPushStateSize);
        GSmemFree(mPushState);
        mPushState = data;
    }
}

void FloorStack::setPushState(void* data, unsigned long size)
{
    mPushState = data;
    mPushStateSize = size;
}

void FloorStack::setGroupID(unsigned long groupID)
{
    enum { THREAD_GROUP_ID_OFFSET = 8192 };

    mGroupID = groupID;
    if (mThread) {
        GSthreadSetGroupID(mThread, groupID + THREAD_GROUP_ID_OFFSET);
    }
}

void FloorStack::setFloorID(unsigned long floorID)
{
    enum { FLOOR_GROUP_ID_BASE = 20481 };

    mFloorID = floorID;
    mGroupID = floorID + FLOOR_GROUP_ID_BASE;
}

FloorEnterMode FloorStack::getObjective()
{
    if (mPushFlag) {
        return FLOOR_ENTER_PUSH;
    }
    if (mPopFlag) {
        return FLOOR_ENTER_POP;
    }
    return FLOOR_ENTER_CHANGE;
}

void FloorStack::clearFlag()
{
    mPushFlag = false;
    mPopFlag = false;
}

void FloorStack::setPopFlag()
{
    mPushFlag = false;
    mPopFlag = true;
}

void FloorStack::setPushFlag()
{
    mPushFlag = true;
    mPopFlag = false;
}

void FloorStack::clear()
{
    mGroupID = 0;
    mPushState = NULL;
    mPushFlag = false;
    mPopFlag = false;
    mType = FLOOR_MODULE_TYPE_NONE;
    mUpdateState = FloorStateUnused;
    mThread = NULL;
    memset(mResources, 0, sizeof(mResources));
}

GSscript::Tiga* FloorStack::getScriptModule()
{
    return mScriptModule;
}

void FloorStack::makeScriptModule()
{
    if (mScriptModule == NULL) {
        mScriptModule = scriptCreateAppScript();
    }
}

FloorStack::FloorStack()
{
    clear();
    mScriptModule = NULL;
    mThread = NULL;
}

void GSresID::set(unsigned long group, unsigned long resource)
{
    groupId = group;
    resourceId = resource;
}

void GSresID::clear()
{
    groupId = 0;
    resourceId = 0;
}
