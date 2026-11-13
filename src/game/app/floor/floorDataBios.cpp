#include <app/floor/floorDataBios.h>
#include <floorBios.h>

u32 floorDataBiosModelIDGetFogState(u32 modelID) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;

    RoomData *room = floorDataBiosGetCurrentPtr();
    if (modelID == 0) {
        return 1;
    }
    if (room == 0) {
        return 1;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 1;
    }
    resources = resourceTable->resources;
    {
        u32 index;
        for (index = 0; index < 3; index++) {
            if (resources->modelID[index] == modelID) {
                return 0;
            }
        }
    }
    return 1;
}

u32 floorDataBiosGetSecondaryRenderNum(RoomData *room) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;
    u32 count = 0;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    if (resources->secondaryRenderID[0] != 0) {
        count++;
    }
    if (resources->secondaryRenderID[1] != 0) {
        count++;
    }
    if (resources->secondaryRenderID[2] != 0) {
        count++;
    }
    return count;
}

u32 floorDataBiosGetSecondaryRenderID(RoomData *room, u32 index) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    if (index >= 3) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    return resources->secondaryRenderID[index];
}

u32 floorDataBiosGetShadowReciveNum(RoomData *room) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;
    u32 count = 0;
    u32 index;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    for (index = 0; index < 8; index++) {
        if (resources->shadowReciveID[index] != 0) {
            count++;
        }
    }
    return count;
}

u32 floorDataBiosGetShadowReciveID(RoomData *room, u32 index) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    if (index >= 8) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    return resources->shadowReciveID[index];
}

u32 floorDataBiosGetShadowLightID(RoomData *room) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    return resources->shadowLightID;
}

u32 floorDataBiosGetLensFlareResID(RoomData *room) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    {
        u32 lensFlareResID = 0;
        if (lensFlareResID != 0) {
            return lensFlareResID;
        }
    }
    return 0;
}

u32 floorDataBiosGetSunResID(RoomData *room) {
    FloorMapResourceTable *resourceTable;
    FloorMapResourceData *resources;

    if (room == 0) {
        return 0;
    }
    resourceTable = room->resourceTable;
    if (resourceTable == 0) {
        return 0;
    }
    resources = resourceTable->resources;
    if (resources == 0) {
        return 0;
    }
    return resources->sunResID;
}
