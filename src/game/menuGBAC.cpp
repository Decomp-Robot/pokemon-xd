#include <types.h>
#include <menuGBA.h>
#include <gbaAgb.h>
#include <gbaCommand.h>
#include <menu.h>
#include <menuCBBios.h>
#include <menuCBCommon.h>
#include <menuCBRule.h>
#include <menuCB_Sub1.h>
#include <winSprite.h>
#include <winMsg.h>
#include <gamedata.h>
#include <savedata.h>
#include <pokecolo.h>
#include <msgctrl.h>
#include <GSinput.h>
#include <GSsndPlayback.h>
#include <GSthread.h>
#include <GSgfx.h>
#include <hero.h>

extern "C" {
void heroInit(Hero *hero);
void heroBiosCopy(Hero *dst, Hero *src);
}

// How many GBAs the connect menu links; the player on port 1 links last.
enum _MENU_GBAC_TYPE {
    MENU_GBAC_TYPE_1GBA = 0,
    MENU_GBAC_TYPE_2GBA = 1,
    MENU_GBAC_TYPE_3GBA = 2,
    MENU_GBAC_TYPE_4GBA = 3
};

// Options openMenuGBA is called with.
enum {
    MENU_GBAC_FLAG_CHANGE = 0x1,        // read the party for a trade.
    MENU_GBAC_FLAG_BATTLE = 0x2,        // read the party for a battle.
    MENU_GBAC_FLAG_RETRY = 0x8,         // a failed port may be linked again.
    MENU_GBAC_FLAG_LOAD_SAVE = 0x10,    // port 1 plays with the GameCube save.
    MENU_GBAC_FLAG_NO_CONNECT_MSG = 0x40
};

// What _waitAction waits for.
enum _GBA_WAIT_FLAG {
    GBA_WAIT_MESSAGE = 1,        // the message to be read.
    GBA_WAIT_CANCEL = 2,         // the cancel key.
    GBA_WAIT_MESSAGE_CANCEL = 3,
    GBA_WAIT_INSERT = 4,         // a GBA to be plugged into the port.
    GBA_WAIT_CANCEL_INSERT = 6,
    GBA_WAIT_ALL = 7
};

// How the last wait of the work ended.
enum {
    GBA_CONNECT_RESULT_NONE = 0,
    GBA_CONNECT_RESULT_DECIDE = 1,
    GBA_CONNECT_RESULT_CANCEL = 2,
    GBA_CONNECT_RESULT_INSERTED = 4
};

// Header word AGB_GetPokemon fills in for the party it reads.
#define GBA_HEADER_PARTY_VALID 0x1
#define GBA_HEADER_RECEIVED 0x2
#define GBA_HEADER_LANGUAGE(h) (((h) >> 4) & 0xF)
#define GBA_HEADER_ERROR(h) (((h) >> 8) & 0x3)
#define GBA_HEADER_ROM_VER(h) (((h) >> 10) & 0xF)
#define GBA_HEADER_READY_FRLG 0x8
#define GBA_HEADER_READY_RS 0x4000

enum {
    COUNTRY_JAPAN = 0,
    COUNTRY_USA = 1,
    COUNTRY_EUROPE = 2
};

// Language of the GBA game and language area of the GameCube save.
enum {
    GBA_LANGUAGE_JAPANESE = 1,
    GBA_LANGUAGE_ENGLISH = 2,
    GBA_LANGUAGE_FRENCH = 3,
    GBA_LANGUAGE_ITALIAN = 4,
    GBA_LANGUAGE_GERMAN = 5,
    GBA_LANGUAGE_SPANISH = 7
};

enum {
    LANGAREA_JAPANESE = 1,
    LANGAREA_ENGLISH = 2,
    LANGAREA_GERMAN = 3,
    LANGAREA_FRENCH = 4,
    LANGAREA_ITALIAN = 5,
    LANGAREA_SPANISH = 6,
    LANGAREA_ENGLISH_EU = 8,
    LANGAREA_UNKNOWN = 0xFF
};

enum {
    MENU_ID_PAD_ERROR = 42,
    MENU_ID_GBA_CONNECT = 176
};

enum {
    SE_GBAC_OPEN = 20,
    SE_GBAC_CLOSE = 21,
    SND_GBAC_DISCONNECT = 19,
    SND_GBAC_RECEIVED = 27
};

enum {
    MSG_GBAC_NONE = 1,
    MSG_GBAC_CONNECT = 17438,
    MSG_GBAC_UPLOAD = 17439,
    MSG_GBAC_RECEIVED = 17440,
    MSG_GBAC_UPLOAD_ERROR = 17441,
    MSG_GBAC_TRANSFER_ERROR = 17442,
    MSG_GBAC_LANGUAGE_ERROR = 17443,
    MSG_GBAC_PARTY_ERROR = 17444,
    MSG_GBAC_RULE_ERROR = 17445,
    MSG_GBAC_DISCONNECTED = 17451,
    MSG_GBAC_POWER_OFF = 17452,
    MSG_GBAC_CONNECT_PAD = 17453,
    MSG_GBAC_CONNECTED = 17461,
    MSG_GBAC_LOAD_SAVE = 17463,
    MSG_GBAC_DISCONNECTED_PAD = 17464,
    MSG_GBAC_VERSION_ERROR_RS = 17477,
    MSG_GBAC_VERSION_ERROR_FRLG = 17478,
    MSG_GBAC_VERSION_ERROR_EMERALD = 17479
};

// Seconds a message stays up before the next step.
#define GBAC_MESSAGE_WAIT 1.0f
// Frames the link may take to answer after the upload.
#define GBAC_CONNECT_WAIT 300
// Frames _showDisconnected waits for the unplugged GBA to show up.
#define GBAC_DISCONNECT_WAIT 15

u8 _mainFlow(_MENU_GBAC_TYPE type, unsigned long flags, Hero **heroes);
u8 _check_langarea(u8 language);
_GBA_Connect_WORK *_open_menu(_MENU_GBAC_TYPE type, unsigned long flags, long start);
u8 _poweroff(_GBA_Connect_WORK *work, _GBA_PHASE phase);
u8 _error(_GBA_Connect_WORK *work, unsigned long msg, _GBA_PHASE phase);
u8 _showDisconnected(_GBA_Connect_WORK *work);
u8 _checkDisconnect(_GBA_Connect_WORK *work);
long _waitComplete(_GBA_Connect_WORK *work);
u8 _waitAction(_GBA_Connect_WORK *work, _GBA_WAIT_FLAG flag);
void _menuWait(f32 seconds);
void _message_type2(unsigned long msg, long port);
void _message(unsigned long msg, long port);
u8 _get_langarea(void);

// Phase each port starts in, by menu type.
const _GBA_PHASE _initial_phase[4][4] = {
    { GBA_PHASE_SAVE_LOAD, GBA_PHASE_WAIT, GBA_PHASE_NONE, GBA_PHASE_NONE },
    { GBA_PHASE_SAVE_LOAD, GBA_PHASE_WAIT, GBA_PHASE_WAIT, GBA_PHASE_NONE },
    { GBA_PHASE_SAVE_LOAD, GBA_PHASE_WAIT, GBA_PHASE_WAIT, GBA_PHASE_WAIT },
    { GBA_PHASE_WAIT, GBA_PHASE_WAIT, GBA_PHASE_WAIT, GBA_PHASE_WAIT },
};

// GBA programs for a battle and for a trade, by language.
const char *_jb_files_lang[6][2] = {
    { "maxbtl.bin", "maxcng.bin" },
    { "maxbtl_english.bin", "maxcng_english.bin" },
    { "maxbtl_france.bin", "maxcng_france.bin" },
    { "maxbtl_germany.bin", "maxcng_germany.bin" },
    { "maxbtl_italy.bin", "maxcng_italy.bin" },
    { "maxbtl_spain.bin", "maxcng_spain.bin" },
};

_GBA_Connect_WORK gbaConectGBA_Connect_WORK;

u8 _menu_ports[4] = { 1, 2, 4, 8 };
// Order the ports are linked in.
s8 chans[4] = { 1, 2, 3, 0 };
// Party entry each port's GBA fills, in link order.
u16 _entries[4] = { 1, 2, 3, 0 };
const char *_jb_files[2] = { "maxbtl.bin", "maxcng.bin" };
u16 _rom_ver[4] = { 0, 0, 0, 0 };

s8 _disconnect_shown;

extern "C" u16 menuGBACGetRomVer(u16 entry)
{
    if (entry > 3) {
        return 0;
    }
    return _rom_ver[entry];
}

extern "C" void menuGBACSetRomVer(unsigned short version, unsigned short entry)
{
    switch (entry) {
    case 0:
    case 1:
    case 2:
    case 3:
        _rom_ver[entry] = version;
        break;
    default:
        _rom_ver[0] = _rom_ver[1] = _rom_ver[2] = _rom_ver[3] = 0;
        break;
    }

    if (entry == GBA_ROM_ENTRY_ALL) {
        gbaCommandSetKeyState(1, GBA_KEY_COMMAND_NONE);
        gbaCommandSetKeyState(2, GBA_KEY_COMMAND_NONE);
        gbaCommandSetKeyState(3, GBA_KEY_COMMAND_NONE);
        gbaCommandSetKeyState(4, GBA_KEY_COMMAND_NONE);
    }
}

extern "C" long openMenuGBA(long menu, long mode, Hero **heroes)
{
    u8 enablePort;
    u8 result;
    long chan;

    enablePort = menuSetEnablePort(1);
    AGB_FileBuffAlloc();
    result = _mainFlow((_MENU_GBAC_TYPE)menu, mode, heroes);
    winMsgClose(0);

    if (menuIsCheck(MENU_ID_GBA_CONNECT)) {
        menuCB_MessageDispOff();
        menuCB_SE_Play(SE_GBAC_CLOSE);
        menuCloseCustom(MENU_ID_GBA_CONNECT, 0, true);
    }

    menuSetEnablePort(enablePort);
    for (chan = 0; chan < 3; chan++) {
        AGB_Terminate(chan);
    }
    AGB_FileBuffFree();

    return result ? 0 : -1;
}

u8 _mainFlow(_MENU_GBAC_TYPE type, unsigned long flags, Hero **heroes)
{
    Hero hero;
    unsigned long header;
    long i;
    long file;
    u8 uploaded;
    _GBA_Connect_WORK *work;
    Hero *save;
    s8 port;
    Hero *party;
    s8 *chan;
    long j;
    long status;
    u8 ok;

    file = 0;
    uploaded = FALSE;
    AGB_Terminate_init();

    if ((flags & MENU_GBAC_FLAG_CHANGE) == MENU_GBAC_FLAG_CHANGE) {
        file = 1;
    }
    if ((flags & MENU_GBAC_FLAG_BATTLE) == MENU_GBAC_FLAG_BATTLE) {
        file = 0;
    }

    switch (pokecoloGetCountryCode()) {
    case COUNTRY_JAPAN:
        while (!AGB_Prepare(_jb_files[file])) {
            GSthreadSwitch();
        }
        break;
    case COUNTRY_USA:
        while (!AGB_Prepare(_jb_files_lang[1][file])) {
            GSthreadSwitch();
        }
        break;
    }

    work = _open_menu(type, flags, 0);

    if ((flags & MENU_GBAC_FLAG_LOAD_SAVE) && (type == MENU_GBAC_TYPE_1GBA || type == MENU_GBAC_TYPE_3GBA)) {
        save = (Hero *)savedataGetStatus(NULL, SAVE_DATA_HERO);
        work->mPort = 0;
        work->mPhase[0] = GBA_PHASE_SAVE_LOAD;

        if (flags & MENU_GBAC_FLAG_BATTLE) {
            _message(MSG_GBAC_LOAD_SAVE, 0);
            _menuWait(GBAC_MESSAGE_WAIT);
            if (!menuCBRule_CheckHero(save)) {
                menuCBEntrySlotConditionsBuildAll(save, 0);
                menuCB_MessageDispOff();
                menuCB_SE_Play(SE_GBAC_CLOSE);
                menuCloseCustom(MENU_ID_GBA_CONNECT, 0, true);
                menuCB_ShowPartyErrors(save, 0);
                return FALSE;
            }
        }

        if (heroes != NULL && heroes[0] != NULL) {
            heroBiosCopy(heroes[0], save);
        }
        work->mPhase[0] = GBA_PHASE_SAVE_COMPLETE;
    }

    menuGBACSetRomVer(GBA_ROM_VERSION_UNKNOWN, GBA_ROM_ENTRY_ALL);

    chan = chans;
    for (i = 0; i < sizeof(chans) / sizeof(chans[0]); i++) {
        port = *chan;
        work->mPort = port;

        if (heroes != NULL && heroes[port] != NULL) {
            party = heroes[port];
        } else {
            party = &hero;
        }

        if (port == 0) {
            if (work->mSlot[0] = GBA_SLOT_SAVE_GBA) {
                work->mSlot[0] = GBA_SLOT_GBA_HOST;
                work->mIconPosReset[0] = 1;
            }
        }

        for (;;) {
            menuSetEnablePort(menuGetEnablePort() & ~_menu_ports[port]);

            if (work->mResult != GBA_CONNECT_RESULT_INSERTED) {
                work->mPhase[port] = GBA_PHASE_CONNECT;
                _message(MSG_GBAC_CONNECT, port);
                if (!_waitAction(work, GBA_WAIT_CANCEL_INSERT)) {
                    return _poweroff(work, GBA_PHASE_ERROR);
                }
            }

            _message(MSG_GBAC_UPLOAD, port);
            work->mPhase[port] = GBA_PHASE_UPLOAD;
            _CB.gbaConnectPort = port;
            _CB.gbaConnectMode = 2;

            if (!uploaded) {
                if (_waitComplete(work) == AGB_STATUS_TERMINATED) {
                    return _poweroff(work, GBA_PHASE_ERROR);
                }
                uploaded = TRUE;
            }

            AGB_Upload(port, AGB_UPLOAD_MODE_POKEMON);
            switch (_waitComplete(work)) {
            case AGB_STATUS_COMPLETE:
                break;
            case AGB_STATUS_TERMINATED:
                return _poweroff(work, GBA_PHASE_ERROR);
            case AGB_STATUS_NO_ANSWER:
            default:
                if (_error(work, MSG_GBAC_UPLOAD_ERROR, GBA_PHASE_ERROR)) {
                    continue;
                }
                return FALSE;
            }

            work->mPhase[port] = GBA_PHASE_TRANSFER;
            for (j = 0; j < GBAC_CONNECT_WAIT; j++) {
                if (gbaCommandIsConnected(port + 1)) {
                    menuSetEnablePort(menuGetEnablePort() | _menu_ports[port]);
                    break;
                }
                GSthreadSwitch();
            }

            if (!(flags & MENU_GBAC_FLAG_NO_CONNECT_MSG)) {
                _message(MSG_GBAC_CONNECTED, port);
                _menuWait(GBAC_MESSAGE_WAIT);
            }

            heroInit(party);
            header = 0;
            AGB_GetPokemon(port, party, &header);
            status = _waitComplete(work);
            switch (status) {
            case AGB_STATUS_TERMINATED:
                return _poweroff(work, GBA_PHASE_ERROR);
            }

            menuGBACSetRomVer(GBA_HEADER_ROM_VER(header), _entries[i]);

            if (GBA_HEADER_ERROR(header)) {
                if (_error(work, MSG_GBAC_TRANSFER_ERROR, GBA_PHASE_ERROR)) {
                    continue;
                }
                return FALSE;
            }
            if (!_check_langarea(GBA_HEADER_LANGUAGE(header))) {
                if (_error(work, MSG_GBAC_LANGUAGE_ERROR, GBA_PHASE_ERROR)) {
                    continue;
                }
                return FALSE;
            }
            switch (status) {
            case AGB_STATUS_COMPLETE:
                if (header & GBA_HEADER_RECEIVED) {
                    break;
                }
            default:
                if (_error(work, MSG_GBAC_TRANSFER_ERROR, GBA_PHASE_ERROR)) {
                    continue;
                }
                return FALSE;
            }

            if (flags & MENU_GBAC_FLAG_CHANGE) {
                ok = TRUE;
                switch (GBA_HEADER_ROM_VER(header)) {
                case GBA_ROM_VERSION_RUBY:
                case GBA_ROM_VERSION_SAPPHIRE:
                    if (!(header & GBA_HEADER_READY_RS)) {
                        ok = FALSE;
                    }
                    if (!ok) {
                        if (_error(work, MSG_GBAC_VERSION_ERROR_RS, GBA_PHASE_ERROR)) {
                            continue;
                        }
                        return FALSE;
                    }
                    break;
                case GBA_ROM_VERSION_FIRE_RED:
                case GBA_ROM_VERSION_LEAF_GREEN:
                    if (!(header & GBA_HEADER_READY_FRLG)) {
                        ok = FALSE;
                    }
                    if (!ok) {
                        if (_error(work, MSG_GBAC_VERSION_ERROR_FRLG, GBA_PHASE_ERROR)) {
                            continue;
                        }
                        return FALSE;
                    }
                    break;
                case GBA_ROM_VERSION_EMERALD:
                    if (!(header & GBA_HEADER_READY_RS)) {
                        ok = FALSE;
                    }
                    if (!ok) {
                        if (_error(work, MSG_GBAC_VERSION_ERROR_EMERALD, GBA_PHASE_ERROR)) {
                            continue;
                        }
                        return FALSE;
                    }
                    break;
                default:
                    if (_error(work, MSG_GBAC_UPLOAD_ERROR, GBA_PHASE_ERROR)) {
                        continue;
                    }
                    return FALSE;
                }
            }

            if (flags & MENU_GBAC_FLAG_BATTLE) {
                if (!(header & GBA_HEADER_PARTY_VALID)) {
                    if (_error(work, MSG_GBAC_PARTY_ERROR, GBA_PHASE_ERROR)) {
                        continue;
                    }
                    return FALSE;
                }

                if (!menuCBRule_CheckHero(party)) {
                    menuCBEntrySlotConditionsBuildAll(party, _entries[i]);
                    menuCB_MessageDispOff();
                    menuCB_SE_Play(SE_GBAC_CLOSE);
                    menuCloseCustom(MENU_ID_GBA_CONNECT, 0, true);
                    menuCB_ShowPartyErrors(party, _entries[i]);
                    if (!(flags & MENU_GBAC_FLAG_RETRY)) {
                        return FALSE;
                    }

                    work->mPhaseBackup[0] = work->mPhase[0];
                    work->mPhaseBackup[1] = work->mPhase[1];
                    work->mPhaseBackup[2] = work->mPhase[2];
                    work->mPhaseBackup[3] = work->mPhase[3];
                    work = _open_menu(type, flags, i);
                    if (port == 0) {
                        if (work->mSlot[0] = GBA_SLOT_SAVE_GBA) {
                            work->mSlot[0] = GBA_SLOT_GBA_HOST;
                            work->mIconPosReset[0] = 1;
                        }
                    }
                    work->mPhase[0] = work->mPhaseBackup[0];
                    work->mPhase[1] = work->mPhaseBackup[1];
                    work->mPhase[2] = work->mPhaseBackup[2];
                    work->mPhase[3] = work->mPhaseBackup[3];
                    work->mPhase[port] = GBA_PHASE_ERROR;
                    GSthreadSwitch();

                    for (j = 0; j < i; j++) {
                        if (work->mPhase[chans[j]] == GBA_PHASE_DISCONNECTED) {
                            return _poweroff(work, GBA_PHASE_ERROR);
                        }
                    }
                    _message_type2(MSG_GBAC_RULE_ERROR, port);
                    continue;
                }
            }
            break;
        }

        AGB_Terminate(port);
        GSsndPlayNow(SND_GBAC_RECEIVED);
        work->mPhase[port] = GBA_PHASE_COMPLETE;
        menuSetEnablePort(menuGetEnablePort() | _menu_ports[port]);
        _message(MSG_GBAC_RECEIVED, port);

        switch (type) {
        case MENU_GBAC_TYPE_1GBA:
            if (_waitAction(work, GBA_WAIT_MESSAGE_CANCEL)) {
                return TRUE;
            }
            return _poweroff(work, GBA_PHASE_ERROR);
        case MENU_GBAC_TYPE_2GBA:
            if (port == 2) {
                if (_waitAction(work, GBA_WAIT_MESSAGE_CANCEL)) {
                    return TRUE;
                }
                return _poweroff(work, GBA_PHASE_ERROR);
            }
            break;
        case MENU_GBAC_TYPE_3GBA:
            if (port == 3) {
                if (_waitAction(work, GBA_WAIT_MESSAGE_CANCEL)) {
                    return TRUE;
                }
                return _poweroff(work, GBA_PHASE_ERROR);
            }
            break;
        case MENU_GBAC_TYPE_4GBA:
            if (port == 0) {
                if (_waitAction(work, GBA_WAIT_MESSAGE_CANCEL)) {
                    return TRUE;
                }
                return _poweroff(work, GBA_PHASE_ERROR);
            }
            break;
        }

        work->mPort = chan[1];
        if (!_waitAction(work, GBA_WAIT_ALL)) {
            return _poweroff(work, GBA_PHASE_ERROR);
        }
        chan++;
    }

    return TRUE;
}

u8 _check_langarea(u8 language)
{
    u8 langarea;
    u8 saveLangarea;
    void *saveData;
    void *gamedata;
    GamedataAttest *attest;

    switch (pokecoloGetCountryCode()) {
    case COUNTRY_JAPAN:
        switch (language) {
        case GBA_LANGUAGE_JAPANESE:
            langarea = LANGAREA_JAPANESE;
            break;
        case GBA_LANGUAGE_ENGLISH:
            langarea = LANGAREA_ENGLISH;
            break;
        case GBA_LANGUAGE_FRENCH:
            langarea = LANGAREA_FRENCH;
            break;
        case GBA_LANGUAGE_ITALIAN:
            langarea = LANGAREA_ITALIAN;
            break;
        case GBA_LANGUAGE_GERMAN:
            langarea = LANGAREA_GERMAN;
            break;
        case GBA_LANGUAGE_SPANISH:
            langarea = LANGAREA_SPANISH;
            break;
        default:
            return FALSE;
        }
        saveData = savedataBiosGetNowSavedataPtr();
        if (saveData != NULL) {
            gamedata = savedataBiosGetGamedataPtr(saveData);
            if (gamedata != NULL) {
                attest = gamedataBiosGetGamedataAtttestPtr(gamedata);
                if (attest != NULL) {
                    saveLangarea = gamedataAttestBiosGetLangareaId(attest);
                    if (saveLangarea == langarea) {
                        return TRUE;
                    }
                }
            }
        }
        break;
    case COUNTRY_USA:
    case COUNTRY_EUROPE:
        switch (language) {
        case GBA_LANGUAGE_JAPANESE:
            langarea = LANGAREA_JAPANESE;
            break;
        case GBA_LANGUAGE_ENGLISH:
            langarea = LANGAREA_ENGLISH;
            break;
        case GBA_LANGUAGE_FRENCH:
            langarea = LANGAREA_FRENCH;
            break;
        case GBA_LANGUAGE_ITALIAN:
            langarea = LANGAREA_ITALIAN;
            break;
        case GBA_LANGUAGE_GERMAN:
            langarea = LANGAREA_GERMAN;
            break;
        case GBA_LANGUAGE_SPANISH:
            langarea = LANGAREA_SPANISH;
            break;
        default:
            return FALSE;
        }
        if (_get_langarea() == langarea) {
            return TRUE;
        }
        break;
    }

    return FALSE;
}

_GBA_Connect_WORK *_open_menu(_MENU_GBAC_TYPE type, unsigned long flags, long start)
{
    _GBA_Connect_WORK *work;
    long i;
    s8 port;

    work = &gbaConectGBA_Connect_WORK;
    work->mFlags = flags;
    work->mType = type;
    work->mPort = chans[start];

    switch (type) {
    case MENU_GBAC_TYPE_1GBA:
        work->mSlot[0] = GBA_SLOT_SAVE;
        work->mSlot[1] = GBA_SLOT_GBA;
        work->mSlot[2] = GBA_SLOT_NONE;
        work->mSlot[3] = GBA_SLOT_NONE;
        break;
    case MENU_GBAC_TYPE_2GBA:
        work->mSlot[0] = GBA_SLOT_SAVE_GBA;
        work->mSlot[1] = GBA_SLOT_GBA;
        work->mSlot[2] = GBA_SLOT_GBA;
        work->mSlot[3] = GBA_SLOT_NONE;
        break;
    case MENU_GBAC_TYPE_3GBA:
        work->mSlot[0] = GBA_SLOT_SAVE;
        work->mSlot[1] = GBA_SLOT_GBA;
        work->mSlot[2] = GBA_SLOT_GBA;
        work->mSlot[3] = GBA_SLOT_GBA;
        break;
    case MENU_GBAC_TYPE_4GBA:
        work->mSlot[0] = GBA_SLOT_SAVE_GBA;
        work->mSlot[1] = GBA_SLOT_GBA;
        work->mSlot[2] = GBA_SLOT_GBA;
        work->mSlot[3] = GBA_SLOT_GBA;
        break;
    }

    for (i = 0; i < 4; i++) {
        port = chans[i];
        work->mPhase[port] = _initial_phase[type][port];
        if (i < start && work->mPhase[port] == GBA_PHASE_WAIT) {
            work->mPhase[port] = GBA_PHASE_COMPLETE;
        }
        work->mIconPosReset[i] = 1;
    }

    menuCB_SE_Play(SE_GBAC_OPEN);
    menuOpen(MENU_ID_GBA_CONNECT, 0);
    menuCB_MessageDispOn(MSG_GBAC_NONE, false);
    _CB.gbaConnectPort = -1;
    _CB.gbaConnectMode = 2;
    return work;
}

u8 _poweroff(_GBA_Connect_WORK *work, _GBA_PHASE phase)
{
    unsigned long msg;

    menuSetEnablePort(menuGetEnablePort() | 1);
    if (!_showDisconnected(work)) {
        work->mPhase[work->mPort] = phase;
    }

    if (_disconnect_shown == TRUE) {
        AGB_Terminate(work->mPort);
        return FALSE;
    }

    if (work->mType == MENU_GBAC_TYPE_4GBA) {
        if (GSinputIsConnected(1)) {
            msg = MSG_GBAC_POWER_OFF;
        } else {
            msg = MSG_GBAC_CONNECT_PAD;
        }
    } else {
        msg = MSG_GBAC_POWER_OFF;
    }
    menuCB_MessageDispOn(msg, true);

    if (msg == MSG_GBAC_CONNECT_PAD) {
        do {
            GSthreadSwitch();
        } while (!GSinputIsConnected(1));
    }

    AGB_Terminate(work->mPort);
    return FALSE;
}

u8 _error(_GBA_Connect_WORK *work, unsigned long msg, _GBA_PHASE phase)
{
    if (_showDisconnected(work)) {
        return (work->mFlags & MENU_GBAC_FLAG_RETRY) != 0;
    }

    work->mPhase[work->mPort] = phase;
    _message(msg, work->mPort);

    if (!(work->mFlags & MENU_GBAC_FLAG_RETRY)) {
        _waitAction(work, GBA_WAIT_MESSAGE);
    } else if (_waitAction(work, GBA_WAIT_ALL)) {
        return TRUE;
    }

    AGB_Terminate(work->mPort);
    return FALSE;
}

u8 _showDisconnected(_GBA_Connect_WORK *work)
{
    long i;
    unsigned long msg;

    _disconnect_shown = FALSE;
    for (i = 0; i < GBAC_DISCONNECT_WAIT; i++) {
        if (_checkDisconnect(work)) {
            break;
        }
        GSthreadSwitch();
    }
    GSsndPlayNow(SND_GBAC_DISCONNECT);

    for (i = 0; i < 4; i++) {
        if (work->mPhase[i] == GBA_PHASE_DISCONNECTED) {
            break;
        }
    }
    if (i > 3 && !(work->mResult & GBA_CONNECT_RESULT_DISCONNECTED)) {
        return FALSE;
    }

    menuSetEnablePort(menuGetEnablePort() | 1);
    if (i == 0) {
        msg = MSG_GBAC_DISCONNECTED_PAD;
    } else if (work->mType == MENU_GBAC_TYPE_4GBA) {
        if (GSinputIsConnected(1)) {
            msg = MSG_GBAC_DISCONNECTED;
        } else {
            msg = MSG_GBAC_CONNECT_PAD;
        }
    } else {
        msg = MSG_GBAC_DISCONNECTED;
    }

    _disconnect_shown = TRUE;
    msgctrlSetValue(MSGCTRL_NUM_INPUT, i + 1);
    menuCB_MessageDispOn(msg, true);

    if (msg == MSG_GBAC_DISCONNECTED_PAD || msg == MSG_GBAC_CONNECT_PAD) {
        do {
            GSthreadSwitch();
        } while (!GSinputIsConnected(1));
    }

    work->mResult = GBA_CONNECT_RESULT_DISCONNECTED;
    return TRUE;
}

u8 _checkDisconnect(_GBA_Connect_WORK *work)
{
    long i;
    u8 disconnected;

    disconnected = FALSE;
    for (i = 0; i <= 3; i++) {
        if ((work->mPhase[i] == GBA_PHASE_COMPLETE || work->mPhase[i] == GBA_PHASE_TRANSFER) &&
            !gbaCommandIsConnected(i + 1)) {
            menuSetEnablePort(menuGetEnablePort() & ~_menu_ports[i]);
            work->mPhase[i] = GBA_PHASE_DISCONNECTED;
            work->mResult = GBA_CONNECT_RESULT_DISCONNECTED;
        }
        if (work->mPhase[i] == GBA_PHASE_DISCONNECTED) {
            disconnected = TRUE;
        }
    }
    return disconnected;
}

long _waitComplete(_GBA_Connect_WORK *work)
{
    work->mResult = GBA_CONNECT_RESULT_NONE;
    while (!AGB_IsComplete(work->mPort)) {
        if (menuIsCheck(MENU_ID_PAD_ERROR)) {
            GSthreadSwitch();
            continue;
        }
        if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
            work->mResult = GBA_CONNECT_RESULT_CANCEL;
            AGB_Terminate(work->mPort);
            return AGB_STATUS_TERMINATED;
        }
        if (work->mResult == GBA_CONNECT_RESULT_DISCONNECTED) {
            return AGB_STATUS_TERMINATED;
        }
        GSthreadSwitch();
    }

    work->mResult = GBA_CONNECT_RESULT_NONE;
    return AGB_GetStatus(work->mPort);
}

u8 _waitAction(_GBA_Connect_WORK *work, _GBA_WAIT_FLAG flag)
{
    u8 unplugged;
    s8 disp;
    u32 type;

    unplugged = TRUE;
    work->mResult = GBA_CONNECT_RESULT_NONE;
    if (work->mPort >= 0 && gba_SIProbe(work->mPort) == SI_GBA) {
        unplugged = FALSE;
    }

    for (;;) {
        if (menuIsCheck(MENU_ID_PAD_ERROR)) {
            GSthreadSwitch();
            continue;
        }

        if ((flag & GBA_WAIT_CANCEL) && (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL)) {
            work->mResult = GBA_CONNECT_RESULT_CANCEL;
            return FALSE;
        }
        if (work->mResult == GBA_CONNECT_RESULT_DISCONNECTED) {
            return FALSE;
        }
        if (work->mPhase[0] == GBA_PHASE_DISCONNECTED) {
            return FALSE;
        }
        if (work->mPhase[1] == GBA_PHASE_DISCONNECTED) {
            return FALSE;
        }
        if (work->mPhase[2] == GBA_PHASE_DISCONNECTED) {
            return FALSE;
        }
        if (work->mPhase[3] == GBA_PHASE_DISCONNECTED) {
            return FALSE;
        }

        if (flag & GBA_WAIT_MESSAGE) {
            disp = menuCB_MessageDispCheck();
            if (disp == 0) {
                work->mResult = GBA_CONNECT_RESULT_DECIDE;
                return TRUE;
            }
            if ((windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_ACCEPT) && disp == -1) {
                work->mResult = GBA_CONNECT_RESULT_DECIDE;
                return TRUE;
            }
        }

        if (flag & GBA_WAIT_INSERT) {
            type = gba_SIProbe(work->mPort);
            if (type != SI_ERROR_BUSY) {
                if (unplugged) {
                    if (type == SI_GBA) {
                        work->mResult = GBA_CONNECT_RESULT_INSERTED;
                        return TRUE;
                    }
                } else if (type != SI_GBA) {
                    unplugged = TRUE;
                }
            }
        }

        GSthreadSwitch();
    }
}

void _menuWait(f32 seconds)
{
    f32 elapsed;

    elapsed = 0.0f;
    while (elapsed < seconds) {
        GSthreadSwitch();
        elapsed += GSgfxVideoGetLastRenderTime() / GSgfxVideoGetVsyncRate();
    }
}

void _message_type2(unsigned long msg, long port)
{
    msgctrlSetValue(MSGCTRL_NUM_INPUT, port + 1);
    menuCB_MessageDispOn(msg, true);
}

void _message(unsigned long msg, long port)
{
    msgctrlSetValue(MSGCTRL_NUM_INPUT, port + 1);
    menuCB_MessageDispOn(msg, false);
}

u8 _get_langarea(void)
{
    u8 langarea;
    void *saveData;
    void *gamedata;
    GamedataAttest *attest;

    langarea = LANGAREA_UNKNOWN;
    saveData = savedataBiosGetNowSavedataPtr();
    if (saveData != NULL) {
        gamedata = savedataBiosGetGamedataPtr(saveData);
        if (gamedata != NULL) {
            attest = gamedataBiosGetGamedataAtttestPtr(gamedata);
            if (attest != NULL) {
                langarea = gamedataAttestBiosGetLangareaId(attest);
                if (langarea == LANGAREA_ENGLISH_EU) {
                    langarea = LANGAREA_ENGLISH;
                }
            }
        }
    }
    return langarea;
}
