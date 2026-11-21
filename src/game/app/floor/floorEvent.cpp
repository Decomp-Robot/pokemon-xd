#include <types.h>

#include <GSgfxVideo.h>
#include <GSflag.h>
#include <GSlog.h>
#include <GSmodel.h>
#include <GSpart.h>
#include <GSsndPlayback.h>
#include <sound.h>
#include <GSthread.h>
#include <GSvec.h>
#include <floor.h>
#include <floorBios.h>
#include <floorManager.h>
#include <hero.h>
#include <heroMove.h>
#include <msgctrl.h>
#include <pcbox.h>
#include <people.h>
#include <peopleSave.h>
#include <peopleTalk.h>
#include <roomData.h>
#include <script.h>
#include <winMsg.h>

void _floorEventWaitMotion(_GSmodel* model);
void _floorEventSetMotion(_GSmodel* model, s16 anim, f32 frame);
void _floorEventWaitME();
s32 _floorEventPlayMEThread(GSthread* thread);

struct FloorDoorEntry {
    s8 openMotion;
    s8 closeMotion;
    s8 middleMotion;
    s8 alternateMotion;
    u8 reserved04;
    s8 enterMotion;
    s8 exitMotion;
    u8 soundKind;
    u8 modelPart;
    u8 reserved09;
    s16 collisionIndex;
    u8 reserved0C;
    u8 reserved0D;
    u16 openFlag;
    u16 closeFlag;
    u16 reserved12;
    u32 modelResourceID;
};

struct FloorScriptEntry {
    u8 kind;
    u8 reserved01;
    u16 floorID;
    u32 scriptID;
    u32 entryPoint;
    s32 parameter1;
    s32 parameter2;
    s32 parameter3;
    s32 parameter4;
};

extern "C" {
extern u32* floor_tresure_list_number;
extern FloorTreasure* floor_tresure_list;
extern u32* floor_event_list_number;
extern FloorScriptEntry* floor_event_list;
extern u32* floor_door_list_number;
extern FloorDoorEntry* floor_door_list;
}

static const char floorEventRangeOver[] = "[%s] Range Over!!\n";

struct PlayMEParam {
    u32 soundID;
    u8 active;
    u8 reserved05;
    u8 reserved06;
    u8 reserved07;
};

PlayMEParam _playMEParam;

enum TreasureID {
    TREASURE_ID_PREFIX = 0x7fff0000,
    TREASURE_ID_INDEX_MASK = 0x1ff
};

enum TreasureKind {
    TREASURE_KIND_ITEM = 1,
    TREASURE_KIND_MONEY = 2,
    TREASURE_KIND_ITEM_VARIANT = 4
};

enum TreasureMode {
    TREASURE_MODE_NO_ME = 0,
    TREASURE_MODE_PLAY_ME = 1
};

enum TreasureCommand {
    TREASURE_COMMAND_DISP = 0,
    TREASURE_COMMAND_RESET = 1,
    TREASURE_COMMAND_PICKUP = 2
};

enum TreasureMotion {
    TREASURE_MOTION_CLOSED = 0,
    TREASURE_MOTION_OPENING = 1,
    TREASURE_MOTION_OPEN = 2
};

enum TreasureFlag {
    TREASURE_FLAG_KIND_SHIFT = 2,
    TREASURE_FLAG_KIND_MASK = 0x7,
    TREASURE_FLAG_STATE_SHIFT = 5,
    TREASURE_FLAG_STATE_MASK = 0x7
};

// Presentation of a treasure actor, carried in the treasure flags.
enum TreasureState {
    TREASURE_STATE_CHEST = 1,
    TREASURE_STATE_RESERVED02 = 2,
    TREASURE_STATE_RESERVED03 = 3
};
enum DoorSoundKind {
    DOOR_SOUND_KIND_DEFAULT = 1,
    DOOR_SOUND_KIND_ALTERNATE = 3,
    DOOR_SOUND_KIND_RESERVED02 = 2,
    DOOR_SOUND_KIND_RESERVED04 = 4,
    DOOR_SOUND_KIND_RESERVED05 = 5,
    DOOR_SOUND_KIND_RESERVED06 = 6
};

enum DoorMode {
    DOOR_MODE_CONDITIONAL_OPEN = 0,
    DOOR_MODE_OPEN = 1,
    DOOR_MODE_CONDITIONAL_CLOSE = 2,
    DOOR_MODE_CLOSE = 3
};

enum ElevatorMode {
    ELEVATOR_MODE_MOVE = 0x01,
    ELEVATOR_MODE_MOVE_REVERSE = 0x02,
    ELEVATOR_MODE_ENTER = 0x04,
    ELEVATOR_MODE_EXIT = 0x08,
    ELEVATOR_MODE_FINAL_FRAME = 0x10,
    ELEVATOR_MODE_ATTACH = 0x20,
    ELEVATOR_MODE_WAIT = 0x40,
    ELEVATOR_MODE_SOUND = 0x80,
    ELEVATOR_MODE_OPEN = 0x81,
    ELEVATOR_MODE_CLOSE = 0x82,
    ELEVATOR_MODE_STOP = 0xc0
};

enum FloorEventSound {
    FLOOR_EVENT_SOUND_DOOR_OPEN = 39,
    FLOOR_EVENT_SOUND_DOOR_CLOSE = 38,
    FLOOR_EVENT_SOUND_ELEVATOR_OPEN = 41,
    FLOOR_EVENT_SOUND_ELEVATOR_CLOSE = 42,
    FLOOR_EVENT_SOUND_DOOR_OPEN_ALT = 43,
    FLOOR_EVENT_SOUND_DOOR_1123 = 1123,
    FLOOR_EVENT_SOUND_DOOR_1325 = 1325,
    FLOOR_EVENT_SOUND_DOOR_1377 = 1377
};

static const u32 FloorEventThreadPriority = 128;

enum TreasureMessage {
    TREASURE_MSG_NONE = 0,
    TREASURE_MSG_MONEY_GAIN = 54001,
    TREASURE_MSG_MONEY_RECEIVED = 54004,
    TREASURE_MSG_ITEM_GAIN = 54002,
    TREASURE_MSG_ITEM_GAIN_MANY = 54003,
    TREASURE_MSG_ITEM_RECEIVED = 54005,
    TREASURE_MSG_ITEM_RECEIVED_MANY = 54006,
    TREASURE_MSG_ITEM_BAG_FULL = 54007,
    TREASURE_MSG_ITEM_BOX_FULL = 54008,
    TREASURE_MSG_ITEM_SENT = 54009,
    TREASURE_MSG_ITEM_SPECIAL = 54010,
    TREASURE_MSG_ITEM_OTHER = 54011
};

enum TreasureSound {
    TREASURE_SOUND_ME = 974,
    TREASURE_SOUND_OPEN = 1121
};

extern "C" {

FloorTreasure* floorEventGetTresureList(u32 id) {
    FloorTreasure* entry;
    u32 index;
    u32 matched;
    u32 wanted;
    entry = 0;
    matched = 0;
    if ((id & TREASURE_ID_PREFIX) != TREASURE_ID_PREFIX) return 0;
    wanted = id & TREASURE_ID_INDEX_MASK;
    index = 0;
    while (index < *floor_tresure_list_number) {
        entry = &floor_tresure_list[index];
        if (entry->floorID == GSfloorGetCurrent()) {
            if (wanted == matched++) break;
        }
        index++;
    }
    if (index == *floor_tresure_list_number) return 0;
    return entry;
}

void floorEventSetTresureDisp(u32 index, bool disp) {
    FloorTreasure* treasure;
    u32 i;
    u32 count;
    u32 mask;
    count = 0;
    i = 0;
    while (i < *floor_tresure_list_number) {
        treasure = &floor_tresure_list[i];
        if (treasure->floorID == GSfloorGetCurrent()) {
            if (i == index) {
                mask = TREASURE_ID_PREFIX | count;
                break;
            }
            count++;
        }
        i++;
    }
    if (i != *floor_tresure_list_number) {
        peopleSetDisp(floorGetCurrentGroupID(), mask, disp);
    }
}

s32 floorEventChangeTresure(u32 index, u16 value, u8 count) {
    if (index >= *floor_tresure_list_number) {
        GSlogWrite(floorEventRangeOver, __FUNCTION__);
        return -1;
    }
    FloorTreasure* treasure = &floor_tresure_list[index];
    treasure->value = value;
    treasure->count = count;
    return 0;
}

s32 floorEventGetTresureEx(u8 kind, u32 item, s32 count, u8 mode) {
    s32 result;
    TreasureMessage message = TREASURE_MSG_NONE;
    u8 boxOverflow = 0;
    while (true) {
        if (GSsndGetStatus(TREASURE_SOUND_ME) != GSSND_PLAYING) break;
        GSthreadSwitch();
    }
    if (mode == TREASURE_MODE_PLAY_ME) {
        soundPlayWait(TREASURE_SOUND_ME);
        _playMEParam.soundID = TREASURE_SOUND_ME;
        _playMEParam.active = 1;
        floorCreateThread(FloorEventThreadPriority, 0, 1, (GSthreadEntry)_floorEventPlayMEThread);
    }
    switch (kind) {
    case TREASURE_KIND_MONEY:
        msgctrlSetValue(MSGCTRL_VALUE_MONEY, item);
        winMsgOpen(WINMSG_WINDOW_FIELD, TREASURE_MSG_MONEY_GAIN, 1, 0);
        winMsgClose(1);
        heroAddPokedoru(0, item);
        winMsgOpen(WINMSG_WINDOW_FIELD, TREASURE_MSG_MONEY_RECEIVED, 1, 0);
        _floorEventWaitME();
        winMsgClose(1);
        break;
    case TREASURE_KIND_ITEM:
    case TREASURE_KIND_ITEM_VARIANT:
        if (count > 0) {
            msgctrlSetValue(MSGCTRL_VALUE_ITEM_ID, item);
            msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, count);
            if (kind == TREASURE_KIND_ITEM) {
                message = (count == 1) ? TREASURE_MSG_ITEM_GAIN : TREASURE_MSG_ITEM_GAIN_MANY;
                winMsgOpen(WINMSG_WINDOW_FIELD, message, 1, 0);
                winMsgClose(1);
            }
            result = heroItemAddItemDataId(0, item, count, -1);
            if (result == 0) {
                message = (count == 1) ? TREASURE_MSG_ITEM_RECEIVED : TREASURE_MSG_ITEM_RECEIVED_MANY;
            } else if (result > 0) {
                result = pcboxAddItem(0, item, result);
                if (result != 0) boxOverflow = 1;
                message = (count == 1) ? TREASURE_MSG_ITEM_BAG_FULL : TREASURE_MSG_ITEM_BOX_FULL;
            }
            winMsgOpen(WINMSG_WINDOW_FIELD, message, 1, 0);
            if (boxOverflow == 1) winMsgOpen(WINMSG_WINDOW_FIELD, TREASURE_MSG_ITEM_SENT, 1, 0);
            _floorEventWaitME();
            winMsgClose(1);
        }
        break;
    }
    _floorEventWaitME();
    return result;
}

s32 floorEventGetTresure(u8 kind, u32 item, s32 count) {
    s32 result;
    s32 storedInBox = 0;
    soundPlayWait(TREASURE_SOUND_ME);
    _playMEParam.soundID = TREASURE_SOUND_ME;
    _playMEParam.active = 1;
    floorCreateThread(FloorEventThreadPriority, 0, 1, (GSthreadEntry)_floorEventPlayMEThread);
    switch (kind) {
    case TREASURE_KIND_MONEY:
        msgctrlSetValue(MSGCTRL_VALUE_MONEY, item);
        winMsgOpen(WINMSG_WINDOW_FIELD, TREASURE_MSG_MONEY_GAIN, 1, 0);
        winMsgClose(1);
        heroAddPokedoru(0, item);
        winMsgOpen(WINMSG_WINDOW_FIELD, TREASURE_MSG_MONEY_RECEIVED, 1, 0);
        _floorEventWaitME();
        winMsgClose(1);
        break;
    case TREASURE_KIND_ITEM:
    case TREASURE_KIND_ITEM_VARIANT:
        if (count > 0) {
            s32 received;
            TreasureMessage message;
            msgctrlSetValue(MSGCTRL_VALUE_ITEM_ID, item);
            msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, count);
            if (kind == TREASURE_KIND_ITEM) {
                winMsgOpen(WINMSG_WINDOW_FIELD, (count == 1) ? TREASURE_MSG_ITEM_GAIN : TREASURE_MSG_ITEM_GAIN_MANY, 1, 0);
                winMsgClose(1);
            }
            result = heroItemAddItemDataId(0, item, count, -1);
            received = count - result;
            if (result > 0) {
                result = pcboxAddItem(0, item, result);
                storedInBox = count - result - received;
            }
            if (received > 0) {
                message = (count == 1) ? TREASURE_MSG_ITEM_RECEIVED : TREASURE_MSG_ITEM_RECEIVED_MANY;
                msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, received);
                winMsgOpen(WINMSG_WINDOW_FIELD, message, 1, 1);
                winMsgClose(1);
            }
            if (count - received > 0) {
                if (storedInBox != 0) {
                    if (count == 1) {
                        message = TREASURE_MSG_ITEM_BAG_FULL;
                        msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, storedInBox);
                    } else if (received == 0) {
                        message = TREASURE_MSG_ITEM_BOX_FULL;
                        msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, count);
                    } else {
                        message = TREASURE_MSG_ITEM_SPECIAL;
                        msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, count - received);
                    }
                } else {
                    message = TREASURE_MSG_ITEM_SPECIAL;
                    msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, count - received);
                }
                winMsgOpen(WINMSG_WINDOW_FIELD, message, 1, 1);
                winMsgClose(1);
            }
            if (count - received - storedInBox > 0) {
                msgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, count - received - storedInBox);
                winMsgOpen(WINMSG_WINDOW_FIELD, (count == 1) ? TREASURE_MSG_ITEM_SENT : TREASURE_MSG_ITEM_OTHER, 1, 1);
                winMsgClose(1);
            }
            _floorEventWaitME();
        }
        break;
    }
    _floorEventWaitME();
    return result;
}

s32 floorEventCtrlTresure(u32 group, u32 id, u8 command) {
    FloorTreasure* treasure = floorEventGetTresureList(id);
    if (!treasure) return -1;
    switch (command) {
    case TREASURE_COMMAND_DISP:
        switch ((treasure->flags >> TREASURE_FLAG_STATE_SHIFT) & TREASURE_FLAG_STATE_MASK) {
        case TREASURE_STATE_CHEST:
            peopleSetMotion(group, id, TREASURE_MOTION_OPEN, 0, 1);
            peopleSetFlagOn(group, id, PEOPLE_FLAG_NO_INTERACTION);
            break;
        case TREASURE_STATE_RESERVED02:
        case TREASURE_STATE_RESERVED03:
            peopleSetDisp(group, id, 0);
            break;
        }
        break;
    case TREASURE_COMMAND_RESET:
        switch ((treasure->flags >> TREASURE_FLAG_STATE_SHIFT) & TREASURE_FLAG_STATE_MASK) {
        case TREASURE_STATE_CHEST:
            peopleSetMotion(group, id, TREASURE_MOTION_CLOSED, 0, 1);
        case TREASURE_STATE_RESERVED02:
        case TREASURE_STATE_RESERVED03:
            peopleSetMotion(group, id, TREASURE_MOTION_CLOSED, 0, 1);
            break;
        }
        break;
    case TREASURE_COMMAND_PICKUP:
        if (treasure->clearFlag != 0 && GSflagTest(treasure->clearFlag)) return 0;
        switch ((treasure->flags >> TREASURE_FLAG_STATE_SHIFT) & TREASURE_FLAG_STATE_MASK) {
        case TREASURE_STATE_CHEST:
            peopleSetMotion(group, id, TREASURE_MOTION_OPENING, 0, 0);
            GSsndPlayNow(TREASURE_SOUND_OPEN);
            peopleWaitSyncMotion(group, id, 1);
            break;
        case TREASURE_STATE_RESERVED02:
        case TREASURE_STATE_RESERVED03:
            break;
        }
        floorEventCtrlTresure(group, id, TREASURE_COMMAND_DISP);
        GSflagOn(treasure->clearFlag);
        floorEventGetTresure((treasure->flags >> TREASURE_FLAG_KIND_SHIFT) & TREASURE_FLAG_KIND_MASK, treasure->value, treasure->count);
        break;
    }
    return 0;
}

s32 floorEventCtrlElevator(u32 floor, u32 index, u16 mode, u32 group, u32 id) {
    FloorDoorEntry* elevator;
    _GSmodel* model;
    GSpart* part;
    s16 anim = -1;
    f32 frame = 0.0f;
    GSvec pos;
    u32 mapGroup;
    if (index >= *floor_door_list_number) return -1;
    elevator = &floor_door_list[index];
    if (!elevator->modelResourceID) return -1;
    model = floorGetResource(floor, elevator->modelResourceID);
    if (!model) return -1;
    mapGroup = mapGetGroupID(floorDataBiosGetPtr(floor));
    switch (mode) {
    case ELEVATOR_MODE_MOVE:
    case ELEVATOR_MODE_MOVE_REVERSE:
    case ELEVATOR_MODE_OPEN:
    case ELEVATOR_MODE_CLOSE: {
        if (elevator->openMotion >= 0) _floorEventSetMotion(model, elevator->openMotion, frame);
        if (mode & ELEVATOR_MODE_SOUND) GSsndPlayNow(FLOOR_EVENT_SOUND_DOOR_OPEN);
        _floorEventWaitMotion(model);
        part = GSmodelGetPart(model, elevator->modelPart);
        GSpartGetTransform(part, &pos, 0, 0);
        GSpartFree(part);
        if (mode & ELEVATOR_MODE_MOVE) {
            peopleWalkTo(group, id, 1, &pos);
            peopleMoveCheck(group, id, 1);
            peopleRotateToAngle(group, id, 0.0f, 0.5f);
            peopleMoveCheck(group, id, 1);
        } else {
            peopleDetach(group, id, 0);
            peopleGetPos(group, id, &pos);
            pos.z += 25.0f;
            peopleWalkTo(group, id, 1, &pos);
            peopleMoveCheck(group, id, 1);
        }
        if (elevator->closeMotion >= 0) _floorEventSetMotion(model, elevator->closeMotion, frame);
        if (mode & ELEVATOR_MODE_SOUND) GSsndPlayNow(FLOOR_EVENT_SOUND_DOOR_OPEN);
        _floorEventWaitMotion(model);
        break;
    }
    case ELEVATOR_MODE_STOP:
        _floorEventWaitMotion(model);
        if (mode & ELEVATOR_MODE_SOUND) {
            GSsndStopNow(FLOOR_EVENT_SOUND_ELEVATOR_OPEN);
            GSsndPlayNow(FLOOR_EVENT_SOUND_ELEVATOR_CLOSE);
        }
        break;
    default:
        if (mode & ELEVATOR_MODE_ENTER) anim = elevator->enterMotion;
        else if (mode & ELEVATOR_MODE_EXIT) anim = elevator->exitMotion;
        if (anim < 0) return -1;
        if (!GSmodelCanAnimate(model)) return -1;
        if (mode & ELEVATOR_MODE_ATTACH) {
            peopleAttach(group, id, mapGroup, elevator->modelResourceID, elevator->modelPart);
            peopleSetPosXYZ(group, id, 0.0f, 0.0f, 0.0f);
        }
        if (mode & ELEVATOR_MODE_FINAL_FRAME) {
            GSmodelSetAnimIndex(model, anim);
            GSmodelGetFrameCount(model, &frame, 0);
            frame -= 1.0f;
        }
        _floorEventSetMotion(model, anim, frame);
        if (mode & ELEVATOR_MODE_WAIT) {
            floorEventCtrlElevator(floor, index, ELEVATOR_MODE_STOP, group, id);
        } else if (mode & ELEVATOR_MODE_SOUND) {
            GSsndPlayNow(FLOOR_EVENT_SOUND_ELEVATOR_OPEN);
        }
        break;
    }
    GSmodelGetFrameCount(model, &frame, 0);
    return 100.0f * ((2.0f * frame) / (f32)GSgfxVideoGetVsyncRate());
}

s32 floorEventCtrlDoor(u32 floor, u32 index, u8 mode) {
    u32 flags[4];
    f32 frame = 0.0f;
    s16 anim = -1;
    u32 sound = 0;
    if (index >= *floor_door_list_number) return -1;
    FloorDoorEntry* door = &floor_door_list[index];
    switch (mode) {
    case DOOR_MODE_CONDITIONAL_OPEN:
        if (door->openFlag && GSflagTest(door->openFlag)) return 0;
        if (door->closeFlag && !GSflagTest(door->closeFlag)) return 1;
    case DOOR_MODE_OPEN:
        if (door->openFlag) GSflagOn(door->openFlag);
        scriptSetCol(door->collisionIndex, 0);
        break;
    case DOOR_MODE_CONDITIONAL_CLOSE:
        if (door->openFlag && !GSflagTest(door->openFlag)) return 0;
    case DOOR_MODE_CLOSE:
        if (door->openFlag) GSflagOff(door->openFlag);
        scriptSetCol(door->collisionIndex, 1);
        break;
    }
    if (!door->modelResourceID) return -1;
    _GSmodel* model = floorGetResource(floor, door->modelResourceID);
    if (!model) return -1;
    switch (mode) {
    case DOOR_MODE_CONDITIONAL_OPEN:
        anim = door->openMotion;
        switch (door->soundKind) {
        case DOOR_SOUND_KIND_DEFAULT: sound = FLOOR_EVENT_SOUND_DOOR_OPEN; break;
        case DOOR_SOUND_KIND_RESERVED02: sound = FLOOR_EVENT_SOUND_DOOR_OPEN; break;
        case DOOR_SOUND_KIND_ALTERNATE: sound = FLOOR_EVENT_SOUND_DOOR_OPEN_ALT; break;
        case DOOR_SOUND_KIND_RESERVED04: sound = FLOOR_EVENT_SOUND_DOOR_1123; break;
        case DOOR_SOUND_KIND_RESERVED05: sound = FLOOR_EVENT_SOUND_DOOR_1325; break;
        case DOOR_SOUND_KIND_RESERVED06: sound = FLOOR_EVENT_SOUND_DOOR_1377; break;
        }
        break;
    case DOOR_MODE_OPEN:
        anim = door->middleMotion;
        if (anim < 0) {
            anim = door->openMotion;
            if (anim >= 0) {
                GSmodelSetAnimIndex(model, anim);
                GSmodelGetFrameCount(model, &frame, 0);
                frame -= 1.0f;
            }
        }
        break;
    case DOOR_MODE_CONDITIONAL_CLOSE:
        anim = door->closeMotion;
        switch (door->soundKind) {
        case DOOR_SOUND_KIND_DEFAULT: sound = FLOOR_EVENT_SOUND_DOOR_OPEN; break;
        case DOOR_SOUND_KIND_RESERVED02: sound = FLOOR_EVENT_SOUND_DOOR_OPEN; break;
        case DOOR_SOUND_KIND_ALTERNATE: sound = FLOOR_EVENT_SOUND_DOOR_OPEN_ALT; break;
        case DOOR_SOUND_KIND_RESERVED04: sound = FLOOR_EVENT_SOUND_DOOR_CLOSE; break;
        case DOOR_SOUND_KIND_RESERVED05: sound = FLOOR_EVENT_SOUND_DOOR_1325; break;
        case DOOR_SOUND_KIND_RESERVED06: sound = FLOOR_EVENT_SOUND_DOOR_1377; break;
        }
        break;
    case DOOR_MODE_CLOSE:
        anim = door->alternateMotion;
        if (anim < 0) {
            anim = door->closeMotion;
            if (anim >= 0) {
                GSmodelSetAnimIndex(model, anim);
                GSmodelGetFrameCount(model, &frame, 0);
                frame -= 1.0f;
            }
        }
        break;
    }
    if (anim < 0) return -1;
    if (!GSmodelCanAnimate(model)) return -1;
    _floorEventSetMotion(model, anim, frame);
    if (sound) GSsndPlayNow(sound);
    switch (mode) {
    case DOOR_MODE_CONDITIONAL_OPEN:
    case DOOR_MODE_CONDITIONAL_CLOSE: {
        flags[0] = peopleGetFlags(0, HERO_RESOURCE_HERO);
        flags[1] = peopleGetFlags(0, HERO_RESOURCE_MANA);
        flags[2] = peopleGetFlags(0, HERO_RESOURCE_FOLLOWER_SLOT_3);
        flags[3] = peopleGetFlags(0, HERO_RESOURCE_FOLLOWER_SLOT_2);
        heroMoveInitEvent();
        _floorEventWaitMotion(model);
        heroMoveTermEvent();
        peopleSetFlags(0, HERO_RESOURCE_HERO, flags[0]);
        peopleSetFlags(0, HERO_RESOURCE_MANA, flags[1]);
        peopleSetFlags(0, HERO_RESOURCE_FOLLOWER_SLOT_3, flags[2]);
        peopleSetFlags(0, HERO_RESOURCE_FOLLOWER_SLOT_2, flags[3]);
        break;
    }
    }
    return 0;
}

s32 floorEventExecScript(s8 kind, s32 scriptID) {
    FloorScriptEntry* script;
    u32 floor;
    u32 i;
    s32 found;
    found = 0;
    EvlogSet(kind, scriptID);
    floor = GSfloorGetCurrent();
    i = 0;
    for (; i < *floor_event_list_number; i++) {
        script = &floor_event_list[i];
        if (script->floorID != floor) continue;
        if (script->kind != kind) continue;
        if (script->scriptID != scriptID) continue;
        if (!script->entryPoint) continue;
        scriptSetEventColID(scriptID);
        floorExecScriptInteger(script->entryPoint, script->parameter1, script->parameter2, script->parameter3, script->parameter4);
        found++;
    }
    return found;
}

}

void _floorEventWaitMotion(_GSmodel* model) {
    if (!model) return;
    while (GSmodelIsAnimating(model)) GSthreadSwitch();
}

void _floorEventSetMotion(_GSmodel* model, s16 anim, f32 frame) {
    if (!model) return;
    GSmodelSetAnimIndex(model, anim);
    GSmodelSetAnimFrame(model, frame);
    GSmodelSetAnimRate(model, 0.5f);
    GSmodelSetAnimType(model, GS_MODEL_ANIM_ONCE);
    GSmodelStartAnimation(model);
}

void _floorEventWaitME() {
    while (_playMEParam.active) GSthreadSwitch();
    GSthreadSwitch();
}

s32 _floorEventPlayMEThread(GSthread* thread) {
    scriptSoundOpenPlayME(_playMEParam.soundID, 0, 255);
    _playMEParam.active = 0;
    return 0;
}
