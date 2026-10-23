#ifndef GS_ATTACH_H
#define GS_ATTACH_H

// Transform components inherited from an attached part; not bit flags.
enum GSattachMode {
    GS_ATTACH_NONE = 0,
    GS_ATTACH_POSITION = 1,
    GS_ATTACH_ROTATION = 2,
    GS_ATTACH_SCALE = 3,
    GS_ATTACH_POSITION_ROTATION = 4,
    GS_ATTACH_ROTATION_SCALE = 5,
    GS_ATTACH_POSITION_SCALE = 6,
    GS_ATTACH_TRANSFORM = 7,
    GS_ATTACH_ALL = GS_ATTACH_TRANSFORM,
    GS_ATTACH_MATRIX = 8 // Model attachments only.
};

#endif
