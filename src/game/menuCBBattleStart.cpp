#include <types.h>

/*
 * menuCBBattleStart.cpp
 *
 * The battle-start screen of a link (GBA) battle: the players pick which of
 * the Pokemon they entered fight, and the screen shows the trainer faces of
 * the players that are in the battle.
 *
 * The module is a set of free functions. The entry points the menu system
 * calls are C functions; the helpers the screen code keeps to itself are C++
 * (which is why their linkage names are mangled).
 */

/* -------------------------------------------------------------------------
 * Work areas
 * ---------------------------------------------------------------------- */

/* One trainer face the battle-start screen shows: `group` is the resource
   read into, `resID` the resource that was asked for. */
typedef struct MenuCBBattleFace {
    /* 0x00 */ u32 group;
    /* 0x04 */ u32 resID;
} MenuCBBattleFace;

/* Battle-start screen work. The layout is shared with the battle animation
   code, which reads the faces and waits for the read flag. */
typedef struct MenuCBBattleWork {
    /* 0x000 */ s32 mode;
    /* 0x004 */ s32 status;
    /* 0x008 */ s32 unk_008;
    /* 0x00C */ MenuCBBattleFace face[4];
    /* 0x02C */ s32 faceIndex;
    /* 0x030 */ s32 faceNum;
    /* 0x034 */ u8 readDone;
    /* 0x035 */ u8 unk_035[0x124 - 0x35];
    /* 0x124 */ u32 soundID;
    /* 0x128 */ u8 unk_128[0x12C - 0x128];
    /* 0x12C */ u16 trainerName[12];
    /* 0x144 */ s32 trainerClassNameID;
} MenuCBBattleWork;

extern MenuCBBattleWork lbl_80435050;

/* Pokemon order table the battle-start screen shows. */
extern s32 lbl_80434FF0[4][6];

/* Starting Pokemon number of each battle slot ("no Pokemon" is 6), kept in the
   shared read-only small data area. */
extern u16 lbl_804EC3A8[2];
extern u16 lbl_804EC3AC[2];

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
s32 toolentryTaisenGetBattleType(void);
s32 toolentryTaisenGetTrainerDataID(s32 index);
u16 toolentryTaisenGetPokemonNum(s32 index);
u16 toolentryTaisenGetEntryPokemonNum(s32 index);
s32 toolentryGetTrainerBicFaceResID(s32 index, s32 big);
s32 toolentryGetTrainerSamllFaceResID(s32 index, s32 small);
void *zz_0231edc_(s32 index);
s32 zz_trainer_class_get_name_id(void *kindData);

void menuCBPokemonEntryTexWorkInit(void);
void menuCBPokemonEntryLoadTex(void);
s32 GSsndGetStatus(u32 soundID);
void soundOpenPlay(u32 soundID, s32 a, s32 b);
u16 HSD_Rand(void);
void *gamedataBiosGetGamedataAtttestPtr(void *hero);
void GScharCpy(u16 *dst, void *src);
u16 *zz_get_pointer_to_string_with_id(s32 id);
s32 fightTrainerDB_GetKindDataId(s32 dataID);
void *fightTrainerKindDataBiosGetPtr(u8 kindID);
u32 GSresGetResourceAllGroup(u32 resID);
void GSresFreeGroup(u32 group);
void GSresFreeResource(u32 group, u32 resID);
s32 GSfsysFree(u32 id);
s32 GSfsysReadIDEX(u32 id, u32 resID, void *callback, void *context, u32 group);
}

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

/* State of the battle-start screen; -1 while the screen is not running. */
extern "C" s32 zz_004725c_(void);
extern "C" void menuCBBattleStartInit(s32 mode, s32 status);
extern "C" void menuCBBattleStartSetStatus(s32 status);
extern "C" u8 menuCB_BattleStartSoundCheck(void);
extern "C" void menuCBBattleStartDispTrainerInit(void);
extern "C" void menuCBBattleStartTrainerLoadTex(void);
extern "C" void menuCBBattleStartTrainerFaceFree(void);

void _menuCBBattleStartSetIndex(void);
void _menuCBBattleStartSetIndexSingle(long index, unsigned short pokeNum);
void _menuCBBattleStartSetIndexMulti(long index, unsigned short pokeNum);
void _menuCBBattleStartWorkInit(void);
void _menuCBBattleStartSetTrainerData(void);
u8 _menuCBBattleCheckTrainer(long work, long mode);
s32 _menuCBBattleStartGetTrainerFaceSize(void);
void _menuCBBattleStartDispTrainerTexCallBack(long id, void *context, long arg);

/* Helpers whose original names are not in the symbol table. */
extern "C" void zz_0046f40_(long index, unsigned short pokeNum);

/* -------------------------------------------------------------------------
 * Battle start screen
 * ---------------------------------------------------------------------- */

/*
 * Fill lbl_80434FF0 with the order the entered Pokemon are shown in. */
void _menuCBBattleStartSetIndex(void)
{
    s32 battleType = toolentryTaisenGetBattleType();
    u16 pokeNum[4];
    s32 i;

    *(u32 *)&pokeNum[0] = *(u32 *)&lbl_804EC3A8[0];
    *(u32 *)&pokeNum[2] = *(u32 *)&lbl_804EC3AC[0];

    _menuCBBattleStartWorkInit();

    switch (lbl_80435050.status) {
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
            zz_0046f40_(i, pokeNum[i]);
        }
        break;
    case 1:
        for (i = 0; i < 2; i++) {
            _menuCBBattleStartSetIndexSingle(i, pokeNum[i]);
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
 * Order a battle table slot is shown in: one row per Pokemon, holding the row
 * of the order table the Pokemon's party slot selects. Only the rows that are
 * not the identity order are written out.
 */
typedef struct MenuCBBattleOrder {
    s32 row[6][6];
} MenuCBBattleOrder;

/* Double battle order table; party slots 1 and 2 are shown in the order they
   were entered. */
static const MenuCBBattleOrder unk_2136 = { {
    { 1, 2, 0, 3, 4, 5 },
    { 2, 0, 1, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
} };

/* Single battle order table; party slot 4 is shown in the order it was
   entered. */
static const MenuCBBattleOrder unk_2145 = { {
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
    { 4, 0, 1, 5, 2, 3 },
    { 0, 1, 2, 3, 4, 5 },
    { 0, 1, 2, 3, 4, 5 },
} };

/* Single battle: the Pokemon in slot `index` is shown first, the rest follow
   in the order of the battle table, picked by the Pokemon's party slot. */
void _menuCBBattleStartSetIndexSingle(long index, unsigned short pokeNum)
{
    s32 *slot = lbl_80434FF0[index];
    MenuCBBattleOrder order = unk_2145;
    s32 *row = order.row[(u16)pokeNum - 1];
    s32 i;

    for (i = 0; i < 6; i++) {
        slot[(u16)i] = row[(u16)i];
    }
}

/* Same as _menuCBBattleStartSetIndexSingle for the double battle table. */
extern "C" void zz_0046f40_(long index, unsigned short pokeNum)
{
    s32 *slot = lbl_80434FF0[index];
    MenuCBBattleOrder order = unk_2136;
    s32 *row = order.row[(u16)pokeNum - 1];
    s32 i;

    for (i = 0; i < 6; i++) {
        slot[(u16)i] = row[(u16)i];
    }
}

/* Multi battle: the Pokemon are shown in the order they were entered. */
void _menuCBBattleStartSetIndexMulti(long index, unsigned short pokeNum)
{
    u16 i;
    s32 *slot = lbl_80434FF0[index];

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
            lbl_80434FF0[i][j] = j;
        }
    }
}

/* Non-zero while the battle-start jingle is still playing. */
extern "C" u8 menuCB_BattleStartSoundCheck(void)
{
    u32 status = GSsndGetStatus(lbl_80435050.soundID);

    return status != 2;
}

extern "C" void menuCBBattleStartSetStatus(s32 status)
{
    lbl_80435050.status = status;
}

extern "C" s32 zz_004725c_(void)
{
    return lbl_80435050.status;
}

/* Read the name and the class name of the trainer the battle-start screen
   shows. */
void _menuCBBattleStartSetTrainerData(void)
{
    s32 mode = toolentryTaisenGetBattleType();

    if (_menuCBBattleCheckTrainer(1, mode) != 0) {
        void *hero = gamedataBiosGetGamedataAtttestPtr(zz_0231edc_(1));
        u16 *name = lbl_80435050.trainerName;

        if (name != 0) {
            GScharCpy(name, hero);
        } else {
            GScharCpy(name, zz_get_pointer_to_string_with_id(1));
        }
        lbl_80435050.trainerClassNameID = zz_trainer_class_get_name_id(
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
    lbl_80435050.mode = mode;
    lbl_80435050.status = status;
    _menuCBBattleStartSetIndex();

    switch (status) {
    case 0:
        menuCBPokemonEntryTexWorkInit();
        menuCBPokemonEntryLoadTex();
        rnd = HSD_Rand();
        switch (rnd % 3) {
        case 0:
            lbl_80435050.soundID = 0x3D2;
            break;
        case 1:
            lbl_80435050.soundID = 0x3F5;
            break;
        case 2:
            lbl_80435050.soundID = 0x3F6;
            break;
        case 3:
        default:
            lbl_80435050.soundID = 0x3F7;
            break;
        }
        soundOpenPlay(lbl_80435050.soundID, 0, 0xFF);
        break;
    case 1:
        menuCBPokemonEntryTexWorkInit();
        menuCBPokemonEntryLoadTex();
        lbl_80435050.soundID = 0x5DB;
        soundOpenPlay(0x5DB, 0, 0xFF);
        break;
    }
}

/* Arm the trainer face loading. */
extern "C" void menuCBBattleStartDispTrainerInit(void)
{
    s32 i;

    MenuCBBattleWork *work = &lbl_80435050;

    work->readDone = 0;
    work->faceIndex = 0;
    if (_menuCBBattleStartGetTrainerFaceSize() == 0) {
        lbl_80435050.faceNum = 4;
    } else {
        lbl_80435050.faceNum = 2;
    }
    for (i = 0; i < 4; i++) {
        work->face[i].resID = 0;
        work->face[i].group = 0;
    }
}

/* Start reading the trainer faces. */
extern "C" void menuCBBattleStartTrainerLoadTex(void)
{
    _menuCBBattleStartGetTrainerFaceSize();
    lbl_80435050.readDone = 1;
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
        ((MenuCBBattleFace *)context)->group =
            GSresGetResourceAllGroup(((MenuCBBattleFace *)context)->resID);
    }

    /* One face per call: read the next one the battle needs, then stop when the
       read has been started or when there is no face left to read. */
    do {
        if (lbl_80435050.faceIndex == lbl_80435050.faceNum) {
            running = 0;
        } else {
            if (_menuCBBattleStartGetTrainerFaceSize() != 0) {
                if (lbl_80435050.faceIndex % 2 != 0) {
                    resID = toolentryGetTrainerBicFaceResID(1, 0);
                } else {
                    resID = toolentryGetTrainerBicFaceResID(0, 1);
                }
            } else {
                if (lbl_80435050.faceIndex < 2) {
                    resID = toolentryGetTrainerSamllFaceResID(
                        lbl_80435050.faceIndex, 1);
                } else {
                    resID = toolentryGetTrainerSamllFaceResID(
                        lbl_80435050.faceIndex, 0);
                }
            }
            if (_menuCBBattleStartGetTrainerFaceSize() != 0) {
                arg = 0x5C3;
            } else {
                arg = 0x5C4;
            }
            face = &lbl_80435050.face[lbl_80435050.faceIndex];
            face->resID = resID;
            face->group = GSresGetResourceAllGroup(face->resID);
            if (face->group != 0) {
                lbl_80435050.faceIndex++;
            } else {
                GSfsysReadIDEX(arg, resID,
                               _menuCBBattleStartDispTrainerTexCallBack, face,
                               resID);
                running = 0;
                lbl_80435050.faceIndex++;
            }
        }
    } while (running != 0);
}


/* Free the trainer faces and arm them again. */
extern "C" void menuCBBattleStartTrainerFaceFree(void)
{
    MenuCBBattleFace *face;
    u32 group;
    s32 i;

    if (_menuCBBattleStartGetTrainerFaceSize() != 0) {
        group = 0x5C3;
    } else {
        group = 0x5C4;
    }
    GSfsysFree(group);
    GSresFreeGroup(group);

    face = &lbl_80435050.face[0];
    if (_menuCBBattleStartGetTrainerFaceSize() == 0) {
        lbl_80435050.faceNum = 4;
    } else {
        lbl_80435050.faceNum = 2;
    }
    for (i = 0; i < lbl_80435050.faceNum; i++) {
        GSresFreeResource(group, face->resID);
        face++;
    }
    menuCBBattleStartDispTrainerInit();
}

// Static initializer of the menuCBBattleStart module, run once at startup through the
// .ctors table.
extern "C" void zz_00477f4_(void)
{
}

extern "C" __declspec(section ".ctors") void (*const zz_00477f4_ctor)(void) = zz_00477f4_;
