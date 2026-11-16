#include <fightEncount.h>
#include <fightEncountBios.h>
#include <fightEncountData.h>
#include <fightTrainer.h>
#include <GSflag.h>
#include <GSmsg.h>
#include <hero.h>
#include <heroBios.h>
#include <hsdRandom.h>
#include <menuCBBattle.h>
#include <menuCBBattleStart.h>
#include <menuCBBios.h>
#include <menuTitle.h>
#include <sound.h>

enum {
    LINK_BATTLE = 0,
    VS_BATTLE = 1
};

enum {
    LINK_DOUBLE_ENCOUNT = 5,
    LINK_SINGLE_ENCOUNT = 7,
    LINK_MULTI_ENCOUNT = 8
};

enum {
    LINK_FIGHT_KIND = 11,
    VS_FIGHT_KIND = 17,
    VS_DISTINCT_TRAINERS_FIGHT_KIND = 10
};

enum {
    LINK_DOUBLE_FIGHT_TYPE = 23,
    LINK_SINGLE_FIGHT_TYPE = 24,
    LINK_MULTI_FIGHT_TYPE = 36,
    VS_FIGHT_TYPE = 16
};

enum { RANDOM_BATTLE_FLOOR = 6 };
enum { FIGHT_ENCOUNT_SKIP_RETURN_FADE_FLAG = 803 };
enum { VS_PLAYER_TRAINER_DATA_ID = 5000 };
enum { VS_FIRST_RANK_BGM = 1485 };

enum {
    VS_RANK_RANGE = 0,
    VS_PLAYER_RANK_BASE = 1,
    VS_OPPONENT_RANK_BASE = 2
};

enum {
    VS_RANK_GROUP_SIZE = 5,
    VS_ALT_RANK_GROUP_MASK = 0x80
};

static const u16 VS_TRAINER_RANK_NUMBER[] = {
        1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 38, 39, 40
};
static const s8 VS_TRAINER_RANK_SELECT[4][2][3] = {
        { { 5, 30, 35 }, { 5, 30, 30 } },
        { { 5, 20, 25 }, { 5, 20, 20 } },
        { { 5, 10, 15 }, { 5, 10, 10 } },
        { { 5, 0, 5 }, { 5, 0, 0 } }
};

static const u16 VS_TRAINER_ENCOUNT_ID[] = {
        15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46,
        47, 48, 49, 50, 51, 52, 53, 54
};
static const u32 VS_TRAINER_BGM[3] = { 1266, 1266, 1266 };
static const u32 LINK_BATTLE_BGM[3] = { 1267, 1267, 1267 };
static const u16 LINK_BATTLE_FLOOR[6] = { 36, 38, 37, 60, 61, 62 };
static const u16 VS_TRAINER_FLOOR[6] = { 36, 38, 37, 60, 61, 62 };

static const u16 _menuCBVsHeroNameMsgId[3] = { 15136, 15137, 15138 };

void _menuCBBattle_DeleteAllItem(Hero *hero);

static inline const u16 menuCBBattleGetVsEncountId(u16 trainerIndex)
{
    return VS_TRAINER_ENCOUNT_ID[trainerIndex];
}

static inline u16 menuCBBattleSelectFloorDataId(_MENUCBWORK *work)
{
    u16 floorDataId;

    if (_CB.connectMode != VS_BATTLE) {
        if (_CB.battleStageMode == RANDOM_BATTLE_FLOOR) {
            work->mVsStage = (u16)HSD_Rand() % 6;
        }
        floorDataId = LINK_BATTLE_FLOOR[work->mVsStage];
    } else {
        work->mVsStage = (u16)HSD_Rand() % 6;
        floorDataId = VS_TRAINER_FLOOR[work->mVsStage];
    }

    return floorDataId;
}

s32 menuCB_Battle(_MENUCBWORK *work)
{
    u16 encountId;
    u16 result;

    if (_CB.connectMode == VS_BATTLE) {
        encountId = menuCBBattleGetVsEncountId(work->mTrainer[1].mVsTrainerIndex);
    } else {
        switch (work->battleMode) {
        case BATTLE_TYPE_DOUBLE:
            encountId = LINK_DOUBLE_ENCOUNT;
            break;
        case BATTLE_TYPE_SINGLE:
            encountId = LINK_SINGLE_ENCOUNT;
            break;
        case BATTLE_TYPE_MULTI:
            encountId = LINK_MULTI_ENCOUNT;
            break;
        }
    }

    GSflagOff(FIGHT_ENCOUNT_SKIP_RETURN_FADE_FLAG);
    soundStop(soundGetBGMID(), 0);
    menuTitleBattleSelectFree();
    menuCB_SetToolBattleModeFlag(1);
    result = (u16)fightEncount(encountId);
    menuCB_SetToolBattleModeFlag(0);
    menuTitleReadWazaSelectMenu();

    return result;
}

s32 menuCB_InitBattle(_MENUCBWORK *work)
{
    Hero hero;
    FightEncountRecord *encountData;
    u16 encountId = 0;
    u16 trainerIndex;
    int opponentTrainerDataId;
    int playerTrainerDataId;
    u16 playerRankIndex;
    u32 bgmSndId;
    u16 floorDataId;
    u16 rankRange;
    int playerRankBase;
    u16 opponentRankBase;
    u16 *nameSource;
    u16 *nameDest;

    if (_CB.mVsHeroNameIndex == -1) {
        rankRange = VS_TRAINER_RANK_SELECT[work->mVsRank][work->mVsBattleKind][VS_RANK_RANGE];
        playerRankBase = VS_TRAINER_RANK_SELECT[work->mVsRank][work->mVsBattleKind][VS_PLAYER_RANK_BASE];
        opponentRankBase = VS_TRAINER_RANK_SELECT[work->mVsRank][work->mVsBattleKind][VS_OPPONENT_RANK_BASE];
        if (work->mVsBattleKind == 0) {
            work->mTrainer[0].mVsTrainerIndex = playerRankBase + (u16)HSD_Rand() % rankRange;
            work->mTrainer[1].mVsTrainerIndex = opponentRankBase + (u16)HSD_Rand() % rankRange;
        } else {
            work->mTrainer[0].mVsTrainerIndex = playerRankBase + (u16)HSD_Rand() % rankRange;
            do {
                work->mTrainer[1].mVsTrainerIndex = opponentRankBase + (u16)HSD_Rand() % rankRange;
            } while (work->mTrainer[0].mVsTrainerIndex == work->mTrainer[1].mVsTrainerIndex);
            if ((u16)HSD_Rand() & VS_ALT_RANK_GROUP_MASK) {
                work->mTrainer[0].mVsTrainerIndex += VS_RANK_GROUP_SIZE;
                work->mTrainer[1].mVsTrainerIndex += VS_RANK_GROUP_SIZE;
            }
        }
    }

    if (_CB.connectMode == VS_BATTLE) {
        encountId = menuCBBattleGetVsEncountId(work->mTrainer[1].mVsTrainerIndex);
    } else {
        switch (work->battleMode) {
        case BATTLE_TYPE_DOUBLE:
            encountId = LINK_DOUBLE_ENCOUNT;
            break;
        case BATTLE_TYPE_SINGLE:
            encountId = LINK_SINGLE_ENCOUNT;
            break;
        case BATTLE_TYPE_MULTI:
            encountId = LINK_MULTI_ENCOUNT;
            break;
        }
    }

    encountData = fightEncountDataBiosGetPtr(encountId);

    work->mVsBgmIndex++;
    if (_CB.connectMode == VS_BATTLE) {
        work->mVsBgmIndex %= 3;
        bgmSndId = VS_TRAINER_BGM[work->mVsBgmIndex];
        if (work->mVsRank == 0) {
            bgmSndId = VS_FIRST_RANK_BGM;
        }
    } else {
        work->mVsBgmIndex %= 3;
        bgmSndId = LINK_BATTLE_BGM[work->mVsBgmIndex];
    }
    fightEncountDataBiosSetBgmSndId(encountData, bgmSndId);

    floorDataId = menuCBBattleSelectFloorDataId(work);
    fightEncountDataBiosSetFightFloorDataId(encountData, floorDataId);

    switch (work->connectMode) {
    case LINK_BATTLE:
        fightEncountDataBiosSetFightKind(encountData, LINK_FIGHT_KIND);
        fightEncountDataBiosSetSyoukaiWzxDataId(encountData, 0);
        break;
    case VS_BATTLE:
    default:
        if (work->mVsBattleKind == 0) {
            fightEncountDataBiosSetFightKind(encountData, VS_FIGHT_KIND);
        } else {
            fightEncountDataBiosSetFightKind(encountData, VS_DISTINCT_TRAINERS_FIGHT_KIND);
        }
        fightEncountDataBiosSetSyoukaiWzxDataId(encountData, 0);
        break;
    }

    switch (work->connectMode) {
    case LINK_BATTLE:
        switch (work->battleMode) {
        case BATTLE_TYPE_SINGLE:
            fightEncountDataBiosSetFightType(encountData, LINK_SINGLE_FIGHT_TYPE);
            break;
        case BATTLE_TYPE_MULTI:
            fightEncountDataBiosSetFightType(encountData, LINK_MULTI_FIGHT_TYPE);
            break;
        case BATTLE_TYPE_DOUBLE:
        default:
            fightEncountDataBiosSetFightType(encountData, LINK_DOUBLE_FIGHT_TYPE);
            break;
        }
        break;
    case VS_BATTLE:
    default:
        fightEncountDataBiosSetFightType(encountData, VS_FIGHT_TYPE);
        break;
    }

    switch (work->connectMode) {
    case VS_BATTLE:
        trainerIndex = work->mTrainer[1].mVsTrainerIndex;
        playerRankIndex = work->mTrainer[0].mVsTrainerIndex;
        playerTrainerDataId = VS_TRAINER_RANK_NUMBER[playerRankIndex];
        opponentTrainerDataId = VS_TRAINER_RANK_NUMBER[trainerIndex];
        menuCBBios_InitTrainerFromDataID(&work->mTrainer[1], opponentTrainerDataId, encountId);
        work->mTrainer[1].mDataId = opponentTrainerDataId;
        work->mTrainer[1].mVsTrainerIndex = trainerIndex;
        menuCBTrainerBios_SetControlerID(&work->mTrainer[1], _CB.nameEntryMode);
        _menuCBBattle_DeleteAllItem(&work->mTrainer[1].mHero);
        _menuCBBattle_DeleteAllItem(&work->mTrainer[1].mBattleHero);
        if (_CB.mVsHeroNameIndex == -1) {
            _CB.mVsHeroNameIndex = (u32)(u16)HSD_Rand() % 3;
            nameSource = GSmsgGetStringPtr(_menuCBVsHeroNameMsgId[_CB.mVsHeroNameIndex]);
            nameDest = _CB.mVsHeroName;
            while (*nameSource != 0) {
                *nameDest = *nameSource;
                nameSource++;
                nameDest++;
            }
            *nameDest = 0;
        }
        fightTrainerCreateFightTrainerDataIdToHero(playerTrainerDataId,
            menuCBBios_PortIDtoControlerID(0), &hero, encountId);
        if (_CB.nameEntryMode == 0) {
            heroBiosSetNamePtr(&hero, _CB.mVsHeroName);
        }
        _menuCBBattle_DeleteAllItem(&hero);
        trainerIndex = work->mTrainer[0].mVsTrainerIndex;
        menuCBBios_InitTrainerFromHero(&work->mTrainer[0], &hero, VS_PLAYER_TRAINER_DATA_ID);
        work->mTrainer[0].mVsTrainerIndex = trainerIndex;
        if (_CB.nameEntryMode != 0) {
            work->mTrainer[0].mDataId = playerTrainerDataId;
        }
        menuCBTrainerBios_SetControlerID(&work->mTrainer[0],
            menuCBBios_PortIDtoControlerID(0));
        menuCBTrainerBios_SetHomePlace(&work->mTrainer[0], 0);
        break;
    case LINK_BATTLE:
        switch (work->battleMode) {
        case BATTLE_TYPE_DOUBLE:
        case BATTLE_TYPE_SINGLE:
            work->mTrainerImmediately[0].mVersusBattleId = 0;
            work->mTrainerImmediately[1].mVersusBattleId = 1;
            work->mTrainerImmediately[2].mVersusBattleId = 2;
            work->mTrainerImmediately[3].mVersusBattleId = 3;
            menuCBBios_CopyTrainer(&work->mTrainer[0], &work->mTrainerImmediately[0]);
            menuCBBios_CopyTrainer(&work->mTrainer[1], &work->mTrainerImmediately[1]);
            break;
        case BATTLE_TYPE_MULTI:
            menuCBBios_CopyTrainer(&work->mTrainer[0],
                &work->mTrainerImmediately[work->mTrainerImmediately[0].mVersusBattleId]);
            menuCBBios_CopyTrainer(&work->mTrainer[1],
                &work->mTrainerImmediately[work->mTrainerImmediately[1].mVersusBattleId]);
            menuCBBios_CopyTrainer(&work->mTrainer[2],
                &work->mTrainerImmediately[work->mTrainerImmediately[2].mVersusBattleId]);
            menuCBBios_CopyTrainer(&work->mTrainer[3],
                &work->mTrainerImmediately[work->mTrainerImmediately[3].mVersusBattleId]);
            break;
        }
        break;
    }

    switch (work->battleMode) {
    case BATTLE_TYPE_MULTI:
        fightEncountDataBiosSetFightTrainerDataId(encountData, 2, work->mTrainer[2].mDataId);
        fightEncountDataBiosSetFightTrainerDataId(encountData, 3, work->mTrainer[3].mDataId);
        fightEncountDataBiosSetGSInputDevice(encountData, 2, work->mTrainer[2].mControlerID);
        fightEncountDataBiosSetGSInputDevice(encountData, 3, work->mTrainer[3].mControlerID);
        /* fall through */
    case BATTLE_TYPE_DOUBLE:
    case BATTLE_TYPE_SINGLE:
        fightEncountDataBiosSetFightTrainerDataId(encountData, 0, work->mTrainer[0].mDataId);
        fightEncountDataBiosSetFightTrainerDataId(encountData, 1, work->mTrainer[1].mDataId);
        fightEncountDataBiosSetGSInputDevice(encountData, 0, work->mTrainer[0].mControlerID);
        fightEncountDataBiosSetGSInputDevice(encountData, 1, work->mTrainer[1].mControlerID);
        break;
    }

    return 0;
}

void _menuCBBattle_DeleteAllItem(Hero *hero)
{
}
