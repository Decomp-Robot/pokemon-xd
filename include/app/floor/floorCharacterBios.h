#ifndef APP_FLOOR_CHARACTER_BIOS_H
#define APP_FLOOR_CHARACTER_BIOS_H

#include <types.h>
#include <GSvec.h>
struct tagPeopleInfoBios;
typedef struct tagPeopleInfoBios PeopleInfo;

struct floorCharacter_dd_ {
    u8 visibility : 1;
    u8 loadInit : 1;
    u8 isHit : 1;
    u8 neckTargetHero : 1;
    u8 talkWallThrough : 1;
    u8 animBlend : 1;
    u8 unknownFlags : 2;
    u8 moveType : 3;
    u8 talkStartType : 2;
    u8 talkEndType : 2;
    u8 unknownTalkFlag : 1;
    u8 unknownByte1;
    u8 initialAnimIndex;
    s16 rotation;
    u16 peopleInfoIndex;
    u16 nameID;
    u32 unknownValue;
    u32 moveSctID;
    u32 talkSctID;
    f32 x;
    f32 y;
    f32 z;
};

typedef struct floorCharacter_dd_ FloorCharacter;

enum FloorCharacterTalkStartType {
    FLOOR_CHARACTER_TALK_START_TURN_TO_HERO = 2
};

enum FloorCharacterTalkEndType {
    FLOOR_CHARACTER_TALK_END_KEEP_FACING = 0
};

#ifdef __cplusplus
extern "C" {
#endif
void floorCharacterBiosSetVisibility(FloorCharacter *character, u32 visible);
void floorCharacterBiosSetPos(FloorCharacter *character, const GSvec *position);
u32 floorCharacterBiosGetTalkSctID(FloorCharacter *character);
u32 floorCharacterBiosGetMoveSctID(FloorCharacter *character);
u16 floorCharacterBiosGetNameID(FloorCharacter *character);
u8 floorCharacterBiosGetTalkWallThrough(FloorCharacter *character);
u8 floorCharacterBiosGetTalkEndType(FloorCharacter *character);
u8 floorCharacterBiosGetTalkStartType(FloorCharacter *character);
u8 floorCharacterBiosGetMoveType(FloorCharacter *character);
u8 floorCharacterBiosGetRot(FloorCharacter *character, GSvec *rotation);
u8 floorCharacterBiosGetPos(FloorCharacter *character, GSvec *position);
u8 floorCharacterBiosGetIsHit(FloorCharacter *character);
u8 floorCharacterBiosGetLoadInit(FloorCharacter *character);
u8 floorCharacterBiosGetVisibility(FloorCharacter *character);
PeopleInfo *floorCharacterBiosGetPeopleInfoPtr(FloorCharacter *character);
FloorCharacter *floorCharacterBiosFindByResID(u32 group, u32 id);
FloorCharacter *floorCharacterBiosGetCurrentPtr(u32 index);
FloorCharacter *floorCharacterBiosGetPtr(u32 floorID, u32 index);
void floorCharacterBiosInitGlobalCharacter(void);

#ifdef __cplusplus
}
#endif

#endif
