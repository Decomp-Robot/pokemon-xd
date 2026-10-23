#ifndef MENUCBBATTLESTART_H
#define MENUCBBATTLESTART_H

#include <menuModel.h>
#include <types.h>
#include <menuModel.h>

/* One trainer face the battle-start screen shows: `resource` is the loaded resource,
   `resID` the resource that was asked for. */
typedef struct MenuCBBattleFace {
    /* 0x00 */ void* resource;
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
    /* 0x035 */ u8 _035[0x038 - 0x035];
    /* 0x038 */ MENU_MODEL model;
    /* 0x124 */ u32 soundID;
    /* 0x128 */ u8 _128[0x12C - 0x128];
    /* 0x12C */ u16 trainerName[12];
    /* 0x144 */ s32 trainerClassNameID;
} MenuCBBattleWork;

extern MenuCBBattleWork _menuCBBattleStartWork;

#ifdef __cplusplus
extern "C" {
#endif
s32 menuCBBattleStartGetStatus(void);
u8 menuCB_BattleStartSoundCheck(void);
#ifdef __cplusplus
}
#endif

/* Battle types the link battles run in, as toolentryTaisenGetBattleType
   reports them. _menuCBBattleStartSetIndex dispatches 0 to the double-battle
   order, 1 to the single-battle order and 2 to the multi-battle order. */
enum BattleType {
    BATTLE_TYPE_DOUBLE = 0,
    BATTLE_TYPE_SINGLE = 1,
    BATTLE_TYPE_MULTI = 2
};

enum MenuCBBattleStartStatus {
    MENU_CB_BATTLE_START_PARTY = 0,
    MENU_CB_BATTLE_START_ENTERED_PARTY = 1
};

#ifdef __cplusplus
extern "C" {
#endif

s32 menuCBBattleStartGetStatus(void);
void menuCBBattleStartInit(s32 mode, s32 status);
void menuCBBattleStartSetStatus(s32 status);

#ifdef __cplusplus
}
#endif


#endif
