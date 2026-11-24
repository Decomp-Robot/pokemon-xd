#ifndef PEOPLE_H
#define PEOPLE_H
#include <GSgfx.h>
#include <GSmodel.h>

#ifdef __cplusplus
extern "C" {
#endif
void peopleAllBlockMoveScript(void);
void peopleBlock(void);
void peopleUnblock(void);

void peopleSetTresureSearchDistance(u16 treasureId, float distance);
void peopleTalkMessageBgmMask(bool enabled);
void peopleTalkMessageBgmSetFadeSpeed(float inTime, float outTime);
void peopleTalkMessageBgmSetVolumeRate(float rate);
void peopleTalkMessageBgmStop(void);
void peopleTalkMessageBgmStart(void);
s32 peopleReadObject(s32 slot, s32 kind, s32 objID);
#ifdef __cplusplus
}
#endif

struct PeopleState {
    bool moveScriptEnabled;
    bool visible;
    bool moveScriptBlocked;
    bool collisionEnabled;
    unsigned long flags;
    unsigned long group;
    unsigned long id;
    unsigned long infoId;
    float walkSpeed;
    float runSpeed;
    float moveSpeed;
    u8 unknown2C[0x14];
    float yaw;
    float targetYaw;
    u8 unknown48[8];
    s32 collisionIndex;
    u8 moveType;
    u8 moveState;
    u8 rotating;
    u8 savedMoveType;
    float moveSpeedRate;
    GSvec sidestepReferencePosition;
    GSvec targetPos;
    u16 walkListCapacity;
    u16 walkListIndex;
    GSvec* walkList;
    u8 strictWalkList;
    u8 unknown7D[31];
    u32 motionType;
    u8 savedFacing;
    u8 neckHold;
    u8 unknownA2[2];
    float savedYaw;
    GSvec previousPosition;
    u8 unknownB4[16];
    u8 neckMode;
    u8 unknownC5[3];
    GSvec neckVector;
    u8 unknownD4[24];
    GSvec savedVector;
    unsigned long targetGroup;
    unsigned long targetId;
    u8 unknown100[12];
    u8 unknown10C;
    u8 unknown10D[15];
    s32 attachGroup;
    s32 attachId;
    s32 attachPartIndex;
    u8 unknown128[32];
    float blinkTimer;
    u8 blinkEnabled;
    u8 speaking;
    u8 unknown14E[6];
    u8 animBlendEnabled;
    u8 unknown155[7];
    bool walkRunMotion;
    u8 unknown15D[3];
};
struct tagPeopleWork {
    u8 getMoveType();
    int getRunMotion();
    int getWalkMotion();
    void setAnimType(GSmodelAnimType type);
    void setAnimFrame(float frame);
    void setAnimBlend(int baseIndex, int blendIndex);
    void setAnimIndex(int index);
    void setAnimRate(float rate, bool scaleByPlaySpeed);
    u8 isAnimLast() const;
    unsigned long getFlags() const;
    u8 checkFlags(unsigned long flags) const;
    bool isWalkStartMotion() const;
    bool isAlwayBlendChara() const;
    bool isHero() const;
    bool isFollower() const;
    u8 isAnimBlend() const;
    bool active;
    u8 restoreFlags;
    u8 unknown02[2];
    int handle;
    _GSmodel* model;
    PeopleState state;
    float moveSpeed;
    s32 waitMotionIndex;
    s32 walkMotionIndex;
    s32 runMotionIndex;
    s32 walkStartMotionIndex;
    s32 runStartMotionIndex;
    float talkDistance;
    u16 treasureId;
    u8 unknown17E[18];
    s32 speechAnimations[7];
    float speechTimer;
};
struct tagPeopleInfoBios {
    u8 talkSe : 3;
    u8 boundCheck : 1;
    u8 colType : 2;
    u8 noWalkStartMotion : 1;
    u8 unknownFlag : 1;
    char neckIndex;
    char chestIndex;
    u8 unknown03;
    unsigned long modelResID;
    float walkLength;
    float runLength;
    float colBallSize;
    float neckLeftLimit;
    float neckRightLimit;
    float neckUpLimit;
    float neckDownLimit;
    float talkDistance;
    char speechParts[7];
    char blinkParts[4];
    u8 unknown33;
};
enum PeopleBiosStateFlags {
    PEOPLE_BIOS_STATE_ACTIVE = 0x1,
    PEOPLE_BIOS_STATE_MODEL_VISIBLE = 0x2,
    PEOPLE_BIOS_STATE_HAS_TREASURE = 0x4
};
enum PeopleBehaviourFlags {
    PEOPLE_FLAG_NO_INTERACTION = 0x1,
    PEOPLE_FLAG_DISABLE_ANIM_RATE_UPDATE = 0x8000,
    PEOPLE_FLAG_DISABLE_MOTION_RESET = 0x80000000
};

// Group of the characters that live outside any floor.
enum PeopleGroupID {
    PEOPLE_GROUP_GLOBAL = 0
};

struct MotionTable {
    long motions[5];
};

extern "C" {
tagPeopleWork* peopleGet(unsigned long group, unsigned long id);
unsigned long peopleGetHumanID(unsigned long group, unsigned long id);
tagPeopleWork* peopleSearchID(unsigned long humanID);
_GSmodel* peopleBiosGetModelPtr(tagPeopleWork* people);
void peopleAttach(unsigned long group, unsigned long id, unsigned long parentGroup, unsigned long parentID, long partIndex);
void peopleWalkTo(unsigned long group, unsigned long id, bool useAngle, GSvec* destination);
u8 peopleMoveCheck(unsigned long group, unsigned long id, u8 wait);
void peopleSetPosXYZ(unsigned long group, unsigned long id, float x, float y, float z);
u8 peopleSetMotion(unsigned long group, unsigned long id, long motion, long frame, bool loop);
void peopleSetFlags(unsigned long group, unsigned long id, unsigned long flags);
void peopleDetach(unsigned long group, unsigned long id, bool keepTransform);
bool peopleWaitSyncMotion(unsigned long group, unsigned long id, bool wait);
void peopleStopMotion(unsigned long group, unsigned long id);
void peopleSetMotionForce(unsigned long group, unsigned long id, long motion, long frame, bool loop);
unsigned long peopleBiosGetPushDataSize();
void peopleBiosPushData(void* data, unsigned long bufferSize);
void peopleBiosPopData(void* data, unsigned long bufferSize);
s32 peopleBiosGetUseObjIDSize(unsigned long objectId);
tagPeopleWork* peopleBiosGetTresureIDToPeopleWork(u16 treasureId);
void peopleBiosSetTresureID(tagPeopleWork* people, u16 treasureId);
void peopleBiosSetCollision(tagPeopleWork* people, bool enable);
void peopleBiosSetDispFlag(tagPeopleWork* people, bool visible);
void peopleBiosSetFlagOff(tagPeopleWork* people, unsigned long flags);
void peopleBiosSetFlagOn(tagPeopleWork* people, unsigned long flags);
void peopleBiosSetFlags(tagPeopleWork* people, unsigned long flags);
s32 peopleBiosGetMaxNum();
unsigned long peopleBiosGetState(tagPeopleWork *people);
tagPeopleWork* peopleBiosGetWorkPtr(s32 index);
u8 peopleBiosCheckFlag(tagPeopleWork* people, unsigned long flag);
u8 peopleCheckFlag(u32 group, u32 id, u32 flag);
void peopleChangeFollower(u32 objectID);
void peopleSetFollowerPtr(tagPeopleWork* people);
tagPeopleInfoBios* peopleInfoBiosGetPtr(unsigned long id);
void peopleSetFlagOn(unsigned long group, unsigned long id, unsigned long flags);
GSvec* peopleBiosGetPosPtr(tagPeopleWork* people);
GSvec* peopleBiosGetPrevPosPtr(tagPeopleWork* people);
void peopleBiosSetPos(tagPeopleWork* people, GSvec* position);
void peopleBiosGetRot(tagPeopleWork* people, GSvec* rotation);
void peopleBiosSetPrevPos(tagPeopleWork* people, GSvec* position);
void peopleBiosSetRot(tagPeopleWork* people, GSvec* rotation);
GSvec* peopleBiosGetRotPtr(tagPeopleWork* people);
void peopleBiosGetPos(tagPeopleWork* people, GSvec* position);
tagPeopleWork* peopleBiosGetNewWork();
tagPeopleWork* peopleBiosGetPeopleWork(unsigned long group, unsigned long id);
bool peopleBiosFreeWork(tagPeopleWork* people);
tagPeopleWork* peopleBiosAllocWork(s32 maxNum);
void peopleSpeakStart(u32 group, u32 id);
u32 peopleGetFlags(u32 group, u32 id);
void peopleRotateToAngle(u32 group, u32 id, float angle, float speed);
u8 peopleSetNeckLimitHold(u32 group, u32 id, u8 hold);
void peopleRotateToTarget(u32 group,u32 id,u32 targetGroup,u32 targetId,float speed);
u8 peopleRotateCheck(u32 group,u32 id,u8 wait);
void peopleMoveNeckTargetPeople(u32 group,u32 id,u32 targetGroup,u32 targetId,float speed);
u8 peopleCheckNeckRotate(u32 group,u32 id,u8 wait);
u8 peopleInfoBiosGetTalkSe(tagPeopleInfoBios* info);
float peopleInfoBiosGetColBallSize(tagPeopleInfoBios* info);
void peopleGetNeckPos(u32 group,u32 id,GSvec* pos);
float peopleVecCalcRotY(const GSvec* a,const GSvec* b,float yaw);
u8 peopleInsideCheck(const GSvec* a,const GSvec* b,GSvec* c,float radius);
void peopleSpeakStop(u32 group, u32 id);
void peopleBlinkStart(u32 group, u32 id);
void peopleBlinkStop(u32 group, u32 id);
void peopleBlinkOnce(u32 group, u32 id);
void peopleTaskSwich(u32 group, u32 id);
void peopleSetFlagOff(u32 group, u32 id, u32 flags);
void peopleSetRotXYZ(u32 group, u32 id, float x, float y, float z);
void peopleWalkToXYZ(u32 group, u32 id, u8 keepMoving, float x, float y, float z);
void peopleWalkToTarget(u32 group, u32 id, u32 targetGroup, u32 targetId, float speed, u8 keepMoving);
void peopleSetNeckTargetXYZ(u32 group, u32 id, long x, long y, long z);
void peopleRemoveNeckTarget(u32 group, u32 id, float speed);
void peopleRotateTo(u32 group, u32 id, float angle);
void peopleRotateToXYZ(u32 group, u32 id, float x, float y, float z, float speed);
void peopleRotateStop(u32 group, u32 id);
void peopleSetSpeedRate(u32 group, u32 id, float rate);
void peopleInitEvent(u32 group, u32 id);
void peopleTermEvent(u32 group, u32 id);
void peopleFallAppear(u32 group, u32 id, float height);
void peopleDispMark(u32 group, u32 id, u32 markType);
void peopleSetCollision(u32 group, u32 id, u8 enable);
u8 peopleGazeHeroCheck(u32 group, u32 id);
u8 peopleSetAnimBlend(u32 group, u32 id, long motion, long frame, long frames);
u8 peopleWaitSyncMotionBlend(u32 group, u32 id, bool wait);
long peopleWalkPause(u32 group, u32 id, u8 pause);
u8 peopleWalkPauseTalkMode(u32 group, u32 id, u8 talkMode);
u32 peopleRunOrNot(u32 group, u32 id);
float peopleGetSpeed(u32 group, u32 id);
void peopleSetDefaultMotionTable(u32 group, u32 id);
void peopleSetMotionTable(u32 group, u32 id, MotionTable table);
void peopleSpeedSlowDownOn(u32 group, u32 id);
void peopleSpeedSlowDownOff(u32 group, u32 id);
void peopleSetRot(u32 group, u32 id, GSvec* rotation);
void peopleMoveNeckTargetHero(u32 group, u32 id, float speed);
void peopleSetNeckTargetHero(u32 group, u32 id);
void peopleSetNeckTarget(u32 group, u32 id, u32 targetGroup, u32 targetId);
void peopleMoveNeckTarget(u32 group, u32 id, u32 targetGroup, u32 targetId, float speed);
void peopleMoveNeckAngle(u32 group, u32 id, float yaw, float pitch, float speed);
void peopleMoveNeckAngleWorld(u32 group, u32 id, float yaw, float pitch, float speed);
void peopleMoveNeckTargetXYZ(u32 group, u32 id, long x, long y, long z, float speed);
float peopleGetYDegreeRotation(u32 group, u32 id);
void peopleAutoAnimation(u32 group, u32 id);
void peopleManualAnimation(u32 group, u32 id);
void peopleSetWalkSpeed(u32 group, u32 id, float speed, long animation);
void peopleAttachWithMode(u32 group, u32 id, u32 parentGroup, long partIndex, long mode, u8 keepTransform);
void peopleSetWaitAnimation(u32 group, u32 id);
void peopleUnsetWaitAnimation(u32 group, u32 id);
}
#endif
