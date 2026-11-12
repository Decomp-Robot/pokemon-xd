#ifndef INCLUDE_APP_FLOOR_FLOORSTACK_H
#define INCLUDE_APP_FLOOR_FLOORSTACK_H

class FloorStack;
class FloorStackList {
public:
    void freeResource(unsigned long groupID, unsigned long resourceID);
    void addResource(unsigned long groupID, unsigned long resourceID);

private:
    FloorStack* stacks;
    unsigned long stackDepth;
    unsigned long stackCount;
};

#endif
