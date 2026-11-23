#include <wazaSequenceCamera.h>
#include <wazaSequence.h>
#include <sequence.h>
#include <GSbound.h>
#include <GSAPI/GSvec/GSvec.h>
#include <app/camera/camera.h>
#include <app/camera/CameraType.h>
#include <GSgfxVideo.h>
#include <hsdRandom.h>
#include <math.h>

wazaSequenceCameraFov gFOVlist[2];
f32 gCurrentFOV = 30.0f;
int gLastMotionType = WAZA_POSITION_MODEL_SIDE;
f32 gCurrentFOVTime;

static inline void selectFOVRatio(u8 mask, f32* minimum, f32* maximum)
{
    bool narrow = false;
    bool middle = false;
    if ((mask & WAZA_CAMERA_FOV_NARROW) == WAZA_CAMERA_FOV_NARROW) {
        *minimum = 0.0f;
        *maximum = 0.2f;
        narrow = true;
    }
    if ((mask & WAZA_CAMERA_FOV_MIDDLE) == WAZA_CAMERA_FOV_MIDDLE) {
        if (!narrow)
            *minimum = 0.35f;
        *maximum = 0.6f;
        middle = true;
    }
    if ((mask & WAZA_CAMERA_FOV_WIDE) == WAZA_CAMERA_FOV_WIDE) {
        if (!middle)
            *minimum = 0.75f;
        *maximum = 1.0f;
    }
}

void _wazaSequenceCameraDoPosition(ModelSequence* model, wazaSequenceCameraParams* params, f32 durationScale, bool reverse)
{
    TemplateExpFileHeader* tmpl = &model->templates[model->motionType];
    u8 lockTargetHeight;
    if (tmpl->flags & TEMPLATE_EXP_LOCK_TARGET_HEIGHT) {
        lockTargetHeight = 1;
    } else {
        lockTargetHeight = 0;
    }
    cameraLockTargetHeight(lockTargetHeight);
    switch (params->positionType) {
    case WAZA_POSITION_DOLLY:
        _wazaSequenceCameraDoDollyPosition(tmpl, params, durationScale, reverse);
        break;
    case WAZA_POSITION_SWEEP: {
        f32 distance, height, travel, duration, squaredDistance;
        duration = tmpl->times[tmpl->phaseCount - 1];
        duration *= durationScale;
        travel = 25.0f + 10.0f * HSD_Randf();
        height = 1.0f + 9.0f * HSD_Randf();
        distance = 20.0f + 30.0f * HSD_Randf();
        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(0.0f);
        cameraUpdate();
        GSvec position;
        cameraGetPosition(&position);
        if (reverse) position.x += 10.0f;
        else position.x -= 10.0f;
        cameraSetPosition(&position);
        if (reverse) position.x += travel;
        else position.x -= travel;
        cameraMovePosition(CAMERA_OFFSET, &position, duration);
        cameraMovePositionAcc(duration * (0.1f / GSgfxVideoGetVsyncRate()));
        squaredDistance = travel * travel + height * height;
        params->distances[0] = sqrtf(squaredDistance);
        params->distances[2] = sqrtf(distance * distance + squaredDistance);
        params->distances[1] = 0.5f * (params->distances[0] + params->distances[2]);
        break;
    }
    case WAZA_POSITION_ORBIT: {
        f32 duration, distance, height, endYaw, startYaw, random;
        duration = tmpl->times[tmpl->phaseCount - 1];
        duration *= durationScale;
        random = HSD_Randf();
        f32 distanceRange = params->maxDistance - params->minDistance;
        distance = distanceRange * random + params->minDistance;
        random = HSD_Randf();
        f32 heightRange = params->maxHeight - params->minHeight;
        height = heightRange * random + params->minHeight;
        if (reverse) {
            random = HSD_Randf();
            f32 startYawRange = params->maxYawOffset - params->minYawOffset;
            startYaw = (params->yaw - params->minYawOffset) - startYawRange * random;
            random = HSD_Randf();
            f32 endYawRange = params->maxYawOffset - params->minYawOffset;
            endYaw = (params->yaw - params->minYawOffset) - endYawRange * random;
        } else {
            random = HSD_Randf();
            f32 startYawRange = params->maxYawOffset - params->minYawOffset;
            startYaw = startYawRange * random + (params->yaw + params->minYawOffset);
            random = HSD_Randf();
            f32 endYawRange = params->maxYawOffset - params->minYawOffset;
            endYaw = endYawRange * random + (params->yaw + params->minYawOffset);
        }
        if (startYaw > endYaw) {
            f32 savedYaw = startYaw;
            startYaw = endYaw;
            endYaw = savedYaw;
        }
        cameraSetDistance(1.25f * distance);
        cameraSetHeight(height);
        cameraSetRotY(startYaw);
        cameraUpdate();
        cameraMoveRotationXYZ(CAMERA_OFFSET, 0.0f, endYaw, 0.0f, duration);
        cameraMoveRotationAcc(duration * (0.1f / GSgfxVideoGetVsyncRate()));
        params->distances[0] = params->distances[1] = params->distances[2] = sqrtf(distance * distance + height * height);
        break;
    }
    case WAZA_POSITION_HOLD: {
        f32 yaw, duration, distance, height, random;
        duration = tmpl->times[tmpl->phaseCount - 1];
        duration *= durationScale;
        random = HSD_Randf();
        f32 distanceRange = params->maxDistance - params->minDistance;
        distance = distanceRange * random + params->minDistance;
        random = HSD_Randf();
        f32 heightRange = params->maxHeight - params->minHeight;
        height = heightRange * random + params->minHeight;
        if (reverse) {
            random = HSD_Randf();
            f32 yawRange = params->maxYawOffset - params->minYawOffset;
            yaw = (params->yaw - params->minYawOffset) - yawRange * random;
        } else {
            random = HSD_Randf();
            f32 yawRange = params->maxYawOffset - params->minYawOffset;
            yaw = yawRange * random + (params->yaw + params->minYawOffset);
        }
        cameraSetDistance(1.25f * distance);
        cameraSetHeight(height);
        cameraSetRotY(yaw);
        cameraUpdate();
        GSvec position;
        cameraGetPosition(&position);
        cameraMovePosition(CAMERA_OFFSET, &position, duration);
        params->distances[0] = params->distances[1] = params->distances[2] = sqrtf(distance * distance + height * height);
        break;
    }
    case WAZA_POSITION_OVERVIEW: {
        f32 yaw, duration;
        duration = tmpl->times[tmpl->phaseCount - 1];
        if (reverse) yaw = params->yaw - 3.1415927f * 0.15f;
        else yaw = 3.1415927f * 0.15f + params->yaw;
        cameraSetDistance(110.0f);
        cameraSetHeight(25.0f);
        cameraSetRotY(yaw);
        cameraUpdate();
        GSvec position;
        cameraGetPosition(&position);
        cameraMovePosition(CAMERA_OFFSET, &position, duration);
        params->distances[0] = params->distances[1] = params->distances[2] = sqrtf(110.0f * 110.0f + 25.0f * 25.0f);
        break;
    }
    case WAZA_POSITION_MODEL_SIDE: {
        f32 yaw, distance, duration, height, random, size;
        duration = tmpl->times[tmpl->phaseCount - 1];
        duration *= durationScale;
        switch (model->modelSize) {
        case MODEL_SIZE_CLASS_SMALLEST: case MODEL_SIZE_CLASS_SMALL: size = 0.875f; break;
        case MODEL_SIZE_CLASS_LARGE: size = 1.4f; break;
        case MODEL_SIZE_CLASS_LARGER: size = 1.8f; break;
        case MODEL_SIZE_CLASS_LARGEST: size = 3.0f; break;
        default: size = 1.0f; break;
        }
        distance = 50.0f * size;
        random = HSD_Randf();
        f32 heightRange = params->maxHeight - params->minHeight;
        height = heightRange * random + params->minHeight;
        if (reverse) yaw = params->yaw - 0.017453292f * 45.0f;
        else yaw = 0.017453292f * 45.0f + params->yaw;
        cameraSetDistance(1.25f * distance);
        cameraSetHeight(height);
        cameraSetRotY(yaw);
        cameraUpdate();
        GSvec position;
        cameraGetPosition(&position);
        cameraMovePosition(CAMERA_OFFSET, &position, duration);
        params->distances[0] = params->distances[1] = params->distances[2] = sqrtf(distance * distance + height * height);
        break;
    }
    }
}

void _wazaSequenceCameraDoDollyPosition(TemplateExpFileHeader* tmpl, wazaSequenceCameraParams* params, f32 durationScale, bool reverse)
{
    f32 distance, otherDistance, yaw, random, fullChance, startTime, endTime, startDistance, endDistance, duration, partialChance, height, chance;
    duration = tmpl->times[tmpl->phaseCount - 1];
    duration *= durationScale;
    fullChance = 0.5f;
    partialChance = 0.75f;
    bool inward;
    if (HSD_Randf() <= 0.7f) {
        fullChance = 0.3f;
        inward = true;
        partialChance = 0.65f;
    } else {
        inward = false;
    }
    chance = HSD_Randf();
    if (chance < fullChance) {
        endTime = duration;
        startTime = 0.0f;
    } else if (chance < partialChance) {
        startTime = 0.0f;
        endTime = duration * HSD_Randf();
        if (endTime < 0.5f * duration) endTime = 0.5f * duration;
    } else {
        startTime = duration * HSD_Randf();
        if (startTime > 0.5f * duration) startTime = 0.5f * duration;
        endTime = duration;
    }
    random = HSD_Randf();
    f32 distanceRange = params->maxDistance - params->minDistance;
    distance = distanceRange * random + params->minDistance;
    startDistance = distance;
    random = HSD_Randf();
    f32 otherDistanceRange = params->maxDistance - params->minDistance;
    otherDistance = otherDistanceRange * random + params->minDistance;
    endDistance = otherDistance;
    if (inward) {
        if (distance < otherDistance) {
            endDistance = distance;
            startDistance = otherDistance;
        }
        random = HSD_Randf();
        f32 heightRange = params->maxHeight - params->minHeight;
        height = heightRange * random + params->minHeight;
    } else {
        if (distance > otherDistance) {
            endDistance = distance;
            startDistance = otherDistance;
        }
        random = HSD_Randf();
        f32 heightRange = params->maxHeight - params->minHeight;
        height = heightRange * random + params->minHeight;
    }
    if (reverse) {
        random = HSD_Randf();
        f32 yawRange = params->maxYawOffset - params->minYawOffset;
        yaw = (params->yaw - params->minYawOffset) - yawRange * random;
    } else {
        random = HSD_Randf();
        f32 yawRange = params->maxYawOffset - params->minYawOffset;
        yaw = yawRange * random + (params->yaw + params->minYawOffset);
    }
    cameraSetDistance(1.25f * startDistance);
    cameraSetHeight(height);
    cameraSetRotY(yaw);
    cameraUpdate();
    GSvec position, target;
    cameraGetPosition(&position);
    cameraGetTargetPos(&target);
    PSVECSubtract(&position, &target, &position);
    position.y = 0.0f;
    PSVECScale(&position, &position, endDistance / startDistance);
    position.y = height;
    PSVECAdd(&position, &target, &position);
    endTime -= startTime;
    cameraMovePosition(CAMERA_OFFSET, &position, endTime);
    cameraMovePositionAcc(endTime * (0.1f / GSgfxVideoGetVsyncRate()));
    params->distances[0] = sqrtf(startDistance * startDistance + height * height);
    f32 middleDistance = 0.5f * (startDistance + endDistance);
    params->distances[1] = sqrtf(middleDistance * middleDistance + height * height);
    params->distances[2] = sqrtf(endDistance * endDistance + height * height);
}

static inline void getFOVRange(wazaSequenceCameraParams* params, f32* minimum,
                        f32* maximum, f32 distance)
{
    f32 size = 0.75f * params->width;
    if (params->height > size)
        size = params->height;
    f32 angle = atan2(0.6f * size, distance);
    *minimum = angle * 2.0f * (f32)(180.0 / 3.141592653589793);
    angle = atan2(1.8f * size, distance);
    *maximum = angle * 2.0f * (f32)(180.0 / 3.141592653589793);
    if (*minimum < 15.0f) *minimum = 15.0f;
    if (*maximum < 15.0f) *maximum = 15.0f;
    if (*minimum > 85.0f) *minimum = 85.0f;
    if (*maximum > 85.0f) *maximum = 85.0f;
}

void _wazaSequenceCameraDoFOV(ModelSequence* model, wazaSequenceCameraParams* params,
                             int flags, f32 duration)
{
    int motionType = model->motionType;
    TemplateExpFileHeader* tmpl = &model->templates[motionType];
    int phaseCount = tmpl->phaseCount;
    f32 ratio, endMinimum, phaseStart, phaseEnd, minimumRatio, maximumRatio, currentFOV;
    f32 minimumFOV, maximumFOV;
    minimumRatio = 0.0f;
    maximumRatio = 1.0f;
    getFOVRange(params, &minimumFOV, &maximumFOV, params->distances[0]);
    switch (params->positionType) {
    case WAZA_POSITION_DOLLY:
    case WAZA_POSITION_OVERVIEW:
    case WAZA_POSITION_MODEL_SIDE: {
        int mask;
        if (flags & WAZA_CAMERA_CLOSE) mask = WAZA_CAMERA_FOV_NARROW;
        else if (flags & WAZA_CAMERA_WIDE) mask = WAZA_CAMERA_FOV_WIDE;
        else mask = WAZA_CAMERA_FOV_MIDDLE;
        selectFOVRatio(mask, &minimumRatio, &maximumRatio);
        ratio = minimumRatio + (maximumRatio - minimumRatio) * HSD_Randf();
        f32 fov = minimumFOV + ratio * (maximumFOV - minimumFOV);
        gCurrentFOV = gFOVlist[0].startFov = gFOVlist[0].endFov = gFOVlist[1].startFov = gFOVlist[1].endFov = fov;
        gCurrentFOVTime = gFOVlist[0].startTime = gFOVlist[0].endTime = gFOVlist[1].startTime = gFOVlist[1].endTime = tmpl->times[0] * duration;
        cameraSetFov(fov);
        break;
    }
    default: {
        bool attack = motionType != MODEL_MOTION_DEFENSE_FIRST && motionType != MODEL_MOTION_DEFENSE_SECOND;
        wazaSequenceCameraPattern* pattern = wazaSequenceCameraGetPattern(attack, flags);
        wazaSequenceCameraShot* shot = pattern->shots;
        selectFOVRatio(shot->startFovMask, &minimumRatio, &maximumRatio);
        ratio = minimumRatio + (maximumRatio - minimumRatio) * HSD_Randf();
        f32 fov = minimumFOV + ratio * (maximumFOV - minimumFOV);
        gCurrentFOV = gFOVlist[0].startFov = gFOVlist[0].endFov = gFOVlist[1].startFov = gFOVlist[1].endFov = fov;
        gCurrentFOVTime = gFOVlist[0].startTime = gFOVlist[0].endTime = gFOVlist[1].startTime = gFOVlist[1].endTime = tmpl->times[0] * duration;
        cameraSetFov(fov);
        if (phaseCount > 2) {
            currentFOV = fov;
            phaseEnd = tmpl->times[0];
            for (int i = 0; i < 2; ++i, ++shot) {
                phaseStart = phaseEnd;
                phaseEnd = tmpl->times[i + 1];
                if (phaseStart == phaseEnd) {
                    gFOVlist[i].startTime = phaseStart * duration;
                    gFOVlist[i].endTime = phaseStart * duration;
                    gFOVlist[i].startFov = currentFOV;
                    gFOVlist[i].endFov = currentFOV;
                } else {
                    switch (shot->fovMotion) {
                    case WAZA_CAMERA_FOV_ZOOM_IN:
                    case WAZA_CAMERA_FOV_ZOOM_OUT: {
                        f32 zoomDuration = _wazaSequenceCameraSelectDuration(shot->durationMask, shot->durationWeights, phaseEnd - phaseStart);
                        f32 endMaximum;
                        getFOVRange(params, &endMinimum, &endMaximum, params->distances[i + 1]);
                        selectFOVRatio(shot->endFovMask, &minimumRatio, &maximumRatio);
                        ratio = minimumRatio + (maximumRatio - minimumRatio) * HSD_Randf();
                        f32 zoomStart, zoomEnd;
                        f32 endFOV = endMinimum + ratio * (endMaximum - endMinimum);
                        if (shot->zoomAlign == WAZA_CAMERA_ZOOM_END) {
                            zoomEnd = phaseEnd;
                            zoomStart = phaseEnd - zoomDuration;
                        } else {
                            zoomStart = phaseStart;
                            zoomEnd = phaseStart + zoomDuration;
                        }
                        gFOVlist[i].startTime = zoomStart * duration;
                        gFOVlist[i].endTime = zoomEnd * duration;
                        gFOVlist[i].startFov = currentFOV;
                        gFOVlist[i].endFov = endFOV;
                        currentFOV = endFOV;
                        break;
                    }
                    case WAZA_CAMERA_FOV_NONE:
                    case WAZA_CAMERA_FOV_HOLD:
                    case WAZA_CAMERA_FOV_HOLD_ANY:
                    default:
                        gFOVlist[i].startTime = phaseStart * duration;
                        gFOVlist[i].endTime = phaseEnd * duration;
                        gFOVlist[i].startFov = currentFOV;
                        gFOVlist[i].endFov = currentFOV;
                        break;
                    }
                }
            }
        }
        break;
    }
    }
}

void _wazaSequenceCameraGetFOVRange(wazaSequenceCameraParams* params,
                                          f32* minimum, f32* maximum,
                                          f32 distance)
{
    getFOVRange(params, minimum, maximum, distance);
}

f32 _wazaSequenceCameraSelectDuration(u8 mask, f32* weights, f32 available)
{
    if (mask == WAZA_CAMERA_DURATION_SHORT)
        return available < 0.2f ? available : 0.2f;
    if (mask == WAZA_CAMERA_DURATION_MEDIUM)
        return available < 0.333f ? available : 0.333f;
    if (mask == WAZA_CAMERA_DURATION_LONG)
        return available < 0.75f ? available : 0.75f;
    if (mask == WAZA_CAMERA_DURATION_FULL)
        return available;
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_MEDIUM)) {
        if (HSD_Randf() < weights[0]) return available < 0.2f ? available : 0.2f;
        return available < 0.333f ? available : 0.333f;
    }
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG)) {
        if (HSD_Randf() < weights[0]) return available < 0.2f ? available : 0.2f;
        return available < 0.75f ? available : 0.75f;
    }
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_FULL)) {
        if (HSD_Randf() < weights[0]) return available < 0.2f ? available : 0.2f;
        return available;
    }
    if (mask == (WAZA_CAMERA_DURATION_MEDIUM | WAZA_CAMERA_DURATION_LONG)) {
        if (HSD_Randf() < weights[1]) return available < 0.333f ? available : 0.333f;
        return available < 0.75f ? available : 0.75f;
    }
    if (mask == (WAZA_CAMERA_DURATION_MEDIUM | WAZA_CAMERA_DURATION_FULL)) {
        if (HSD_Randf() < weights[1]) return available < 0.333f ? available : 0.333f;
        return available;
    }
    if (mask == (WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL)) {
        if (HSD_Randf() < weights[2]) return available < 0.75f ? available : 0.75f;
        return available;
    }
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_MEDIUM | WAZA_CAMERA_DURATION_LONG)) {
        f32 random = HSD_Randf();
        if (random < weights[0]) return available < 0.2f ? available : 0.2f;
        if (random < weights[1]) return available < 0.333f ? available : 0.333f;
        return available < 0.75f ? available : 0.75f;
    }
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_MEDIUM | WAZA_CAMERA_DURATION_FULL)) {
        f32 random = HSD_Randf();
        if (random < weights[0]) return available < 0.2f ? available : 0.2f;
        if (random < weights[1]) return available < 0.333f ? available : 0.333f;
        return available;
    }
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL)) {
        f32 random = HSD_Randf();
        if (random < weights[0]) return available < 0.2f ? available : 0.2f;
        if (random < weights[2]) return available < 0.75f ? available : 0.75f;
        return available;
    }
    if (mask == (WAZA_CAMERA_DURATION_MEDIUM | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL)) {
        f32 random = HSD_Randf();
        if (random < weights[1]) return available < 0.2f ? available : 0.2f;
        if (random < weights[2]) return available < 0.333f ? available : 0.333f;
        return available;
    }
    if (mask == (WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_MEDIUM | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL)) {
        f32 random = HSD_Randf();
        if (random < weights[0]) return available < 0.2f ? available : 0.2f;
        if (random < weights[1]) return available < 0.2f ? available : 0.2f;
        if (random < weights[2]) return available < 0.333f ? available : 0.333f;
        return available;
    }
    return available;
}

void _wazaSequenceCameraSelectMotion(ModelSequence* model, WazaSequence* waza,
                                    wazaSequenceCameraParams* params)
{
    bool hold, dolly, sweep, orbit;
    int count = 0;
    if (model->modelID == WAZA_CAMERA_OVERVIEW_MODEL) {
        params->positionType = WAZA_POSITION_OVERVIEW;
        return;
    }
    if (waza) {
        if (waza->flags & WAZA_CAMERA_MODEL_SIDE) {
            params->positionType = WAZA_POSITION_MODEL_SIDE;
            return;
        }
        hold = (waza->flags & WAZA_CAMERA_ALLOW_HOLD) != 0;
        if (hold) ++count;
        dolly = (waza->flags & WAZA_CAMERA_ALLOW_DOLLY) != 0;
        if (dolly) ++count;
        sweep = (waza->flags & WAZA_CAMERA_ALLOW_SWEEP) != 0;
        if (sweep) ++count;
        orbit = (waza->flags & WAZA_CAMERA_ALLOW_ORBIT) != 0;
        if (orbit) ++count;
    }
    if (count == 0) {
        count = 4;
        hold = true; dolly = true; sweep = true; orbit = true;
    } else if (count == 1) {
        if (hold) params->positionType = WAZA_POSITION_HOLD;
        if (dolly) params->positionType = WAZA_POSITION_DOLLY;
        if (sweep) params->positionType = WAZA_POSITION_SWEEP;
        if (orbit) params->positionType = WAZA_POSITION_ORBIT;
        gLastMotionType = params->positionType;
        return;
    }
    f32 weight = 1.0f / count;
    while (true) {
        f32 random = HSD_Randf();
        f32 threshold = weight;
        if (hold) {
            if (threshold > random) {
                params->positionType = WAZA_POSITION_HOLD;
                if (gLastMotionType != params->positionType) break;
            }
            threshold += weight;
        }
        if (dolly) {
            if (threshold > random) {
                params->positionType = WAZA_POSITION_DOLLY;
                if (gLastMotionType != params->positionType) break;
            }
            threshold += weight;
        }
        if (sweep) {
            if (threshold > random) {
                params->positionType = WAZA_POSITION_SWEEP;
                if (gLastMotionType != params->positionType) break;
            }
            threshold += weight;
        }
        if (orbit) {
            if (threshold > random) {
                params->positionType = WAZA_POSITION_ORBIT;
                if (gLastMotionType != params->positionType) break;
            }
        }
    }
    gLastMotionType = params->positionType;
}

void _wazaSequenceCameraCalculateParams(ModelSequence* model, int flags,
                                       wazaSequenceCameraParams* params)
{
    _GSmodel* gsModel = model->model;
    GSbound* bound = GSmodelGetBound(gsModel);
    int size = model->modelSize;
    GSvec extent, minimum, maximum, rotation;
    GSmodelGetRotation(gsModel, &rotation);
    params->yaw = rotation.y;
    f32 framingScale, distanceScale;
    switch (size) {
    case MODEL_SIZE_CLASS_SMALLEST: framingScale = 2.4f; distanceScale = 0.875f; break;
    case MODEL_SIZE_CLASS_SMALL: framingScale = 1.5f; distanceScale = 0.875f; break;
    case MODEL_SIZE_CLASS_LARGE: framingScale = 1.25f; distanceScale = 1.4f; break;
    case MODEL_SIZE_CLASS_LARGER: framingScale = 1.3f; distanceScale = 1.8f; break;
    case MODEL_SIZE_CLASS_LARGEST: framingScale = 1.5f; distanceScale = 3.0f; break;
    default: framingScale = 1.2f; distanceScale = 1.0f; break;
    }
    switch (flags & WAZA_CAMERA_YAW_MASK) {
    case WAZA_CAMERA_YAW_FRONT: params->minYawOffset = 0.0f; params->maxYawOffset = 0.017453292f * 20.0f; break;
    case WAZA_CAMERA_YAW_FRONT_OFFSET: params->minYawOffset = 0.017453292f * 10.0f; params->maxYawOffset = 0.017453292f * 20.0f; break;
    case WAZA_CAMERA_YAW_FRONT_ANGLED: params->minYawOffset = 0.017453292f * 12.0f; params->maxYawOffset = 0.017453292f * 42.0f; break;
    case WAZA_CAMERA_YAW_THREE_QUARTER:
        if (size > MODEL_SIZE_CLASS_MEDIUM) { params->minYawOffset = 0.017453292f * 18.0f; params->maxYawOffset = 0.017453292f * 48.0f; }
        else { params->minYawOffset = 0.017453292f * 30.0f; params->maxYawOffset = 0.017453292f * 60.0f; }
        break;
    case WAZA_CAMERA_YAW_SIDE_APPROACH:
        if (size > MODEL_SIZE_CLASS_MEDIUM) { params->minYawOffset = 0.017453292f * 36.0f; params->maxYawOffset = 0.017453292f * 66.0f; }
        else { params->minYawOffset = 0.017453292f * 60.0f; params->maxYawOffset = 0.017453292f * 78.0f; }
        break;
    case WAZA_CAMERA_YAW_SIDE: params->minYawOffset = 0.017453292f * 70.0f; params->maxYawOffset = 0.017453292f * 80.0f; break;
    case WAZA_CAMERA_YAW_SIDE_FULL: params->minYawOffset = 0.017453292f * 70.0f; params->maxYawOffset = 0.017453292f * 90.0f; break;
    default:
        if (size > MODEL_SIZE_CLASS_MEDIUM) { params->minYawOffset = 0.017453292f * 12.0f; params->maxYawOffset = 0.017453292f * 60.0f; }
        else { params->minYawOffset = 0.017453292f * 12.0f; params->maxYawOffset = 0.017453292f * 72.0f; }
        break;
    }
    f32 minimumHeight, maximumHeight;
    if (flags & WAZA_CAMERA_CLOSE) {
        params->minDistance = 25.0f;
        minimumHeight = 6.0f; maximumHeight = 8.0f;
        params->maxDistance = 35.0f;
    } else if (flags & WAZA_CAMERA_MEDIUM) {
        params->minDistance = 35.0f; params->maxDistance = 50.0f;
        minimumHeight = 6.0f; maximumHeight = 11.0f;
    } else if (flags & WAZA_CAMERA_WIDE) {
        params->minDistance = 50.0f; params->maxDistance = 60.0f;
        minimumHeight = 6.0f; maximumHeight = 20.0f;
    } else {
        minimumHeight = 6.0f; maximumHeight = 20.0f;
        params->minDistance = 20.0f; params->maxDistance = 60.0f;
    }
    params->minDistance *= distanceScale;
    params->maxDistance *= distanceScale;
    GSboundGetMinMax(bound, &minimum, &maximum, false, false, false, false);
    extent = maximum - minimum;
    if (extent.x < 0.0f) extent.x = -extent.x;
    if (extent.y < 0.0f) extent.y = -extent.y;
    if (extent.z < 0.0f) extent.z = -extent.z;
    f32 magnitude = PSVECMag(&extent);
    f32 height = extent.y;
    params->width = (1.0f / 1.7320508f) * magnitude;
    params->height = height;
    params->minHeight = minimum.y;
    params->maxHeight = maximum.y;
    if (params->minHeight < minimumHeight) {
        params->minHeight = minimumHeight;
        if (params->maxHeight < minimumHeight) params->maxHeight = 1.5f * minimumHeight;
    }
    if (params->maxHeight > maximumHeight) {
        params->maxHeight = maximumHeight;
        if (params->minHeight > maximumHeight) params->minHeight = 0.8f * maximumHeight;
    }
    params->width *= framingScale;
    params->height *= framingScale;
    if (params->minDistance < 20.0f) params->minDistance = 20.0f;
    if (params->maxDistance < 40.0f) params->maxDistance = 40.0f;
    if (params->minDistance > 48.0f) params->minDistance = 48.0f;
    if (params->maxDistance > 60.0f) params->maxDistance = 60.0f;
}

