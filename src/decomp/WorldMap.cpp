#include <types.h>

class glowEffect;

/* Partial WorldMap declaration.
 *
 * The constructor below only initializes the leading three words; the rest
 * of the object (icon array at 0x0C, cursor state, camera work at 0x44, for
 * a total size of 0x48 bytes) is set up by Init and the other member
 * functions, so those members are intentionally not declared here. */
class WorldMap {
public:
    WorldMap();

private:
    /* 0x00: Purpose unknown. No WorldMap member function besides this
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
     * Release frees it and nulls the pointer. */
    void* mModelWork;
};

/* WorldMap::WorldMap — clear the reserved word and both work pointers.
 * The assignment order matches the original codegen. */
WorldMap::WorldMap()
{
    _00 = 0;
    mModelWork = 0;
    mDemoEffect = 0;
}
