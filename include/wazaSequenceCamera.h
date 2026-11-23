#ifndef WAZA_SEQUENCE_CAMERA_H
#define WAZA_SEQUENCE_CAMERA_H

#include <types.h>
#include <bool.h>

enum {
    WAZA_CAMERA_CLOSE = 0x80,
    WAZA_CAMERA_MEDIUM = 0x100,
    WAZA_CAMERA_WIDE = 0x200
};

enum {
    WAZA_CAMERA_FOV_NONE = 0,
    WAZA_CAMERA_FOV_HOLD = 1,
    WAZA_CAMERA_FOV_HOLD_ANY = 2,
    WAZA_CAMERA_FOV_ZOOM_IN = 3,
    WAZA_CAMERA_FOV_ZOOM_OUT = 4
};

enum {
    WAZA_CAMERA_DURATION_SHORT = 0x1,
    WAZA_CAMERA_DURATION_MEDIUM = 0x2,
    WAZA_CAMERA_DURATION_LONG = 0x4,
    WAZA_CAMERA_DURATION_FULL = 0x8
};

enum {
    WAZA_CAMERA_FOV_NARROW = 0x1,
    WAZA_CAMERA_FOV_MIDDLE = 0x2,
    WAZA_CAMERA_FOV_WIDE = 0x4
};

enum {
    WAZA_CAMERA_ZOOM_START = 1,
    WAZA_CAMERA_ZOOM_END = 2
};

struct wazaSequenceCameraShot {
    f32 durationWeights[4];
    u8 fovMotion;
    u8 durationMask;
    u8 startFovMask;
    u8 endFovMask;
    u8 zoomAlign;
};

struct wazaSequenceCameraPattern {
    f32 weight;
    wazaSequenceCameraShot shots[2];
};

struct wazaSequenceCameraFov {
    f32 startFov;
    f32 endFov;
    f32 startTime;
    f32 endTime;
};

extern wazaSequenceCameraPattern attackPatterns[];
extern wazaSequenceCameraPattern defensePatterns[];

wazaSequenceCameraPattern* wazaSequenceCameraGetPattern(bool attack, int flags);

class ModelSequence;
class WazaSequence;
struct TemplateExpFileHeader;

enum WazaCameraPosition {
    WAZA_POSITION_DOLLY,
    WAZA_POSITION_SWEEP,
    WAZA_POSITION_ORBIT,
    WAZA_POSITION_HOLD,
    WAZA_POSITION_OVERVIEW,
    WAZA_POSITION_MODEL_SIDE
};

/* The yaw range of the camera is held in the low three bits of its flags. */
enum WazaCameraYawMask {
    WAZA_CAMERA_YAW_MASK = 0x7
};

enum WazaCameraYawRange {
    WAZA_CAMERA_YAW_DEFAULT,
    WAZA_CAMERA_YAW_FRONT,
    WAZA_CAMERA_YAW_FRONT_OFFSET,
    WAZA_CAMERA_YAW_FRONT_ANGLED,
    WAZA_CAMERA_YAW_THREE_QUARTER,
    WAZA_CAMERA_YAW_SIDE_APPROACH,
    WAZA_CAMERA_YAW_SIDE,
    WAZA_CAMERA_YAW_SIDE_FULL
};

/* Stage model whose move sequence is watched from further away than the ones
   the camera patterns frame. */
enum WazaCameraOverviewModel {
    WAZA_CAMERA_OVERVIEW_MODEL = 314
};

enum WazaCameraMotionFlags {
    WAZA_CAMERA_MODEL_SIDE = 0x1,
    WAZA_CAMERA_ALLOW_HOLD = 0x8,
    WAZA_CAMERA_ALLOW_DOLLY = 0x10,
    WAZA_CAMERA_ALLOW_SWEEP = 0x20,
    WAZA_CAMERA_ALLOW_ORBIT = 0x40
};

struct wazaSequenceCameraParams {
    WazaCameraPosition positionType;
    f32 width, height;
    f32 minYawOffset, maxYawOffset, yaw;
    f32 minHeight, maxHeight;
    f32 minDistance, maxDistance;
    f32 distances[3];
};

void _wazaSequenceCameraDoPosition(ModelSequence *model, wazaSequenceCameraParams *params, f32 durationScale, bool reverse);
void _wazaSequenceCameraDoDollyPosition(TemplateExpFileHeader *tmpl, wazaSequenceCameraParams *params, f32 durationScale, bool reverse);
void _wazaSequenceCameraDoFOV(ModelSequence *model, wazaSequenceCameraParams *params, int flags, f32 duration);
void _wazaSequenceCameraGetFOVRange(wazaSequenceCameraParams *params, f32 *minimum, f32 *maximum, f32 distance);
f32 _wazaSequenceCameraSelectDuration(u8 mask, f32 *weights, f32 available);
void _wazaSequenceCameraSelectMotion(ModelSequence *model, WazaSequence *waza, wazaSequenceCameraParams *params);
void _wazaSequenceCameraCalculateParams(ModelSequence *model, int flags, wazaSequenceCameraParams *params);

extern f32 gCurrentFOV;
extern f32 gCurrentFOVTime;
extern int gLastMotionType;
extern wazaSequenceCameraFov gFOVlist[2];

#endif
