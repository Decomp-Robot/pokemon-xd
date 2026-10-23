#include <menuPocketBattleDisk.h>

// getCursorPositionY -- cursor Y for the battle-disk list.
// The row index advances in 30-dot steps; while the list is held the
// fractional scroll offset is ignored, otherwise it is added.
s32 getCursorPositionY(void)
{
    s32 y = (s16)(menuPocketBattleDiskWork.mRow * 30);

    if (menuPocketBattleDiskWork.mHold == 0) {
        y += (s16)(s32)menuPocketBattleDiskWork.mScroll;
    }
    return y;
}
