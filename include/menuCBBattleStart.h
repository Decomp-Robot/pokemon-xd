#ifndef MENUCBBATTLESTART_H
#define MENUCBBATTLESTART_H

#include <types.h>

/* One trainer face the battle-start screen shows: `group` is the resource
   read into, `resID` the resource that was asked for. */
typedef struct MenuCBBattleFace {
    /* 0x00 */ u32 group;
    /* 0x04 */ u32 resID;
} MenuCBBattleFace;

/* Battle-start screen work. The fight-side presentation code reads the faces
   from here and waits until faceIndex catches up with faceNum. */
typedef struct MenuCBBattleWork {
    /* 0x000 */ s32 mode;
    /* 0x004 */ s32 status;
    /* 0x008 */ u8 _008[0x00C - 0x008];
    /* 0x00C */ MenuCBBattleFace face[4];
    /* 0x02C */ s32 faceIndex;
    /* 0x030 */ s32 faceNum;
    /* 0x034 */ u8 readDone;
    /* 0x035 */ u8 _035[0x124 - 0x035];
    /* 0x124 */ u32 soundID;
    /* 0x128 */ u8 _128[0x12C - 0x128];
    /* 0x12C */ u16 trainerName[12];
    /* 0x144 */ s32 trainerClassNameID;
} MenuCBBattleWork;

extern MenuCBBattleWork menuCBBattleStartWork;

#endif
