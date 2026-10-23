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

/* Partial WorldMap declaration.
 *
 * The constructor only initializes the leading three words; the rest
 * of the object (icon array at 0x0C, cursor state, camera work at 0x44, for
 * a total size of 0x48 bytes) is set up by Init and the other member
 * functions, so those members are intentionally not declared here. */
class WorldMap {
public:
    WorldMap();
    s32 GetNowCursor(tagWINDOW_WORK* window);

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
};

#endif
