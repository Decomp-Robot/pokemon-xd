#include <types.h>
#include <GSres.h>
#include <app/camera/camera.h>
#include <app/camera/CameraType.h>
#include <floor.h>
#include <floorFieldCamera.h>
#include <AppScript.h>

int AppScript::cmpCamera(int command, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum CameraMethod {
        CameraMethodSetFloorDefault = 16,
        CameraMethodSetType,
        CameraMethodSetTarget,
        CameraMethodSetTargetPosXYZ,
        CameraMethodSetTargetPosVector,
        CameraMethodSetTargetOffsetXYZ,
        CameraMethodSetTargetOffsetVector,
        CameraMethodSetPositionXYZ,
        CameraMethodSetPositionVector,
        CameraMethodSetRotationXYZ,
        CameraMethodSetRotationVector,
        CameraMethodPlayAnime,
        CameraMethodStopAnime,
        CameraMethodPauseAnime,
        CameraMethodResumeAnime,
        CameraMethodWaitSyncAnime,
        CameraMethodSetAnimeRate,
        CameraMethodMoveTargetPeople,
        CameraMethodMoveTargetXYZ,
        CameraMethodMoveTargetVector,
        CameraMethodMovePositionXYZ,
        CameraMethodMovePositionVector,
        CameraMethodMoveRotationXYZ,
        CameraMethodMoveRotationVector,
        CameraMethodMoveEndCheck,
        CameraMethodSetHeight,
        CameraMethodSetDistance,
        CameraMethodSetRotY,
        CameraMethodSetFov,
        CameraMethodUpdate,
        CameraMethodReturn,
        CameraMethodEnableFieldCamera,
        CameraMethodDisableFieldCamera,
        CameraMethodBlendFieldCamera,
        CameraMethodGetFov,
        CameraMethodGetPosition,
        CameraMethodGetRotation,
        CameraMethodMovePositionVectorAcceleration = 53,
        CameraMethodMovePositionXYZAcceleration,
        CameraMethodMoveRotationVectorAcceleration,
        CameraMethodMoveRotationXYZAcceleration,
        CameraMethodMoveTargetVectorAcceleration,
        CameraMethodMoveTargetXYZAcceleration,
        CameraMethodMoveFov,
        CameraMethodMoveFovAcceleration,
        CameraMethodSetTargetId = 61,
        CameraMethodSetTargetExt,
        CameraMethodSetPartTargetOffset
    };

    GSvec cameraVector;

    switch (command) {
    case CameraMethodSetFloorDefault:
        cameraSetFloorDefault(args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
        break;

    case CameraMethodSetType:
        if (args[1].getInteger() == CAMERA_FREE)
            floorFieldCameraDisable();
        cameraSetType(args[1].getInteger());
        break;

    case CameraMethodSetTarget: {
        GSresID* people = args[1].getPeople();
        cameraSetTarget(people->groupId, people->resourceId);
        break;
    }

    case CameraMethodSetTargetId:
        cameraSetTarget(floorGetCurrentGroupID(), args[1].getInteger());
        break;

    case CameraMethodSetTargetPosXYZ:
        cameraSetTargetPosXYZ(args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
        break;

    case CameraMethodSetTargetPosVector: {
        GSvec* target = args[1].getVector();
        cameraSetTargetPosXYZ(target->x, target->y, target->z);
        break;
    }

    case CameraMethodSetTargetOffsetXYZ:
        cameraSetTargetOfsXYZ(args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
        break;

    case CameraMethodSetTargetOffsetVector: {
        GSvec* offset = args[1].getVector();
        cameraSetTargetOfsXYZ(offset->x, offset->y, offset->z);
        break;
    }

    case CameraMethodSetPositionXYZ: {
        GScamera* camera = (GScamera*)GSresGetResource(0, 0);
        cameraVector.set(args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
        cameraSetPosition(&cameraVector);
        GScameraSetPosition(camera, &cameraVector);
        break;
    }

    case CameraMethodSetPositionVector: {
        GScamera* camera = (GScamera*)GSresGetResource(0, 0);
        cameraSetPosition(args[1].getVector());
        GScameraSetPosition(camera, args[1].getVector());
        break;
    }

    case CameraMethodSetRotationXYZ:
        cameraSetRotationXYZ(args[1].getSingle(), args[2].getSingle(), args[3].getSingle());
        break;

    case CameraMethodSetRotationVector: {
        GSvec* rotation = args[1].getVector();
        cameraSetRotationXYZ(rotation->x, rotation->y, rotation->z);
        break;
    }

    case CameraMethodPlayAnime:
        cameraPlayAnime(args[1].getInteger(), args[2].getInteger(), args[3].getSingle(),
                        args[4].getInteger());
        break;

    case CameraMethodStopAnime:
        cameraStopAnime();
        break;

    case CameraMethodPauseAnime:
        cameraPauseAnime();
        break;

    case CameraMethodResumeAnime:
        cameraResumeAnime();
        break;

    case CameraMethodWaitSyncAnime:
        result->setInteger(cameraWaitSyncAnime(args[1].getInteger() != 0));
        break;

    case CameraMethodSetAnimeRate:
        cameraSetAnimeRate(args[1].getSingle());
        break;

    case CameraMethodMoveTargetPeople: {
        GSresID* people = args[2].getPeople();
        cameraSetType(args[1].getInteger());
        cameraMoveTarget(args[1].getInteger(), people->groupId, people->resourceId,
                         args[3].getSingle());
        break;
    }

    case CameraMethodMoveTargetXYZ:
        cameraSetType(args[1].getInteger());
        cameraMoveTargetXYZ(args[1].getInteger(), args[2].getInteger(), args[3].getInteger(),
                            args[4].getInteger(), args[5].getInteger());
        break;

    case CameraMethodMoveTargetVector: {
        GSvec* target = args[2].getVector();
        cameraSetType(args[1].getInteger());
        cameraMoveTargetXYZ(args[1].getInteger(), target->x, target->y, target->z,
                            args[3].getSingle());
        break;
    }

    case CameraMethodMovePositionXYZ:
        cameraSetType(args[1].getInteger());
        cameraMovePositionXYZ(args[1].getInteger(), args[2].getSingle(), args[3].getSingle(),
                              args[4].getSingle(), args[5].getSingle());
        break;

    case CameraMethodMovePositionVector: {
        GSvec* position = args[2].getVector();
        cameraSetType(args[1].getInteger());
        cameraMovePositionXYZ(args[1].getInteger(), position->x, position->y, position->z,
                              args[3].getSingle());
        break;
    }

    case CameraMethodMoveRotationXYZ:
        cameraSetType(args[1].getInteger());
        cameraMoveRotationXYZ(args[1].getSingle(), args[2].getSingle(), args[3].getSingle(),
                              args[4].getSingle(), args[5].getSingle());
        break;

    case CameraMethodMoveRotationVector: {
        GSvec* rotation = args[2].getVector();
        cameraSetType(args[1].getInteger());
        cameraMoveRotationXYZ(args[1].getInteger(), rotation->x, rotation->y, rotation->z,
                              args[3].getSingle());
        break;
    }

    case CameraMethodMoveEndCheck:
        result->setInteger(cameraMoveEndCheck(args[1].getInteger() != 0));
        break;

    case CameraMethodSetHeight:
        cameraSetHeight(args[1].getSingle());
        break;

    case CameraMethodSetDistance:
        cameraSetDistance(args[1].getSingle());
        break;

    case CameraMethodSetRotY:
        cameraSetRotY(args[1].getSingle());
        break;

    case CameraMethodSetFov: {
        f32 fov;
        f32 aspect;
        f32 nearPlane;
        f32 farPlane;
        GScamera* camera = (GScamera*)GSresGetResource(0, 0);
        GScameraGetPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
        fov = args[1].getSingle();
        cameraSetFov(fov);
        GScameraSetPerspective(camera, fov, aspect, nearPlane, farPlane);
        break;
    }

    case CameraMethodUpdate:
        cameraUpdate();
        break;

    case CameraMethodReturn:
        cameraReturn(args[1].getSingle(), args[2].getInteger() != 0);
        break;

    case CameraMethodEnableFieldCamera:
        cameraSetType(CAMERA_NORMAL);
        floorFieldCameraEnable();
        break;

    case CameraMethodDisableFieldCamera:
        floorFieldCameraDisable();
        cameraSetType(CAMERA_FREE);
        break;

    case CameraMethodBlendFieldCamera:
        floorFieldCameraBlend(args[1].getSingle());
        break;

    case CameraMethodGetFov: {
        f32 fov = cameraGetFov();
        result->kind = GSscript::VariantSingle;
        result->single = fov;
        break;
    }

    case CameraMethodGetPosition: {
        cameraGetPosition(&cameraVector);
        setRpVec(&cameraVector);
        break;
    }

    case CameraMethodGetRotation: {
        cameraGetRotation(&cameraVector);
        setRpVec(&cameraVector);
        break;
    }

    case CameraMethodMoveTargetXYZAcceleration:
        cameraMoveTargetXYZ(args[1].getInteger(), args[2].getInteger(), args[3].getInteger(),
                            args[4].getInteger(), args[5].getInteger());
        cameraMoveTargetPosAcc(args[6].getSingle());
        break;

    case CameraMethodMoveTargetVectorAcceleration: {
        GSvec* target = args[2].getVector();
        cameraMoveTargetXYZ(args[1].getInteger(), target->x, target->y, target->z,
                            args[3].getSingle());
        cameraMoveTargetPosAcc(args[4].getSingle());
        break;
    }

    case CameraMethodMovePositionXYZAcceleration:
        cameraMovePositionXYZ(args[1].getInteger(), args[2].getSingle(), args[3].getSingle(),
                              args[4].getSingle(), args[5].getSingle());
        cameraMovePositionAcc(args[6].getSingle());
        break;

    case CameraMethodMovePositionVectorAcceleration: {
        GSvec* position = args[2].getVector();
        cameraMovePositionXYZ(args[1].getInteger(), position->x, position->y, position->z,
                              args[3].getSingle());
        cameraMovePositionAcc(args[4].getSingle());
        break;
    }

    case CameraMethodMoveRotationXYZAcceleration:
        cameraMoveRotationXYZ(args[1].getSingle(), args[2].getSingle(), args[3].getSingle(),
                              args[4].getSingle(), args[5].getSingle());
        cameraMoveRotationAcc(args[6].getSingle());
        break;

    case CameraMethodMoveRotationVectorAcceleration: {
        GSvec* rotation = args[2].getVector();
        cameraMoveRotationXYZ(args[1].getInteger(), rotation->x, rotation->y, rotation->z,
                              args[3].getSingle());
        cameraMoveRotationAcc(args[4].getSingle());
        break;
    }

    case CameraMethodMoveFov:
        cameraMoveFov(args[1].getInteger(), args[2].getSingle(), args[3].getSingle());
        break;

    case CameraMethodMoveFovAcceleration:
        cameraMoveFov(args[1].getInteger(), args[2].getSingle(), args[3].getSingle());
        cameraMoveFovAcc(args[4].getSingle());
        break;

    case CameraMethodSetTargetExt:
        cameraSetTargetExt(floorGetCurrentGroupID(), args[1].getInteger(), args[2].getInteger());
        break;

    case CameraMethodSetPartTargetOffset:
        cameraSetPartTargetOffset(args[1].getVector());
        break;
    }

    return 0;
}
