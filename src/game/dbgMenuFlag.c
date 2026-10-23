#include <types.h>

// dbg_menuflag work ("btldbg_menuflag" system-vars block):
// gDbgMenuFlagWork holds the flag-menu tree (count + node array; nodes are
// three s16s with the first-child link at +4), and gDbgMenuFlagIdTable maps
// tree nodes to GS flag IDs (three s16s per entry, ID at +4).
extern u32 *gDbgMenuFlagWork;
extern s16 *gDbgMenuFlagIdTable;

// _dbgMenuFlagGetID -- flag ID shown for the flag-info list cursor. The
// cursor indexes the flag tree in depth order: cursor 0 is the root's first
// child and every further step walks one node down the firstChild links.
// The ID returns zero-extended, so the menu can sprintf it directly.
//
// Note: this translation unit builds with the peephole optimizer disabled
// (GC/2.6 -opt nopeephole). With the peephole pass on, the loop bound folds
// into a recording subtract (subic.) and the branch pairs re-schedule; the
// original object keeps the separate addi/cmpwi guard and load order.
u16 _dbgMenuFlagGetID(s32 cursor) {
    s32 i;
    s16 node;

    if (cursor < 0) {
        return 0;
    }
    if (cursor >= *gDbgMenuFlagWork) {
        return 0;
    }

    node = ((s16 *)gDbgMenuFlagWork)[2];
    if (cursor != 0) {
        for (i = 0; i < cursor - 1; i++) {
            node = gDbgMenuFlagIdTable[node * 3 + 2];
        }
        node = gDbgMenuFlagIdTable[node * 3 + 2];
    }
    return node;
}
