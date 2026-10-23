#include <HSD_Rand.h>
#include <GSfsys.h>
#include <GSres.h>
#include <GSsnd.h>
#include <sound.h>
#include <types.h>
#include <GSmsg.h>
#include <hero.h>
#include <toolentry.h>
#include <menuCBBattleStart.h>

/*
 * menuCB_BattleStart.cpp
 *
 * The battle-start screen of a link (GBA) battle: the players pick which of
 * the Pokemon they entered fight, and the screen shows the trainer faces of
 * the players that are in the battle.
 */

/* -------------------------------------------------------------------------
 * Data of the screen
 * ---------------------------------------------------------------------- */

// Battle-start screen state.
MenuCBBattleWork menuCBBattleStartWork;

/* Pokemon order table the battle-start screen shows. */
s32 menuCBBattleStartDispWork[4][6];
MenuCBBattleWork _menuCBBattleStartWork;

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
s32 toolentryTaisenGetTrainerDataID(s32 index);
u16 toolentryTaisenGetEntryPokemonNum(s32 index);
s32 toolentryGetTrainerBicFaceResID(s32 index, s32 big);
s32 toolentryGetTrainerSamllFaceResID(s32 index, s32 small);
s32 fightTrainerKindDataBiosGetPrefixName(void *kindData);

void menuCBPokemonEntryTexWorkInit(void);
void menuCBPokemonEntryLoadTex(void);
s32 fightTrainerDB_GetKindDataId(s32 dataID);
void *fightTrainerKindDataBiosGetPtr(u8 kindID);
void GSresFreeResource(u32 group, u32 resID);
}

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

/* State of the battle-start screen. */
extern "C" void menuCBBattleStartInit(s32 mode, s32 status);
extern "C" void menuCBBattleStartSetStatus(s32 status);
extern "C" void menuCBBattleStartDispTrainerInit(void);
extern "C" void menuCBBattleStartTrainerLoadTex(void);
extern "C" void menuCBBattleStartTrainerFaceFree(void);

void _menuCBBattleStartSetIndexSingle(long index, unsigned short pokeNum);

void _menuCBBattleStartSetIndex(void);
void _menuCBBattleStartSetIndexDouble(long index, unsigned short pokeNum);
void _menuCBBattleStartSetIndexMulti(long index, unsigned short pokeNum);
void _menuCBBattleStartWorkInit(void);
void _menuCBBattleStartSetTrainerData(void);
u8 _menuCBBattleCheckTrainer(long work, long mode);
s32 _menuCBBattleStartGetTrainerFaceSize(void);
void _menuCBBattleStartDispTrainerTexCallBack(long id, void *context, long arg);

/* -------------------------------------------------------------------------
 * Battle start screen
 * ---------------------------------------------------------------------- */

/*
 * Fill menuCBBattleStartDispWork with the order the entered Pokemon are shown in. */
void _menuCBBattleStartSetIndex(void)
{
    s32 battleType = toolentryTaisenGetBattleType();
    u16 pokeNum[4] = {6, 6, 6, 6};
    s32 i;

    _menuCBBattleStartWorkInit();

    switch (_menuCBBattleStartWork.status) {
    case 0:
        for (i = 0; i < 4; i++) {
            pokeNum[i] = toolentryTaisenGetPokemonNum(i);
        }
        break;
    case 1:
        for (i = 0; i < 4; i++) {
            pokeNum[i] = toolentryTaisenGetEntryPokemonNum(i);
        }
        break;
    }

    switch (battleType) {
    case 0:
        for (i = 0; i < 2; i++) {
            _menuCBBattleStartSetIndexSingle(i, pokeNum[i]);
        }
        break;
    case 1:
        for (i = 0; i < 2; i++) {
            _menuCBBattleStartSetIndexDouble(i, pokeNum[i]);
        }
        break;
    case 2:
        for (i = 0; i < 4; i++) {
            _menuCBBattleStartSetIndexMulti(i, pokeNum[i]);
        }
        break;
    }
}

/*
 * Order a battle slot is shown in: one row per party size; each row gives the
 * order in which that party's Pokemon are shown.
 */
typedef struct MenuCBBattleOrder {
    s32 row[6][6];
} MenuCBBattleOrder;

/* Single battle order table. */
static const MenuCBBattleOrder BattleOrderSingle = { {
    { 1, 2, 0, 3, 4, 5 },
    { 2, 0, 1, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
} };

/* Double battle order table. */
static const MenuCBBattleOrder BattleOrderDouble = { {
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 4, 0, 1, 5, 2, 3 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
} };

/* Double battle: the Pokemon in slot `index` is shown first, the rest follow
   in the order of the battle table, picked by the Pokemon's party slot. */
void _menuCBBattleStartSetIndexDouble(long index, unsigned short pokeNum)
{
    s32 *slot = menuCBBattleStartDispWork[index];
    MenuCBBattleOrder order = BattleOrderDouble;
    const s32 *row = order.row[pokeNum - 1];
    s32 i;

    for (i = 0; i < 6; i++) {
        slot[i] = row[i];
    }
}

/* Same as _menuCBBattleStartSetIndexDouble for the single battle table. */
void _menuCBBattleStartSetIndexSingle(long index, unsigned short pokeNum)
{
    s32 *slot = menuCBBattleStartDispWork[index];
    MenuCBBattleOrder order = BattleOrderSingle;
    const s32 *row = order.row[pokeNum - 1];
    s32 i;

    for (i = 0; i < 6; i++) {
        slot[i] = row[i];
    }
}

/* Multi battle: the Pokemon are shown in the order they were entered. */
void _menuCBBattleStartSetIndexMulti(long index, unsigned short pokeNum)
{
    u16 i;
    s32 *slot = menuCBBattleStartDispWork[index];

    for (i = 0; i < pokeNum; i++) {
        slot[i] = i;
    }
    for (i = pokeNum; i < 6; i++) {
        slot[i] = pokeNum++;
    }
}

/* Reset the display order to the identity order. */
void _menuCBBattleStartWorkInit(void)
{
    s32 i;
    s32 j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 6; j++) {
            menuCBBattleStartDispWork[i][j] = j;
        }
    }
}

/* Non-zero while the battle-start jingle is still playing. */
extern "C" u8 menuCB_BattleStartSoundCheck(void)
{
    u32 status = GSsndGetStatus(_menuCBBattleStartWork.soundID);

    return status != 2;
}

extern "C" void menuCBBattleStartSetStatus(s32 status)
{
    _menuCBBattleStartWork.status = status;
}

extern "C" s32 menuCBBattleStartGetStatus(void)
{
    return _menuCBBattleStartWork.status;
}

/* Read the name and the class name of the trainer the battle-start screen
   shows. */
void _menuCBBattleStartSetTrainerData(void)
{
    s32 mode = toolentryTaisenGetBattleType();

    if (_menuCBBattleCheckTrainer(1, mode) != 0) {
        const u16 *trainerName = heroBiosGetNamePtr(toolentryTaisenGetHeroPtr(1));
        u16 *name = _menuCBBattleStartWork.trainerName;

        if (name != 0) {
            GScharCpy(name, trainerName);
        } else {
            GScharCpy(name, GSmsgGetStringPtr(1));
        }
        _menuCBBattleStartWork.trainerClassNameID = fightTrainerKindDataBiosGetPrefixName(
            fightTrainerKindDataBiosGetPtr(fightTrainerDB_GetKindDataId(
                toolentryTaisenGetTrainerDataID(1))));
    }
}

/*
 * Non-zero when the trainer of player 1 has to be shown for the given battle
 * mode: versus battles show the other player's trainer, the other modes show
 * the player's own.
 */
u8 _menuCBBattleCheckTrainer(long work, long mode)
{
    u8 show = 1;
    s32 type = toolentryTaisenGetBattleType();

    if (mode == 2) {
        if (type != 2) {
            show = 0;
        }
    } else {
        if (type == 2) {
            show = 0;
        }
    }
    return show;
}

/* Battle start screen: load the background, pick the jingle and show the
   trainers of the players that are in the battle. */
extern "C" void menuCBBattleStartInit(s32 mode, s32 status)
{
    u16 rnd;

    _menuCBBattleStartSetTrainerData();
    _menuCBBattleStartWork.mode = mode;
    _menuCBBattleStartWork.status = status;
    _menuCBBattleStartSetIndex();

    switch (status) {
    case 0:
        menuCBPokemonEntryTexWorkInit();
        menuCBPokemonEntryLoadTex();
        rnd = (u16)HSD_Rand();
        switch (rnd % 3) {
        case 0:
            _menuCBBattleStartWork.soundID = 0x3D2;
            break;
        case 1:
            _menuCBBattleStartWork.soundID = 0x3F5;
            break;
        case 2:
            _menuCBBattleStartWork.soundID = 0x3F6;
            break;
        case 3:
        default:
            _menuCBBattleStartWork.soundID = 0x3F7;
            break;
        }
        soundOpenPlay(_menuCBBattleStartWork.soundID, 0, 0xFF);
        break;
    case 1:
        menuCBPokemonEntryTexWorkInit();
        menuCBPokemonEntryLoadTex();
        _menuCBBattleStartWork.soundID = 0x5DB;
        soundOpenPlay(0x5DB, 0, 0xFF);
        break;
    }
}

/* Arm the trainer face loading. */
extern "C" void menuCBBattleStartDispTrainerInit(void)
{
    s32 i;

    MenuCBBattleWork *work = &_menuCBBattleStartWork;

    work->readDone = 0;
    work->faceIndex = 0;
    if (_menuCBBattleStartGetTrainerFaceSize() == 0) {
        _menuCBBattleStartWork.faceNum = 4;
    } else {
        _menuCBBattleStartWork.faceNum = 2;
    }
    for (i = 0; i < 4; i++) {
        work->face[i].resID = 0;
        work->face[i].resource = 0;
    }
}

/* Start reading the trainer faces. */
extern "C" void menuCBBattleStartTrainerLoadTex(void)
{
    _menuCBBattleStartGetTrainerFaceSize();
    _menuCBBattleStartWork.readDone = 1;
    _menuCBBattleStartDispTrainerTexCallBack(0, 0, 0);
}

/* Non-zero when the battle shows the small trainer faces; the versus battle
   shows the big ones. */
s32 _menuCBBattleStartGetTrainerFaceSize(void)
{
    return toolentryTaisenGetBattleType() != 2;
}

/*
 * Read callback for the trainer faces: handles one face per call and stops
 * once as many faces as the battle needs have been started.
 */
void _menuCBBattleStartDispTrainerTexCallBack(long id, void *context, long arg)
{
    MenuCBBattleFace *face;
    long resID = id;
    s32 running = 1;

    if (context != 0) {
        ((MenuCBBattleFace *)context)->resource =
            GSresGetResourceAllGroup(((MenuCBBattleFace *)context)->resID);
    }

    /* One face per call: read the next one the battle needs, then stop when the
       read has been started or when there is no face left to read. */
    do {
        if (_menuCBBattleStartWork.faceIndex == _menuCBBattleStartWork.faceNum) {
            running = 0;
        } else {
            if (_menuCBBattleStartGetTrainerFaceSize() != 0) {
                if (_menuCBBattleStartWork.faceIndex % 2 != 0) {
                    resID = toolentryGetTrainerBicFaceResID(1, 0);
                } else {
                    resID = toolentryGetTrainerBicFaceResID(0, 1);
                }
            } else {
                if (_menuCBBattleStartWork.faceIndex < 2) {
                    resID = toolentryGetTrainerSamllFaceResID(
                        _menuCBBattleStartWork.faceIndex, 1);
                } else {
                    resID = toolentryGetTrainerSamllFaceResID(
                        _menuCBBattleStartWork.faceIndex, 0);
                }
            }
            if (_menuCBBattleStartGetTrainerFaceSize() != 0) {
                arg = 0x5C3;
            } else {
                arg = 0x5C4;
            }
            face = &_menuCBBattleStartWork.face[_menuCBBattleStartWork.faceIndex];
            face->resID = resID;
            face->resource = GSresGetResourceAllGroup(face->resID);
            if (face->resource != 0) {
                _menuCBBattleStartWork.faceIndex++;
            } else {
                GSfsysReadIDEX(arg, resID,
                               _menuCBBattleStartDispTrainerTexCallBack, face,
                               resID);
                running = 0;
                _menuCBBattleStartWork.faceIndex++;
            }
        }
    } while (running != 0);
}

/* Free the trainer faces and arm them again. */
extern "C" void menuCBBattleStartTrainerFaceFree(void)
{
    MenuCBBattleFace *face;
    unsigned long group;
    s32 i;

    if (_menuCBBattleStartGetTrainerFaceSize() != 0) {
        group = 0x5C3;
    } else {
        group = 0x5C4;
    }
    GSfsysFree(group);
    GSresFreeGroup(group);

    face = &_menuCBBattleStartWork.face[0];
    if (_menuCBBattleStartGetTrainerFaceSize() == 0) {
        _menuCBBattleStartWork.faceNum = 4;
    } else {
        _menuCBBattleStartWork.faceNum = 2;
    }
    for (i = 0; i < _menuCBBattleStartWork.faceNum; i++) {
        GSresFreeResource(group, face->resID);
        face++;
    }
    menuCBBattleStartDispTrainerInit();
}

