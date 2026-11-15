#include <floor.h>
#include <GSmodel.h>
#include <GSlight.h>

struct FloorMapStateEntry {
    unsigned long resource;
    FloorMapResourceKind kind;
    union {
        GSmodelState model;
        GSlightState light;
    } state;
};

void _floorMapPushCB(unsigned long kind, unsigned long group, unsigned long resource,
                     void* object, void* context)
{
    FloorMapStateEntry** cursor = (FloorMapStateEntry**)context;
    FloorMapStateEntry* entry = *cursor;

    entry->resource = resource;
    entry->kind = (FloorMapResourceKind)kind;
    switch (kind) {
    case FLOOR_MAP_RESOURCE_MODEL:
        GSmodelPushState((_GSmodel*)object, &entry->state.model);
        break;
    case FLOOR_MAP_RESOURCE_LIGHT:
        GSlightPushState((GSlight*)object, &entry->state.light);
        break;
    }
    *cursor = entry + 1;
}
