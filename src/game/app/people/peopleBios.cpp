#include <people.h>

#include <GSmemory.h>
#include <GSmodel.h>
#include <GScolsys2.h>
#include <GSvec.h>
#include <hero.h>
#include <peopleCollision.h>
#include <peopleWalkList.h>

#include <string.h>

/* Snapshot of the per-person status block: the state plus the work fields that
   directly follow it in tagPeopleWork. */
struct PeopleBiosSavedStatus {
    PeopleState state;
    u8 unknown160[24];
    float talkDistance;
    u16 treasureId;
    u8 unknown17E[18];
};

/* One record per active person in the peopleBiosPushData buffer. */
struct PeopleBiosSavedWork {
    PeopleBiosSavedStatus status;
    GSvec modelPosition;
    GSvec modelRotation;
    GSvec attachedPosition;
    GSvec attachedRotation;
    GSmodelState modelState;
};

extern "C" {
void peopleOpenSub(tagPeopleWork* people, unsigned long group, unsigned long id, unsigned long resourceId);
void peopleSetMotionType(tagPeopleWork* people, unsigned long motionType);
void peopleDoAttach(_GSmodel* model, unsigned long group, unsigned long id, s32 partIndex, bool reset);
void _peopleDoNeckUpdate(tagPeopleWork* people);
void _peopleRegisterAnimMix(tagPeopleWork* people);
}

static s32 _people_num;
static tagPeopleWork* _pPeopleWorkTop;

unsigned long peopleBiosGetPushDataSize()
{
    s32 i;
    u32 size = 8;
    for (i = 0; i < peopleBiosGetMaxNum(); ++i) {
        tagPeopleWork* people = peopleBiosGetWorkPtr(i);
        if (!people->active) {
            continue;
        }
        size += sizeof(PeopleBiosSavedWork);
        u16 walkCount = people->state.walkListCapacity;
        if (walkCount == 0 || people->state.walkList == 0) {
            continue;
        }
        size += walkCount * sizeof(GSvec);
    }
    return size;
}

void peopleBiosPushData(void* data, unsigned long bufferSize)
{
    u8* cursor = (u8*)data + 2 * sizeof(u32);
    _GSmodel* model;
    s32 i;
    u32 count = 0;

    for (i = 0; i < peopleBiosGetMaxNum(); i++) {
        tagPeopleWork* people = peopleBiosGetWorkPtr(i);
        if (!people->active) {
            continue;
        }
        PeopleBiosSavedWork* saved = (PeopleBiosSavedWork*)cursor;
        model = people->model;
        count++;
        memcpy(&saved->status, &people->state, sizeof(saved->status));
        memcpy(&saved->modelPosition, GSmodelGetPositionPtr(model), sizeof(GSvec));
        memcpy(&saved->modelRotation, GSmodelGetRotationPtr(model), sizeof(GSvec));
        memcpy(&saved->attachedPosition, &model->mAttachedPosition, sizeof(GSvec));
        memcpy(&saved->attachedRotation, &model->mAttachedRotation, sizeof(GSvec));
        GSmodelPushState(model, &saved->modelState);
        if (people->state.walkListCapacity != 0 && people->state.walkList != 0) {
            u8* next = cursor + sizeof(PeopleBiosSavedWork);
            memcpy(next, people->state.walkList, people->state.walkListCapacity * sizeof(GSvec));
            next += people->state.walkListCapacity * sizeof(GSvec);
            peopleFreeWalkList(people->state.group, people->state.id);
            cursor = next;
        } else {
            cursor += sizeof(PeopleBiosSavedWork);
        }
        if (cursor > (u8*)data + bufferSize) {
            return;
        }
    }
    ((u32*)data)[0] = bufferSize;
    ((u32*)data)[1] = count;
}

void peopleBiosPopData(void* data, unsigned long bufferSize)
{
    u32* header = (u32*)data;
    tagPeopleWork* restored[64];
    u8* cursor;
    s32 i;
    u32 total;
    u32 count;
    _GSmodel* model;

    memset(_pPeopleWorkTop, 0, _people_num * sizeof(tagPeopleWork));
    if (header[0] != bufferSize) {
        return;
    }
    if (header[1] > 64) {
        return;
    }
    total = header[1];
    count = total;
    cursor = (u8*)data + 2 * sizeof(u32);
    i = 0;
    while (count--) {
        tagPeopleWork* people = peopleBiosGetNewWork();
        PeopleBiosSavedWork* saved = (PeopleBiosSavedWork*)cursor;
        peopleOpenSub(people, saved->status.state.group, saved->status.state.id, saved->status.state.infoId);
        memcpy(&people->state, &saved->status, sizeof(PeopleBiosSavedStatus));
        if (people->model != 0) {
            GSmodelPopState(people->model, &saved->modelState);
        }
        restored[i++] = people;
        if (people->state.walkListCapacity != 0 && people->state.walkList != 0) {
            u8* next = cursor + sizeof(PeopleBiosSavedWork);
            next += people->state.walkListCapacity * sizeof(GSvec);
            cursor = next;
        } else {
            cursor += sizeof(PeopleBiosSavedWork);
        }
    }
    cursor = (u8*)&((u32*)data)[2];
    i = 0;
    while (total--) {
        tagPeopleWork* people = restored[i++];
        PeopleBiosSavedWork* saved = (PeopleBiosSavedWork*)cursor;
        model = people->model;
        peopleBiosSetDispFlag(people, people->state.visible);
        peopleBiosSetCollision(people, people->state.collisionEnabled);
        peopleSetMotionType(people, people->state.motionType);
        GSmodelPopState(model, &saved->modelState);
        if (people->state.attachGroup != -1 && people->state.attachId != -1) {
            peopleDoAttach(model, people->state.attachGroup, people->state.attachId, people->state.attachPartIndex, false);
            *GSmodelGetPositionPtr(model) = saved->modelPosition;
            *GSmodelGetRotationPtr(model) = saved->modelRotation;
            GSmodelSetPosition(model, &saved->attachedPosition);
            GSmodelSetRotation(model, &saved->attachedRotation);
        }
        people->state.unknownB4[12] = 0;
        people->state.unknown10C = 0;
        _peopleRegisterAnimMix(people);
        if (people->state.animBlendEnabled != 0) {
            GSmodelEnableAnimBlend(people->model);
        }
        if (people->state.neckMode != 0) {
            for (s32 j = 0; j < 60; ++j) {
                _peopleDoNeckUpdate(people);
            }
        }
        if (people->state.walkListCapacity != 0 && people->state.walkList != 0) {
            GSvec* points = (GSvec*)((u8*)saved + sizeof(PeopleBiosSavedWork));
            people->state.walkList = (GSvec*)GSmemAlloc(people->state.walkListCapacity * sizeof(GSvec));
            if (people->state.walkList != 0) {
                memcpy(people->state.walkList, points, people->state.walkListCapacity * sizeof(GSvec));
            } else {
                people->state.walkListCapacity = 0;
                people->state.walkListIndex = 0;
            }
            points += people->state.walkListCapacity;
            cursor = (u8*)points;
        } else {
            cursor += sizeof(PeopleBiosSavedWork);
        }
    }
    gimmickBoxPop();
}

s32 peopleBiosGetUseObjIDSize(unsigned long objectId)
{
    s32 i;
    s32 result = 0;

    for (i = 0; i < peopleBiosGetMaxNum(); ++i) {
        tagPeopleWork* people = peopleBiosGetWorkPtr(i);
        if (people->active && people->state.infoId == objectId) {
            ++result;
        }
    }
    return result;
}

tagPeopleWork* peopleBiosGetTresureIDToPeopleWork(u16 treasureId)
{
    for (s32 i = 0; i < peopleBiosGetMaxNum(); ++i) {
        tagPeopleWork* people = peopleBiosGetWorkPtr(i);
        if (people->active && people->treasureId == treasureId) {
            return people;
        }
    }
    return 0;
}

void peopleBiosSetTresureID(tagPeopleWork* people, u16 treasureId)
{
    people->treasureId = treasureId;
}

void peopleBiosSetCollision(tagPeopleWork* people, bool enable)
{
    people->state.collisionEnabled = enable;
    if (people->state.collisionIndex >= 0) {
        GScolsys2HumanEnable(people->state.collisionIndex, enable);
    }
}

void peopleBiosSetDispFlag(tagPeopleWork* people, bool visible)
{
    _GSmodel* model = peopleBiosGetModelPtr(people);
    if (model != 0) {
        people->state.visible = visible;
        GSmodelSetVisibility(model, visible);
    }
}

u8 peopleBiosCheckFlag(tagPeopleWork* people, unsigned long flag)
{
    return (people->state.flags & flag) != 0;
}

void peopleBiosSetFlagOff(tagPeopleWork* people, unsigned long flags)
{
    people->state.flags &= ~flags;
}

void peopleBiosSetFlagOn(tagPeopleWork* people, unsigned long flags)
{
    people->state.flags |= flags;
}

void peopleBiosSetFlags(tagPeopleWork* people, unsigned long flags)
{
    people->state.flags = flags;
}

_GSmodel* peopleBiosGetModelPtr(tagPeopleWork* people)
{
    return people->model;
}

void peopleBiosSetPrevPos(tagPeopleWork* people, GSvec* position)
{
    people->state.previousPosition = *position;
}

GSvec* peopleBiosGetPrevPosPtr(tagPeopleWork* people)
{
    return &people->state.previousPosition;
}

void peopleBiosSetRot(tagPeopleWork* people, GSvec* rotation)
{
    GSmodelSetRotation(people->model, rotation);
}

void peopleBiosGetRot(tagPeopleWork* people, GSvec* rotation)
{
    GSmodelGetRotation(people->model, rotation);
}

GSvec* peopleBiosGetRotPtr(tagPeopleWork* people)
{
    return GSmodelGetRotationPtr(people->model);
}

void peopleBiosSetPos(tagPeopleWork* people, GSvec* position)
{
    GSmodelSetPosition(people->model, position);
}

void peopleBiosGetPos(tagPeopleWork* people, GSvec* position)
{
    GSmodelGetPosition(people->model, position);
}

GSvec* peopleBiosGetPosPtr(tagPeopleWork* people)
{
    return GSmodelGetPositionPtr(people->model);
}

unsigned long peopleBiosGetState(tagPeopleWork* people)
{
    if (people == 0) {
        return 0;
    }
    if (!people->active) {
        return 0;
    }
    PeopleBiosStateFlags state = PEOPLE_BIOS_STATE_ACTIVE;
    if (GSmodelGetVisibility(people->model) != 0) {
        state = (PeopleBiosStateFlags)(state | PEOPLE_BIOS_STATE_MODEL_VISIBLE);
    }
    if (people->treasureId != 0) {
        state = (PeopleBiosStateFlags)(state | PEOPLE_BIOS_STATE_HAS_TREASURE);
    }
    return state;
}

tagPeopleWork* peopleBiosGetNewWork()
{
    tagPeopleWork* people;
    s32 i;
    for (i = 0; i < peopleBiosGetMaxNum(); ++i) {
        people = peopleBiosGetWorkPtr(i);
        if (!people->active) {
            memset(people, 0, sizeof(tagPeopleWork));
            people->active = true;
            people->handle = (int)people;
            people->state.collisionIndex = -1;
            people->state.moveSpeed = people->state.walkSpeed;
            return people;
        }
    }
    return 0;
}

tagPeopleWork* peopleBiosGetPeopleWork(unsigned long group, unsigned long id)
{
    tagPeopleWork* people = _pPeopleWorkTop;
    tagPeopleWork* end = people + _people_num;
    while (people != end) {
        if (people->active && people->state.id == id && people->state.group == group) {
            return people;
        }
        ++people;
    }
    return 0;
}

tagPeopleWork* peopleBiosGetWorkPtr(s32 index)
{
    if (index < 0 || _people_num <= index) {
        return 0;
    }
    return _pPeopleWorkTop + index;
}

s32 peopleBiosGetMaxNum()
{
    return _people_num;
}

bool peopleBiosFreeWork(tagPeopleWork* people)
{
    people->active = false;
    people->state.moveScriptEnabled = false;
    return true;
}

tagPeopleWork* peopleBiosAllocWork(s32 maxNum)
{
    tagPeopleWork* work = (tagPeopleWork*)GSmemAllocClr(maxNum * sizeof(tagPeopleWork));
    _pPeopleWorkTop = work;
    _people_num = maxNum;
    return work;
}

u8 tagPeopleWork::isAnimBlend() const
{
    return state.animBlendEnabled;
}

bool tagPeopleWork::isFollower() const
{
    Hero* hero = Hero::getHeroPtr();
    if (state.group == hero->getFollowerGrpID() && state.id == hero->getFollowerResID()) {
        return true;
    }
    return false;
}

bool tagPeopleWork::isHero() const
{
    return state.id == HERO_RESOURCE_HERO;
}
