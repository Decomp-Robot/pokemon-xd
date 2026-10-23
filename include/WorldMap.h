#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <types.h>

class glowEffect;

/* Window work, partial: GetNowCursor only reads the cursor slot at 0x9E. */
struct tagWINDOW_WORK {
    u8 _00[0x9E];
    s16 cursor;
};

/* Partial view of the WorldMap work block addressed by the pointer at 0x08.
 * Only the selected-cursor slot at +0x30 (read by GetNowCursor) is
 * recovered so far; the remainder of the block layout is unknown. */
struct WorldMapWork {
    u8 _00[0x30];
    s32 cursor;
};

/* Camera-blend work block (0x68 bytes, allocated by Init). IsBlending only
 * reads the blend parameter at +0x08, which ControlCamera drives from 0 to
 * 1 (its per-frame increment +0x10 is clamped so the blend never passes 1);
 * the rest of the block (target position/angle work at +0x18, camera
 * position at +0x40) is still unrecovered. */
struct WorldMapCamWork {
    u8 _00[0x08];
    /* 0x08: camera blend parameter; 0 = camera at the old position,
     * 1 = fully blended to the new one. */
    f32 blend;
    /* 0x0C: rate feeding the per-frame increment; ControlCamera adds
     * rate * dt into +0x10 every frame. */
    f32 _0C;
    /* 0x10: per-frame blend increment added into the blend parameter. */
    f32 _10;
    /* 0x14: elapsed camera-move time; ControlCamera accumulates dt here. */
    f32 _14;
    u8 _18[0x50];
};

/* Partial WorldMap declaration (0x48 bytes; menuWorldMapMXInit allocates
 * 0x48 and runs the constructor over it).
 *
 * The constructor only initializes the leading three words; the icon
 * array at 0x0C, the cursor state and the camera work at 0x44 are set
 * up by Init and the other member functions, so only the members those
 * functions have exposed so far are declared here. */
class WorldMap {
public:
    WorldMap();
    s32 GetNowCursor(tagWINDOW_WORK* window);
    s32 GetStartIconAnimIndex(unsigned long index);
    /* True once the camera blend started by SetCameraNextWork has fully
     * faded in (blend parameter reached 1). */
    bool IsBlending();
    f32 GetAngle(long work);
    void SetCameraSpeed(f32 speed);

private:
    /* 0x00: Purpose unknown. No WorldMap member function besides the
     * constructor writes it; the only other access is the null-check in
     * the kyogreMoveDemo teardown (__dt__14kyogreMoveDemoFv), which runs
     * on this same heap instance (menuWorldMapMXInit constructs it here
     * and menuWorldMapMXExit tears it down via that dtor) before
     * releasing the offscreen data. This is distinct from
     * menuWorldMapMoveDemoExit, which checks offset 0 of the separate
     * 4-byte WorldMapMoveDemo block. */
    u32 _00;
    /* 0x04: Move-demo effect. moveDemo allocates a glowEffect here;
     * RenderMoveDemoModel uses it, and MainSub invokes it
     * and releases it back to NULL. */
    glowEffect* mDemoEffect;
    /* 0x08: Model work block (0x94 bytes). Init allocates and clears it;
     * Release frees it and nulls the pointer. GetNowCursor reads the
     * selected cursor at +0x30 of this same block (see WorldMapWork). */
    WorldMapWork* mModelWork;
    /* 0x0C: Icon works (array of 0x2C-byte WorldMapIconWork entries, one
     * per flag data entry). Init allocates and constructs the array;
     * InitModel, SelectIcon and SetCursorPosition index it. */
    void* mIcons;
    /* 0x10..0x43: Unrecovered map state (cursor position, camera targets).
     * Only MainSub/ControlCamera touch these; left as padding until those
     * functions are recovered. */
    u8 _10[0x34];
    /* 0x44: Camera work block (0x68 bytes). Init allocates it and zeroes
     * the blend fields; IsBlending reads the blend parameter at +0x08 and
     * ControlCamera drives it toward the targets set by SetCameraNextWork
     * (see WorldMapCamWork). */
    WorldMapCamWork* mCamWork;
};

#endif
