#include <charNameBios.h>
#include <peopleTalk.h>
#include <people.h>
#include <peopleCollision.h>
#include <floorBios.h>
#include <floorBgmMask.h>
#include <hero.h>
#include <GSflag.h>
#include <GSlog.h>
#include <GSthread.h>
#include <msgctrl.h>
#include <time.h>
#include <winMsg.h>
#include <math.h>

enum {
    TALK_FACE_NECK = 0x10,
    TALK_FACE_BODY = 0x20,
    TALK_RESTORE_FACING = 0x40,
    TALK_SPEAKER_NAME_VALUE = 89,
    TALK_UNNAMED_SPEAKER = 6002,
    TALK_HERO_GROUP = 0,
    TALK_HERO_ID = 100,
    TALK_START_DISABLED = 3,
    PEOPLE_NO_INTERACTION = 1,
    HERO_PARTNER_MEMBER = 1,
    FLOOR_KIND_FIELD = 1
};
static float _talkWindowCloseDelay;
static u32 _peopleTalkRes[2];
extern "C" {
void peopleSetTalkDistance(u32 group,u32 id,float distance) {
    tagPeopleWork* people=peopleGet(group,id);
    if(people) people->talkDistance=distance;
}
float peopleGetTalkDistance(u32 group,u32 id) {
    tagPeopleWork* people=peopleGet(group,id);
    if(people == 0) return 0.0f;
    return people->talkDistance;
}
void peopleTalkLeaveFloor() {}
void peopleTalkEnterFloor() { _talkWindowCloseDelay=0.0f; }
void peopleTalkMessageDaemon() {
    if(floorDataBiosGetFloorKind(floorDataBiosGetCurrentPtr())==FLOOR_KIND_FIELD && _talkWindowCloseDelay>0.0f)
        _talkWindowCloseDelay-=timeGetLastFrameSec();
}
void peopleTalkMessageBgmStop() {
    FloorBgmMask* mask=floorSound_getBgmMaskObj(); if(mask) mask->endMask();
}
void peopleTalkMessageBgmStart() {
    FloorBgmMask* mask=floorSound_getBgmMaskObj(); if(mask) mask->startMask();
}
void peopleTalkMessageBgmSetVolumeRate(float rate) {
    FloorBgmMask* mask=floorSound_getBgmMaskObj(); if(mask) mask->setVolumeRate(rate);
}
void peopleTalkMessageBgmSetFadeSpeed(float inTime,float outTime) {
    FloorBgmMask* mask=floorSound_getBgmMaskObj(); if(mask) {
        mask->setFadeInTime(inTime); mask->setFadeOutTime(outTime);
    }
}
void peopleTalkMessageBgmMask(bool on) {
    FloorBgmMask* mask=floorSound_getBgmMaskObj(); if(mask) {
        if(on) mask->setActive(1); else mask->setActive(0);
    }
}
void peopleTalkMsgEnd(u32 group, u32 id) {
    winMsgClose(1);
    _talkWindowCloseDelay = 0.2f;
    tagPeopleWork* people = peopleGet(group, id);
    if (people) {
        u32 flags = peopleGetFlags(group, id);
        if (people->state.savedFacing) {
            people->state.savedFacing = 0;
            if (flags & TALK_FACE_BODY) {
                if (flags & TALK_RESTORE_FACING) peopleRotateToAngle(group, id, people->state.savedYaw, 1.0f);
            } else if (flags & TALK_FACE_NECK) {
                people->state.neckVector = people->state.savedVector;
                people->state.neckMode = 1;
                peopleSetNeckLimitHold(group, id, people->state.neckHold);
            }
        }
    }
    while (winMsgCheckField() != -1) GSthreadSwitch();
    _peopleTalkRes[0] = 0;
    _peopleTalkRes[1] = 0;
    msgctrlSetValue(TALK_SPEAKER_NAME_VALUE, TALK_UNNAMED_SPEAKER);
}
void peopleSetTalkFlagCallback() {
    if (!_peopleTalkRes[0] && !_peopleTalkRes[1]) {
        msgctrlSetValue(TALK_SPEAKER_NAME_VALUE, TALK_UNNAMED_SPEAKER);
    } else {
        u32 name = TALK_UNNAMED_SPEAKER;
        FloorCharacter* character = floorCharacterBiosFindByResID(_peopleTalkRes[0], _peopleTalkRes[1]);
        if (character) {
            u32 nameId = floorCharacterBiosGetNameID(character);
            u16 hearFlag = charNameBiosGetHearFlag(nameId);
            if (hearFlag) {
                GSflagOn(hearFlag);
                name = charNameBiosGetNameID(nameId);
            }
        }
        msgctrlSetValue(TALK_SPEAKER_NAME_VALUE, name);
    }
}
void peopleSetTalkKeyWaitCallback() {
    if (_peopleTalkRes[0] || _peopleTalkRes[1])
        peopleSpeakStart(_peopleTalkRes[0], _peopleTalkRes[1]);
}
static inline void _peopleTalkPrepare(u32 group, u32 id, tagPeopleWork* people, u8& sound) {
    u32 nameId;
    u16 hearFlag;
    u32 name;
    if (people) {
        u32 flags = peopleGetFlags(group, id);
        if (flags & TALK_FACE_BODY) {
            if (!people->state.savedFacing) {
                people->state.savedYaw = people->state.yaw;
                people->state.savedFacing = 1;
            }
            peopleRotateToTarget(group, id, TALK_HERO_GROUP, TALK_HERO_ID, 1.0f);
            peopleRotateCheck(group, id, 1);
        } else if (flags & TALK_FACE_NECK) {
            if (!people->state.savedFacing) {
                people->state.savedVector = people->state.neckVector;
                peopleMoveNeckTargetPeople(group, id, TALK_HERO_GROUP, TALK_HERO_ID, 2.0f);
                people->state.neckHold = peopleSetNeckLimitHold(group, id, 1);
                peopleCheckNeckRotate(group, id, 1);
                people->state.savedFacing = 1;
            }
        }
        tagPeopleInfoBios* info = peopleInfoBiosGetPtr(people->state.infoId);
        if (info) sound = peopleInfoBiosGetTalkSe(info);
        name = TALK_UNNAMED_SPEAKER;
        FloorCharacter* character = floorCharacterBiosFindByResID(people->state.group, people->state.id);
        if (character) {
            nameId = floorCharacterBiosGetNameID(character);
            hearFlag = charNameBiosGetHearFlag(nameId);
            if (!hearFlag) name = TALK_UNNAMED_SPEAKER;
            else if (GSflagTest(hearFlag) == 1) name = charNameBiosGetNameID(nameId);
        }
        msgctrlSetValue(TALK_SPEAKER_NAME_VALUE, name);
    }
}
void peopleTalkMsg(u32 group, u32 id, u32 message) {
    u8 sound = 0;
    tagPeopleWork* people = peopleGet(group, id);
    if (people) {
        _peopleTalkPrepare(group, id, people, sound);
        while (_talkWindowCloseDelay > 0.0f) GSthreadSwitch();
        _peopleTalkRes[0] = people->state.group;
        _peopleTalkRes[1] = people->state.id;
        FloorBgmMask* mask = floorSound_getBgmMaskObj();
        if (mask) {
            mask->startMask();
            winMsgOpenFieldWithSE(message, 1, 0, sound);
            mask->endMask();
        }
    }
}
tagPeopleWork* peopleTalkCheck(u32 group, u32 id, float, float angle) {
    float distance;
    float targetRadius;
    float targetAngle;
    float bestAngle;
    float radius;
    float bestArea = 10000.0f;
    float area;
    tagPeopleWork* people;
    tagPeopleWork* best;
    s32 i;
    bool hasPartner;
    tagPeopleWork* target;
    tagPeopleInfoBios* targetInfo;
    FloorCharacter* character;
    tagPeopleInfoBios* info;
    u32 partnerGroup;
    u32 partnerId;
    GSvec targetPos;
    GSvec partnerPos(0.0f, 0.0f, 0.0f);
    GSvec pos;
    GSvec rotation;
    angle = 0.017453292f * angle;
    best = 0;
    partnerGroup = 0;
    partnerId = 0;
    people = peopleGet(group, id);
    if (!people) return 0;
    pos = *peopleBiosGetPosPtr(people);
    info = peopleInfoBiosGetPtr(people->state.infoId);
    if (info) radius = peopleInfoBiosGetColBallSize(info);
    else radius = 4.0f;
    hasPartner = heroMoveIsMember(HERO_PARTNER_MEMBER) != 0;
    heroMoveGetResID(&partnerGroup, &partnerId, HERO_PARTNER_MEMBER);
    if (hasPartner) peopleGetPos(partnerGroup, partnerId, &partnerPos);
    for (i = 0; i < peopleBiosGetMaxNum(); ++i) {
        target = peopleBiosGetWorkPtr(i);
        if (!target->active) continue;
        if (people == target) continue;
        if (!target->state.visible) continue;
        if (peopleBiosCheckFlag(target, PEOPLE_NO_INTERACTION)) continue;
        character = floorCharacterBiosFindByResID(target->state.group, target->state.id);
        if (character && floorCharacterBiosGetTalkStartType(character) == TALK_START_DISABLED) continue;
        targetInfo = peopleInfoBiosGetPtr(target->state.infoId);
        if (!targetInfo) continue;
        peopleGetNeckPos(target->state.group, target->state.id, &targetPos);
        distance = PSVECDistance((const Vec*)&pos, (const Vec*)&targetPos);
        targetRadius = peopleInfoBiosGetColBallSize(targetInfo);
        float reach = radius + peopleGetTalkDistance(target->state.group, target->state.id);
        if (distance > reach + targetRadius) continue;
        targetAngle = peopleVecCalcRotY(&pos, &targetPos, people->state.yaw);
        if ((float)fabs(targetAngle) > angle) continue;
        if (gimmickBoxIsPushBoxFloor()) {
            if ((s32)(pos.y / 10.0f) != (s32)(targetPos.y / 10.0f)) continue;
        }
        bool checkWall;
        if (character) {
            checkWall = !floorCharacterBiosGetTalkWallThrough(character);
        } else {
            FloorTreasure* treasure = floorEventGetTresureList(target->state.id);
            if (treasure && (((u32)treasure->flags >> 5) & 7u) == 1u) {
                peopleBiosGetRot(target, &rotation);
                if ((float)fabs(peopleVecCalcRotY(&targetPos, &pos, rotation.y)) > 2.0943952f) continue;
            }
            checkWall = 0;
        }
        if (checkWall && (u8)GScolsys2HitCollision(&pos, &targetPos, radius, 0)) continue;
        if (hasPartner && peopleInsideCheck(&pos, &targetPos, &partnerPos, peopleInfoBiosGetColBallSize(targetInfo))) continue;
        area = distance * distance * (float)fabs(targetAngle) / 2.0f;
        GSlogWritef("talk ->  people(%d,%d)  len =%.2f  ang =%.2f  area =%.2f\n", target->state.group, target->state.id, distance, targetAngle, area);
        u8 replace = 0;
        if (area < bestArea) replace = 1;
        else if (area == bestArea) {
            float absAngle = (float)fabs(targetAngle);
            float absBest = (float)fabs(bestAngle);
            if (absBest < absAngle) replace = 1;
            else if (absBest == absAngle && targetAngle > 0.0f) replace = 1;
        }
        if (replace) {
            bestArea = area;
            best = target;
            bestAngle = targetAngle;
        }
    }
    return best;
}
}
