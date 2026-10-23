#include <fade.h>
#include <savedata.h>
#include <heroBios.h>
#include <Pokemon.h>
#include <gsFlag.h>
#include <menuSub.h>
#include <types.h>

// Debug menu test entries: PDA flags and functions, D2 presents, the dark
// Pokemon snatch list, the memo, the evolution screen and the relive hall.
// These entries are reached through the debug menu.
#pragma force_active on

// Values the D2 present can be started with, and the argument the thread that
// runs it is given for each of them.
extern u32 gD2PresentArgs[6];
// Number of words gD2PresentArgs holds, one id and one argument per D2 present,
// and the number of ids it names.
#define D2_PRESENT_ARGS_COUNT 6
#define D2_PRESENT_COUNT 3
// Moves the evolution screen offers at once.
#define EVOLUTION_MOVE_COUNT 20

// Rate the fade helpers fade at.
extern const float gFadeSpeed;

// The screen fade mode the entries below use to hide a transition: out before
// it, in once it has run.
#define FADE_MODE_IN 2
#define FADE_MODE_OUT 3
// Priority and stack the two threads the entries below start are run on.
#define THREAD_PRIORITY_NORMAL 1
#define THREAD_STACK_SIZE 0x4000
// Message text the two entries below push a floor with fills one of its
// substitution slots with: the slot number, and the move demo end message the
// world map fills the same slot with.
#define MESSAGE_VALUE_SLOT_ID 0x59
#define MESSAGE_DEMO_END_ID 0x1772
// Move the test entry below opens the move learn screen with. The move learn
// screen keeps it in the extra slot past the Pokemon's four moves.
#define WAZA_LEARN_NO 37

// Flags the PDA keeps its panels and its state in, and the flags and floors the
// entries below jump to.
#define PDA_ENABLE_FLAG 1009
#define PDA_DPM_FLAG 1010
#define PDA_ESA_FLAG 1011
#define PDA_MR_FLAG 1012
#define PDA_SYOUGOU_FLAG 1730
#define FLOOR_PC_BOX 913
#define FLOOR_RELIVEHALL_TUTORIAL 935

// ---- engine, library and game entry points -------------------------------

extern "C" {
// menuSub.a
void menuSubCloseNumberInputWait(void);

// menu scripts and floors
void menuScriptReciveMail(void);
void menuWorldMapMXDebug(void);
// Opens the D2 present of the given kind and keeps the state in its own global.
void d2presentOpen(s32 kind);
void GSfloorPush(s32 floor);
void reliveHallDebugPokemonSetup(s32 index);

// PDA menu scripts
void menuScriptPdaAddFunc(u32 function);
void menuScriptReadMail(u32 mail);

// threads
// A thin wrapper in the floor code that hands out the thread group id of the floor
// on top of the stack; the entries below start their thread inside it.
u32 floorGetStackGroupID(void);
// The thread allocates its own stack. groupID says which group the thread
// belongs to, usesFPU whether its context keeps the floating point registers,
// and closeProtected keeps GSthreadClose from closing the thread.
u32 GSthreadCreate(s32 priority, u32 groupID, u32 stackSize, s32 usesFPU, s32 closeProtected,
    void (*entry)(void));
void GSthreadSetArgs(u32 thread, s32 count, ...);

// pokemon
void *gamedataGetStatus(s32 a, s32 b);
void pokemonCreate(Pokemon *pokemon, u16 species, s32 level, void *status);
u32 memoDataGetPokemonRndFromID(s32 a, u16 species);
u32 memoDataGetPokemonTrainerRndFromID(s32 a, u16 species);
void pokemonSetPid(Pokemon *pokemon, u32 rnd);
void pokemonSetCatchTrainerRnd(Pokemon *pokemon, u32 rnd);
u8 pokemonCheckValid(Pokemon *pokemon);
void pokemonSetDarkPokemonStatus(Pokemon *pokemon, u16 shadowID);
void heroCatchPokemon(s32 a, Pokemon *pokemon, s32 b, s32 c, s32 d);

// dark Pokemon
u16 darkpokemonBiosGetScenarioNum(void);
DarkPokemon *darkPokemonStoredGetFromShadowID(u16 shadowID);
u16 darkPokemonStoredGetSpecies(DarkPokemon *darkPokemon);
void darkPokemonBiosSetFightFlag(u16 shadowID, bool flag);

// memo
void memoDataSetMemoFlag(s32 memo);
void memoInitDebug(s32 kind);
void memoDataSetPokemonID(s32 slot, u16 pokemonID);

// relive hall
// The two fill the trainer data's pockets with pokemon made for the debug
// entries and hand each one to the dark Pokemon's place setter. The "Best" one
// fills all eight pockets completely (30 slots each), making the first slot of
// every pocket a dark Pokemon and giving each one a 0.5 dark ratio. The "All" one
// fills only three slots of pockets 0-6 and then a complete pocket 7; that last
// pocket is a party of its own and holds no dark Pokemon, while the sparse
// pockets use a 0.1 dark ratio.
void reliveHallDebugMakePocketBest(void);
void reliveHallDebugMakePocketAll(void);

// screens
// The third argument is handed to the evolution floor through its shared work
// area and reaches the evolution demo as its input flag: zero runs the demo
// without any button check, nonzero lets the demo take the player's input and
// leave early.
s32 evolutionOpen(Pokemon *from, Pokemon *to, s32 inputEnabled, u16 *wazaNo, s32 wazaCount, u8 *result);
void menuPokemonStatusOpenWazaLearn(Pokemon *pokemon, s32 waza);
void msgctrlSetValue(s32 id, s32 value);
void menuPokemonChange(void);
}

// Helpers the entries below share. The leading underscore of the ones that take
// part in the dark Pokemon's own calls is part of their name in the link map.
void _dbgMenuMenuTestPdaFlagSet(unsigned long flag);
void _debugPokemonCreate(Pokemon *pokemon, u16 species);
void _setDPStatus(long shadowID, darkPokemonStatus status);
long _getDPStatus(long shadowID);
void testEvolution(void);

extern "C" void dbgMenuMenuTestD2PresentThread(s32 kind);
extern "C" u16 debugPokemonCreateSpecies(u32 shadowID);

void makePocketDeubugMakeDarkPokemonReset(void);

// ---- PDA -----------------------------------------------------------------

// Test entry: add the function of the PDA the player enters.
extern "C" s32 dbgMenuMenuTestPdaAddFunc(void)
{
    u32 function = 0;

    if (menuSubOpenNumberInput(0, &function, 1, 50, 50, 0)) {
        menuSubCloseNumberInput();
        menuSubCloseNumberInputWait();
        if (function > 1) {
            function = 0;
        }
        menuScriptPdaAddFunc(function);
    }
    menuSubCloseNumberInput();
    return 0;
}

// Test entry: read the mail the player enters.
extern "C" s32 dbgMenuMenuTestMailRead(void)
{
    u32 mail = 0;

    if (menuSubOpenNumberInput(0, &mail, 1, 50, 50, 0)) {
        menuSubCloseNumberInput();
        menuSubCloseNumberInputWait();
        menuScriptReadMail(mail);
    }
    menuSubCloseNumberInput();
    return 0;
}

// Test entry: run the script that hands the player a mail.
extern "C" s32 dbgMenuMenuTestMailRecive(void)
{
    menuScriptReciveMail();
    return 0;
}

// Test entry: set the title the PDA's entries are listed under.
extern "C" s32 dbgMenuMenuTestPdaSyougou(void)
{
    u32 syougou = GSflagGet(PDA_SYOUGOU_FLAG);

    for (;;) {
        if (!menuSubOpenNumberInput(syougou, &syougou, 1, 50, 50, 0)) {
            break;
        }
        if (syougou > 7) {
            continue;
        }
        GSflagSet(PDA_SYOUGOU_FLAG, syougou);
    }
    menuSubCloseNumberInput();
    return 0;
}

// Test entry: flag the PDA's mail report panel on.
extern "C" s32 dbgMenuMenuTestPdaMR(void)
{
    _dbgMenuMenuTestPdaFlagSet(PDA_MR_FLAG);
    return 1;
}

// Test entry: flag the ESA panel on.
extern "C" s32 dbgMenuMenuTestPdaEsa(void)
{
    _dbgMenuMenuTestPdaFlagSet(PDA_ESA_FLAG);
    return 1;
}

// Test entry: flag the PDA's dark Pokemon map on.
extern "C" s32 dbgMenuMenuTestPdaDPM(void)
{
    _dbgMenuMenuTestPdaFlagSet(PDA_DPM_FLAG);
    return 1;
}

// Test entry: enable the PDA.
extern "C" s32 dbgMenuMenuTestPdaEnable(void)
{
    _dbgMenuMenuTestPdaFlagSet(PDA_ENABLE_FLAG);
    return 1;
}

// Set the flag `flag` to the value the player enters; a cancelled number input
// leaves the flag as it was.
void _dbgMenuMenuTestPdaFlagSet(unsigned long flag)
{
    u8 flagValue = flagGetBooleanValue(flag);
    u32 value = flagValue;

    for (;;) {
        if (!menuSubOpenNumberInput(value, &value, 1, 50, 50, 0)) {
            break;
        }
        if (value != 0) {
            flagSetTrue(flag);
        } else {
            flagSetFalse(flag);
        }
    }
    menuSubCloseNumberInput();
}

// ---- D2 present ----------------------------------------------------------

// Body of the thread the D2 present test entry starts. The kind the entry
// picked is handed over as the thread's argument and passed straight on.
extern "C" void dbgMenuMenuTestD2PresentThread(s32 kind)
{
    d2presentOpen(kind);
}

// Test entry: run the D2 present the player enters.
extern "C" s32 dbgMenuMenuTestD2Present(s32 id)
{
    u32 index;
    s32 args[D2_PRESENT_ARGS_COUNT];
    s32 arg;
    u32 thread;
    s32 i;

    for (i = 0; i < D2_PRESENT_ARGS_COUNT; i++) {
        args[i] = gD2PresentArgs[i];
    }
    for (index = 0; index < D2_PRESENT_COUNT; index++) {
        if (id == args[index * 2]) {
            break;
        }
    }
    arg = args[index * 2 + 1];
    thread = GSthreadCreate(THREAD_PRIORITY_NORMAL, floorGetStackGroupID(), THREAD_STACK_SIZE, 1, 1, (void (*)(void))dbgMenuMenuTestD2PresentThread);
    GSthreadSetArgs(thread, 1, arg);
    return 0;
}

// Test entry: open the world map's own debug window.
extern "C" s32 dbgMenuMenuTestWorldMapMXDebug(void)
{
    menuWorldMapMXDebug();
    return 0;
}

// ---- dark Pokemon --------------------------------------------------------

// Leave every dark Pokemon of the scenario that is in the shadow data already
// marked as relived.
extern "C" s32 dbgMenuEntrySnatchListRelive(void)
{
    s32 i;

    for (i = 0; i <= darkpokemonBiosGetScenarioNum(); i++) {
        if (_getDPStatus(i) >= DARK_POKEMON_STATUS_FIGHT) {
            _setDPStatus(i, DARK_POKEMON_STATUS_RELIVED);
        }
    }
    return 1;
}

// Make a pokemon of the species `species`, with the random numbers the species
// is stored under, for the debug entries that hand one out.
void _debugPokemonCreate(Pokemon *pokemon, u16 species)
{
    u32 rnd;
    u32 trainerRnd;

    pokemonCreate(pokemon, species, 10, gamedataGetStatus(0, 1));
    rnd = memoDataGetPokemonRndFromID(0, species);
    trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
    pokemonSetPid(pokemon, rnd);
    pokemonSetCatchTrainerRnd(pokemon, trainerRnd);
}

// Test entry: catch every dark Pokemon of the scenario that is in the shadow
// data already, and leave each of them marked as snatched.
extern "C" s32 dbgMenuEntrySnatchListSuccess(void)
{
    s32 i;

    for (i = 0; i <= darkpokemonBiosGetScenarioNum(); i++) {
        if (_getDPStatus(i) >= DARK_POKEMON_STATUS_FIGHT) {
            Pokemon pokemon;
            u16 species;

            species = debugPokemonCreateSpecies(i);
            _debugPokemonCreate(&pokemon, species);
            pokemonSetDarkPokemonStatus(&pokemon, i);
            heroCatchPokemon(0, &pokemon, 0, 4, 1);
            _setDPStatus(i, DARK_POKEMON_STATUS_SNATCH_SUCCESS);
        }
    }
    return 1;
}

// Test entry: mark every dark Pokemon of the scenario as one that fights.
extern "C" s32 dbgMenuEntrySnatchListAll(void)
{
    s32 i;

    for (i = 0; i <= darkpokemonBiosGetScenarioNum(); i++) {
        if (_getDPStatus(i) == DARK_POKEMON_STATUS_NONE) {
            _setDPStatus(i, DARK_POKEMON_STATUS_FIGHT);
        }
    }
    return 1;
}

// Test entry: make the dark Pokemon the player enters fight the player.
extern "C" s32 dbgMenuEntrySnatchList(void)
{
    u32 shadowID = 0;

    for (;;) {
        DarkPokemon *darkPokemon;

        if (!menuSubOpenNumberInput(shadowID, &shadowID, 1, 50, 50, 0)) {
            break;
        }
        darkPokemon = darkPokemonStoredGetFromShadowID(shadowID);
        if (darkPokemon == 0) {
            continue;
        }
        darkPokemonBiosSetFightFlag(shadowID, true);
        darkPokemon->setUpdate(true);
    }
    menuSubCloseNumberInput();
    return 1;
}

// Species the dark Pokemon of the given shadow id is stored under.
extern "C" u16 debugPokemonCreateSpecies(u32 shadowID)
{
    return darkPokemonStoredGetSpecies(darkPokemonStoredGetFromShadowID(shadowID));
}

// Leave the dark Pokemon of the given shadow id in `status`; a shadow id the
// scenario does not hold keeps the status it has.
void _setDPStatus(long shadowID, darkPokemonStatus status)
{
    DarkPokemon *darkPokemon = darkPokemonStoredGetFromShadowID(shadowID);

    if (darkPokemonStoredGetSpecies(darkPokemon) != 0) {
        darkPokemon->setStatus(status);
        darkPokemon->setUpdate(true);
    }
}

// Status the dark Pokemon of the given shadow id is in, or 0 when the scenario
// does not hold it.
long _getDPStatus(long shadowID)
{
    DarkPokemon *darkPokemon = darkPokemonStoredGetFromShadowID(shadowID);

    if (darkPokemonStoredGetSpecies(darkPokemon) != 0) {
        return darkPokemon->getStatus();
    }
    return 0;
}

// ---- memo ----------------------------------------------------------------

// Test entry: mark the memo as finished.
extern "C" s32 dbgMenuMenuTestCompleteMemo(void)
{
    memoDataSetMemoFlag(0);
    return 1;
}

// Test entry: start the memo over.
extern "C" s32 dbgMenuMenuTestMemoInitDebug(void)
{
    memoInitDebug(0);
    return 1;
}

// Test entry: add the memo entry of the pokemon the player enters.
extern "C" s32 dbgMenuMenuTestAddPokemonMemo(void)
{
    u32 pokemonID = 0;

    for (;;) {
        if (!menuSubOpenNumberInput(pokemonID, &pokemonID, 1, 50, 50, 0)) {
            break;
        }
        memoDataSetPokemonID(0, pokemonID);
    }
    menuSubCloseNumberInput();
    return 1;
}

// Test entry: leave the memo as it is.
extern "C" s32 dbgMenuMenuTestMemoKeep(void)
{
    return 1;
}

// ---- evolution -----------------------------------------------------------

// Run the evolution of the hero's first two party pokemon.
void testEvolution(void)
{
    // One result byte per offered move.
    u8 result[EVOLUTION_MOVE_COUNT];
    u16 wazaNo[EVOLUTION_MOVE_COUNT];
    s32 i;
    Pokemon *from;
    Pokemon *to;

    from = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 0);
    if (!pokemonCheckValid(from)) {
        return;
    }
    to = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 1);
    if (!pokemonCheckValid(to)) {
        return;
    }
    fadeSet(FADE_MODE_OUT, gFadeSpeed);
    fadeCheck(1);
    for (i = 0; i < 2; i++) {
        wazaNo[i] = i + 1;
    }
    evolutionOpen(from, to, 1, wazaNo, 2, result);
    fadeSet(FADE_MODE_IN, gFadeSpeed);
    fadeCheck(1);
}

// Test entry: run the evolution of the hero's first two party pokemon on a
// thread of its own.
extern "C" s32 dbgMenuMenuTestEvolution(void)
{
    GSthreadCreate(THREAD_PRIORITY_NORMAL, floorGetStackGroupID(), THREAD_STACK_SIZE, 1, 1, testEvolution);
    return 0;
}

// ---- menus that open a screen of their own -------------------------------

// Test entry: open the move learn screen of the hero's first party pokemon.
extern "C" s32 dbgMenuMenuTestWazaMenu(void)
{
    Pokemon *pokemon;
    void *heroBase;

    heroBase = savedataGetStatus(0, SAVE_DATA_HERO);
    if (heroBase == 0) {
        return 0;
    }
    pokemon = heroBiosGetPokemonPtr(heroBase, 0);
    if (!pokemonCheckValid(pokemon)) {
        return 0;
    }
    menuPokemonStatusOpenWazaLearn(pokemon, WAZA_LEARN_NO);
    return 0;
}

// Test entry: open the party change screen.
extern "C" s32 dbgMenuMenuTestPokemonChangeMenu(void)
{
    msgctrlSetValue(MESSAGE_VALUE_SLOT_ID, MESSAGE_DEMO_END_ID);
    menuPokemonChange();
    return 0;
}

// Test entry: push the floor of the player's PC box.
extern "C" s32 dbgMenuMenuTestPCBox(void)
{
    GSfloorPush(FLOOR_PC_BOX);
    return 0;
}

// Test entry: push the floor of the relive hall tutorial.
extern "C" s32 dbgMenuMenuTestRelivehallTutorialMenu(void)
{
    msgctrlSetValue(MESSAGE_VALUE_SLOT_ID, MESSAGE_DEMO_END_ID);
    GSfloorPush(FLOOR_RELIVEHALL_TUTORIAL);
    return 0;
}

// ---- relive hall ---------------------------------------------------------

// Test entry: set the hero's best party pokemon up for the relive hall.
extern "C" s32 dbgMenuMenuTestReliveHallPokemonSetBest(void)
{
    makePocketDeubugMakeDarkPokemonReset();
    reliveHallDebugMakePocketBest();
    reliveHallDebugPokemonSetup(2);
    return 0;
}

// Test entry: set the whole party up for the relive hall.
extern "C" s32 dbgMenuMenuTestReliveHallPokemonSetAll(void)
{
    makePocketDeubugMakeDarkPokemonReset();
    reliveHallDebugMakePocketAll();
    reliveHallDebugPokemonSetup(2);
    return 0;
}

#pragma force_active reset
