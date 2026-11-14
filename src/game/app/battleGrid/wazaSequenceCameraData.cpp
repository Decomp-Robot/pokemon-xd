#include <wazaSequenceCamera.h>
#include <hsdRandom.h>

enum {
    AttackPatternCount = 8,
    DefensePatternCount = 13,
    AttackClosePattern = 4,
    AttackMediumPattern = 5,
    AttackWidePattern = 6,
    DefenseClosePattern = 9,
    DefenseMediumPattern = 10,
    DefenseWidePattern = 11
};

wazaSequenceCameraPattern attackPatterns[AttackPatternCount] = {
    { 0.2f, {
        { { 0.2f, 0.0f, 0.7f, 0.1f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          WAZA_CAMERA_FOV_MIDDLE | WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_NARROW,
          WAZA_CAMERA_ZOOM_START },
        { { 0.8f, 0.0f, 0.2f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          0, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.25f, {
        { { 0.3f, 0.0f, 0.4f, 0.3f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_END },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.2f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_NARROW, 0,
          0 },
        { { 0.8f, 0.0f, 0.2f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          0, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.1f, {
        { { 0.1f, 0.0f, 0.2f, 0.7f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          WAZA_CAMERA_FOV_NARROW, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_END },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_NARROW, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_MIDDLE, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_WIDE, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.1f, {
        { { 0.0f, 1.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_MEDIUM,
          WAZA_CAMERA_FOV_NARROW, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_END },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
};

wazaSequenceCameraPattern defensePatterns[DefensePatternCount] = {
    { 0.15f, {
        { { 0.7f, 0.0f, 0.15f, 0.15f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          WAZA_CAMERA_FOV_NARROW, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.2f, {
        { { 0.8f, 0.0f, 0.2f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          WAZA_CAMERA_FOV_NARROW, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
        { { 0.5f, 0.0f, 0.0f, 0.5f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          0, WAZA_CAMERA_FOV_NARROW,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.05f, {
        { { 0.7f, 0.0f, 0.3f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          WAZA_CAMERA_FOV_MIDDLE | WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_NARROW,
          WAZA_CAMERA_ZOOM_START },
        { { 0.2f, 0.0f, 0.5f, 0.3f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          0, WAZA_CAMERA_FOV_MIDDLE | WAZA_CAMERA_FOV_WIDE,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.05f, {
        { { 0.8f, 0.0f, 0.2f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_NARROW | WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
        { { 0.2f, 0.0f, 0.5f, 0.3f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          0, WAZA_CAMERA_FOV_WIDE,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.05f, {
        { { 0.7f, 0.0f, 0.3f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          WAZA_CAMERA_FOV_MIDDLE | WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_NARROW,
          WAZA_CAMERA_ZOOM_START },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.15f, {
        { { 0.7f, 0.0f, 0.3f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG,
          WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.1f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_MIDDLE, 0,
          0 },
        { { 0.7f, 0.0f, 0.0f, 0.3f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          0, WAZA_CAMERA_FOV_NARROW,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_NARROW, 0,
          0 },
        { { 0.1f, 0.0f, 0.7f, 0.2f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          0, WAZA_CAMERA_FOV_MIDDLE | WAZA_CAMERA_FOV_WIDE,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.1f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_MIDDLE, 0,
          0 },
        { { 0.1f, 0.0f, 0.7f, 0.2f },
          WAZA_CAMERA_FOV_ZOOM_OUT, WAZA_CAMERA_DURATION_SHORT | WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          0, WAZA_CAMERA_FOV_WIDE,
          WAZA_CAMERA_ZOOM_START },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_NARROW, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_MIDDLE, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.05f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD, 0,
          WAZA_CAMERA_FOV_WIDE, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f }, WAZA_CAMERA_FOV_NONE, 0, 0, 0, 0 },
    } },
    { 0.0f, {
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_HOLD_ANY, 0,
          0, 0,
          0 },
        { { 0.0f, 0.0f, 0.0f, 0.0f },
          WAZA_CAMERA_FOV_ZOOM_IN, WAZA_CAMERA_DURATION_LONG | WAZA_CAMERA_DURATION_FULL,
          WAZA_CAMERA_FOV_WIDE, WAZA_CAMERA_FOV_NARROW | WAZA_CAMERA_FOV_MIDDLE,
          WAZA_CAMERA_ZOOM_START },
    } },
};

wazaSequenceCameraPattern* wazaSequenceCameraGetPattern(bool attack, int flags)
{
    f32 random = HSD_Randf();
    f32 weight = 0.0f;
    wazaSequenceCameraPattern* pattern;
    int count;

    if (attack) {
        pattern = attackPatterns;
        count = AttackPatternCount;
        if (flags & WAZA_CAMERA_CLOSE) {
            return &pattern[AttackClosePattern];
        }
        if (flags & WAZA_CAMERA_MEDIUM) {
            return &pattern[AttackMediumPattern];
        }
        if (flags & WAZA_CAMERA_WIDE) {
            return &pattern[AttackWidePattern];
        }
    } else {
        pattern = defensePatterns;
        count = DefensePatternCount;
        if (flags & WAZA_CAMERA_CLOSE) {
            return &pattern[DefenseClosePattern];
        }
        if (flags & WAZA_CAMERA_MEDIUM) {
            return &pattern[DefenseMediumPattern];
        }
        if (flags & WAZA_CAMERA_WIDE) {
            return &pattern[DefenseWidePattern];
        }
    }

    for (int i = 0; i < count; i++, pattern++) {
        weight += pattern->weight;
        if (random < weight) {
            return pattern;
        }
    }

    if (attack) {
        return &attackPatterns[((HSD_Rand() << 16) | HSD_Rand()) % AttackPatternCount];
    } else {
        return &defensePatterns[((HSD_Rand() << 16) | HSD_Rand()) % DefensePatternCount];
    }
}
