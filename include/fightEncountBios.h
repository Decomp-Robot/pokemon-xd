#ifndef FIGHT_ENCOUNT_BIOS_H
#define FIGHT_ENCOUNT_BIOS_H

#include <fade.h>
#include <types.h>

/* Battle wipes, indexed by the wipe id an encounter record selects. */
enum FIGHT_WIPE_ID {
    FIGHT_WIPE_ID_FADEOUT_IN = 0,
    FIGHT_WIPE_ID_YOKO = 1,
    FIGHT_WIPE_ID_TATE = 2,
    FIGHT_WIPE_ID_BALL = 3,
    FIGHT_WIPE_ID_BOSS = 4,
    FIGHT_WIPE_ID_CARDE = 5,
    FIGHT_WIPE_ID_YOKO_OR_TATE = 6,
    FIGHT_WIPE_ID_YOKO_OR_TATE_OR_BALL = 7,
    FIGHT_WIPE_ID_FADEOUT_IN_2 = 8, /* fades out like FIGHT_WIPE_ID_FADEOUT_IN */
    FIGHT_WIPE_ID_TRAINER = 9,
    FIGHT_WIPE_ID_FADEIN = 10,
    FIGHT_WIPE_ID_DISTORTION = 11,
    FIGHT_WIPE_ID_TEAR_OFF = 12,
    FIGHT_WIPE_ID_BOSS_SECOND = 13,
    FIGHT_WIPE_ID_BOUND_BALL = 14,
    FIGHT_WIPE_ID_BALL_SECOND = 15,
    FIGHT_WIPE_ID_BLUR_BALL = 16,
    FIGHT_WIPE_ID_DIST_BALL = 17,
    FIGHT_WIPE_ID_DIST_ZOOM_IN = 18,
    FIGHT_WIPE_ID_DIST_ZOOM_OUT = 19,
    FIGHT_WIPE_ID_DIST_YOKO = 20,
    FIGHT_WIPE_ID_DISTORTION_STRIP_TATE = 21,
    FIGHT_WIPE_ID_DISTORTION_STRIP_YOKO = 22,
    FIGHT_WIPE_ID_FOUR_BALL = 23,
    FIGHT_WIPE_ID_SLIDE_BALL = 24,
    FIGHT_WIPE_ID_TRAINER_SECOND = 25
};

/* One trainer entry of an encounter record. The first word packs the deck
 * group the side fights with (upper half) and the trainer data it plays
 * (lower half); the second word holds the controller its player uses. */
typedef struct {
    u32 mDeckGrpAndTrainerDataId;
    u32 mGSInputDevice;
} FightEncountTrainerInfo;

/* One record of the encounter data table: the fight it sets up and the four
 * trainers that take part in it. */
typedef struct {
    u8 mFightKind;
    u8 mTrainer;
    u8 mPokemon;
    u8 mEntry;
    u8 mZenmetuFlag;
    u16 mFightFloorDataId;
    u16 mDiskId; /* 0 uses the trainer entry's own deck group, nonzero the battle disk's */
    u32 mFightName;
    u32 mBgmSndId;
    enum FIGHT_WIPE_ID mWipeId;
    u32 mSyoukaiWzxDataId;
    FightEncountTrainerInfo mTrainerInfo[4];
} FightEncountRecord;

/* One battle wipe transition: whether it keeps the picture as a snapshot, the
 * sound effect played with it, how long it runs and the fade effect that draws
 * it. */
typedef struct {
    u8 mSnapshotUse;
    u16 mEffectSndID;
    f32 mTime;
    FadeFunction mFunction;
} FightEncountWipeData;

/* Deck group the two sides of a battle disk fight with. */
enum FIGHT_DECK_GRP_ID {
    FIGHT_DECK_GRP_ID_BATTLE_DISK = 4
};

/* Fight type an encounter record is reported as when no fight type record
 * matches its trainer, fightout and entry Pokemon counts. */
enum FIGHT_TYPE_ID {
    FIGHT_TYPE_ID_FALLBACK = 3
};

/* Sound effect played with a battle wipe transition, FIGHT_ENCOUNT_WIPE_SND_ID_NONE
 * for a transition that runs silently. */
enum FIGHT_ENCOUNT_WIPE_SND_ID {
    FIGHT_ENCOUNT_WIPE_SND_ID_NONE = 0,
    FIGHT_ENCOUNT_WIPE_SND_ID_DEFAULT = 34
};

/* The transition table battle wipes select from and the number of transitions
 * it holds. */
extern FightEncountWipeData fight_encount_wipe_data[];
extern u32 fight_encount_wipe_data_number;

#ifdef __cplusplus
extern "C" {
#endif

u16 fightEncountDataBiosGetDiskId(FightEncountRecord *data);
u8 fightEncountDataBiosGetZenmetuFlag(FightEncountRecord *data);
u32 fightEncountDataBiosGetBgmSndId(FightEncountRecord *data);
enum FIGHT_WIPE_ID fightEncountDataBiosGetWipeId(FightEncountRecord *data);
FightEncountWipeData *fightEncountWipeDataBiosGetPtr(enum FIGHT_WIPE_ID wipeId);
u16 fightEncountDataBiosGetWipeEffectSndID(FightEncountWipeData *wipeData);
f32 fightEncountDataBiosGetWipeEffectTime(FightEncountWipeData *wipeData);
#ifdef __cplusplus
bool fightEncountDataBiosGetWipeSnapshotUse(FightEncountWipeData *wipeData);
#else
u8 fightEncountDataBiosGetWipeSnapshotUse(FightEncountWipeData *wipeData);
#endif
FadeFunction fightEncountDataBiosGetWipeFunction(FightEncountWipeData *wipeData);
FightEncountRecord *fightEncountDataBiosGetPtr(u16 encountNo);
u32 fightEncountDataBiosGetFightKind(FightEncountRecord *data);
u8 fightEncountDataBiosGetFightType(FightEncountRecord *data);
void fightEncountDataBiosSetFightKind(FightEncountRecord *data, u8 value);
u32 fightEncountDataBiosGetTrainer(FightEncountRecord *data);
void fightEncountDataBiosSetTrainer(FightEncountRecord *data, u8 value);
u32 fightEncountDataBiosGetPokemon(FightEncountRecord *data);
void fightEncountDataBiosSetPokemon(FightEncountRecord *data, u8 value);
u32 fightEncountDataBiosGetEntry(FightEncountRecord *data);
void fightEncountDataBiosSetEntry(FightEncountRecord *data, u8 value);
u16 fightEncountDataBiosGetFightFloorDataId(FightEncountRecord *data);
void fightEncountDataBiosSetFightFloorDataId(FightEncountRecord *data, u16 value);
u16 fightEncountDataBiosGetFightTrainerDataId(FightEncountRecord *data, u8 player);
void fightEncountDataBiosSetFightTrainerDataId(FightEncountRecord *data, u8 player, u16 trainerDataId);
FightEncountTrainerInfo *fightEncountDataBiosGetFightTrainerInfoPtr(FightEncountRecord *data, u8 player);
u32 fightEncountDataBiosGetGSInputDevice(FightEncountRecord *data, u8 player);
void fightEncountDataBiosSetGSInputDevice(FightEncountRecord *data, u8 player, s32 devId);
void fightEncountDataBiosSetDiskId(FightEncountRecord *data, s32 id);
u16 fightEncountDataBiosGetFightDeckGrpId(FightEncountRecord *data, u8 player);
u32 fightEncountDataBiosGetFightName(FightEncountRecord *data);

extern u32 *fight_encount_data_number;

void fightEncountDataBiosSetSyoukaiWzxDataId(FightEncountRecord *data, u32 value);
void fightEncountDataBiosSetBgmSndId(FightEncountRecord *data, u32 value);
void fightEncountDataBiosSetFightType(FightEncountRecord *data, u8 value);
#ifdef __cplusplus
}
#endif

#endif
