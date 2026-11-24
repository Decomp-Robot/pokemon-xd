#include <types.h>
#include <AppScript.h>
#include <GSscript.h>
#include <GSgfx.h>
#include <GSmodel.h>
#include <GSpart.h>
#include <GSres.h>
#include <dolphin/mtx.h>
#include <fightEncount.h>
#include <floor.h>
#include <hero.h>
#include <menuSub.h>
#include <people.h>
#include <peopleCollision.h>
#include <peopleSave.h>
#include <peopleTalk.h>
#include <peopleWalkList.h>
#include <script.h>
#include <scriptModel.h>
#include <winMsg.h>
#include <app/recovery/recoveryEvent.h>

enum PeopleMethodResult {
    PEOPLE_METHOD_CONTINUE = 0,
    PEOPLE_METHOD_STOP = 1
};

enum PeopleCommand {
    PEOPLE_COMMAND_INIT = 1,
    PEOPLE_COMMAND_SET_DISPLAY = 16,
    PEOPLE_COMMAND_SET_MOTION,
    PEOPLE_COMMAND_SET_MOTION_FORCE,
    PEOPLE_COMMAND_STOP_MOTION,
    PEOPLE_COMMAND_WAIT_SYNC_MOTION,
    PEOPLE_COMMAND_TALK_MESSAGE,
    PEOPLE_COMMAND_TALK_MESSAGE_END,
    PEOPLE_COMMAND_READ_CHARACTER,
    PEOPLE_COMMAND_CLOSE,
    PEOPLE_COMMAND_RECT_CHECK,
    PEOPLE_COMMAND_RECT_CHECK_VECTOR,
    PEOPLE_COMMAND_LINE_CHECK,
    PEOPLE_COMMAND_LINE_CHECK_VECTOR,
    PEOPLE_COMMAND_SET_POSITION,
    PEOPLE_COMMAND_SET_POSITION_VECTOR,
    PEOPLE_COMMAND_SET_ROTATION,
    PEOPLE_COMMAND_SET_ROTATION_VECTOR,
    PEOPLE_COMMAND_GET_POSITION,
    PEOPLE_COMMAND_GET_POSITION_VECTOR,
    PEOPLE_COMMAND_NO_OPERATION,
    PEOPLE_COMMAND_WALK_TO_XYZ,
    PEOPLE_COMMAND_WALK_TO_VECTOR,
    PEOPLE_COMMAND_WALK_TO_TARGET,
    PEOPLE_COMMAND_MOVE_CHECK,
    PEOPLE_COMMAND_SET_FLAG,
    PEOPLE_COMMAND_CLEAR_FLAG,
    PEOPLE_COMMAND_SET_NECK_TARGET_HERO,
    PEOPLE_COMMAND_SET_NECK_TARGET_PEOPLE,
    PEOPLE_COMMAND_SET_NECK_TARGET_POSITION,
    PEOPLE_COMMAND_REMOVE_NECK_TARGET,
    PEOPLE_COMMAND_CHECK_NECK_ROTATE,
    PEOPLE_COMMAND_ROTATE_TO,
    PEOPLE_COMMAND_ROTATE_TO_XYZ,
    PEOPLE_COMMAND_ROTATE_TO_TARGET,
    PEOPLE_COMMAND_SET_SPEED_RATE,
    PEOPLE_COMMAND_ATTACH,
    PEOPLE_COMMAND_DETACH,
    PEOPLE_COMMAND_TASK_SWITCH,
    PEOPLE_COMMAND_INIT_WALK_LIST,
    PEOPLE_COMMAND_ADD_WALK_LIST,
    PEOPLE_COMMAND_START_WALK_LIST,
    PEOPLE_COMMAND_FREE_WALK_LIST,
    PEOPLE_COMMAND_INIT_EVENT,
    PEOPLE_COMMAND_TERM_EVENT,
    PEOPLE_COMMAND_FALL_APPEAR,
    PEOPLE_COMMAND_DISP_MARK,
    PEOPLE_COMMAND_SET_COLLISION,
    PEOPLE_COMMAND_GAZE_HERO_CHECK,
    PEOPLE_COMMAND_SET_ANIM_BLEND,
    PEOPLE_COMMAND_WAIT_SYNC_MOTION_BLEND,
    PEOPLE_COMMAND_WALK_PAUSE,
    PEOPLE_COMMAND_SET_NECK_TARGET_VECTOR,
    PEOPLE_COMMAND_ROTATE_TO_VECTOR,
    PEOPLE_COMMAND_ADD_WALK_LIST_VECTOR,
    PEOPLE_COMMAND_READ_OBJECT,
    PEOPLE_COMMAND_RUN_OR_NOT,
    PEOPLE_COMMAND_GET_SPEED,
    PEOPLE_COMMAND_TALK,
    PEOPLE_COMMAND_ATTACH_WITH_MODE,
    PEOPLE_COMMAND_MODEL_GET_POSITION,
    PEOPLE_COMMAND_MODEL_GET_ROTATION,
    PEOPLE_COMMAND_SET_WAIT_ANIMATION,
    PEOPLE_COMMAND_UNSET_WAIT_ANIMATION,
    PEOPLE_COMMAND_ROTATE_TO_ANGLE,
    PEOPLE_COMMAND_MOVE_NECK_ANGLE,
    PEOPLE_COMMAND_MOVE_NECK_TARGET,
    PEOPLE_COMMAND_MOVE_NECK_TARGET_HERO,
    PEOPLE_COMMAND_MOVE_NECK_ANGLE_WORLD,
    PEOPLE_COMMAND_MOVE_NECK_TARGET_POSITION,
    PEOPLE_COMMAND_GET_Y_DEGREE_ROTATION,
    PEOPLE_COMMAND_NO_OPERATION_END,
    PEOPLE_COMMAND_AUTO_ANIMATION,
    PEOPLE_COMMAND_MANUAL_ANIMATION,
    PEOPLE_COMMAND_SET_MOTION_TABLE,
    PEOPLE_COMMAND_SET_DEFAULT_MOTION_TABLE,
    PEOPLE_COMMAND_SPEED_SLOW_DOWN_ON,
    PEOPLE_COMMAND_SPEED_SLOW_DOWN_OFF,
    PEOPLE_COMMAND_BLINK_ONCE,
    PEOPLE_COMMAND_BLINK_START,
    PEOPLE_COMMAND_BLINK_STOP,
    PEOPLE_COMMAND_SPEAK_START,
    PEOPLE_COMMAND_SPEAK_STOP,
    PEOPLE_COMMAND_SET_TALK_DISTANCE,
    PEOPLE_COMMAND_GET_TALK_DISTANCE,
    PEOPLE_COMMAND_START_WALK_LIST_STRICT,
    PEOPLE_COMMAND_RECOVERY_EVENT,
    PEOPLE_COMMAND_WALK_PAUSE_TALK_MODE,
    PEOPLE_COMMAND_SET_MODEL_ANIM_RATE,
    PEOPLE_COMMAND_SET_WALK_SPEED,
    PEOPLE_COMMAND_SET_MODEL_BOUND_CHECK,
    PEOPLE_COMMAND_SET_ENDED_MOTION,
    PEOPLE_COMMAND_MODEL_PARTICLE,
    PEOPLE_COMMAND_SET_MODEL_SCALE,
    PEOPLE_COMMAND_RECALCULATE_MODEL_BOUND,
    PEOPLE_COMMAND_INSIDE_CHECK,
    PEOPLE_COMMAND_MODEL_PART_TRANSFORM,
    PEOPLE_COMMAND_MODEL_COLOR_SWAP,
    PEOPLE_COMMAND_CLEAR_MOVE_TYPE,
    PEOPLE_COMMAND_ROTATE_STOP_CLEAR_MOVE_TYPE
};

enum PeopleTalkMode {
    PEOPLE_TALK_NONE = 0,
    PEOPLE_TALK_SIMPLE,
    PEOPLE_TALK_CHAT_MOTION,
    PEOPLE_TALK_CONDITIONAL_END,
    PEOPLE_TALK_NONE_ALT,
    PEOPLE_TALK_NONE_END,
    PEOPLE_TALK_ENCOUNT,
    PEOPLE_TALK_CUTSCENE_MOTIONS,
    PEOPLE_TALK_YES_NO,
    PEOPLE_TALK_FIGHT,
    PEOPLE_TALK_END_MOTIONS,
    PEOPLE_TALK_LOOK_AT_HERO,
    PEOPLE_TALK_LOOK_AT_HERO_FIRST,
    PEOPLE_TALK_FACE_BODY,
    PEOPLE_TALK_GET_ITEM,
    PEOPLE_TALK_POKEMON_CRY,
    PEOPLE_TALK_MESSAGE_WINDOW,
    PEOPLE_TALK_FACE_BODY_NO_ROTATE,
    PEOPLE_TALK_MANUAL_ANIMATION,
    PEOPLE_TALK_RESTORE_FACING,
    PEOPLE_TALK_NECK_TARGET,
    PEOPLE_TALK_TALK_ONLY
};

enum PeopleAnimation {
    PEOPLE_ANIM_WAIT = 3,
    PEOPLE_ANIM_CHAT_START = 11,
    PEOPLE_ANIM_CHAT_LOOP = 12,
    PEOPLE_ANIM_CHAT_END = 13
};

enum PeopleTalkFacing {
    PEOPLE_FACE_NECK = 0x10,
    PEOPLE_FACE_BODY = 0x20
};

extern "C" {
long floorReadCharacter(unsigned long group, unsigned long id, long character);
long scriptGetItem(long itemId, long count);
long scriptPeopleInsideCheck(GSvec* point, GSvec* points, float radius, GSvec* normal);
}

struct RareColEntry {
    GScolor colorSwap;
    GScolor modulation;
};

extern "C" RareColEntry _pokeRareCol[];

int AppScript::cmpPeople(int methodId, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    GSresID* people;
    GSmodel* model;
    unsigned long savedFlags;

    static GSvec Vector;

    if (methodId != PEOPLE_COMMAND_INIT) {
        if (args->kind != GSscript::VariantPeople) {
            result->setInteger(0);
            return PEOPLE_METHOD_STOP;
        }
        people = args->getPeople();
    } else {
        return PEOPLE_METHOD_CONTINUE;
    }

    switch (methodId) {
        case PEOPLE_COMMAND_SET_DISPLAY:
            peopleSetDisp(people->groupId, people->resourceId, (u8)(args[1].getInteger() != 0));
            break;

        case PEOPLE_COMMAND_SET_MOTION:
            result->setInteger(peopleSetMotion(people->groupId, people->resourceId, args[1].getInteger(), args[2].getInteger(), (u8)(args[3].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_SET_MOTION_FORCE:
            peopleSetMotionForce(people->groupId, people->resourceId, args[1].getInteger(), args[2].getInteger(), (u8)(args[3].getInteger() != 0));
            break;

        case PEOPLE_COMMAND_STOP_MOTION:
            peopleStopMotion(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_WAIT_SYNC_MOTION:
            peopleWaitSyncMotion(people->groupId, people->resourceId, (u8)(args[1].getInteger() != 0));
            break;

        case PEOPLE_COMMAND_TALK_MESSAGE:
            peopleSpeakStart(people->groupId, people->resourceId);
            peopleTalkMsg(people->groupId, people->resourceId, args[1].getInteger());
            if (args[2].getInteger() != 0)
                peopleTalkMsgEnd(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_TALK_MESSAGE_END:
            peopleTalkMsgEnd(people->groupId, people->resourceId);
            peopleSpeakStop(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_READ_CHARACTER:
            result->setInteger((u8)floorReadCharacter(people->groupId, people->resourceId, args[1].getInteger()));
            break;

        case PEOPLE_COMMAND_CLOSE:
            peopleClose(people->groupId, people->resourceId, 1);
            break;

        case PEOPLE_COMMAND_RECT_CHECK:
            result->setInteger(peopleRectCheck(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle(), args[4].getSingle()));
            break;

        case PEOPLE_COMMAND_RECT_CHECK_VECTOR: {
            GSvec* pos1 = args[1].getVector();
            GSvec* pos2 = args[2].getVector();
            result->setInteger(peopleRectCheck(people->groupId, people->resourceId, pos1->x, pos1->z, pos2->x, pos2->z));
            break;
        }

        case PEOPLE_COMMAND_LINE_CHECK:
            result->setInteger(peopleLineCheck(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle(), args[4].getSingle(), args[5].getSingle(), (args[6].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_LINE_CHECK_VECTOR: {
            GSvec* pos1 = args[1].getVector();
            GSvec* pos2 = args[2].getVector();
            result->setInteger(peopleLineCheck(people->groupId, people->resourceId, pos1->x, pos1->z, pos2->x, pos2->z, 0.0f, (args[3].getInteger() != 0)));
            break;
        }

        case PEOPLE_COMMAND_SET_POSITION:
            peopleSetPosXYZ(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
            break;

        case PEOPLE_COMMAND_SET_POSITION_VECTOR: {
            GSvec* pos = args[1].getVector();
            peopleSetPosXYZ(people->groupId, people->resourceId, pos->x, pos->y, pos->z);
            break;
        }

        case PEOPLE_COMMAND_SET_ROTATION:
            peopleSetRotXYZ(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
            break;

        case PEOPLE_COMMAND_SET_ROTATION_VECTOR:
            peopleSetRot(people->groupId, people->resourceId, args[1].getVector());
            break;

        case PEOPLE_COMMAND_GET_POSITION:
            Vector.x = args[1].getSingle();
            Vector.y = args[2].getSingle();
            Vector.z = args[3].getSingle();
            peopleGetPos(people->groupId, people->resourceId, &Vector);
            break;

        case PEOPLE_COMMAND_GET_POSITION_VECTOR:
            peopleGetPos(people->groupId, people->resourceId, args[1].getVector());
            break;

        case PEOPLE_COMMAND_WALK_TO_XYZ:
            peopleWalkToXYZ(people->groupId, people->resourceId, (args[1].getInteger() != 0), args[2].getSingle(), args[3].getSingle(), args[4].getSingle());
            break;

        case PEOPLE_COMMAND_WALK_TO_VECTOR: {
            GSvec* target = args[2].getVector();
            peopleWalkToXYZ(people->groupId, people->resourceId, (args[1].getInteger() != 0), target->x, target->y, target->z);
            break;
        }

        case PEOPLE_COMMAND_WALK_TO_TARGET: {
            GSresID* target = args[1].getPeople();
            peopleWalkToTarget(people->groupId, people->resourceId, target->groupId, target->resourceId, args[2].getSingle(), (args[3].getInteger() != 0));
            break;
        }

        case PEOPLE_COMMAND_MOVE_CHECK:
            result->setInteger(peopleMoveCheck(people->groupId, people->resourceId, (args[1].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_SET_FLAG:
            peopleSetFlagOn(people->groupId, people->resourceId, args[1].getInteger());
            break;

        case PEOPLE_COMMAND_CLEAR_FLAG:
            peopleSetFlagOff(people->groupId, people->resourceId, args[1].getInteger());
            break;

        case PEOPLE_COMMAND_SET_NECK_TARGET_HERO:
            peopleSetNeckTargetHero(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SET_NECK_TARGET_PEOPLE: {
            GSresID* target = args[1].getPeople();
            peopleSetNeckTarget(people->groupId, people->resourceId, target->groupId, target->resourceId);
            break;
        }

        case PEOPLE_COMMAND_SET_NECK_TARGET_POSITION:
            peopleSetNeckTargetXYZ(people->groupId, people->resourceId, args[1].getInteger(), args[2].getInteger(), args[3].getInteger());
            break;

        case PEOPLE_COMMAND_SET_NECK_TARGET_VECTOR: {
            GSvec* pos = args[1].getVector();
            peopleSetNeckTargetXYZ(people->groupId, people->resourceId, (long)pos->x, (long)pos->y, (long)pos->z);
            break;
        }

        case PEOPLE_COMMAND_REMOVE_NECK_TARGET:
            peopleRemoveNeckTarget(people->groupId, people->resourceId, args[1].getSingle());
            break;

        case PEOPLE_COMMAND_CHECK_NECK_ROTATE:
            result->setInteger(peopleCheckNeckRotate(people->groupId, people->resourceId, (args[1].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_MOVE_NECK_ANGLE:
            peopleMoveNeckAngle(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
            break;

        case PEOPLE_COMMAND_MOVE_NECK_ANGLE_WORLD:
            peopleMoveNeckAngleWorld(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
            break;

        case PEOPLE_COMMAND_ROTATE_TO:
            peopleRotateTo(people->groupId, people->resourceId, args[1].getSingle());
            break;

        case PEOPLE_COMMAND_ROTATE_TO_XYZ:
            peopleRotateToXYZ(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle(), args[4].getSingle());
            break;

        case PEOPLE_COMMAND_ROTATE_TO_VECTOR: {
            GSvec* rot = args[1].getVector();
            peopleRotateToXYZ(people->groupId, people->resourceId, rot->x, rot->y, rot->z, args[2].getSingle());
            break;
        }

        case PEOPLE_COMMAND_ROTATE_TO_TARGET: {
            GSresID* target = args[1].getPeople();
            peopleRotateToTarget(people->groupId, people->resourceId, target->groupId, target->resourceId, args[2].getSingle());
            break;
        }

        case PEOPLE_COMMAND_ROTATE_TO_ANGLE:
            peopleRotateToAngle(people->groupId, people->resourceId, MTXDegToRad(args[1].getSingle()), args[2].getSingle());
            break;

        case PEOPLE_COMMAND_SET_SPEED_RATE:
            peopleSetSpeedRate(people->groupId, people->resourceId, args[1].getSingle());
            break;

        case PEOPLE_COMMAND_ATTACH: {
            GSresID* partner = args[1].getPeople();
            peopleAttach(people->groupId, people->resourceId, partner->groupId, partner->resourceId, args[2].getInteger());
            break;
        }

        case PEOPLE_COMMAND_DETACH:
            peopleDetach(people->groupId, people->resourceId, (u8)(args[1].getInteger() != 0));
            break;

        case PEOPLE_COMMAND_TASK_SWITCH:
            peopleTaskSwich(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_INIT_WALK_LIST:
            result->setInteger(peopleInitWalkList(people->groupId, people->resourceId, args[1].getInteger()));
            break;

        case PEOPLE_COMMAND_ADD_WALK_LIST:
            result->setInteger(peopleAddWalkList(people->groupId, people->resourceId, args[1].getSingle(), args[2].getSingle(), args[3].getSingle()));
            break;

        case PEOPLE_COMMAND_ADD_WALK_LIST_VECTOR: {
            GSvec* target = args[1].getVector();
            result->setInteger(peopleAddWalkList(people->groupId, people->resourceId, target->x, target->y, target->z));
            break;
        }

        case PEOPLE_COMMAND_START_WALK_LIST:
            result->setInteger(peopleStartWalkList(people->groupId, people->resourceId, (args[1].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_FREE_WALK_LIST:
            result->setInteger(peopleFreeWalkList(people->groupId, people->resourceId));
            break;

        case PEOPLE_COMMAND_INIT_EVENT:
            peopleInitEvent(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_TERM_EVENT:
            peopleTermEvent(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_FALL_APPEAR:
            peopleFallAppear(people->groupId, people->resourceId, args[1].getSingle());
            break;

        case PEOPLE_COMMAND_DISP_MARK:
            peopleDispMark(people->groupId, people->resourceId, args[1].getInteger());
            break;

        case PEOPLE_COMMAND_SET_COLLISION:
            peopleSetCollision(people->groupId, people->resourceId, (args[1].getInteger() != 0));
            break;

        case PEOPLE_COMMAND_GAZE_HERO_CHECK:
            result->setInteger(peopleGazeHeroCheck(people->groupId, people->resourceId));
            break;

        case PEOPLE_COMMAND_SET_ANIM_BLEND:
            result->setInteger(peopleSetAnimBlend(people->groupId, people->resourceId, args[1].getInteger(), args[2].getInteger(), args[3].getInteger()));
            break;

        case PEOPLE_COMMAND_WAIT_SYNC_MOTION_BLEND:
            result->setInteger(peopleWaitSyncMotionBlend(people->groupId, people->resourceId, (u8)(args[1].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_WALK_PAUSE:
            result->setInteger(peopleWalkPause(people->groupId, people->resourceId, (args[1].getInteger() != 0)));
            break;

        case PEOPLE_COMMAND_RECOVERY_EVENT: {
            long itemIds[6];
            for (u16 i = 0; i < 6; i++)
                itemIds[i] = args[2].getArray()->get(i)->getInteger();
            long partyIds[7];
            for (u16 i = 0; i < 7; i++)
                partyIds[i] = args[3].getArray()->get(i)->getInteger();
            recoveryEventPC(people->groupId, people->resourceId, args[1].getInteger(), itemIds, partyIds);
            break;
        }

        case PEOPLE_COMMAND_READ_OBJECT:
            if (peopleGetHumanID(people->groupId, people->resourceId) == 0)
                floorReadCharacter(people->groupId, people->resourceId, args[1].getInteger());
            else
                peopleReadObject(people->groupId, people->resourceId, args[1].getInteger());
            break;

        case PEOPLE_COMMAND_RUN_OR_NOT:
            result->setInteger((u8)peopleRunOrNot(people->groupId, people->resourceId));
            break;

        case PEOPLE_COMMAND_GET_SPEED:
            result->setSingle(peopleGetSpeed(people->groupId, people->resourceId));
            break;

        case PEOPLE_COMMAND_TALK:
            switch (args[1].getInteger()) {
                case PEOPLE_TALK_SIMPLE:
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    break;

                case PEOPLE_TALK_CHAT_MOTION:
                    peopleSetMotion(people->groupId, people->resourceId, PEOPLE_ANIM_CHAT_START, 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleSetMotion(people->groupId, people->resourceId, PEOPLE_ANIM_CHAT_LOOP, 0, 1);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleSetMotion(people->groupId, people->resourceId, PEOPLE_ANIM_CHAT_END, 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleSetMotion(people->groupId, people->resourceId, PEOPLE_ANIM_WAIT, 0, 1);
                    break;

                case PEOPLE_TALK_CONDITIONAL_END:
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    if (args[3].getInteger() == 1) {
                        peopleTalkMsgEnd(people->groupId, people->resourceId);
                        peopleSpeakStop(people->groupId, people->resourceId);
                    }
                    break;

                case PEOPLE_TALK_ENCOUNT:
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    scriptEncount(args[3].getInteger(), 1, 0);
                    break;

                case PEOPLE_TALK_CUTSCENE_MOTIONS:
                    peopleSetMotion(people->groupId, people->resourceId, args[2].getInteger(), 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleSetMotion(people->groupId, people->resourceId, args[3].getInteger(), 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[4].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    break;

                case PEOPLE_TALK_YES_NO: {
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMessageBgmStart();
                    long answer = menuSubOpenYesNoField();
                    winMsgCloseField(1);
                    peopleTalkMessageBgmStop();
                    peopleSpeakStop(people->groupId, people->resourceId);
                    if (answer == 0)
                        result->setInteger(0);
                    else
                        result->setInteger(1);
                    break;
                }

                case PEOPLE_TALK_FIGHT:
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    fightEncount(args[3].getInteger());
                    break;

                case PEOPLE_TALK_END_MOTIONS:
                    peopleSetMotion(people->groupId, people->resourceId, args[2].getInteger(), 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleSetMotion(people->groupId, people->resourceId, args[3].getInteger(), 0, 1);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[4].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleSetMotion(people->groupId, people->resourceId, args[5].getInteger(), 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleSetMotion(people->groupId, people->resourceId, PEOPLE_ANIM_WAIT, 0, 1);
                    break;

                case PEOPLE_TALK_LOOK_AT_HERO:
                    peopleDispMark(people->groupId, people->resourceId, 1);
                    scriptWaitTime(1.0f);
                    peopleRotateToTarget(people->groupId, people->resourceId, PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_HERO, 1.0f);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    break;

                case PEOPLE_TALK_LOOK_AT_HERO_FIRST:
                    peopleRotateToTarget(people->groupId, people->resourceId, PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_HERO, 1.0f);
                    peopleDispMark(people->groupId, people->resourceId, 1);
                    scriptWaitTime(1.0f);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    break;

                case PEOPLE_TALK_FACE_BODY: {
                    savedFlags = peopleGetFlags(people->groupId, people->resourceId);
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleSetFlagOn(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    peopleRotateToTarget(people->groupId, people->resourceId, PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_HERO, 1.0f);
                    peopleSetFlags(people->groupId, people->resourceId, savedFlags);
                    break;
                }

                case PEOPLE_TALK_GET_ITEM:
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    scriptGetItem(args[3].getInteger(), args[4].getInteger());
                    break;

                case PEOPLE_TALK_POKEMON_CRY:
                    scriptPlayPokemonNakigoe(args[2].getInteger());
                    peopleTalkMsg(people->groupId, people->resourceId, args[3].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    break;

                case PEOPLE_TALK_MESSAGE_WINDOW:
                    winMsgOpenField(args[2].getInteger(), 1, 0);
                    winMsgCloseField(1);
                    break;

                case PEOPLE_TALK_FACE_BODY_NO_ROTATE:
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleSetFlagOn(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    break;

                case PEOPLE_TALK_MANUAL_ANIMATION:
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleManualAnimation(people->groupId, people->resourceId);
                    scriptSetEndedMotion(people->groupId, people->resourceId, args[3].getInteger(), 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleReadObject(people->groupId, people->resourceId, args[4].getInteger());
                    peopleManualAnimation(people->groupId, people->resourceId);
                    peopleSetMotionForce(people->groupId, people->resourceId, args[5].getInteger(), 0, 0);
                    peopleWaitSyncMotion(people->groupId, people->resourceId, 1);
                    peopleAutoAnimation(people->groupId, people->resourceId);
                    break;

                case PEOPLE_TALK_RESTORE_FACING: {
                    savedFlags = peopleGetFlags(people->groupId, people->resourceId);
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_NECK);
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleSetFlags(people->groupId, people->resourceId, savedFlags);
                    break;
                }

                case PEOPLE_TALK_NECK_TARGET: {
                    savedFlags = peopleGetFlags(people->groupId, people->resourceId);
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_NECK);
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    GSresID* talker = args[3].getPeople();
                    peopleSetNeckTarget(people->groupId, people->resourceId, talker->groupId, talker->resourceId);
                    peopleCheckNeckRotate(people->groupId, people->resourceId, 1);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    if (args[4].getInteger() == 1) {
                        peopleRemoveNeckTarget(people->groupId, people->resourceId, args[5].getSingle());
                        peopleCheckNeckRotate(people->groupId, people->resourceId, 1);
                    }
                    peopleSetFlags(people->groupId, people->resourceId, savedFlags);
                    break;
                }

                case PEOPLE_TALK_TALK_ONLY: {
                    savedFlags = peopleGetFlags(people->groupId, people->resourceId);
                    peopleSetFlagOff(people->groupId, people->resourceId, PEOPLE_FACE_BODY);
                    peopleSpeakStart(people->groupId, people->resourceId);
                    peopleTalkMsg(people->groupId, people->resourceId, args[2].getInteger());
                    peopleTalkMsgEnd(people->groupId, people->resourceId);
                    peopleSpeakStop(people->groupId, people->resourceId);
                    peopleSetFlags(people->groupId, people->resourceId, savedFlags);
                    break;
                }
            }
            break;

        case PEOPLE_COMMAND_ATTACH_WITH_MODE:
            peopleAttachWithMode(people->groupId, people->resourceId, floorGetCurrentGroupID(), args[1].getInteger(), args[2].getInteger(), args[3].getInteger());
            break;

        case PEOPLE_COMMAND_MODEL_GET_POSITION: {
            model = (GSmodel*)GSresGetResource(people->groupId, people->resourceId);
            if (model != 0)
                GSmodelGetPosition(model, &Vector);
            setRpVec(&Vector);
            break;
        }

        case PEOPLE_COMMAND_MODEL_GET_ROTATION: {
            model = (GSmodel*)GSresGetResource(people->groupId, people->resourceId);
            if (model != 0)
                GSmodelGetRotation(model, &Vector);
            const float degrees = MTXRadToDeg(Vector.y);
            Vector.y = degrees;
            result->setSingle(degrees);
            break;
        }

        case PEOPLE_COMMAND_SET_WAIT_ANIMATION:
            peopleSetWaitAnimation(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_UNSET_WAIT_ANIMATION:
            peopleUnsetWaitAnimation(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_MOVE_NECK_TARGET: {
            GSresID* target = args[1].getPeople();
            peopleMoveNeckTarget(people->groupId, people->resourceId, target->groupId, target->resourceId, args[2].getSingle());
            break;
        }

        case PEOPLE_COMMAND_MOVE_NECK_TARGET_HERO:
            peopleMoveNeckTargetHero(people->groupId, people->resourceId, args[1].getSingle());
            break;

        case PEOPLE_COMMAND_MOVE_NECK_TARGET_POSITION:
            peopleMoveNeckTargetXYZ(people->groupId, people->resourceId, args[1].getInteger(), args[2].getInteger(), args[3].getInteger(), args[4].getSingle());
            break;

        case PEOPLE_COMMAND_GET_Y_DEGREE_ROTATION:
            peopleGetYDegreeRotation(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_AUTO_ANIMATION:
            peopleAutoAnimation(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_MANUAL_ANIMATION:
            peopleManualAnimation(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SET_MOTION_TABLE: {
            MotionTable motionTable;
            motionTable.motions[0] = args[1].getArray()->get(0)->getInteger();
            motionTable.motions[1] = args[1].getArray()->get(1)->getInteger();
            motionTable.motions[2] = args[1].getArray()->get(2)->getInteger();
            motionTable.motions[3] = args[1].getArray()->get(3)->getInteger();
            motionTable.motions[4] = args[1].getArray()->get(4)->getInteger();
            peopleSetMotionTable(people->groupId, people->resourceId, motionTable);
            break;
        }

        case PEOPLE_COMMAND_SET_DEFAULT_MOTION_TABLE:
            peopleSetDefaultMotionTable(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SPEED_SLOW_DOWN_ON:
            peopleSpeedSlowDownOn(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SPEED_SLOW_DOWN_OFF:
            peopleSpeedSlowDownOff(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_BLINK_ONCE:
            peopleBlinkOnce(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_BLINK_START:
            peopleBlinkStart(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_BLINK_STOP:
            peopleBlinkStop(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SPEAK_START:
            peopleSpeakStart(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SPEAK_STOP:
            peopleSpeakStop(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_SET_TALK_DISTANCE:
            peopleSetTalkDistance(people->groupId, people->resourceId, args[1].getSingle());
            break;

        case PEOPLE_COMMAND_GET_TALK_DISTANCE:
            peopleGetTalkDistance(people->groupId, people->resourceId);
            break;

        case PEOPLE_COMMAND_START_WALK_LIST_STRICT:
            result->setInteger(peopleStartWalkListStrict(people->groupId, people->resourceId, args[1].getInteger()));
            break;

        case PEOPLE_COMMAND_WALK_PAUSE_TALK_MODE:
            peopleWalkPauseTalkMode(people->groupId, people->resourceId, (args[1].getInteger() != 0));
            break;

        case PEOPLE_COMMAND_SET_MODEL_ANIM_RATE: {
            model = (GSmodel*)GSresGetResource(people->groupId, people->resourceId);
            GSmodelSetAnimRate(model, args[1].getSingle());
            break;
        }

        case PEOPLE_COMMAND_SET_WALK_SPEED:
            peopleSetWalkSpeed(people->groupId, people->resourceId, args[1].getSingle(), args[2].getInteger());
            break;

        case PEOPLE_COMMAND_SET_MODEL_BOUND_CHECK: {
            model = (GSmodel*)GSresGetResource(people->groupId, people->resourceId);
            GSmodelSetBoundCheck(model, args[1].getInteger() == 0);
            break;
        }

        case PEOPLE_COMMAND_SET_ENDED_MOTION:
            scriptSetEndedMotion(people->groupId, people->resourceId, args[1].getInteger(), args[2].getInteger() != 0);
            break;

        case PEOPLE_COMMAND_MODEL_PARTICLE:
            scriptModelPartPaticle(people->resourceId, args[1].getInteger(), args[2].getInteger());
            break;

        case PEOPLE_COMMAND_SET_MODEL_SCALE: {
            model = (GSmodel*)GSresGetResource(people->groupId, people->resourceId);
            GSmodelSetScale(model, args[1].getVector());
            break;
        }

        case PEOPLE_COMMAND_RECALCULATE_MODEL_BOUND:
            GSmodelRecalculateBound((GSmodel*)GSresGetResource(people->groupId, people->resourceId));
            break;

        case PEOPLE_COMMAND_INSIDE_CHECK:
            peopleGetPos(people->groupId, people->resourceId, &Vector);
            result->setInteger((u8)scriptPeopleInsideCheck(&Vector, args[1].getVector(), args[2].getSingle(), args[3].getVector()));
            break;

        case PEOPLE_COMMAND_MODEL_PART_TRANSFORM: {
            long partId = args[1].getInteger();
            GSpart* part = GSmodelGetPart((GSmodel*)GSresGetResource(floorGetCurrentGroupID(), people->resourceId), partId);
            GSpartGetTransform(part, &Vector, 0, 0);
            GSpartFree(part);
            setRpVec(&Vector);
            break;
        }

        case PEOPLE_COMMAND_MODEL_COLOR_SWAP: {
            model = (GSmodel*)GSresGetResource(people->groupId, people->resourceId);
            if (model == 0)
                break;
            RareColEntry* col = &_pokeRareCol[args[1].getInteger()];
            GSmodelEnableColorSwap(model, &col->colorSwap);
            GSmodelEnableModulation(model, &col->modulation);
            break;
        }

        case PEOPLE_COMMAND_CLEAR_MOVE_TYPE: {
            tagPeopleWork* work = peopleGet(people->groupId, people->resourceId);
            work->state.moveType = PEOPLE_MOVE_NONE;
            break;
        }

        case PEOPLE_COMMAND_ROTATE_STOP_CLEAR_MOVE_TYPE: {
            tagPeopleWork* work = peopleGet(people->groupId, people->resourceId);
            peopleRotateStop(people->groupId, people->resourceId);
            work->state.moveType = PEOPLE_MOVE_NONE;
            break;
        }
    }

    return PEOPLE_METHOD_CONTINUE;
}
