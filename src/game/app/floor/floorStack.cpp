#include <floorStack.h>
#include <floorBios.h>
#include <floorDataBios.h>
#include <GSthread.h>

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
