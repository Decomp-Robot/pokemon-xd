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

extern wazaSequenceCameraPattern attackPatterns[];
extern wazaSequenceCameraPattern defensePatterns[];

wazaSequenceCameraPattern* wazaSequenceCameraGetPattern(bool attack, int flags);

#endif
