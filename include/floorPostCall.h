#ifndef FLOOR_POST_CALL_H
#define FLOOR_POST_CALL_H

typedef void *(*FloorPostCallFunc)(unsigned long group, unsigned long resource,
                                  unsigned long argument);

class FloorPostCallManager
{
public:
    FloorPostCallManager(void);
    void update(void);
    void add(unsigned long group, unsigned long resource, unsigned long argument,
             FloorPostCallFunc callback);

private:
    struct PendingCall {
        unsigned long occupied;
        unsigned long group;
        unsigned long resource;
        unsigned long argument;
        FloorPostCallFunc callback;
    };
    PendingCall pendingCalls[8];
};

#endif
