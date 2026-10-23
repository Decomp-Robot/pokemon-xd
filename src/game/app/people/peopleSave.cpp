#include "peopleSave.h"

enum {
    PEOPLE_SAVE_SIGNATURE = 0x103,
    PEOPLE_SAVE_VERSION = 4,
    PEOPLE_RESTORE_KEEP = 1,
    PEOPLE_UNSAVED_RESOURCE_MASK = 0xffff0000,
    PEOPLE_UNSAVED_RESOURCE_CLASS = 0x7ffe0000,
    PEOPLE_TARGET_PERSON = 1,
    PEOPLE_TARGET_POSITION = 2
};

struct PeopleSaveFlags {
    u8 moveScriptEnabled : 1;
    u8 visible : 1;
    u8 moveScriptBlocked : 1;
    u8 walkRunMotion : 1;
    u8 collisionEnabled : 1;
    u8 rotating : 1;
    u8 : 2;
    u8 moveType : 3;
    u8 savedMoveType : 3;
    u8 moveState : 2;
    u8 neckMode : 2;
    u8 : 6;
    u8 mUnknown[2];
};

unsigned long peopleGetAnywaySaveSize() {
    GSBinWriteSizeStream stream;
    _makeAnywaySaveData(&stream);
    return stream.getWriteSize();
}

void _peopleRestoreAnywaySaveData(GSBinReadStream* stream) {
    int slotIndex, recordIndex;
    int count = stream->readU8();
    unsigned long recordsOffset = stream->getReadSize();
    tagPeopleWork* bios;
    PeopleState* work;
    PeopleSaveFlags flags;

    for (slotIndex = 0; slotIndex < peopleBiosGetMaxNum(); slotIndex++) {
        bios = peopleBiosGetWorkPtr(slotIndex);
        if (bios) bios->restoreFlags = 0;
    }
    for (recordIndex = 0; recordIndex < count; recordIndex++) {
        if (stream->readU8()) {
            unsigned long recordOffset = stream->getReadSize();
            unsigned short recordSize = stream->readU16();
            stream->read(&flags, sizeof(flags));
            stream->readU32();
            unsigned long groupID = stream->readU32();
            unsigned long resourceID = stream->readU32();
            stream->readU32();
            for (slotIndex = 0; slotIndex < peopleBiosGetMaxNum(); slotIndex++) {
                bios = peopleBiosGetWorkPtr(slotIndex);
                work = &bios->state;
                if (bios->active && groupID == work->group &&
                    resourceID == work->id) {
                    bios->restoreFlags |= PEOPLE_RESTORE_KEEP;
                    break;
                }
            }
            stream->seek(recordOffset + recordSize);
        }
    }
    for (slotIndex = 0; slotIndex < peopleBiosGetMaxNum(); slotIndex++) {
        bios = peopleBiosGetWorkPtr(slotIndex);
        work = &bios->state;
        if (!(bios->restoreFlags & PEOPLE_RESTORE_KEEP)) {
            peopleClose(work->group, work->id, true);
        }
    }
    stream->seek(recordsOffset);
    for (recordIndex = 0; recordIndex < count; recordIndex++) {
        if (!stream->readU8()) continue;
        stream->readU16();
        stream->read(&flags, sizeof(flags));
        unsigned long peopleFlags = stream->readU32();
        unsigned long groupID = stream->readU32();
        unsigned long resourceID = stream->readU32();
        unsigned long objectID = stream->readU32();
        bool found = false;
        for (slotIndex = 0; slotIndex < peopleBiosGetMaxNum(); slotIndex++) {
            bios = peopleBiosGetWorkPtr(slotIndex);
            work = &bios->state;
            if (bios->active && groupID == work->group &&
                resourceID == work->id) {
                found = true;
                break;
            }
        }
        if (!found) {
            peopleOpen(groupID, resourceID, objectID);
            peopleAddCollision(groupID, resourceID);
            bios = peopleGet(groupID, resourceID);
            work = &bios->state;
        }
        unsigned long oldObjectID = work->infoId;
        _GSmodel* model = bios->model;
        work->moveScriptEnabled = flags.moveScriptEnabled;
        work->visible = flags.visible;
        work->moveScriptBlocked = flags.moveScriptBlocked;
        work->walkRunMotion = flags.walkRunMotion;
        work->collisionEnabled = flags.collisionEnabled;
        work->rotating = flags.rotating;
        work->moveType = flags.moveType;
        work->savedMoveType = flags.savedMoveType;
        work->moveState = flags.moveState;
        work->neckMode = flags.neckMode;
        work->flags = peopleFlags;
        work->group = groupID;
        work->id = resourceID;
        work->infoId = objectID;
        if (oldObjectID != work->infoId) {
            GSlogWrite("people %d (%08x) : objID error (%08x, %08x)\n", recordIndex,
                        work->id, oldObjectID, work->infoId);
            work->infoId = oldObjectID;
        }
        work->moveSpeedRate = stream->readFloat();
        work->savedMoveType = stream->readU8();
        work->moveSpeedRate = stream->readFloat();
        stream->read(&work->sidestepReferencePosition, sizeof(GSvec));
        stream->read(&work->targetPos, sizeof(GSvec));
        work->walkListCapacity = stream->readU8();
        if (work->walkListCapacity) {
            work->walkListIndex = stream->readU8();
            work->walkList = static_cast<GSvec*>(GSmemAllocClr(work->walkListCapacity * sizeof(GSvec)));
            for (int j = 0; j < work->walkListCapacity; j++) {
                stream->read(&work->walkList[j], sizeof(GSvec));
            }
        }
        switch (work->neckMode) {
        case PEOPLE_TARGET_PERSON:
            work->targetGroup = stream->readU32();
            work->targetId = stream->readU32();
            break;
        case PEOPLE_TARGET_POSITION:
            stream->read(&work->neckVector, sizeof(GSvec));
            break;
        }
        if (model) {
            stream->read(GSmodelGetPositionPtr(model), sizeof(GSvec));
            stream->read(&work->previousPosition, sizeof(GSvec));
            work->yaw = stream->readFloat();
            work->targetYaw = stream->readFloat();
            GSmodelGetRotationPtr(model)->y = work->yaw;
            GSmodelGetPositionPtr(model);
            GSmodelSetPosition(model, GSmodelGetPositionPtr(model));
            GSmodelSetRotation(model, GSmodelGetRotationPtr(model));
            peopleSetDisp(work->group, work->id, work->visible);
        } else {
            work->yaw = stream->readFloat();
            work->yaw = stream->readFloat();
            work->yaw = stream->readFloat();
            work->yaw = stream->readFloat();
        }
    }
}

void peopleRestoreAnywaySaveData(void* data, unsigned long size) {
    GSBinReadStream readStream(data, size);
    GSBinReadStream* stream = &readStream;
    if (stream->readU16() == PEOPLE_SAVE_SIGNATURE &&
        stream->readU8() == PEOPLE_SAVE_VERSION) {
        _peopleRestoreAnywaySaveData(stream);
        heroMoveTermEvent();
        gimmickBoxPop();
    }
}

void peopleMakeAnywaySaveData(void* data, unsigned long size) {
    GSBinWriteStream stream(data, size);
    _makeAnywaySaveData(&stream);
}

void _makeAnywaySaveData(GSBinWriteStream* stream) {
    int count = peopleBiosGetMaxNum();
    stream->writeU16(PEOPLE_SAVE_SIGNATURE);
    stream->writeU8(PEOPLE_SAVE_VERSION);
    stream->writeU8(count);
    _GSmodel* model;
    for (int i = 0; i < peopleBiosGetMaxNum(); i++) {
        tagPeopleWork* bios = peopleBiosGetWorkPtr(i);
        if (!bios->active || (bios->state.id & PEOPLE_UNSAVED_RESOURCE_MASK) == PEOPLE_UNSAVED_RESOURCE_CLASS) {
            stream->writeU8(0);
            continue;
        }
        stream->writeU8(1);
        unsigned long recordOffset = stream->getWriteSize();
        stream->writeU16(0);
        PeopleState* work = &bios->state;
        model = bios->model;
        PeopleSaveFlags flags;
        flags.moveScriptEnabled = work->moveScriptEnabled;
        flags.visible = work->visible;
        flags.moveScriptBlocked = work->moveScriptBlocked;
        flags.walkRunMotion = work->walkRunMotion;
        flags.collisionEnabled = work->collisionEnabled;
        flags.rotating = work->rotating;
        flags.moveType = work->moveType;
        flags.savedMoveType = work->savedMoveType;
        flags.moveState = work->moveState;
        flags.neckMode = work->neckMode;
        stream->write(&flags, sizeof(flags));
        stream->writeU32(work->flags);
        stream->writeU32(work->group);
        stream->writeU32(work->id);
        stream->writeU32(work->infoId);
        stream->writeFloat(work->moveSpeedRate);
        stream->writeU8(work->savedMoveType);
        stream->writeFloat(work->moveSpeedRate);
        stream->write(&work->sidestepReferencePosition, sizeof(GSvec));
        stream->write(&work->targetPos, sizeof(GSvec));
        stream->writeU8(work->walkListCapacity);
        if (work->walkListCapacity) {
            stream->writeU8(work->walkListIndex);
            for (int j = 0; j < work->walkListCapacity; j++) {
                stream->write(&work->walkList[j], sizeof(GSvec));
            }
        }
        switch (work->neckMode) {
        case PEOPLE_TARGET_PERSON:
            stream->writeU32(work->targetGroup);
            stream->writeU32(work->targetId);
            break;
        case PEOPLE_TARGET_POSITION:
            stream->write(&work->neckVector, sizeof(GSvec));
            break;
        }
        if (model) {
            stream->write(GSmodelGetPositionPtr(model), sizeof(GSvec));
            stream->write(&work->previousPosition, sizeof(GSvec));
            stream->writeFloat(GSmodelGetRotationPtr(model)->y);
            stream->writeFloat(work->targetYaw);
        } else {
            for (int remaining = 20; remaining > 0; remaining--) {
                stream->writeU8(0);
            }
        }
        int endOffset = stream->getWriteSize();
        stream->seek(recordOffset);
        stream->writeU16(endOffset - recordOffset);
        stream->seek(endOffset);
        GSmodelGetPositionPtr(model);
    }
}
