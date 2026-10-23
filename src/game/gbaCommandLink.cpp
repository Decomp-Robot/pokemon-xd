// The link sits on the SI ports, so the SDK's SI header comes first: it
// declares the probe and controller types, and brings in the SDK's scalar
// types, which the game headers use as well.
#include <dolphin/si.h>

#include <GSgfx.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <gbaCommand.h>
#include <menuCBBios.h>
#include <types.h>

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

// Species the link exchange special-cases and the abilities that keep a
// Pokemon from being switched out.
#define SPECIES_NIDORAN_F 0x1D
#define SPECIES_NIDORAN_M 0x20
#define SPECIES_JIRACHI 0x181
#define SPECIES_DEOXYS 0x19A
#define ABILITY_SHADOW_TAG 0x17
#define ABILITY_MAGNET_PULL 0x2A
#define ABILITY_ARENA_TRAP 0x47

// Elemental type data ids the ban kind of a Pokemon that may not be sent out
// is read from, in the order the packet reports them as the ban kind (1, 2, 3)
// the GBA has to apply for it; any other type is reported as no ban.
#define ZOKUSEI_DATA_ID_BAN_1 10
#define ZOKUSEI_DATA_ID_BAN_2 11
#define ZOKUSEI_DATA_ID_BAN_3 15
// State data id the battle layer keeps while a Pokemon may not report its
// form: the packet sends the base form of such a Pokemon.
#define JOUTAI_DATA_ID_FORM_HIDDEN 0x10
// Status of the battle floor that carries the fight type of the running
// encounter, which the fight type data of the turn is looked up by.
#define FIGHT_FLOOR_STATUS_FIGHT_TYPE 0x14
// Battle records fightTargetGetPtr resolves a trainer by: the first side of a
// battle is its host side, whose first trainer is the player of this GameCube,
// and the ids below name that side's trainers or the trainers of the side
// that opposes it (relative to the record passed as the target).
#define FIGHT_TARGET_HOST_TRAINER_1 0xB  // the player's own trainer.
#define FIGHT_TARGET_HOST_TRAINER_2 7    // the trainer that partners it.
#define FIGHT_TARGET_OPPONENT_TRAINER_1 9
#define FIGHT_TARGET_OPPONENT_TRAINER_2 0xA
// Ban kind fightOutPokemonCheckCanOutOkWazaBanme reports for a waza that bans
// the Pokemon with a waza: the out value then carries that waza.
#define WAZA_BAN_KIND_FORCED_WAZA 5

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
// Size of the waza text the exchange carries for one waza.
#define GBA_WAZA_TEXT_SIZE 0x780
// Bit a battle request sets in the selection nibble of one waza when the menu
// picked that waza; the other bits of the nibble carry its ban kind.
#define GBA_WAZA_SELECTION_PICKED 8
// Length of the nickname a battle request reports for one Pokemon, whose last
// byte the request clears.
#define GBA_SUMMARY_NAME_SIZE 0x0A
// Ribbons the entry request reports for the Pokemon it hands over, and the
// length of the field they go into, whose last byte stays zero.
#define GBA_ENTRY_RIBBON_COUNT 11
// Size and alignment of the scratch buffer the GBA file transfer runs in.
#define GBA_FILE_BUFFER_SIZE 0x20000
#define GBA_FILE_BUFFER_ALIGN 0x20

// Ending the request reports when it closes the link exchange itself: the
// battle answered with a result or the fight timer ran out. gbaCommandGetAction
// reads the action back out of the top half of the key word it returns.
#define GBA_ACTION_BATTLE_RESULT 0x50000
#define GBA_ACTION_FIGHT_TIMER 0x40000

// Entry points of the link library the channel commands are handed to.
extern "C" {
long PKJB_ReturnTitle(long chan);
long PKJB_ExchangeIsPrepared(long chan);
long PKJB_EntryRequest(long chan, void *data);
long PKJB_GetKey(long chan, u16 *key);
long PKJB_IdleSend(long chan);
long PKJB_SendWazaText(long chan, void *data);
long PKJB_EntryGetStatus(long chan, u32 *data);
long PKJB_EntryRequestWait(long chan, u32 data);
long PKJB_ReturnTitleWait(long chan);
long PKJB_BattleGetStatus(long chan, u32 *status);
long PKJB_BattleRequest(long chan, void *data, long size);
}

// Runtime the command layer builds on: the cooperative scheduler's thread
// yield, the aligned allocator the link scratch buffer comes from and the
// changed flag of the colosseum rule sets the save keeps.
extern "C" {
// Changed flag of the colosseum rule sets in the save; while it is raised the
// channel reports its SI port as a Game Boy Advance.
u8 menuCBRulesetGetDirty(void);
void GSthreadSwitch(void);
// Aligned allocation out of the GSmem current heap, the allocator GSmemFree
// pairs with; the GSmem module carries two copies of this body.
void *GSmemAllocAlignedDefaultHeap(unsigned long size, unsigned long alignment);
void GSmemFree(void *ptr);
}

// Battle data of the Pokemon that is handed over.
extern "C" u8 pokemon_GetTokuseiFlag(struct Pokemon *pokemon);
extern "C" void pokemon_SetTokuseiFlag(struct Pokemon *pokemon, bool flag);
extern "C" void gbaPokemonFromGC(struct Pokemon *pokemon, void *data);
extern "C" u8 exribbonGetNo(long index);
extern "C" long wazaconv(Hero *hero, void *data);

// SI probes and their cached results, shared with the connect-battle menus.
extern u32 _SIProbe_Result[4];
// Result the command layer reported last for each channel.
extern long _keyResult[4];

// Per-channel state of the command layer. `gbaKeyState` is the command the
// channel currently runs, `gbaKeyWaitCnt` counts the unanswered retries of the
// running transfer and `gbaCommandKeyStates` holds the key word the GBA sent
// back.
extern long gbaKeyState[4];
extern u16 gbaCommandKeyStates[4];
extern u16 gbaKeyWaitCnt[4];
// Set by the Pokemon selection callback while the menu is picking one.
extern u8 gbaBackFBCalled;
// SI probes left before the controller of a channel is looked up again.
extern u32 gbaProbeGateCnt[4];
// Scratch buffer the GBA file transfer runs in.
extern void *gbaFileBuffer;

extern "C" unsigned long gbaPacketSwapLong(unsigned long value);
extern "C" unsigned short gbaPacketSwapShort(unsigned short value);

// Leaves the link session the channel had open.
extern "C" void gbaCommandChangePokemonEnd(long chan)
{
    PKJB_ReturnTitle(chan - 1);
}

// Watches the exchange the last change-Pokemon command started. The wait count
// of the channel counts the polls, and the channel gives up once the exchange
// answered that often without having finished.
extern "C" long gbaCommandChangePokemonIsPrepared(long chan)
{
    long result = PKJB_ExchangeIsPrepared(chan - 1);

    if (result < 0) {
        gbaKeyWaitCnt[chan - 1] = 0;
    } else if (result == 1 || result == 2) {
        u16 count = ++gbaKeyWaitCnt[chan - 1];

        if (count <= GBA_KEY_WAIT_LIMIT) {
            result = -1;
        }
    }
    return result;
}

// One entry request, as the link library sends it out: the waza the Pokemon is
// asked to use, its species together with the partner slot, a copy of the
// Pokemon as the GBA stores it and one byte per ribbon the GBA reports.
struct GbaEntryRequest {
    u32 wazaTarget;
    u32 speciesPartner;
    u8 packet[0x64];
    u8 ribbons[0xC];
};

// Hands one Pokemon over to the GBA.
extern "C" long gbaCommandChangePokemon(long chan, struct Pokemon *pokemon, u32 wazaTarget, long partner)
{
    GbaEntryRequest request;
    long result;

    request.wazaTarget = gbaPacketSwapLong(wazaTarget);
    request.speciesPartner = gbaPacketSwapLong((partner << 16) | pokemonBiosGetPokemonDataId(pokemon));
    if (pokemonBiosGetPokemonDataId(pokemon) == SPECIES_JIRACHI) {
        pokemon_SetTokuseiFlag(pokemon, pokemon_GetTokuseiFlag(pokemon) == 0);
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
    result = PKJB_EntryRequest(chan - 1, &request);
    gbaKeyWaitCnt[chan - 1] = 0;
    return result;
}

// The waza id of a received key word.
extern "C" u16 gbaCommandGetWazaId(u32 key)
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

// Battle layout and battle state of the fight that is running. The fight floor
// answers the fight type index of the running encounter and, for one of its
// sides, the trainer record that side fights with; the fight type data record
// the index resolves to carries the fight kind at +0 (the trainer number the
// request reports), the opposing trainer's number at +1 and the active Pokemon
// per player at +2.
extern "C" u16 fightFloorGetStatus(long a, long b, long c, long d);
extern "C" void *fightTypeDataBiosGetPtr(long type);
extern "C" u8 fightTypeDataBiosGetTrainerNum(void *fightTypeData);
extern "C" u8 fightTypeDataGetOpposingTrainerNum(void *fightTypeData);
extern "C" u8 fightTypeDataGetActivePokemonPerPlayerNum(void *fightTypeData);
extern "C" void *fightTargetGetPtr(long kind, void *target, long status);
extern "C" u8 fightTrainer_GetControllerId(void *trainer);

// Records the fight module keeps for the Pokemon of a battle: FightPokemon is
// one Pokemon of a battle trainer -- the fight trainer data holds six of them,
// each with the battle copy of the Pokemon at +4 -- and FightOutPokemon one
// Pokemon that is out in the battle, which keeps the FightPokemon it was made
// from at +4. Both records belong to the fight module, so only pointers to them
// are declared here.
struct FightPokemon;
struct FightOutPokemon;

// The battle Pokemon record that sits on top of a FightPokemon: the trainer-AI
// record the fight code keeps, and the dark Pokemon one the fight data holds.
struct DeckTrainerAI : public Pokemon {
    u8 getLastPPDetermRate() const;
};
struct DeckDarkPokemon : public DeckTrainerAI {
    u8 getUnderTakerFlag() const;
};

// Readers of the two records: the index'th FightPokemon of the given fight
// trainer data and the index'th FightOutPokemon of the given trainer data,
// checked with fightPokemonCheckValid / fightOutPokemonCheckValid so both
// answer null when the slot does not hold a valid Pokemon. The same index also
// reaches the record's Pokemon through the stats start pointers below.
extern "C" FightOutPokemon *fightTrainerGetFightoutPokemonPtrValid(void *trainerData, u16 index);
extern "C" FightPokemon *fightTrainerGetFightPokemonPtrValid(void *trainerData, u16 index);
extern "C" DeckTrainerAI *fightPokemonGetStatsStartPtr(FightPokemon *fightPokemon);
extern "C" DeckTrainerAI *fightOutPokemonGetStatsStartPtr(FightOutPokemon *fightOutPokemon);
extern "C" u16 pokemon_GetFightTrainerPokemonDataId(DeckTrainerAI *pokemon);
extern "C" u8 fightTrainerCheckCanIrekaeFightPokemon(void *trainer, FightPokemon *fightPokemon);
extern "C" u16 fightPokemonGetItemDataId(FightPokemon *fightPokemon);
extern "C" long pokemon_GetAttest(DeckTrainerAI *pokemon);
extern "C" u16 gbaPokemonConditonFromGC(DeckTrainerAI *pokemon);
extern "C" u8 pokemonGetDeoxysSel(u8 sel);
extern "C" u8 menuCBRule_DEOKISISUCamouflage(void);
extern "C" u8 pokemonAttestGetLanguage(long attest);
extern "C" u32 pokemon_GetPid(DeckTrainerAI *pokemon);
// Writes the single byte name the GBA stores from a UTF-16 name: every
// character is looked up in the gba_code table the region and the language of
// the Pokemon pick, and characters outside it become 0xB7. Returns the number
// of characters written.
extern "C" long GScharConvToGBACode(void *dst, const u16 *name, long language);
extern "C" u16 fightOutPokemonGetZokuseiDataId(FightOutPokemon *fightOutPokemon, long index);
extern "C" u8 fightOutPokemonCheckCanOutOkWazaBanme(FightOutPokemon *fightOutPokemon, u16 index, long flag, u16 *out);
extern "C" u8 fightOutPokemonCheckFightActionWazaSelect(FightOutPokemon *fightOutPokemon, long index);
extern "C" u8 fightOutPokemonIsJoutaiDataId(FightOutPokemon *fightOutPokemon, long id);
extern "C" void fightOutPokemonGetRndStatus(FightOutPokemon *fightOutPokemon, u32 *o1, u32 *o2);
extern "C" u8 fightFloorCheckFightActionFightOutPokemonIrekaeSelect(void *battleData, FightOutPokemon *fightOutPokemon, FightOutPokemon **out);
extern "C" u16 fightOutPokemonGetTokuseiDataId(FightOutPokemon *fightOutPokemon);
extern "C" void *fightFloorGetPtr(void);
extern "C" FightOutPokemon *fightSeqSelectDefensePokemonPtr(FightOutPokemon *fightOutPokemon, u16 waza, long cbData, long (*cb)(GStexture *, unsigned long, void *), long a, long b, long c);
extern "C" u8 fightTimerAllIsOver(void);
extern "C" u8 fightFloorIsUseFightTimerAll(long flag);
extern "C" long GetWazaDocGBA(void *dst, u16 waza);
long myBackFB(GStexture *texture, unsigned long key, void *data);

struct GbaBattleRequestEntry {
    u16 hp;
    u8 condition;
    u8 itemFlag : 1;
    u8 darkFlag : 1;
    u8 rateHigh : 1;
    u8 rateLow : 1;
    u8 condHigh : 4;
    u16 item;
    u8 _06[0x02]; // 0x06-0x07: gap before the waza list.
    u16 waza[4];
    u8 wazaPp[4];
    u8 wazaText[4][0x50];
};

struct GbaBattleRequestHeader {
    u32 state : 24;
    u32 flag : 1;
    u32 mode : 7;
    u32 ability : 24;
    u32 trainerCount : 8;
    u32 random1;
    u32 random2;
    u16 species;
    u16 banOut;
    u16 waza[4];
    u8 wazaPp[4];
    u16 selection;
    u8 selHigh : 4;
    u8 selLow : 4;
    u8 banHigh : 4;
    u8 banLow : 4;
};

struct GbaBattleSummary {
    u8 name[GBA_SUMMARY_NAME_SIZE];
    u8 sex : 2;
    u8 nidoranMale : 1; // set for SPECIES_NIDORAN_M, whose gender is in its species id.
    u8 nidoranFemale : 1; // set for SPECIES_NIDORAN_F.
    u8 flag : 1;
    u8 controller : 3;
    u8 level;   // 0x0B: level the Pokemon is reported with.
    u32 pid;    // 0x0C: personality id of the Pokemon.
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
    u8 playerNum;
    long status;
    u8 count;
    u8 trainerNum;
    long selectionIndex;
    u16 species;
    FightOutPokemon *targetPokemon;
    u8 trainerCount;
    void *player;
    long i, j;
    u16 banOut;
    u32 battleStatus;
    void *layout;
    u32 state;
    u8 kind;
    long n;
    u32 ability;
    long channelIndex;
    DeckTrainerAI *pokemon;
    u16 state2;
    long ret;

    status = fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_TYPE, 0);
    layout = fightTypeDataBiosGetPtr(status);
    trainerNum = fightTypeDataBiosGetTrainerNum(layout);
    trainerCount = fightTypeDataGetOpposingTrainerNum(layout);
    playerNum = fightTypeDataGetActivePokemonPerPlayerNum(layout);
    count = (trainerNum * playerNum) * 2;
    player = fightTargetGetPtr(FIGHT_TARGET_HOST_TRAINER_1, target, status);
    selection[0] = fightTrainerGetFightoutPokemonPtrValid(player, 0);
    controllers[0] = 1;
    if (trainerNum == 2) {
        void *trainer = fightTargetGetPtr(FIGHT_TARGET_HOST_TRAINER_2, player, status);
        selection[1] = fightTrainerGetFightoutPokemonPtrValid(trainer, 0);
        controllers[1] = fightTrainer_GetControllerId(trainer);
        trainer = fightTargetGetPtr(FIGHT_TARGET_OPPONENT_TRAINER_1, player, status);
        selection[2] = fightTrainerGetFightoutPokemonPtrValid(trainer, 0);
        controllers[2] = fightTrainer_GetControllerId(trainer);
        trainer = fightTargetGetPtr(FIGHT_TARGET_OPPONENT_TRAINER_2, player, status);
        selection[3] = fightTrainerGetFightoutPokemonPtrValid(trainer, 0);
        controllers[3] = fightTrainer_GetControllerId(trainer);
    } else if (playerNum == 2) {
        void *trainer;
        selection[1] = fightTrainerGetFightoutPokemonPtrValid(player, 1);
        trainer = fightTargetGetPtr(FIGHT_TARGET_OPPONENT_TRAINER_1, player, status);
        selection[2] = fightTrainerGetFightoutPokemonPtrValid(trainer, 0);
        selection[3] = fightTrainerGetFightoutPokemonPtrValid(trainer, 1);
        controllers[1] = 1;
        controllers[2] = 2;
        controllers[3] = 2;
    } else {
        selection[1] = fightTrainerGetFightoutPokemonPtrValid(fightTargetGetPtr(FIGHT_TARGET_OPPONENT_TRAINER_1, player, status), 0);
        controllers[1] = 1;
    }
    targetPokemon = fightTrainerGetFightoutPokemonPtrValid(target, slot);
    packet.header.mode = (playerNum == 2) ? slot + 1 : 0;
    packet.header.flag = flag;
    packet.header.trainerCount = count;
    state = 0;
    ability = 0;
    for (i = 0; i < trainerCount; i++) {
        long cond;
        FightPokemon *fightPokemon = fightTrainerGetFightPokemonPtrValid(target, i);
        u8 dark;
        DeckTrainerAI *pokemon;
        u8 rate;

        if (fightPokemon == 0) {
            break;
        }
        pokemon = fightPokemonGetStatsStartPtr(fightPokemon);
        state |= pokemon_GetFightTrainerPokemonDataId(pokemon) << (i * 4 + 8);
        ability |= fightTrainerCheckCanIrekaeFightPokemon(target, fightPokemon) << (i * 4 + 8);
        rate = pokemon->getLastPPDetermRate();
        packet.entry[i].hp = gbaPacketSwapShort(battlePokemonStatusGetCurrentHp(pokemon));
        cond = gbaPokemonConditonFromGC(pokemon);
        packet.entry[i].condition = cond;
        packet.entry[i].itemFlag = fightPokemonGetItemDataId(fightPokemon) != 0;
        dark = ((DeckDarkPokemon *)pokemon)->getUnderTakerFlag();
        packet.entry[i].darkFlag = dark != 0xFFu;
        packet.entry[i].rateLow = (rate & 0xF) != 0;
        packet.entry[i].rateHigh = (rate & 0xF0) != 0;
        packet.entry[i].condHigh = cond >> 8;
        packet.entry[i].item = gbaPacketSwapShort(fightPokemonGetItemDataId(fightPokemon));
        for (j = 0; j < 4; j++) {
            u16 waza = pokemonBiosGetPokemonWazaDataId(pokemon, j);
            packet.entry[i].waza[j] = gbaPacketSwapShort(waza);
            packet.entry[i].wazaPp[j] = pokemonBiosGetPokemonWazaPp(pokemon, j);
            GetWazaDocGBA(packet.entry[i].wazaText[j], waza);
        }
    }
    pokemonNum = i;
    state |= -(1 << (pokemonNum * 4 + 8));
    packet.header.state = gbaPacketSwapLong(state);
    packet.header.ability = gbaPacketSwapLong(ability);
    state2 = 0;
    {
        pokemon = fightOutPokemonGetStatsStartPtr(targetPokemon);

        fightOutPokemonGetRndStatus(targetPokemon, &random1, &random2);
        packet.header.random1 = gbaPacketSwapLong(random1);
        packet.header.random2 = gbaPacketSwapLong(random2);
        species = pokemonBiosGetPokemonDataId(pokemon);
        packet.header.species = gbaPacketSwapShort(species);
        if (species == SPECIES_JIRACHI) {
            // Ban the packet reports for the Pokemon that may not be sent
            // out, as the kind the GBA has to apply for it.
            u16 zokuseiDataId = fightOutPokemonGetZokuseiDataId(targetPokemon, 0);
            long banKind;

            if (zokuseiDataId == ZOKUSEI_DATA_ID_BAN_1) {
                banKind = 1;
            } else if (zokuseiDataId == ZOKUSEI_DATA_ID_BAN_2) {
                banKind = 2;
            } else if (zokuseiDataId == ZOKUSEI_DATA_ID_BAN_3) {
                banKind = 3;
            } else {
                banKind = 0;
            }
            packet.header.banOut = gbaPacketSwapShort(banKind);
        } else {
            packet.header.banOut = 0;
        }
        if (flag != 0) {
            kind = 0;
            selectionIndex = 0;
        } else {
            kind = fightFloorCheckFightActionFightOutPokemonIrekaeSelect(
                fightFloorGetPtr(), targetPokemon, &irekaeSelect);
            if (kind == 2) {
                u32 abilityKind;
                u16 ability = fightOutPokemonGetTokuseiDataId(irekaeSelect);

                if (ability == ABILITY_SHADOW_TAG) {
                    abilityKind = 2;
                } else if (ability == ABILITY_MAGNET_PULL) {
                    abilityKind = 3;
                } else if (ability == ABILITY_ARENA_TRAP) {
                    abilityKind = 4;
                } else {
                    abilityKind = 0;
                }
                kind = abilityKind;
            }
            for (selectionIndex = count - 1; selectionIndex > 0; selectionIndex--) {
                if (irekaeSelect == selection[selectionIndex]) {
                    break;
                }
            }
        }
        packet.header.banLow = kind;
        packet.header.banHigh = selectionIndex;
        u16 banme;

        packet.header.selLow = fightOutPokemonCheckFightActionWazaSelect(targetPokemon, 0);
        for (i = 0; i < 4; i++) {
            u16 waza;

            gbaBackFBCalled = 0;
            waza = pokemonBiosGetPokemonWazaDataId(pokemon, i);
            packet.header.waza[i] = gbaPacketSwapShort(waza);
            packet.header.wazaPp[i] = pokemonBiosGetPokemonWazaPp(pokemon, i);
            fightSeqSelectDefensePokemonPtr(targetPokemon, waza, status, myBackFB, 1, 0, -1);
            if (gbaBackFBCalled) {
                state2 |= GBA_WAZA_SELECTION_PICKED << (i * 4);
            }
            {
                u8 ban = fightOutPokemonCheckCanOutOkWazaBanme(targetPokemon, i, 1, &banOut);

                state2 |= ban << (i * 4);
                if (ban == WAZA_BAN_KIND_FORCED_WAZA) {
                    banme = banOut;
                }
            }
        }
        packet.header.selection = gbaPacketSwapShort(state2);
        for (n = 3; n > 0; n--) {
            if (packet.header.waza[n] == gbaPacketSwapShort(banme)) {
                break;
            }
        }
        packet.header.selHigh = n;
        for (i = 0; i < count; i++) {
            memset(&packet.summary[i], 0, sizeof(GbaBattleSummary));
            FightOutPokemon *entryTarget = selection[i];
            if (entryTarget == targetPokemon) {
                packet.summary[i].name[0] = 0xFF;
            } else {
                long attest;
                DeckTrainerAI *other = fightOutPokemonGetStatsStartPtr(selection[i]);
                attest = pokemon_GetAttest(other);
                long len = GScharConvToGBACode(&packet.summary[i], pokemon_GetNickname(other), pokemonAttestGetLanguage(attest));

                if (len < GBA_SUMMARY_NAME_SIZE) {
                    packet.summary[i].name[len] = 0xFF;
                }
                species = pokemonBiosGetPokemonDataId(other);
                packet.summary[i].sex = pokemonGetSex(other);
                packet.summary[i].nidoranMale = species == SPECIES_NIDORAN_M;
                packet.summary[i].nidoranFemale = species == SPECIES_NIDORAN_F;
                packet.summary[i].controller = controllers[i];
                if (pokemonAttestGetLanguage(attest) == 1) {
                    packet.summary[i].flag = 1;
                } else {
                    packet.summary[i].flag = 0;
                }
                packet.summary[i].level = pokemonBiosGetLevel(other);
                packet.summary[i].pid = gbaPacketSwapLong(pokemon_GetPid(other));
                packet.summary[i].species =
                    gbaPacketSwapShort(pokemonBiosGetPokemonDataId(other));
                packet.summary[i].hp =
                    gbaPacketSwapShort(battlePokemonStatusGetCurrentHp(other));
                packet.summary[i].maxHp =
                    gbaPacketSwapShort(battlePokemonStatusGetMaxHp(other));
                packet.summary[i].form = 0;
                if (pokemonBiosGetPokemonDataId(other) == SPECIES_DEOXYS) {
                    if (menuCBRule_DEOKISISUCamouflage() != 0) {
                        if (menuCBTaisenWork.battleMode == 2 || menuCBTaisenWork.mTrainer[0].mHomePlace == 0) {
                            packet.summary[i].form =
                                gbaPacketSwapShort(pokemonGetDeoxysSel(controllers[i]));
                        } else {
                            packet.summary[i].form =
                                gbaPacketSwapShort(pokemonGetDeoxysSel(controllers[i] + 1));
                        }
                    }
                    if (fightOutPokemonIsJoutaiDataId(selection[i], JOUTAI_DATA_ID_FORM_HIDDEN) == 1) {
                        packet.summary[i].form = 0;
                    }
                }
            }
        }
        memmove(&packet.entry[pokemonNum], packet.summary, count * sizeof(GbaBattleSummary));
        channelIndex = chan - 1;
        PKJB_BattleRequest(channelIndex, &packet.header,
            sizeof(GbaBattleRequestHeader) + pokemonNum * sizeof(GbaBattleRequestEntry)
                + count * sizeof(GbaBattleSummary));
        gba_SIProbe_ErrorCntClear(channelIndex);
        for (;;) {
            if (battleGetResult() == 1) {
                PKJB_ReturnTitleWait(channelIndex);
                return GBA_ACTION_BATTLE_RESULT;
            }
            if (fightFloorIsUseFightTimerAll(0) == 1 && fightTimerAllIsOver() == 1) {
                PKJB_ReturnTitleWait(channelIndex);
                return GBA_ACTION_FIGHT_TIMER;
            }
            ret = PKJB_BattleGetStatus(channelIndex, &battleStatus);
            if (ret == GBA_LINK_RESULT_NO_GBA) {
                SIProbeErrorCnt[chan - 1]++;
                if (SIProbeErrorCnt[chan - 1] != SI_PROBE_ERROR_REPORT_COUNT) {
                    ret = -1;
                }
            } else {
                gba_SIProbe_ErrorCntClear(channelIndex);
            }
            if (ret > 0) {
                return ret | GBA_ACTION_BATTLE_RESULT;
            }
            if (ret == GBA_LINK_RESULT_ANSWERED) {
                return gbaPacketSwapLong(battleStatus);
            }
            GSthreadSwitch();
        }
    }
}

// Callback the menu installs while it picks the Pokemon of a waza; only its
// being called matters, the argument values are unused.
long myBackFB(GStexture *texture, unsigned long key, void *data)
{
    gbaBackFBCalled = 1;
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
        SIProbeErrorCnt[chan - 1]++;
        if (SIProbeErrorCnt[chan - 1] != SI_PROBE_ERROR_REPORT_COUNT) {
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
        *result = gbaPacketSwapLong(data);
        if ((*result >> 24) == GBA_ENTRY_RESULT_ENTRY) {
            gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_READ);
        }
    }
    return 0;
}

// Sends the request word of the entry exchange: the trainer slot, the entry
// kind and the Pokemon the trainer picked.
extern "C" void gbaCommandEntryRequest(long chan, u32 trainer, u32 kind, u32 partner)
{
    PKJB_EntryRequestWait(chan - 1, (kind << 8) | ((partner << 24) | trainer));
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
    return gbaCommandKeyStates[chan - 1];
}

// Whether a controller that answers the link commands is registered on the
// channel and the last command has been answered already.
extern "C" u8 gbaCommandIsConnected(s32 chan)
{
    u8 connected = 0;

    if (gbaKeyState[chan - 1] != 0 && _keyResult[chan - 1] == 0) {
        connected = 1;
    }
    return connected;
}

// Makes the channel run the given command and clears the state of the
// previous one. Returns the command that was running before.
extern "C" long gbaCommandSetKeyState(long chan, long state)
{
    long previous = gbaKeyState[chan - 1];

    gbaKeyState[chan - 1] = state;
    _keyResult[chan - 1] = 0;
    gbaKeyWaitCnt[chan - 1] = 0;
    gbaCommandKeyStates[chan - 1] = 0;
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
    if (chan == 2) {
        for (i = 0; i < 4; i++) {
            probe = SIProbe(i);

            if (menuCBBios_GetEncountFlag() == 1 && menuCBRulesetGetDirty() != 0) {
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
    switch (gbaKeyState[chan - 1]) {
    case GBA_KEY_COMMAND_READ:
        ret = PKJB_GetKey(chan - 1, &key);
        if (ret == GBA_LINK_RESULT_ANSWERED) {
            gbaKeyWaitCnt[chan - 1] = 0;
            gbaCommandKeyStates[chan - 1] = key;
        } else if (ret <= GBA_LINK_RESULT_NOT_ANSWERING
            && ++gbaKeyWaitCnt[chan - 1] <= GBA_KEY_WAIT_LIMIT) {
            ret = 0;
        } else {
            gbaCommandKeyStates[chan - 1] = 0;
            gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_NONE);
        }
        break;
    case GBA_KEY_COMMAND_IDLE:
        ret = PKJB_IdleSend(chan - 1);
        if (ret == GBA_LINK_RESULT_ANSWERED) {
            gbaKeyWaitCnt[chan - 1] = 0;
        } else if (ret <= GBA_LINK_RESULT_NOT_ANSWERING
            && ++gbaKeyWaitCnt[chan - 1] <= GBA_KEY_WAIT_LIMIT) {
            ret = 0;
        } else {
            gbaCommandKeyStates[chan - 1] = 0;
            gbaCommandSetKeyState(chan, GBA_KEY_COMMAND_NONE);
        }
        gbaCommandKeyStates[chan - 1] = 0;
        break;
    case GBA_KEY_COMMAND_TRANSFER:
        gbaCommandKeyStates[chan - 1] = 0;
        ret = 0;
        break;
    default:
        gbaCommandKeyStates[chan - 1] = 0;
        ret = GBA_LINK_RESULT_NO_GBA;
        break;
    }
    _keyResult[chan - 1] = ret;
    return ret;
}

// Byte order of the words of the packets the link exchanges with the GBA.
extern "C" unsigned long gbaPacketSwapLong(unsigned long value)
{
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
}

// Byte order of the half words of those packets.
extern "C" unsigned short gbaPacketSwapShort(unsigned short value)
{
    return (value << 8) | (value >> 8);
}

// Releases the scratch buffer of the GBA file transfer.
extern "C" void GSscratchWaitForCompletion(void)
{
    GSmemFree(gbaFileBuffer);
    gbaFileBuffer = 0;
}

// Hands out the scratch buffer the GBA file transfer runs in.
extern "C" void AGB_FileBuffAlloc(void)
{
    gbaFileBuffer = GSmemAllocAlignedDefaultHeap(GBA_FILE_BUFFER_SIZE, GBA_FILE_BUFFER_ALIGN);
}

