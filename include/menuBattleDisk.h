#ifndef MENU_BATTLE_DISK_H
#define MENU_BATTLE_DISK_H

#include <hero.h>
#include <types.h>

// Battle CD selection state and the two generated battle parties.
struct MenuBattleDiskWork {
    MenuBattleDiskWork() {
        mScrollTarget = 0;
        mCursorRow = 0;
        mScrollPosition = 0;
        mListTop = 0;
        mDiskCount = 0;
    }

    u8 _00[0xA];
    s16 mDiskCount;
    s16 mListTop;
    s16 mScrollTarget;
    s16 mScrollPosition;
    s16 mCursorRow;
    u8 _14[0x5C8 - 0x14];
    Hero mBattleHero[2];
    u8 _18B8[0x18C0 - 0x18B8];
};

extern MenuBattleDiskWork gBDisk;

#endif
