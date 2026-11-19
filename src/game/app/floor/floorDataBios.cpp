#include <app/floor/floorDataBios.h>
#include <floorBios.h>
#include <fight.h>
#include <floorManager.h>
#include <pokecolo.h>
#include <relglobal.h>

/* Treasure entries are addressed by this prefix plus the index of the entry a
   room owns. */
static const u32 TREASURE_ID_PREFIX = 0x7FFF0000;

extern "C" static u32 roomDataGetLanguageIndex(void);

u32 floorTresureGetResID(u16 tableIndex) {
    u32 id = 0;
    RoomData *room = floorDataBiosGetCurrentPtr();
    FloorTreasure *treasure = floor_tresure_list;

    for (u32 index = 0; index < tableIndex; index++) {
        if (treasure->floorID == room->floorID) {
            id++;
        }
        treasure++;
    }
    return TREASURE_ID_PREFIX | id;
}

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

void floorDataBiosSetMapResID(RoomData *floor, u32 resID) {
    if (floor == 0) {
        return;
    }
    floor->mapResID = resID;
}

struct FloorFieldCameraList *floorDataBiosGetFieldCameraListPtr(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    struct FloorFieldCameraList **cameraList = floor->fieldCameraList;
    if (cameraList == 0) {
        return 0;
    }
    return *cameraList;
}

u8 floorDataBiosGetPosListData(u32 index, GSvec *position, f32 *angle) {
    FloorPosList *posList = floorDataBiosGetPosListPtr(floorDataBiosGetCurrentPtr());

    if (posList == 0) {
        return 0;
    }
    if (index < *posList->count) {
        FloorPosData *data = &posList->data[index];
        if (position != 0) {
            position->set(data->x, data->y, data->z);
        }
        if (angle != 0) {
            /* The stored heading is in degrees; callers work in radians. */
            *angle = data->angle * (3.1415927f / 180.0f);
        }
        return 1;
    }
    return 0;
}

FloorPosList *floorDataBiosGetPosListPtr(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    FloorPosList **posList = floor->posList;
    if (posList == 0) {
        return 0;
    }
    return *posList;
}

FloorCharacter *floorDataBiosGetCharInfo(RoomData *floor, u32 index) {
    if (floor == 0) {
        return 0;
    }
    FloorCharacterList **characterList = floor->characterList;
    if (characterList == 0) {
        return 0;
    }
    FloorCharacterList *list = *characterList;
    if (index >= *list->count) {
        return 0;
    }
    return &list->characters[index];
}

u32 floorDataBiosGetCharNum(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    if (floor->characterList == 0) {
        return 0;
    }
    FloorCharacterList *list = *floor->characterList;
    if (list == 0) {
        return 0;
    }
    return *list->count;
}

FloorRoomCallback floorDataBiosGetPostFunc(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->postFunc;
}

FloorUpdateCallback floorDataBiosGetMainFunc(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->mainFunc;
}

FloorRoomCallback floorDataBiosGetPreFunc(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->preFunc;
}

u32 floorDataBiosGetPlaceName(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->placeName;
}

u16 floorDataBiosGetFloorID(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->floorID;
}

u32 roomDataGetFsysGidForLanguage(GSfloor_dd_ *room) {
    u32 languageIndex = roomDataGetLanguageIndex();

    if (room == 0) {
        return 0;
    }
    return room->fsysGid[languageIndex];
}

u32 mapGetGroupID(GSfloor_dd_ *room) {
    u32 languageIndex = roomDataGetLanguageIndex();

    if (room == 0) {
        return 0;
    }
    return room->fsysGid[languageIndex];
}

u8 floorDataBiosGetFloorKind(GSfloor_dd_ *floor) {
    if (floor == 0) {
        return 0;
    }
    if (floor->kind == FLOOR_KIND_FIGHT) {
        if (fightGetFightEncountDataId() == 0) {
            /* Encounter-less fight rooms use the alternate fight kind here. */
            if (pokecoloGetRomVersion() == POKECOLO_ROM_VERSION_ENCOUNTERLESS_FIGHT) {
                return FLOOR_KIND_FIGHT_ALT;
            }
            return FLOOR_KIND_FIELD;
        }
    }
    return floor->kind;
}

u32 floorDataBiosGetMapResID(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->mapResID;
}

u8 floorDataBiosGetArea(RoomData *floor) {
    if (floor == 0) {
        return 0;
    }
    return floor->area;
}

RoomData *floorDataBiosGetCurrentPtr(void) {
    return floorDataBiosGetPtr(GSfloorGetCurrent());
}

RoomData *floorDataBiosGetPtr(u32 floorID) {
    GSfloor_dd_ *floor = floordata;
    u32 count = floordata_number->floorCount;

    while (count != 0) {
        if (floor->floorID == floorID) {
            return floor;
        }
        floor++;
        count--;
    }
    return 0;
}

extern "C" static u32 roomDataGetLanguageIndex(void) {
    switch (pokecoloGetLanguage()) {
    case POKECOLO_LANGUAGE_ENGLISH:
        return 0;
    case POKECOLO_LANGUAGE_GERMAN:
        return 1;
    case POKECOLO_LANGUAGE_FRENCH:
        return 2;
    case POKECOLO_LANGUAGE_ITALIAN:
        return 3;
    case POKECOLO_LANGUAGE_SPANISH:
        return 4;
    }
    return 0;
}
