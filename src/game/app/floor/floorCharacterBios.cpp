#include <app/floor/floorCharacterBios.h>
#include <app/floor/floorDataBios.h>
#include <floorManager.h>
#include <hero.h>
#include <GSmath.h>
#include <peopleInfoBios.h>
#include <string.h>

enum FloorCharacterResourceGroup {
    FLOOR_CHARACTER_RESOURCE_GROUP_GLOBAL = 0
};

enum GlobalCharacterIndex {
    GLOBAL_CHARACTER_MANA = 0,
    GLOBAL_CHARACTER_KLEIN = 1,
    GLOBAL_CHARACTER_GIRL = 2,
    GLOBAL_CHARACTER_COUNT = 3
};

enum GlobalCharacterNameID {
    GLOBAL_CHARACTER_NAME_MANA = 1,
    GLOBAL_CHARACTER_NAME_KLEIN = 3,
    GLOBAL_CHARACTER_NAME_GIRL = 24
};

enum GlobalCharacterPeopleInfoID {
    GLOBAL_CHARACTER_PEOPLE_INFO_MANA = 2,
    GLOBAL_CHARACTER_PEOPLE_INFO_KLEIN = 90,
    GLOBAL_CHARACTER_PEOPLE_INFO_GIRL = 45
};

enum FloorCharacterMoveType {
    FLOOR_CHARACTER_MOVE_TYPE_DEFAULT = 0
};

static FloorCharacter _globalCharacter[GLOBAL_CHARACTER_COUNT];

void floorCharacterBiosSetVisibility(FloorCharacter *character, u32 visible) {
    if (character) character->visibility = visible;
}

void floorCharacterBiosSetPos(FloorCharacter *character, const GSvec *position) {
    if (!character) return;
    if (!position) return;
    character->x = position->x;
    character->y = position->y;
    character->z = position->z;
}

u32 floorCharacterBiosGetTalkSctID(FloorCharacter *character) {
    if (character) return character->talkSctID;
    return 0;
}

u32 floorCharacterBiosGetMoveSctID(FloorCharacter *character) {
    if (character) return character->moveSctID;
    return 0;
}

u16 floorCharacterBiosGetNameID(FloorCharacter *character) {
    if (character) return character->nameID;
    return 0;
}

u8 floorCharacterBiosGetTalkWallThrough(FloorCharacter *character) {
    if (character) return character->talkWallThrough != 0;
    return 0;
}

u8 floorCharacterBiosGetTalkEndType(FloorCharacter *character) {
    if (character) return character->talkEndType;
    return 0;
}

u8 floorCharacterBiosGetTalkStartType(FloorCharacter *character) {
    if (character) return character->talkStartType;
    return 0;
}

u8 floorCharacterBiosGetMoveType(FloorCharacter *character) {
    if (character) return character->moveType;
    return 0;
}

u8 floorCharacterBiosGetRot(FloorCharacter *character, GSvec *rotation) {
    if (!character) return 0;
    if (!rotation) return 0;
    GSvecSet(rotation, 0.0f, 0.017453292f * character->rotation, 0.0f);
    return 1;
}

u8 floorCharacterBiosGetPos(FloorCharacter *character, GSvec *position) {
    if (!character) return 0;
    if (!position) return 0;
    GSvecSet(position, character->x, character->y, character->z);
    return 1;
}

u8 floorCharacterBiosGetIsHit(FloorCharacter *character) {
    if (character) return character->isHit != 0;
    return 0;
}

u8 floorCharacterBiosGetLoadInit(FloorCharacter *character) {
    if (character) return character->loadInit != 0;
    return 0;
}

u8 floorCharacterBiosGetVisibility(FloorCharacter *character) {
    if (character) return character->visibility != 0;
    return 0;
}

PeopleInfo *floorCharacterBiosGetPeopleInfoPtr(FloorCharacter *character) {
    if (character) return peopleInfoBiosGetPtrFromIndex(character->peopleInfoIndex);
    return NULL;
}

FloorCharacter *floorCharacterBiosFindByResID(u32 group, u32 id) {
    if (group == FLOOR_CHARACTER_RESOURCE_GROUP_GLOBAL) {
        if (id == HERO_RESOURCE_MANA) return &_globalCharacter[GLOBAL_CHARACTER_MANA];
        if (id == HERO_RESOURCE_KLEIN) return &_globalCharacter[GLOBAL_CHARACTER_KLEIN];
        if (id == HERO_RESOURCE_GIRL) return &_globalCharacter[GLOBAL_CHARACTER_GIRL];
        return NULL;
    }
    RoomData *floor = floorDataBiosGetPtr(GSfloorGetCurrent());
    if (group != mapGetGroupID(floor)) return NULL;
    return floorDataBiosGetCharInfo(floor, id);
}

FloorCharacter *floorCharacterBiosGetCurrentPtr(u32 index) {
    return floorCharacterBiosGetPtr(GSfloorGetCurrent(), index);
}

FloorCharacter *floorCharacterBiosGetPtr(u32 floorID, u32 index) {
    return floorDataBiosGetCharInfo(floorDataBiosGetPtr(floorID), index);
}

void floorCharacterBiosInitGlobalCharacter() {
    memset(_globalCharacter, 0, sizeof(_globalCharacter));
    FloorCharacter character;
    memset(&character, 0, sizeof(character));
    character.visibility = 1;
    character.isHit = 1;
    character.neckTargetHero = 1;
    character.talkWallThrough = 1;
    character.animBlend = 1;
    character.moveType = FLOOR_CHARACTER_MOVE_TYPE_DEFAULT;
    character.talkStartType = FLOOR_CHARACTER_TALK_START_TURN_TO_HERO;
    character.talkEndType = FLOOR_CHARACTER_TALK_END_KEEP_FACING;
    character.unknownByte1 = 0;
    character.initialAnimIndex = 0;
    character.rotation = 0;
    character.peopleInfoIndex = 0;
    character.nameID = 0;
    character.unknownValue = 0;
    character.moveSctID = 0;
    character.talkSctID = 0;
    character.x = 0.0f;
    character.y = 0.0f;
    character.z = 0.0f;
    character.nameID = GLOBAL_CHARACTER_NAME_MANA;
    character.peopleInfoIndex = GLOBAL_CHARACTER_PEOPLE_INFO_MANA;
    _globalCharacter[GLOBAL_CHARACTER_MANA] = character;
    character.nameID = GLOBAL_CHARACTER_NAME_KLEIN;
    character.peopleInfoIndex = GLOBAL_CHARACTER_PEOPLE_INFO_KLEIN;
    _globalCharacter[GLOBAL_CHARACTER_KLEIN] = character;
    character.nameID = GLOBAL_CHARACTER_NAME_GIRL;
    character.peopleInfoIndex = GLOBAL_CHARACTER_PEOPLE_INFO_GIRL;
    _globalCharacter[GLOBAL_CHARACTER_GIRL] = character;
}

void GSvecSet(GSvec *out, f32 x, f32 y, f32 z) {
    out->x = x;
    out->y = y;
    out->z = z;
}
