#ifndef MENU_BATTLE_DISK_H
#define MENU_BATTLE_DISK_H

#include <hero.h>
#include <types.h>

// Battle CD selection state and the two generated battle parties.
struct BattleDiskListEntry {
    u8 mDiskID;
    u8 mReservedID;
    u16 mItemID;
    u32 mItemNameMessage;
    u32 mItemDocumentMessage;
    u32 mBattleDescriptionMessage;
    u32 mBattleDetailsMessage;
    u32 mBattleTitleMessage;
};

struct MenuBattleDiskWork {
    s16 mCurrentX;
    s16 mCurrentY;
    s16 mRequestedX;
    s16 mRequestedY;
    s16 mMode;
    s16 mDiskCount;
    s16 mFirstDiskRow;
    s16 mScrollTargetY;
    s16 mScrollY;
    s16 mSelectedDiskRow;
    s16 mSelectedPokemonIndex;       // 0x14
    u8 mBattlePokemonCount;          // 0x16
    u8 mBattleDiskID;                // 0x17
    u16 mBattleResult;               // 0x18
    u16 mResultAnimationFrame;       // 0x1A
    u16 mResultAnimationEndFrame;    // 0x1C
    u8 mReservedResultData[0x02];
    f32 mPreviewElapsedSec;
    s32 mDocumentChoice;
    BattleDiskListEntry mDiskEntries[60];
    Hero mHeroes[2];                 // 0x5C8

    MenuBattleDiskWork()
    {
        mScrollTargetY = 0;
        mSelectedDiskRow = 0;
        mScrollY = 0;
        mFirstDiskRow = 0;
        mDiskCount = 0;
    }
};
extern MenuBattleDiskWork gBDisk;

#endif
