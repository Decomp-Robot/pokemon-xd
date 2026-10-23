#include <menuPocketBattleDisk.h>

// getCursorPositionY -- cursor Y for the BattleDisk item list.
// The item row advances in 30-pixel steps; while the scroll hold flag is
// clear the fractional scroll offset is folded in (truncated to short).
// Groups with the other BattleDisk helpers sharing gMenuPocketBattleDiskWork
// (DrawRollDown/Up, PageBar, Ctrl, Main, _controlCursor, printItemList,
// selectItem, getItemIDFromMenuPos, getNbItemSlot), which will join this
// module as they are recovered.
//
// Note: this translation unit builds with the peephole optimizer disabled
// (menuPocket, GC/1.3.2 -opt nopeephole). With GC/2.6 the prologue
// schedules the work pointer before the stack adjust; GC/1.3.2 with
// nopeephole keeps the original stwu-first order exactly.
s32 getCursorPositionY()
{
    s32 y = (s16)(gMenuPocketBattleDiskWork.mItemRow * 30);

    if (gMenuPocketBattleDiskWork.mScrollHold == 0) {
        y += (s16)(s32)gMenuPocketBattleDiskWork.mScrollOffset;
    }
    return y;
}
