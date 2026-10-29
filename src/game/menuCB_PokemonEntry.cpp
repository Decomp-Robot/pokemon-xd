#include <GSsnd.h>
#include <menuCBPokemonEntry.h>
#include <pause.h>
#include <pokemonBios.h>
#include <menuCB.h>
#include <toolentry.h>
#include <gbaCommand.h>
#include <sound.h>
#include <menu.h>
#include <savedata.h>
#include <menuCBBattleStart.h>
#include <menuCBBios.h>
#include <winSprite.h>

enum {
    ENTRY_STATE_SELECT = 0,
    ENTRY_STATE_CONFIRM = 1,
    ENTRY_STATE_READY = 2,
    ENTRY_STATE_DONE = 0x7FFFFFFF,
    ENTRY_ORDER_UNUSED = 0xF,
    ENTRY_POKEMON_DEOXYS = 0x19A
};

enum {
    ENTRY_REPLY_COMPLETE = 0,
    ENTRY_REPLY_ADD = 0x01000000,
    ENTRY_REPLY_DISCONNECTED = 0x03000000,
    ENTRY_REPLY_REMOVE = 0xFF000000,
    ENTRY_REPLY_MASK = 0xFF000000
};

enum {
    ENTRY_BUTTON_LEFT = 0x1,
    ENTRY_BUTTON_RIGHT = 0x2,
    ENTRY_BUTTON_DOWN = 0x4,
    ENTRY_BUTTON_UP = 0x8,
    ENTRY_BUTTON_B = 0x40,
    ENTRY_BUTTON_A = 0x100,
    ENTRY_BUTTON_X = 0x400,
    ENTRY_BUTTON_Y = 0x800,
    ENTRY_BUTTON_ACTIONS = ENTRY_BUTTON_A | ENTRY_BUTTON_B,
    ENTRY_BUTTON_PARTY = ENTRY_BUTTON_LEFT | ENTRY_BUTTON_RIGHT |
                        ENTRY_BUTTON_DOWN | ENTRY_BUTTON_UP |
                        ENTRY_BUTTON_X | ENTRY_BUTTON_Y
};

MenuCBPokemonEntryWork _menuCBPokemonWork;

/* Base stats for the four Deoxys forms. */
typedef struct GBA_DeokisisuParam {
    s32 mStat[5];
} GBA_DeokisisuParam;

extern const GBA_DeokisisuParam _GBA_DeokisisuParamTable[];

extern "C" {
s32 menuGBACGetRomVer(s32 index);
void menuCBBattleStartTrainerFaceFree(void);
void GSthreadSwitch(void);
}

/* Recalculate Deoxys stats using the connected cartridge form. */
void _menuCB_DeoxysParamCustom(Pokemon *pokemon, u16 port)
{
    s32 level = pokemonGetStatus(pokemon, 0, 0x7A, 0);
    s32 nature = pokemon_GetSeikaku(pokemon);
    s32 ivPhyAtk = pokemonGetStatus(pokemon, 0, 0x94, 0);
    s32 ivPhyDef = pokemonGetStatus(pokemon, 0, 0x95, 0);
    s32 ivSpeAtk = pokemonGetStatus(pokemon, 0, 0x96, 0);
    s32 ivSpeDef = pokemonGetStatus(pokemon, 0, 0x97, 0);
    s32 ivNimbleness = pokemonGetStatus(pokemon, 0, 0x98, 0);
    s32 effortPhyAtk = pokemonGetStatus(pokemon, 0, 0x8E, 0);
    s32 effortPhyDef = pokemonGetStatus(pokemon, 0, 0x8F, 0);
    s32 effortSpeAtk = pokemonGetStatus(pokemon, 0, 0x90, 0);
    s32 effortSpeDef = pokemonGetStatus(pokemon, 0, 0x91, 0);
    s32 effortNimbleness = pokemonGetStatus(pokemon, 0, 0x92, 0);
    s32 statPhyAtk = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[0] * 2 + ivPhyAtk + effortPhyAtk / 4) * level / 100 + 5;
    s32 statPhyDef = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[1] * 2 + ivPhyDef + effortPhyDef / 4) * level / 100 + 5;
    s32 statSpeAtk = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[2] * 2 + ivSpeAtk + effortSpeAtk / 4) * level / 100 + 5;
    s32 statSpeDef = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[3] * 2 + ivSpeDef + effortSpeDef / 4) * level / 100 + 5;
    s32 statNimbleness = (_GBA_DeokisisuParamTable[_menuCB_pokemonGetDeoxysSel(port, 0)].mStat[4] * 2 + ivNimbleness + effortNimbleness / 4) * level / 100 + 5;

    statPhyAtk = pokemonAdjustValueBySeikaku(nature, 0x88, statPhyAtk);
    statPhyDef = pokemonAdjustValueBySeikaku(nature, 0x89, statPhyDef);
    statSpeAtk = pokemonAdjustValueBySeikaku(nature, 0x8A, statSpeAtk);
    statSpeDef = pokemonAdjustValueBySeikaku(nature, 0x8B, statSpeDef);
    statNimbleness = pokemonAdjustValueBySeikaku(nature, 0x8C, statNimbleness);
    pokemonBiosSetPhyAtk(pokemon, statPhyAtk);
    pokemonBiosSetPhyDef(pokemon, statPhyDef);
    pokemonBiosSetSpeAtk(pokemon, statSpeAtk);
    pokemonBiosSetSpeDef(pokemon, statSpeDef);
    pokemonBiosSetNimbleness(pokemon, statNimbleness);
}

/* The Deoxys form of a port, taken from the ROM version of the GBA it
   plays with.  `flag` asks for the form to stay at 0 while the camouflage
   rule of the save is off. */
u8 _menuCB_pokemonGetDeoxysSel(u16 port, bool flag)
{
    u8 form = 0;
    u16 gbaIndex = (u16)(_CB.mTrainer[port].mControlerID - 1);

    if (_CB.connectMode == 1) {
        return 0;
    }
    if (menuCBRule_DEOKISISUCamouflage() || !flag) {
        u16 romVer = menuGBACGetRomVer(gbaIndex);

        switch (romVer) {
        case 1:
        case 2:
            form = 0;
            break;
        case 3:
            form = 1;
            break;
        case 4:
            form = 2;
            break;
        case 5:
            form = 3;
            break;
        default:
            form = 0;
            break;
        }
    }
    return form;
}

/* -------------------------------------------------------------------------
 * Pokemon entry screen
 * ---------------------------------------------------------------------- */

/* The entry screen itself: opens the window, waits until every player
   handed its party over and hands the entries on. Returns the menu the
   caller has to go to next. */
extern "C" s32 menuCB_PokemonEntry(void)
{
    s32 opened;
    s32 i;
    s32 j;
    s32 pending;
    s32 battleType;
    s32 count;

    _menuCBBattleStartWork.soundID = 0x5DB;
    soundOpenPlay(0x5DB, 0, 0xFF);
    _menuCBPokemonEntryWorkInit();
    gba_SIProbe_ErrorCntClear(-1);
    _CB.mAbortRequest = -1;
    _CB.mEntryState = ENTRY_STATE_SELECT;
    _CB.selectResult = 0;
    _CB.confirmationComplete = 0;
    _CB.confirmationInputDelay = 2;
    _CB.mPlayerLeftEntry = 0;
    menuCB_SE_Play(0x14);
    menuOpen(0xC6, 0);
    menuOpen(0xC5, 0);
    opened = 0;
    do {
        if (_CB.mEntryState == ENTRY_STATE_CONFIRM) {
            if (!opened) {
                menuOpenCustom(0xE2, windowGetActiveID(), &_CB.selectResult, 0, 0, 0);
                opened = 1;
            }
            if (_CB.confirmationComplete != 0) {
                if (_CB.selectResult == 0) {
                    _menuCBPokemonEntrySetEntryFlag(0, 1);
                    _CB.mEntryState = ENTRY_STATE_READY;
                } else {
                    toolentryTaisenInitPokemonOrder(0);
                    _CB.mEntryState = ENTRY_STATE_SELECT;
                    _CB.selectResult = 0;
                    _CB.confirmationComplete = 0;
                    _CB.confirmationInputDelay = 2;
                    menuCloseCustom(0xE2, 0, 0);
                    opened = 0;
                }
            }
        }
        if (_CB.mPlayerLeftEntry == 1) {
            _menuCBPokemonWork.mConnectionOK = 0;
        }
        if (_menuCBPokemonEntryAllPlayer() != 0) {
            _CB.mEntryState = ENTRY_STATE_DONE;
        }
        if (_menuCBPokemonWork.mConnectionOK == 0) {
            _CB.mEntryState = ENTRY_STATE_DONE;
        }
        GSthreadSwitch();
    } while (_CB.mEntryState != ENTRY_STATE_DONE);

    if (_menuCBPokemonWork.mConnectionOK == 0) {
        menuCB_SE_Play(0x15);
        menuCloseCustom(0xE2, 0, 0);
        menuCloseCustom(0xC5, 0, 0);
        menuCloseCustom(0xC6, 0, 1);
        menuCB_GBADisconnectedErrorJob();
        menuCBPokemonEntryPokemonFaceFree();
        menuCBBattleStartTrainerFaceFree();
        return 0xA9;
    }
    for (;;) {
        GSthreadSwitch();
        pending = 0;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 6; j++) {
                if (_CB.mPartySlotFlag[i][j] == 1) {
                    pending += _CB.mPartySlotValue[i][j];
                }
            }
        }
        if (pending == 0) {
            break;
        }
    }
    /* Wait for the parties the GBAs send back before the entries are used. */
    GSthreadSwitch();
    GSthreadSwitch();
    GSthreadSwitch();
    battleType = toolentryTaisenGetBattleType();
    if (toolentryTaisenGetBattleVersus() == 4) {
        switch (battleType) {
        case 0:
        case 1:
            count = 2;
            break;
        default:
            count = 4;
            break;
        }
    } else {
        switch (battleType) {
        case 0:
        case 1:
            count = 2;
            break;
        default:
            count = 1;
            break;
        }
    }
    for (i = 0; i < count; i++) {
        toolentryTaisenEntryPokemon(i);
    }
    if (_CB.connectMode == 0) {
        for (i = 0; i < count; i++) {
            u16 slot;
            Pokemon *pokemon;
            _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(i);
            Hero *hero = &trainer->mHero;

            for (slot = 0; slot < 6; slot++) {
                pokemon = heroBiosGetPokemonPtr(hero, slot);

                if (pokemonGetStatus(pokemon, 0, 0x6E, 0) == ENTRY_POKEMON_DEOXYS) {
                    _menuCB_DeoxysParamCustom(pokemon, i);
                }
            }
        }
    }
    return 0xB1;
}
extern "C" void menuCBPokemonEntryButtonTask_20041006(void)
{
    int playerNum = toolentryTaisenGetEntryPlayerNum();
    int player;
    long port;

    toolentryTaisenGetBattleType();
    toolentryTaisenGetHomePlace(0);
    toolentryTaisenGetHomePlace(1);
    toolentryTaisenGetHomePlace(2);
    toolentryTaisenGetHomePlace(3);
    for (player = 0; player < playerNum; player++) {
        if (toolentryTaisenGetBattleVersus() == 4) {
            if (_menuCBPokemonEntryGetEntryFlag(player)) {
                _MENUCBTRAINER *trainer = menuCBBios_GetTrainer(toolentryTaisenGetControlerType(player));

                if (trainer->mHomePlace == 1 || trainer->mHomePlace == 2 || trainer->mHomePlace == 3) {
                    port = player;
                    port = toolentryTaisenGetControlerType(port);
                    if (!gbaCommandIsConnected(port)) {
                        _menuCBPokemonWork.mConnectionOK = 0;
                        if (_menuCBPokemonWork.mDisconnectedPort < 0) {
                            _menuCBPokemonWork.mDisconnectedPort = port;
                        }
                    }
                }
            }
        }
        if (!_menuCBPokemonEntryGetEntryFlag(player)) {
            port = player;
            if (toolentryTaisenGetBattleVersus() == 4) {
                _MENUCBTRAINER *trainer = menuCBBios_GetTrainer(toolentryTaisenGetControlerType(player));

                if (trainer->mHomePlace == 1 || trainer->mHomePlace == 2 || trainer->mHomePlace == 3) {
                    _menuCBPokemonEntryEntCheckGBA((GSinputDevice)toolentryTaisenGetControlerType(port), port);
                } else {
                    _menuCBPokemonEntryEntCheck((GSinputDevice)toolentryTaisenGetControlerType(player), player);
                }
            }
            if (toolentryTaisenGetControlerType(port) == 1) {
                if (!toolentryTaisenGetHomePlace(port)) {
                    toolentryTaisengetEtnryPokemonOrderNum(port);
                    _menuCBPokemonEntryPokemonNum(port);
                }
            }
        }
    }
}

/* What a GBA reports back while its player enters a party: the result code
   followed by the party it sent. */
void _menuCBPokemonEntryEntCheckGBA(GSinputDevice device, long port)
{
    u32 result;
    u8 raw[8];
    u32 word[6];
    s32 num;
    s32 i;

    toolentryTaisenGetBattlePlayerID(port);
    gbaCommandEntryResult(device, &result);
    switch (result & ENTRY_REPLY_MASK) {
    case ENTRY_REPLY_ADD: {
        s32 order = toolentryTaisengetEtnryPokemonOrderNum(port);
        s32 slot = toolentryTaisenSetEtnryPokemonOrder(port, order);

        if (slot >= 0) {
            if (!pauseIsPause()) {
                GSsndPlay(0x19, 0, 0);
            }
            MenuCBPokemonEntryPlayer *player = &_menuCBPokemonWork.mPlayer[port];

            player->mPos[slot] = (5 - slot) * 24;
            player->mPos[slot + 6] = 0.0f;
            _menuCBPokemonEntryEntCheckDebug(port);
        }
        break;
    }
    case ENTRY_REPLY_REMOVE: {
        s32 order = toolentryTaisengetEtnryPokemonOrderNum(port);

        if (order != toolentryTaisenDeleteEtnryPokemonOrder(port)) {
            if (!pauseIsPause()) {
                GSsndPlay(0x12, 0, 0);
            }
        }
        break;
    }
    case ENTRY_REPLY_COMPLETE:
        num = toolentryTaisenGetEntryPokemonNum(port);
        gbaCommandEntryPokemon(result, raw);
        for (i = 0; i < num; i++) {
            word[i] = raw[i];
        }
        toolentryTaisenSetEtnryPokemonOrderGBA(port, num, word);
        _menuCBPokemonEntrySetEntryFlag(port, 1);
        break;
    case ENTRY_REPLY_DISCONNECTED:
        _menuCBPokemonWork.mConnectionOK = 0;
        if (_menuCBPokemonWork.mDisconnectedPort < 0) {
            _menuCBPokemonWork.mDisconnectedPort = device;
        }
        break;
    }
}

void _menuCBPokemonEntryEntCheckDebug(long)
{
}

/* Read the buttons of one port and act on the party list of its player. */
void _menuCBPokemonEntryEntCheck(GSinputDevice device, long port)
{
    u32 changed = GSinputButtonsChanged(device);
    u32 pressed = GSinputButtonsPressed(device);
    u32 buttons = pressed & changed;
    s32 i;
    s32 order;
    u16 num;

    toolentryTaisenGetBattlePlayerID(port);
    if (!pauseIsPause() && (windowGetKeyInfo()->mKeyOn & ENTRY_BUTTON_X) == 0 && windowSearchID(0xE2) == 0) {
        if ((buttons & ENTRY_BUTTON_ACTIONS) != 0) {
            if ((buttons & ENTRY_BUTTON_B) != 0) {
                s32 order = toolentryTaisengetEtnryPokemonOrderNum(port);

                if (toolentryTaisenDeleteEtnryPokemonOrder(port) != order) {
                    GSsndPlay(0x12, 0, 0);
                }
            } else if ((buttons & ENTRY_BUTTON_A) != 0) {
                _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
                num = (u16)toolentryTaisenGetEntryPokemonNum(port);
                order = trainer->mEntryCount;

                if (order >= (u16)menuCBPokemonEntryPokemonNumMin(port)) {
                    for (i = order; i < num; i++) {
                        trainer->mEntryList[i] = ENTRY_ORDER_UNUSED;
                        trainer->mEntryCount++;
                    }
                    GSsndPlay(0x11, 0, 0);
                }
            }
        } else if ((buttons & ENTRY_BUTTON_PARTY) != 0) {
            s32 order = _menuCBPokemonEntryButtonCheck(buttons, port);

            if (order >= 0) {
                s32 slot = toolentryTaisenSetEtnryPokemonOrder(port, order);

                if (slot >= 0) {
                    GSsndPlay(0x19, 0, 0);

                    MenuCBPokemonEntryPlayer *player = &_menuCBPokemonWork.mPlayer[port];

                    player->mPos[slot] = (5 - slot) * 24;
                    player->mPos[slot + 6] = 0.0f;
                }
            }
        }
    }
}

/* Party slot a direction or button key asks for, -1 if it is out of reach. */
s32 _menuCBPokemonEntryButtonCheck(unsigned long buttons, long port)
{
    s32 order = -1;
    s32 num = (u16)toolentryTaisenGetPokemonNum(port);

    if (buttons & ENTRY_BUTTON_LEFT) {
        order = 0;
    }
    if (buttons & ENTRY_BUTTON_UP) {
        order = 1;
    }
    if (buttons & ENTRY_BUTTON_Y) {
        order = 2;
    }
    if (buttons & ENTRY_BUTTON_DOWN) {
        order = 3;
    }
    if (buttons & ENTRY_BUTTON_RIGHT) {
        order = 4;
    }
    if (buttons & ENTRY_BUTTON_X) {
        order = 5;
    }
    if (num <= order) {
        return -1;
    }
    return order;
}

void _menuCBPokemonEntryWork(void)
{
    _MENUCBTRAINER *found[4];
    s32 i;

    for (i = 0; i < 4; i++) {
        found[i] = menuCBBios_GetTrainerFromVersusBattleID(i);
        if (found[i] != 0) {
            _menuCBPokemonWork.mVersusId[i] = (s8)found[i]->mVersusBattleId;
        } else {
            _menuCBPokemonWork.mVersusId[i] = -1;
        }
    }
}

/* Limit the entry count to the available party. */
extern "C" u16 menuCBPokemonEntryPokemonNumMin(long port)
{
    u16 limit;
    u16 num;

    if (menuCBTaisenWorkActive->mRulesetData[menuCBTaisenWorkActive->ruleId].mPokemonCountMode == 0) {
        limit = menuCBTaisenWorkActive->mRulesetData[menuCBTaisenWorkActive->ruleId].mPokemonCount;
    } else {
        limit = 1;
    }
    num = _menuCBPokemonEntryPokemonNum(port);
    if (num < limit) {
        return num;
    }
    return limit;
}

u16 _menuCBPokemonEntryPokemonNum(long port)
{
    u16 limit = menuCBBios_ParticipationPokemonMax();
    u16 num = toolentryTaisenGetPokemonNum(port);

    return num < limit ? num : limit;
}

void _menuCBPokemonEntryWorkInit(void)
{
    s32 battleType = toolentryTaisenGetBattleType();
    s32 playerNum = toolentryTaisenGetEntryPlayerNum();
    long port;

    _menuCBPokemonWork.mConnectionOK = 1;
    _menuCBPokemonWork.mDisconnectedPort = -1;
    _menuCBPokemonWork.mBaseX = 0.0f;
    _menuCBPokemonWork.mBaseIndex = 0;
    _menuCBPokemonWork.mCursor = 0;
    _menuCBPokemonWork.mScroll = 0;
    _menuCBPokemonWork.mState = 0;
    _menuCBPokemonWork.mTimer = 0;
    for (port = 0; port < playerNum; port++) {
        _MENUCBTRAINER *trainer = menuCBBios_GetTrainerImmediately(port);
        u32 gbaPort = menuCBTrainerBios_GetControlerID(trainer);
        Hero *hero = menuCBBios_GetHeroImmediately(port);
        s32 type;

        _menuCBPokemonEntrySetEntryFlag(port, 0);
        if (trainer->mHomePlace != 0) {
            gbaCommandSendWazaText(gbaPort, hero);
            type = battleType == 1 ? 2 : 1;
            gbaCommandEntryRequest(gbaPort, (u16)_menuCBPokemonEntryPokemonNum(port),
                (u16)menuCBPokemonEntryPokemonNumMin(port), type);
        }
    }
    for (port = 0; port < 4; port++) {
        toolentryTaisenInitPokemonOrder(port);
    }
    _menuCBPokemonEntryWork();
    _menuCBPokemonEntryDispPosition();
}

/* -------------------------------------------------------------------------
 * Face texture work of the entry screen
 * ---------------------------------------------------------------------- */

/* Face ID the loader keeps for the slot it loaded last. */
extern u16 uToolPokemon_faceId;

/* The face texture work walks four parties of six slots each. */
enum { FACE_WORK_PLAYER_NUM = 4, FACE_WORK_SLOT_NUM = 6 };

Pokemon *_menuCBPokemonEntryLoadCallBack(void *arg);
MenuCBPokemonEntryFaceWork *_menuCBPokemonEntryGetFaceBase(long player, long slot);

extern "C" void menuCBPokemonEntryTexWorkInit(void)
{
    MenuCBPokemonEntryFaceWork *faceRow;
    s32 player;
    s32 slot;

    _menuCBPokemonWork.mCursor = 0;
    _menuCBPokemonWork.mReadFlag = 0;

    for (player = 0; player < FACE_WORK_PLAYER_NUM; player++) {
        faceRow = &_menuCBPokemonWork.mFaceWork[player][0];

        for (slot = 0; slot < FACE_WORK_SLOT_NUM; slot++) {
            MenuCBPokemonEntryFaceWork *face = &faceRow[slot];

            face->mLoaded = 0;
            face->mUnknown04 = 0;
        }
    }
}

extern "C" u32 menuCBPokemonEntryGetReadFlag(void)
{
    return _menuCBPokemonWork.mReadFlag;
}

void _menuCBPokemonEntrySetEntryFlag(long port, bool flag)
{
    _menuCBPokemonWork.mEntryFlag[port] = flag;
}

u8 _menuCBPokemonEntryGetEntryFlag(long port)
{
    return _menuCBPokemonWork.mEntryFlag[port];
}

/* The 2D entry screen draws its party rows lower down than the others. */
void _menuCBPokemonEntryDispPosition(void)
{
    s32 battleType = toolentryTaisenGetBattleType();

    switch (battleType) {
    case BATTLE_TYPE_DOUBLE:
    case BATTLE_TYPE_SINGLE:
        _menuCBPokemonWork.mScroll = 310;
        break;

    case BATTLE_TYPE_MULTI:
        _menuCBPokemonWork.mScroll = 0;
        break;

    default:
        _menuCBPokemonWork.mScroll = 0;
        break;
    }
}

/* Whether every player of the battle has handed its party over. */
u8 _menuCBPokemonEntryAllPlayer(void)
{
    s32 playerNum = 1;
    s32 versus = toolentryTaisenGetBattleVersus();
    s32 battleType = toolentryTaisenGetBattleType();
    s32 port;

    switch (battleType) {
    case BATTLE_TYPE_DOUBLE:
    case BATTLE_TYPE_SINGLE:
        if (versus == 4) {
            playerNum = 2;
            _menuCBPokemonWork.mPlayerNum = 4;
        } else {
            playerNum = 2;
            _menuCBPokemonWork.mPlayerNum = versus;
        }
        break;

    case BATTLE_TYPE_MULTI:
        _menuCBPokemonWork.mPlayerNum = 4;
        playerNum = 4;
        break;

    default:
        break;
    }

    for (port = 0; port < playerNum; port++) {
        if (!_menuCBPokemonEntryGetEntryFlag(port)) {
            return 0;
        }
    }

    return 1;
}

extern "C" void menuCBPokemonEntryLoadTex(void)
{
    menuFaceLoad_Tool(0, _menuCBPokemonEntryLoadCallBack, 0);
}

/* Returns the Pokemon whose face it loaded last, 0 once the walk is over. */
Pokemon *_menuCBPokemonEntryLoadCallBack(void *arg)
{
    Pokemon *pokemon;
    s32 slot;
    s32 player;
    s32 keepLoading = 1;

    toolentryTaisenGetEntryPlayerNum();
    toolentryTaisenGetBattleType();

    pokemon = (Pokemon *)arg;

    if (arg != 0) {
        *(u8 *)arg = 1;
    }

    do {
        MenuCBPokemonEntryFaceWork *face;
        u16 faceID;

        if (_menuCBPokemonWork.mCursor == FACE_WORK_PLAYER_NUM * FACE_WORK_SLOT_NUM) {
            _menuCBPokemonWork.mReadFlag = 1;
            keepLoading = 0;
            pokemon = 0;
        } else {
            player = _menuCBPokemonWork.mCursor / FACE_WORK_SLOT_NUM;
            slot = _menuCBPokemonWork.mCursor % FACE_WORK_SLOT_NUM;

            if (menuCBBattleStartGetStatus() == 0) {
                pokemon = toolentryTaisenGetPokemonPtr(player, slot);
            } else if (toolentryTaisenGetEntryPokemonNum(player) <= slot) {
                pokemon = 0;
            } else {
                pokemon = toolentryTaisenGetEntryPokemonPtr(player, slot);
            }

            face = _menuCBPokemonEntryGetFaceBase(player, slot);
            faceID = menuFaceGetID2(pokemon, _menuCB_pokemonGetDeoxysSel(player, 1));

            if (faceID != 0 && pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0) != 0) {
                face->mLoaded = true;
                face->mFaceID = faceID;
                uToolPokemon_faceId = faceID;
                keepLoading = 0;
                _menuCBPokemonWork.mCursor = _menuCBPokemonWork.mCursor + 1;
            } else {
                face->mLoaded = false;
                _menuCBPokemonWork.mCursor = _menuCBPokemonWork.mCursor + 1;
            }
        }
    } while (keepLoading);

    return pokemon;
}

MenuCBPokemonEntryFaceWork *_menuCBPokemonEntryGetFaceBase(long player, long slot)
{
    return &_menuCBPokemonWork.mFaceWork[player][slot];
}

extern "C" void menuCBPokemonEntryPokemonFaceFree(void)
{
    menuCBPokemonEntryTexWorkInit();
}

extern "C" u8 menuCBPokemonEntryDispPokemonFace(tagWINDOW_WORK *window,
                                                tagSPRITE_WORK *sprite,
                                                s32 player, s32 slot)
{
    MenuCBPokemonEntryFaceWork *face = _menuCBPokemonEntryGetFaceBase(player, slot);

    if (face->mLoaded) {
        menuFaceDispSub(window, sprite, face->mFaceID);
        return 1;
    }

    return 0;
}
