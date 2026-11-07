#include <string.h>
#include <floorPostCall.h>

// Floor/map resource reads hand their post-read work to this manager: a read
// that could not finish in place registers a callback here, and the floor
// update loop drains the registered calls once a frame.

void FloorPostCallManager::update(void)
{
    PendingCall *call;

    call = pendingCalls;
    while (call != pendingCalls + 8) {
        if (call->occupied != 0) {
            call->callback(call->group, call->resource, call->argument);
            memset(call, 0, sizeof(*call));
        }
        call++;
    }
}

void FloorPostCallManager::add(unsigned long group, unsigned long resource,
                               unsigned long argument, FloorPostCallFunc callback)
{
    PendingCall *call;

    call = pendingCalls;
    while (call != pendingCalls + 8) {
        if (call->occupied == 0) {
            call->occupied = 1;
            call->group = group;
            call->resource = resource;
            call->argument = argument;
            call->callback = callback;
            return;
        }
        call++;
    }
}

FloorPostCallManager::FloorPostCallManager(void)
{
    memset(pendingCalls, 0, sizeof(pendingCalls));
}
