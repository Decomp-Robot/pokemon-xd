#include <fightTrainer.h>
#include <app/fight/fightType.h>
#include <app/fight/fightTrainer.h>
#include <app/fight/fightTimer/fightTimer.h>
#include <fightFloor.h>
#include <fightTarget.h>
#include <fightPokemonBios.h>
#include <fightPokemon.h>
#include <exribbon.h>
#include <fightSeq.h>
#include <dolphin/si.h>

#include <GSgfx.h>
#include <GSmsg.h>
#include <GSthread.h>
#include <GSmem.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokemonDB.h>
#include <menuCBRule.h>
#include <gbaCommand.h>
#include <gbaAgb.h>
#include <menuCBBios.h>
#include <FightResult.h>
#include <types.h>
#include <waza.h>

static const long GBA_LANGUAGE_JAPANESE = 1;
static const long GBA_BATTLE_MODE_MULTI = 2;
static const long TRAINER_HOME_PLACE_DEFAULT = 0;

static const u8 CONTROLLER_ID_FIRST = 1;
static const u8 CONTROLLER_ID_SECOND = 2;

/*
 * Link commands of the GBA connection.
 *
 * The GameCube side of the GBA link runs a small command/key protocol: the
 * battle menus send a command word (hand a Pokemon over, ask for the entry
 * result, read the pressed key, ...) and the GBA answers with one. The
 * commands themselves are passed on to the PKJB entry points of the link
 * library, which drives the exchange with the GBA; the per-channel replies
 * are collected here.
 */

// State gbaCommandEntryResult reports in the top byte of the entry result
// word: the result word carries the entry itself, the reply of the exchange is
// still outstanding, or the exchange answered with an error.
#define GBA_ENTRY_RESULT_ENTRY 0
#define GBA_ENTRY_RESULT_NONE 0x02000000
#define GBA_ENTRY_RESULT_ERROR 0x03000000

// What a link-library call answers with: the channel answered the call, the
// port carries no Game Boy Advance (its probe stopped reporting one), or the
// GBA did not answer the reset and status requests the call starts with.
// gbaCommandKeyRead keeps retrying a command while its call answers with one of
// the last two, and the exchange calls report a call that answers
// GBA_LINK_RESULT_NO_GBA as a probe error of the port.
#define GBA_LINK_RESULT_ANSWERED 0
#define GBA_LINK_RESULT_NO_GBA 1
#define GBA_LINK_RESULT_NOT_ANSWERING 2

// Battle status used while a Pokemon is transformed; transformed Deoxys is
// reported in its base form.
#define JOUTAI_DATA_ID_TRANSFORM 16
// Status of the battle floor that carries the fight type of the running
// encounter, which the fight type data of the turn is looked up by.
#define FIGHT_FLOOR_STATUS_FIGHT_TYPE 20
// Ban kind fightOutPokemonCheckCanOutOkWazaBanme reports for a waza that bans
// the Pokemon with a waza: the out value then carries that waza.
#define WAZA_BAN_KIND_FORCED_WAZA 5

enum GbaSiRefreshChannel {
    GBA_SI_REFRESH_CHANNEL = 2
};

enum GbaFormSelector {
    GBA_FORM_SELECTOR_NONE = 0,
    GBA_FORM_SELECTOR_CASTFORM_FIRE = 1,
    GBA_FORM_SELECTOR_CASTFORM_WATER = 2,
    GBA_FORM_SELECTOR_CASTFORM_ICE = 3
};

enum GbaBanReason {
    GBA_BAN_REASON_NONE = 0,
    GBA_BAN_REASON_SHADOW_TAG = 2,
    GBA_BAN_REASON_MAGNET_PULL = 3,
    GBA_BAN_REASON_ARENA_TRAP = 4
};

// Polls a channel spends on a command the link library has not answered yet
// before the channel gives up on it.
#define GBA_KEY_WAIT_LIMIT 10
// SI probe errors a channel collects before the link error is handed back to
// the caller: the first error of a channel is still answered as pending, so
// only the poll that reaches this count ends the call.
#define SI_PROBE_ERROR_REPORT_COUNT 2
// Probes a channel keeps the controller type it cached for a Game Boy Advance
// over before the type is looked up again.
#define GBA_PROBE_HOLD_COUNT 2
// Size of the waza text payload for a six-Pokemon party.
#define GBA_WAZA_TEXT_SIZE 1920
// Bit a battle request sets in a waza nibble when selecting a target for that
// waza requires a target choice; the other bits carry its ban kind.
#define GBA_WAZA_REQUIRES_TARGET_SELECTION 8
// Length of the nickname a battle request reports for one Pokemon, whose last
// byte the request clears.
#define GBA_SUMMARY_NAME_SIZE 10
#define GBA_MAIL_ID_NONE 0xFFu
#define GBA_NICKNAME_END 0xFF
// Number of ribbons the exchange payload reports for the Pokemon it hands over.
#define GBA_ENTRY_RIBBON_COUNT 11

// Ending the request reports when it closes the link exchange itself: the
// battle answered with a result or the fight timer ran out. gbaCommandGetAction
// reads the action back out of the top half of the key word it returns.
#define GBA_ACTION_BATTLE_RESULT 0x50000
#define GBA_ACTION_FIGHT_TIMER 0x40000

// Battle data of the Pokemon that is handed over.
extern "C" void gbaPokemonFromGC(struct Pokemon *pokemon, void *data);
extern "C" long wazaconv(Hero *hero, void *data);

// SI probes and their cached results, shared with the connect-battle menus.
extern u32 _SIProbe_Result[4];
// Result the command layer reported last for each channel.
extern long _keyResult[4];

// Per-channel state of the command layer. `_keyState` is the command the
// channel currently runs, `_keyCounter` counts the unanswered retries of the
// running transfer and `_keyPressed` holds the key word the GBA sent
// back.
extern long _keyState[4];
extern u16 _keyPressed[4];
extern u16 _keyCounter[4];
// Set when the target-selection callback is invoked for a waza.
extern u8 gbaTargetSelected;
// SI probes left before the controller of a channel is looked up again.
extern u32 gbaProbeGateCnt[4];

static u32 _swap32(u32 value);
static u16 _swap16(u16 value);

// Clear the selected channel's probe error counter, or all counters for chan -1.
void gba_SIProbe_ErrorCntClear(s32 chan)
{
    if (chan == -1) {
        gba_SIProbe_ErrorCnt[0] = gba_SIProbe_ErrorCnt[1] = gba_SIProbe_ErrorCnt[2] = gba_SIProbe_ErrorCnt[3] = 0;
    } else {
        gba_SIProbe_ErrorCnt[chan] = 0;
    }
}

u32 gba_SIProbe(long chan)
{
    return _SIProbe_Result[chan];
}

// Leaves the link session the channel had open.
extern "C" void gbaCommandChangePokemonEnd(long chan)
{
    PKJB_ExchangeEnd(chan - 1);
}

// Watches the exchange the last change-Pokemon command started. The wait count
// of the channel counts the polls, and the channel gives up once the exchange
// answered that often without having finished.
extern "C" long gbaCommandChangePokemonIsPrepared(long chan)
{
    long result = PKJB_ExchangeIsPrepared(chan - 1);

    if (result < 0) {
        _keyCounter[chan - 1] = 0;
    } else if (result == GBA_LINK_RESULT_NO_GBA || result == GBA_LINK_RESULT_NOT_ANSWERING) {
        u16 count = ++_keyCounter[chan - 1];

        if (count <= GBA_KEY_WAIT_LIMIT) {
            result = -1;
        }
    }
    return result;
}

// One Pokemon exchange payload containing the party index, species, a copy
// of the Pokemon as the GBA stores it and one byte per ribbon the GBA reports.
struct GbaExchangePayload {
    u32 partyIndex;
    u32 species;
    u8 packet[100];
    u8 ribbons[12];
};

// Hands one Pokemon over to the GBA.
extern "C" long gbaCommandChangePokemon(long chan, struct Pokemon *pokemon, u32 partyIndex, long species)
{
    GbaExchangePayload request;
    long result;

    request.partyIndex = _swap32(partyIndex);
    request.species = _swap32((species << 16) | pokemonBiosGetPokemonDataId(pokemon));
    if (pokemonBiosGetPokemonDataId(pokemon) == POKEMON_DATA_ID_CASTFORM) {
        pokemonBiosSetTokuseiFlag(pokemon, pokemonBiosGetTokuseiFlag(pokemon) == 0);
    }
    gbaPokemonFromGC(pokemon, request.packet);
    memset(request.ribbons, 0, sizeof(request.ribbons));
    {
        u8 *ribbons = request.ribbons;
        s32 ribbon;

        for (ribbon = 0; ribbon < GBA_ENTRY_RIBBON_COUNT; ribbon++) {
            *ribbons++ = exribbonGetNo(ribbon);
        }
    }
    result = PKJB_ExchangeStart(chan - 1, &request);
    _keyCounter[chan - 1] = 0;
    return result;
}

// The party Pokemon selected for a switch by a received key word.
extern "C" u16 gbaCommandGetGBAChangePokemon(u32 key)
{
    return key;
}

// The waza target (one based) of a received key word.
extern "C" u8 gbaCommandGetWazaTarget(u32 key)
{
    return (key >> 8) & 0xFF;
}

// The waza data id of a received key word.
extern "C" u8 gbaCommandGetWazaDataId(u32 key)
{
    return key;
}

// The action of a received key word.
extern "C" u32 gbaCommandGetAction(u32 key)
{
    return key >> 16;
}

// Battle data accessors used to build the GBA battle request.

extern "C" u16 gbaPokemonConditonFromGC(Pokemon *pokemon);
extern "C" long GetWazaDocGBA(void *dst, u16 waza);
static FightOutPokemon *_dummySelect(FightOutPokemon *pokemon, u16 waza, u16 fightType);

struct GbaBattleRequestEntry {
    u16 hp;
    u8 condition;
    u8 itemFlag : 1;
    u8 mailFlag : 1;
    u8 pokerusStrain : 1;
    u8 pokerusDays : 1;
    u8 condHigh : 4;
    u16 item;
    u8 _06[2]; // unused
    u16 waza[4];
    u8 wazaPp[4];
    u8 wazaText[4][80];
};

struct GbaBattleRequestHeader {
    u32 partyDataIds : 24;
    u32 flag : 1;
    u32 mode : 7;
    u32 switchEligibility : 24;
    u32 pokemonCount : 8;
    u32 random1;
    u32 random2;
    u16 species;
    u16 formSelector;
    u16 waza[4];
    u8 wazaPp[4];
    u16 selection;
    u8 forcedWazaIndex : 4;
    u8 selectedWazaIndex : 4;
    u8 blockingPokemonIndex : 4;
    u8 switchBanReason : 4;
};

struct GbaBattleSummary {
    u8 name[GBA_SUMMARY_NAME_SIZE];
    u8 sex : 2;
    u8 nidoranMale : 1; // set for POKEMON_DATA_ID_NIDORAN_M, whose gender is in its species id.
    u8 nidoranFemale : 1; // set for POKEMON_DATA_ID_NIDORAN_F.
    u8 japaneseLanguage : 1;
    u8 controller : 3;
    u8 level;   // Level the Pokemon is reported with.
    u32 pid;    // Personality ID of the Pokemon.
    u16 species;
    u16 hp;
    u16 maxHp;
    u16 form;
};

struct GbaBattleRequestPacket {
    struct GbaBattleRequestHeader header;
    struct GbaBattleRequestEntry entry[6];
    struct GbaBattleSummary summary[4];
};

// Builds the battle request the GBA side needs for one turn of a link battle
// and hands it over: the trainers Pokemon with their waza and state, the
// Pokemon that is asked to act and the summaries of the Pokemon the GBA may
// pick from. Returns the answer of the link library, or the packed battle
// state once the GBA side reported one.
extern "C" long gbaCommandRequestAction(u32 chan, void *target, u32 slot, long flag)
{
    u8 controllers[4];
    FightOutPokemon *selection[4];
    GbaBattleRequestPacket packet;
    u32 random1;
    u32 random2;
    FightOutPokemon *irekaeSelect;
    u8 pokemonNum;
    u8 fightoutPokemonNum;
    long fightType;
    u8 count;
    u8 trainerNum;
    long selectionIndex;
    u16 species;
    FightOutPokemon *targetPokemon;
    u8 entryPokemonNum;
    void *player;
    long i, j;
    u16 banOut;
    u32 battleStatus;
    void *layout;
    u32 partyDataIds;
    u8 kind;
    long n;
    u32 switchEligibility;
    long channelIndex;
    Pokemon *pokemon;
    u16 wazaSelection;
    long ret;

    fightType = fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_TYPE, 0) & FIGHT_TYPE_ID_MASK;
    layout = fightTypeDataBiosGetPtr(fightType);
    trainerNum = fightTypeDataBiosGetTrainerNum(layout);
    entryPokemonNum = fightTypeDataBiosGetEntryPokemonNum(layout);
    fightoutPokemonNum = fightTypeDataBiosGetFightoutPokemonNum(layout);
    count = (trainerNum * fightoutPokemonNum) * 2;
    player = fightTargetGetPtr(FIGHT_TARGET_ID_HOST_TRAINER_1, target, fightType);
    selection[0] = fightTrainerGetValidFightOutPokemonPtr(player, 0);
    controllers[0] = CONTROLLER_ID_FIRST;
    if (trainerNum == 2) {
        void *trainer = fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE_TRAINER_2, player, fightType);
        selection[1] = fightTrainerGetValidFightOutPokemonPtr(trainer, 0);
        controllers[1] = fightTrainerBiosGetControllerId(trainer);
        trainer = fightTargetGetPtr(FIGHT_TARGET_ID_OPPONENT_TRAINER_1, player, fightType);
        selection[2] = fightTrainerGetValidFightOutPokemonPtr(trainer, 0);
        controllers[2] = fightTrainerBiosGetControllerId(trainer);
        trainer = fightTargetGetPtr(FIGHT_TARGET_ID_OPPONENT_TRAINER_2, player, fightType);
        selection[3] = fightTrainerGetValidFightOutPokemonPtr(trainer, 0);
        controllers[3] = fightTrainerBiosGetControllerId(trainer);
    } else if (fightoutPokemonNum == 2) {
        void *trainer;
        selection[1] = fightTrainerGetValidFightOutPokemonPtr(player, 1);
        trainer = fightTargetGetPtr(FIGHT_TARGET_ID_OPPONENT_TRAINER_1, player, fightType);
        selection[2] = fightTrainerGetValidFightOutPokemonPtr(trainer, 0);
        selection[3] = fightTrainerGetValidFightOutPokemonPtr(trainer, 1);
        controllers[1] = CONTROLLER_ID_FIRST;
        controllers[2] = CONTROLLER_ID_SECOND;
        controllers[3] = CONTROLLER_ID_SECOND;
    } else {
        selection[1] = fightTrainerGetValidFightOutPokemonPtr(fightTargetGetPtr(FIGHT_TARGET_ID_OPPONENT_TRAINER_1, player, fightType), 0);
        controllers[1] = CONTROLLER_ID_FIRST;
    }
    targetPokemon = fightTrainerGetValidFightOutPokemonPtr(target, slot);
    packet.header.mode = (fightoutPokemonNum == 2) ? slot + 1 : 0;
    packet.header.flag = flag;
    packet.header.pokemonCount = count;
    partyDataIds = 0;
    switchEligibility = 0;
    for (i = 0; i < entryPokemonNum; i++) {
        long cond;
        FightPokemon *fightPokemon = fightTrainerGetValidFightPokemonPtr(static_cast<FIGHT_TRAINER *>(target), i);
        u8 mailId;
        Pokemon *pokemon;
        u8 pokerus;

        if (fightPokemon == 0) {
            break;
        }
        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
        partyDataIds |= pokemonBiosGetFightTrainerPokemonDataId(pokemon) << (i * 4 + 8);
        switchEligibility |= fightTrainerCheckCanIrekaeFightPokemon(static_cast<FIGHT_TRAINER *>(target), fightPokemon) << (i * 4 + 8);
        pokerus = pokemonBiosGetPokerus(pokemon);
        packet.entry[i].hp = _swap16(pokemonBiosGetHp(pokemon));
        cond = gbaPokemonConditonFromGC(pokemon);
        packet.entry[i].condition = cond;
        packet.entry[i].itemFlag = fightPokemonGetSoubiItemDataId(fightPokemon) != 0;
        mailId = pokemonBiosGetMailId(pokemon);
        packet.entry[i].mailFlag = mailId != GBA_MAIL_ID_NONE;
        packet.entry[i].pokerusDays = (pokerus & 0xF) != 0;
        packet.entry[i].pokerusStrain = (pokerus & 0xF0) != 0;
        packet.entry[i].condHigh = cond >> 8;
        packet.entry[i].item = _swap16(fightPokemonGetSoubiItemDataId(fightPokemon));
        for (j = 0; j < 4; j++) {
            u16 waza = pokemonBiosGetPokemonWazaDataId(pokemon, j);
            packet.entry[i].waza[j] = _swap16(waza);
            packet.entry[i].wazaPp[j] = pokemonBiosGetPokemonWazaPp(pokemon, j);
            GetWazaDocGBA(packet.entry[i].wazaText[j], waza);
        }
    }
    pokemonNum = i;
    partyDataIds |= -(1 << (pokemonNum * 4 + 8));
    packet.header.partyDataIds = _swap32(partyDataIds);
    packet.header.switchEligibility = _swap32(switchEligibility);
    wazaSelection = 0;
    {
        pokemon = fightOutPokemonGetPokemonPtr(targetPokemon);

        fightOutPokemonGetRndStatus(targetPokemon, &random1, &random2);
        packet.header.random1 = _swap32(random1);
        packet.header.random2 = _swap32(random2);
        species = pokemonBiosGetPokemonDataId(pokemon);
        packet.header.species = _swap16(species);
        if (species == POKEMON_DATA_ID_CASTFORM) {
            u16 typeDataId = fightOutPokemonBiosGetZokuseiDataId(targetPokemon, 0);
            long formType;

            if (typeDataId == ZOKUSEI_DATA_ID_FIRE) {
                formType = GBA_FORM_SELECTOR_CASTFORM_FIRE;
            } else if (typeDataId == ZOKUSEI_DATA_ID_WATER) {
                formType = GBA_FORM_SELECTOR_CASTFORM_WATER;
            } else if (typeDataId == ZOKUSEI_DATA_ID_ICE) {
                formType = GBA_FORM_SELECTOR_CASTFORM_ICE;
            } else {
                formType = GBA_FORM_SELECTOR_NONE;
            }
            packet.header.formSelector = _swap16(formType);
        } else {
            packet.header.formSelector = 0;
        }
        if (flag != 0) {
            kind = FIGHT_FLOOR_SWITCH_ALLOWED;
            selectionIndex = 0;
        } else {
            kind = fightFloorCheckFightActionFightOutPokemonIrekaeSelect(
                fightFloorBiosGetFightFloorPtr(), targetPokemon, &irekaeSelect);
            if (kind == FIGHT_FLOOR_SWITCH_BLOCKED_BY_ABILITY) {
                u32 abilityKind;
                u16 tokuseiId = fightOutPokemonGetTokuseiDataId(irekaeSelect);

                if (tokuseiId == TOKUSEI_DATA_ID_SHADOW_TAG) {
                    abilityKind = GBA_BAN_REASON_SHADOW_TAG;
                } else if (tokuseiId == TOKUSEI_DATA_ID_MAGNET_PULL) {
                    abilityKind = GBA_BAN_REASON_MAGNET_PULL;
                } else if (tokuseiId == TOKUSEI_DATA_ID_ARENA_TRAP) {
                    abilityKind = GBA_BAN_REASON_ARENA_TRAP;
                } else {
                    abilityKind = GBA_BAN_REASON_NONE;
                }
                kind = abilityKind;
            }
            for (selectionIndex = count - 1; selectionIndex > 0; selectionIndex--) {
                if (irekaeSelect == selection[selectionIndex]) {
                    break;
                }
            }
        }
        packet.header.switchBanReason = kind;
        packet.header.blockingPokemonIndex = selectionIndex;
        u16 banme;

        packet.header.selectedWazaIndex = fightOutPokemonCheckFightActionWazaSelect(targetPokemon, 0);
        for (i = 0; i < 4; i++) {
            u16 waza;

            gbaTargetSelected = 0;
            waza = pokemonBiosGetPokemonWazaDataId(pokemon, i);
            packet.header.waza[i] = _swap16(waza);
            packet.header.wazaPp[i] = pokemonBiosGetPokemonWazaPp(pokemon, i);
            fightSeqSelectDefensePokemonPtr(targetPokemon, waza, fightType, _dummySelect, 1, 0, -1);
            if (gbaTargetSelected) {
                wazaSelection |= GBA_WAZA_REQUIRES_TARGET_SELECTION << (i * 4);
            }
            {
                u8 ban = fightOutPokemonCheckCanOutOkWazaBanme(targetPokemon, i, 1, &banOut);

                wazaSelection |= ban << (i * 4);
                if (ban == WAZA_BAN_KIND_FORCED_WAZA) {
                    banme = banOut;
                }
            }
        }
        packet.header.selection = _swap16(wazaSelection);
        for (n = 3; n > 0; n--) {
            if (packet.header.waza[n] == _swap16(banme)) {
                break;
            }
        }
        packet.header.forcedWazaIndex = n;
        for (i = 0; i < count; i++) {
            memset(&packet.summary[i], 0, sizeof(GbaBattleSummary));
            FightOutPokemon *entryTarget = selection[i];
            if (entryTarget == targetPokemon) {
                packet.summary[i].name[0] = GBA_NICKNAME_END;
            } else {
                GamedataAttest *attest;
                Pokemon *other = fightOutPokemonGetPokemonPtr(selection[i]);
                attest = pokemonBiosGetAttest(other);
                long len = GScharConvToGBACode(&packet.summary[i], pokemon_GetNickname(other), gamedataAttestBiosGetLangareaId(attest));

                if (len < GBA_SUMMARY_NAME_SIZE) {
                    packet.summary[i].name[len] = GBA_NICKNAME_END;
                }
                species = pokemonBiosGetPokemonDataId(other);
                packet.summary[i].sex = pokemonGetSex(other);
                packet.summary[i].nidoranMale = species == POKEMON_DATA_ID_NIDORAN_M;
                packet.summary[i].nidoranFemale = species == POKEMON_DATA_ID_NIDORAN_F;
                packet.summary[i].controller = controllers[i];
                if (gamedataAttestBiosGetLangareaId(attest) == GBA_LANGUAGE_JAPANESE) {
                    packet.summary[i].japaneseLanguage = 1;
                } else {
                    packet.summary[i].japaneseLanguage = 0;
                }
                packet.summary[i].level = pokemonBiosGetLevel(other);
                packet.summary[i].pid = _swap32(pokemonBiosGetRnd(other));
                packet.summary[i].species =
                    _swap16(pokemonBiosGetPokemonDataId(other));
                packet.summary[i].hp =
                    _swap16(pokemonBiosGetHp(other));
                packet.summary[i].maxHp =
                    _swap16(pokemonBiosGetMaxHp(other));
                packet.summary[i].form = 0;
                if (pokemonBiosGetPokemonDataId(other) == POKEMON_SPECIES_DEOXYS) {
                    if (menuCBRule_DEOKISISUCamouflage() != 0) {
                        if (_CB.battleMode == GBA_BATTLE_MODE_MULTI ||
                            _CB.mTrainer[0].mHomePlace == TRAINER_HOME_PLACE_DEFAULT) {
                            packet.summary[i].form =
                                _swap16(pokemonGetDeoxysSel(controllers[i]));
                        } else {
                            packet.summary[i].form =
                                _swap16(pokemonGetDeoxysSel(controllers[i] + 1));
                        }
                    }
                    if (fightOutPokemonIsJoutaiDataId(selection[i], JOUTAI_DATA_ID_TRANSFORM) == 1) {
                        packet.summary[i].form = 0;
                    }
                }
            }
        }
        memmove(&packet.entry[pokemonNum], packet.summary, count * sizeof(GbaBattleSummary));
        channelIndex = chan - 1;
        PKJB_BattleRequest(channelIndex, (const u8 *)&packet.header,
            sizeof(GbaBattleRequestHeader) + pokemonNum * sizeof(GbaBattleRequestEntry)
                + count * sizeof(GbaBattleSummary));
        gba_SIProbe_ErrorCntClear(channelIndex);
        for (;;) {
            if (fightGetFightResultId() == kFightResultCommunicationError) {
                PKJB_ReturnTitle(channelIndex);
                return GBA_ACTION_BATTLE_RESULT;
            }
            if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1) {
                PKJB_ReturnTitle(channelIndex);
                return GBA_ACTION_FIGHT_TIMER;
            }
            ret = PKJB_BattleGetStatus(channelIndex, &battleStatus);
            if (ret == GBA_LINK_RESULT_NO_GBA) {
                gba_SIProbe_ErrorCnt[chan - 1]++;
                if (gba_SIProbe_ErrorCnt[chan - 1] != SI_PROBE_ERROR_REPORT_COUNT) {
                    ret = -1;
                }
            } else {
                gba_SIProbe_ErrorCntClear(channelIndex);
            }
            if (ret > 0) {
                return ret | GBA_ACTION_BATTLE_RESULT;
            }
            if (ret == GBA_LINK_RESULT_ANSWERED) {
                return _swap32(battleStatus);
            }
            GSthreadSwitch();
        }
    }
}

// Callback the menu installs while it picks the Pokemon of a waza; only its
// being called matters, the argument values are unused.
static FightOutPokemon *_dummySelect(FightOutPokemon *pokemon, u16 waza, u16 fightType)
{
    gbaTargetSelected = 1;
    return 0;
}

// Splits a packed entry result into its six nibbles: the six Pokemon slots
// the GBA reports as available for the exchange.
extern "C" long gbaCommandEntryPokemon(u32 packed, u8 *slots)
{
    slots[0] = packed & 0xF;
    slots[1] = (packed >> 4) & 0xF;
    slots[2] = (packed >> 8) & 0xF;
    slots[3] = (packed >> 12) & 0xF;
    slots[4] = (packed >> 16) & 0xF;
    slots[5] = (packed >> 20) & 0xF;
    return 0;
}

// Polls the result word of the entry request the channel sent.
extern "C" long gbaCommandEntryResult(long chan, u32 *result)
{
    u32 data;
    long channel = chan - 1;
    long ret;

    *result = GBA_ENTRY_RESULT_NONE;
    ret = PKJB_EntryGetStatus(channel, &data);
    if (ret == GBA_LINK_RESULT_NO_GBA) {
        gba_SIProbe_ErrorCnt[chan - 1]++;
        if (gba_SIProbe_ErrorCnt[chan - 1] != SI_PROBE_ERROR_REPORT_COUNT) {
            ret = -1;
        }
    } else {
        gba_SIProbe_ErrorCntClear(channel);
    }
    if (ret < 0) {
        data = GBA_ENTRY_RESULT_NONE;
    } else if (ret != GBA_LINK_RESULT_ANSWERED) {
        *result = GBA_ENTRY_RESULT_ERROR;
        gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_READ);
        return ret;
    } else {
        *result = _swap32(data);
        if ((*result >> 24) == GBA_ENTRY_RESULT_ENTRY) {
            gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_READ);
        }
    }
    return 0;
}

// Sends the requested Pokemon count, minimum count and entry type.
extern "C" void gbaCommandEntryRequest(long chan, u32 pokemonCount, u32 minimumPokemonCount, u32 entryType)
{
    PKJB_EntryRequest(chan - 1, (minimumPokemonCount << 8) | ((entryType << 24) | pokemonCount));
}

// Sends the waza docs of a hero's party to the GBA, in the layout the
// exchange carries them: four waza per Pokemon with the doc of each.
extern "C" void gbaCommandSendWazaText(long chan, Hero *hero)
{
    u8 data[GBA_WAZA_TEXT_SIZE];

    wazaconv(hero, data);
    PKJB_SendWazaText(chan - 1, data);
}

// The key the GBA sent back for the channel, zero while none arrived yet.
extern "C" u16 gbaCommandKeyPressed(long chan)
{
    return _keyPressed[chan - 1];
}

// Whether a controller that answers the link commands is registered on the
// channel and the last command has been answered already.
extern "C" u8 gbaCommandIsConnected(s32 chan)
{
    u8 connected = 0;

    if (_keyState[chan - 1] != GBA_KEY_COMMAND_NONE && _keyResult[chan - 1] == GBA_LINK_RESULT_ANSWERED) {
        connected = 1;
    }
    return connected;
}

// Makes the channel run the given command and clears the state of the
// previous one. Returns the command that was running before.
extern "C" long gbaCommandSetKeyState(long chan, long state)
{
    long previous = _keyState[chan - 1];

    _keyState[chan - 1] = state;
    _keyResult[chan - 1] = 0;
    _keyCounter[chan - 1] = 0;
    _keyPressed[chan - 1] = 0;
    return previous;
}

// Reads the key the GBA answers the current command with. The command of the
// channel decides what the answer means: GBA_KEY_COMMAND_READ waits for the
// key word, GBA_KEY_COMMAND_IDLE for the answer of the idle send and
// GBA_KEY_COMMAND_TRANSFER means the GBA is reachable but its key is not read;
// any other command means the channel is not reachable. A command the library
// keeps busy for longer than the wait limit is given up on.
extern "C" long gbaCommandKeyRead(long chan)
{
    u16 key;
    long ret;
    long probe;
    long i;

    // The link key layer runs on port 2; its probe keeps the cached controller
    // types of all four SI ports current.
    if (chan == GBA_SI_REFRESH_CHANNEL) {
        for (i = 0; i < 4; i++) {
            probe = SIProbe(i);

            if (menuCB_GetToolBattleModeFlag() == 1 && menuCBBios_CurrentlyWorkingToolBattle() != 0) {
                probe = SI_GBA;
            }
            if (probe == SI_GBA) {
                _SIProbe_Result[i] = probe;
                gbaProbeGateCnt[i] = GBA_PROBE_HOLD_COUNT;
            } else if (gbaProbeGateCnt[i] == 0) {
                _SIProbe_Result[i] = probe;
            } else {
                gbaProbeGateCnt[i] = gbaProbeGateCnt[i] - 1;
            }
        }
    }
    switch (_keyState[chan - 1]) {
    case GBA_KEY_COMMAND_READ:
        ret = PKJB_GetKey(chan - 1, &key);
        if (ret == GBA_LINK_RESULT_ANSWERED) {
            _keyCounter[chan - 1] = 0;
            _keyPressed[chan - 1] = key;
        } else if (ret <= GBA_LINK_RESULT_NOT_ANSWERING
            && ++_keyCounter[chan - 1] <= GBA_KEY_WAIT_LIMIT) {
            ret = GBA_LINK_RESULT_ANSWERED;
        } else {
            _keyPressed[chan - 1] = 0;
            gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_NONE);
        }
        break;
    case GBA_KEY_COMMAND_IDLE:
        ret = PKJB_IdleSend(chan - 1);
        if (ret == GBA_LINK_RESULT_ANSWERED) {
            _keyCounter[chan - 1] = 0;
        } else if (ret <= GBA_LINK_RESULT_NOT_ANSWERING
            && ++_keyCounter[chan - 1] <= GBA_KEY_WAIT_LIMIT) {
            ret = GBA_LINK_RESULT_ANSWERED;
        } else {
            _keyPressed[chan - 1] = 0;
            gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_NONE);
        }
        _keyPressed[chan - 1] = 0;
        break;
    case GBA_KEY_COMMAND_TRANSFER:
        _keyPressed[chan - 1] = 0;
        ret = GBA_LINK_RESULT_ANSWERED;
        break;
    default:
        _keyPressed[chan - 1] = 0;
        ret = GBA_LINK_RESULT_NO_GBA;
        break;
    }
    _keyResult[chan - 1] = ret;
    return ret;
}

// Byte order of the words of the packets the link exchanges with the GBA.
static u32 _swap32(u32 value)
{
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
}

// Byte order of the half words of those packets.
static u16 _swap16(u16 value)
{
    return (value << 8) | (value >> 8);
}
