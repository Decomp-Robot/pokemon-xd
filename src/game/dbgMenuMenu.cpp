#include <esaba.h>
#include <menu.h>
#include <GSflag.h>
#include <Pokemon.h>
#include <gamedata.h>
#include <hero.h>
#include <WorldMap.h>
#include <GSflag.h>
#include <types.h>
#include <winSprite.h>
#include <menuSub.h>
#include <menuOrre.h>

// Debug menu entries for the world map, the Pokespot ("esaba") and the Under
// ("undertaker"), and the cursor hooks of the windows those entries open.
//
// The two world map entries open their window with the same argument record,
// which the world map cursor hook edits: the map the cursor stands on, its
// flag value and how many maps the world map is made of.

// ---- data this unit owns -------------------------------------------------

// Argument the world map test windows are opened with.
struct WorldMapMenuArg {
    u16 mMapIndex;   // 0x00: map the cursor stands on.
    u16 mFlagValue;  // 0x02: flag value of that map, cleared when the window opens.
    u16 mMapCount;   // 0x04: number of maps in the world map list.
    u16 _06;         // 0x06: unknown.
};

// The two rows of the Pokespot select window, one list entry each: the window
// hands back the row the player picked, which chooses how the pokemon of the
// spot is generated - row 0 from a pokemon created for the spot, row 1 from
// the hero's random number.
static const u32 lPokespotSelectGenerateRow[] = {0x0000F86F};
static const u32 lPokespotSelectHeroRow[] = {0x0000F146};

// Places the Under menu can send the hero to; undertakerGetPlace fills them.
static u32 _undertakerFloorID[8];

// Encounter numbers of the Orre Colosseum test entry, picked by the number the
// player enters there; anything outside the table starts its first entry.
u16 _testEncount[0x1C] = {
    0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
    0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72,
};

// Argument the world map windows are opened with.
static WorldMapMenuArg _debugWorldMapData;
// Number of places undertakerGetPlace reported for the Under place window.
static u32 _undertakerFloorCount;
// Pokespot the Pokespot entries work on, and the species the Pokespot window
// reports for it.
static s32 _esabaPlace;
static u16 _esabaSelectPokemon;

// Window ids of the debug menus the entries below open.
#define WIN_ID_WORLDMAP_SHOW 0xD7
#define WIN_ID_WORLDMAP_FLAGSET 0xC4
#define WIN_ID_UNDERTAKER_DISP 0xC0
#define WIN_ID_ESABA_DISP 0xB7
#define WIN_ID_POKESPOT 0x10A
#define WIN_ID_UNDERTAKER_PLACE 0x111

// Fixed species placed in the Pokespot without generating a Pokemon.
#define SPECIES_POKESPOT_FIXED_1 0x19D
#define SPECIES_POKESPOT_FIXED_2 0x19E
// Flag that turns the Under icon on and off.
#define UNDERTAKER_ICON_FLAG 0x5AD

// ---- engine, library and game entry points -------------------------------

extern "C" {
// menu.a
u8 menuIsOpen(s32 menuID);
// The 3rd argument is the cursor the window starts on, the 7th and later
// arguments are the window's own argument record.

// flags

// menu scripts
void menuScriptOpenWorldMap(u16 mapIndex);
void menuScriptUsohachi(void);
void menuScriptInfoRelive(s32 a, s32 b);
void menuScriptInfoEsa(void);
void menuScriptTutorial(void);
void menuScriptEevee(void);
void menuMoveDemo(s32 demo);
void mewWazaSequence(void);
void menuOrreColosseum(u16 encountID);
void menuIntUnderTakerOpenIcon(void);

// pokemon
u32 memoDataGetPokemonRndFromID(s32 a, u16 species);
void pokemonBiosSetRnd(Pokemon *pokemon, u32 rnd);
void pokemonBiosSetCatchTrainerRnd(Pokemon *pokemon, u32 rnd);

// pokespot ("esaba")
void esabaGet(s32 place);
void esabaPut(s32 place);
void esabaSet(s32 place);
u16 esabaGetPokemonMax(u16 place);
void esabaSetPokemon(u16 place, u16 species);

// Under ("undertaker")
u32 undertakerGetPlace(u32 *places);
void undertakerSetPlace(u32 place);
long undertakerGetStartFlagID(void);
}

// Menu scripts.
extern "C" void menuScriptUsohachi2(void);
extern "C" void menuNintendoWarning(void);
extern "C" void menuScriptWazaMeijin(s32 value);
extern "C" void menuScriptWazaBasan(s32 *value);

// Helpers defined further down the unit.
void _pokemonCreate(Pokemon *pokemon, unsigned short species, unsigned long rnd);
void _esabaSetPokemon(long place, long species);

// ---- world map -----------------------------------------------------------

// Save flag the map at `mapIndex` keeps its state in, -1 while the map has
// none.
s32 _getWorldMapFlagData(long mapIndex)
{
    WorldMapMapData *map = worldmapXD_data;

    map += mapIndex;

    if (map->mFlagID == 0) {
        return -1;
    }
    return GSflagGet(map->mFlagID);
}

// Store `value` in the flag of the map at `mapIndex`.
void _setWorldMapFlagData(long mapIndex, long value)
{
    WorldMapMapData *map = worldmapXD_data;

    map += mapIndex;

    if (map->mFlagID != 0) {
        GSflagSet(map->mFlagID, value);
    }
}

#pragma force_active on

// Test entry: run its menu script.
extern "C" s32 dbgMenuMenuTestUsohachi2(void)
{
    menuScriptUsohachi2();
    return 0;
}

// Test entry: play the Usohachi menu script.
extern "C" s32 dbgMenuMenuTestUsohachi(void)
{
    menuScriptUsohachi();
    return 0;
}

// Test entry: show the world map, starting on the first map.
extern "C" s32 dbgMenuMenuTestWorlMapShow(void)
{
    s32 result;

    _debugWorldMapData.mMapIndex = 0;
    _debugWorldMapData.mFlagValue = 0;
    _debugWorldMapData.mMapCount = (u16)*worldmapXD_data_number;

    result = menuOpenCustom(WIN_ID_WORLDMAP_SHOW, windowGetActiveID(), 0, 0, 1, 1,
                            &_debugWorldMapData);
    menuClose(WIN_ID_WORLDMAP_SHOW);
    menuCloseSync(WIN_ID_WORLDMAP_SHOW, 1);

    if (result >= 0) {
        menuScriptOpenWorldMap(_debugWorldMapData.mMapIndex);
    }
    return 0;
}

// Cursor hook of the world map windows: the top row moves the map cursor and
// the bottom row edits the flag of the map it stands on. The show window has
// no bottom row, so it only moves the map cursor.
extern "C" s32 dbgMenuMenuWorldMapCursor(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key = windowGetKeyInfo();
    WorldMapMenuArg *arg = (WorldMapMenuArg *)window->mArg;

    if (window->mMenuID != WIN_ID_WORLDMAP_SHOW) {
        if (key->mKeyRepeat & 0x1) {
            if (--window->mCursor.y < 0) {
                window->mCursor.y = 0;
            }
        } else if (key->mKeyRepeat & 0x2) {
            if (++window->mCursor.y > 1) {
                window->mCursor.y = 1;
            }
        }
    }

    switch (window->mCursor.y) {
    case 0:
        if (key->mKeyRepeat & 0x8) {
            if (arg->mMapIndex < (u16)(arg->mMapCount - 1)) {
                arg->mMapIndex++;
            }
        } else if (key->mKeyRepeat & 0x4) {
            if (arg->mMapIndex != 0) {
                arg->mMapIndex--;
            }
        }
        break;

    case 1: {
        s32 flagValue = _getWorldMapFlagData(arg->mMapIndex);

        if (flagValue < 0) {
            break;
        }
        if (key->mKeyRepeat & 0x8) {
            if (flagValue < 2) {
                flagValue++;
            }
        } else if (key->mKeyRepeat & 0x4) {
            if (flagValue != 0) {
                flagValue--;
            }
        }
        _setWorldMapFlagData(arg->mMapIndex, flagValue);
        break;
    }
    }
    return 0;
}

// Test entry: edit the flag of every map in a row until the window is left.
extern "C" s32 dbgMenuMenuTestWorlMapFlagSet(void)
{
    _debugWorldMapData.mMapIndex = 0;
    _debugWorldMapData.mFlagValue = 0;
    _debugWorldMapData.mMapCount = (u16)*worldmapXD_data_number;

    while (menuOpenCustom(WIN_ID_WORLDMAP_FLAGSET, windowGetActiveID(), 0, 0, 1, 1,
                          &_debugWorldMapData) != -1) {
    }
    menuClose(WIN_ID_WORLDMAP_FLAGSET);
    menuCloseSync(WIN_ID_WORLDMAP_FLAGSET, 1);
    return 0;
}

// ---- pokespot ("esaba") and Under ("undertaker") -------------------------

// Cursor hook of the Pokespot window: step through the pokemon the spot
// holds. The spot adds three rows of its own below them.
extern "C" s32 dbgMenuEsabaDebugCursor(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key = windowGetKeyInfo();
    u16 count = esabaGetPokemonMax((u16)_esabaPlace);

    if (key->mKeyRepeat & 0x5) {
        if (--window->mCursor.y < 0) {
            window->mCursor.y = 0;
        }
    } else if (key->mKeyRepeat & 0xA) {
        u16 maxRow = count + 3;

        if (++window->mCursor.y >= maxRow) {
            window->mCursor.y = (s16)(maxRow - 1);
        }
    }
    return 0;
}

// Cursor hook of the Under place window: step through the places the Under
// menu lists.
extern "C" s32 dbgMenuUnderTakerebugCursor(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key = windowGetKeyInfo();

    _undertakerFloorCount = undertakerGetPlace(_undertakerFloorID);
    if (_undertakerFloorCount != 0) {
        if (key->mKeyRepeat & 0x5) {
            if (--window->mCursor.y < 0) {
                window->mCursor.y = 0;
            }
        } else if (key->mKeyRepeat & 0xA) {
            if (++window->mCursor.y >= _undertakerFloorCount) {
                window->mCursor.y = (s16)(_undertakerFloorCount - 1);
            }
        }
    }
    return 0;
}

// Test entry: run its menu script.
extern "C" s32 dbgMenuMenuTestNintendoWarning(void)
{
    menuNintendoWarning();
    return 0;
}

// Test entry: send the hero to the place the Under menu picked.
extern "C" s32 dbgMenuMenuUnderTakerIconAppear(void)
{
    s32 place = menuOpenCustom(WIN_ID_UNDERTAKER_PLACE, windowGetActiveID(), 0, 0, 1, 0);

    if (place >= 0 && _undertakerFloorID[place] != 0) {
        undertakerSetPlace(_undertakerFloorID[place]);
    }
    menuClose(WIN_ID_UNDERTAKER_PLACE);
    menuCloseSync(WIN_ID_UNDERTAKER_PLACE, 1);
    return place < 0;
}

// Test entry: switch the flag that turns the Under icon on and off.
extern "C" s32 dbgMenuMenuUnderTakerIconFlagOnOff(void)
{
    if ((u8)GSflagTest(UNDERTAKER_ICON_FLAG)) {
        GSflagOff(UNDERTAKER_ICON_FLAG);
    } else {
        GSflagOn(UNDERTAKER_ICON_FLAG);
    }
    return 0;
}

// Test entry: open the Under icon.
extern "C" s32 dbgMenuMenuUnderTakerIconOnOff(void)
{
    menuIntUnderTakerOpenIcon();
    return 0;
}

// Test entry: toggle the display of the Under window.
extern "C" s32 dbgMenuMenuUnderTakerDisp(void)
{
    if (menuIsOpen(WIN_ID_UNDERTAKER_DISP)) {
        menuClose(WIN_ID_UNDERTAKER_DISP);
    } else {
        menuOpenCustom(WIN_ID_UNDERTAKER_DISP, 0, 0, 0, 1, 0);
    }
    return 0;
}

// Test entry: switch the Under start flag on and off.
extern "C" s32 dbgMenuMenuUnderTakerOnOff(void)
{
    long flag = undertakerGetStartFlagID();

    if ((u8)GSflagTest(flag)) {
        GSflagOff(flag);
    } else {
        GSflagOn(flag);
    }
    return 0;
}

// Test entry: run the Relive-hall info script.
extern "C" s32 dbgMenuMenuTestInfoRelive(void)
{
    menuScriptInfoRelive(0, 1);
    return 0;
}

// Test entry: run the Pokespot info script.
extern "C" s32 dbgMenuMenuTestInfoEsa(void)
{
    menuScriptInfoEsa();
    return 0;
}

// Test entry: toggle the display of the Pokespot window.
extern "C" s32 dbgMenuMenuEsabaDisp(void)
{
    if (menuIsOpen(WIN_ID_ESABA_DISP)) {
        menuClose(WIN_ID_ESABA_DISP);
    } else {
        menuOpenCustom(WIN_ID_ESABA_DISP, 0, 0, 0, 1, 0);
    }
    return 0;
}

// Test entry: generate the pokemon the spot the entries work on holds.
extern "C" s32 dbgMenuMenuTestEsabaPokemon(void)
{
    u32 select[2];
    Pokemon pokemon;
    u32 trainerRnd;
    s32 result;

    select[0] = lPokespotSelectGenerateRow[0];
    select[1] = lPokespotSelectHeroRow[0];
    _esabaSelectPokemon = 0;
    trainerRnd = heroGetStatus(0, HERO_STATUS_TRAINER_RND, 0);

    result = menuOpenCustom(WIN_ID_POKESPOT, windowGetActiveID(), 0, 0, 1, 0);
    if (result > 0) {
        if (_esabaSelectPokemon == SPECIES_POKESPOT_FIXED_1 ||
            _esabaSelectPokemon == SPECIES_POKESPOT_FIXED_2) {
            // These two species need no generated pokemon.
            _esabaSetPokemon(_esabaPlace, _esabaSelectPokemon);
        } else {
            switch (menuSubOpenSelect2(0, select, 2, 0x96, 0x96, 0, 2, 1)) {
            case 0:
                _esabaSetPokemon(_esabaPlace, _esabaSelectPokemon);
                u32 rnd;

                _pokemonCreate(&pokemon, _esabaSelectPokemon, trainerRnd);
                rnd = pokemonCreateRndFit(&pokemon, -1, -1, 0, trainerRnd);
                esabaSetPokemonRnd(_esabaPlace, rnd);
                break;

            case 1:
                _esabaSetPokemon(_esabaPlace, _esabaSelectPokemon);
                esabaSetPokemonRnd(_esabaPlace, trainerRnd);
                break;
            }
        }
    } else if (result == 0) {
        _esabaSetPokemon(_esabaPlace, _esabaSelectPokemon);
    }
    menuClose(WIN_ID_POKESPOT);
    menuCloseSync(WIN_ID_POKESPOT, 1);
    return result < 0;
}

// Fill pokemon from the species and the random number handed in.
void _pokemonCreate(Pokemon *pokemon, unsigned short species, unsigned long rnd)
{
    u32 rndValue;

    pokemonCreate(pokemon, species, 10, gamedataGetStatus(0, 1));
    rndValue = memoDataGetPokemonRndFromID(0, species);
    pokemonBiosSetRnd(pokemon, rndValue);
    pokemonBiosSetCatchTrainerRnd(pokemon, rnd);
}

// Hand the pokemon of the spot to the spot's setter.
void _esabaSetPokemon(long place, long species)
{
    esabaSetPokemon((u16)_esabaPlace, _esabaSelectPokemon);
}

// Test entry: hand the spot's pokemon out.
extern "C" s32 dbgMenuMenuTestEsabaGet(void)
{
    esabaGet(_esabaPlace);
    return 0;
}

// Test entry: put the spot pokemon back.
extern "C" s32 dbgMenuMenuTestEsabaPut(void)
{
    esabaPut(_esabaPlace);
    return 0;
}

// Test entry: reset the spot from the place the entries work on.
extern "C" s32 dbgMenuMenuTestEsabaSet(void)
{
    esabaSet(_esabaPlace);
    return 0;
}

// Callback the Pokespot menu hands the place it picked to.
extern "C" s32 dbgMenuMenuSetPlace(s32, s32 place)
{
    _esabaPlace = place;
    return 1;
}

// Test entry: run the move-tutor demo of the move master.
extern "C" s32 dbgMenuMenuMoveDemo(s32, s32 demo)
{
    menuMoveDemo(demo);
    return 0;
}

// Test entry: run the move sequence of Mew.
extern "C" s32 dbgMenuMenuTestMewWazaMenu(void)
{
    mewWazaSequence();
    return 0;
}

// Test entry: start the Orre Colosseum battle whose encounter number the
// player enters.
extern "C" s32 dbgMenuMenuTestOrreColosseumMenu(void)
{
    u32 encount = 0;

    menuSubOpenNumberInput(0, &encount, 1, 0x32, 0x32, 1);
    if (encount >= 0x1C) {
        encount = 0;
    }
    menuOrreColosseum(_testEncount[encount]);
    menuOrreColosseumGetResult();
    return 0;
}

// Test entry: run the Basan move-tutor menu over the whole move table.
extern "C" s32 dbgMenuMenuTestWazaOshieBasanMenu(void)
{
    s32 selected;

    menuScriptWazaBasan(&selected);
    return 0;
}

// Test entry: pick a move from the move list.
extern "C" s32 dbgMenuMenuTestWazaOshieMeijinMenu(s32, s32 value)
{
    menuScriptWazaMeijin(value);
    return 0;
}

// Test entry: run the hall tutorial script.
extern "C" s32 dbgMenuMenuTestTutorialMenu(void)
{
    menuScriptTutorial();
    return 0;
}

// Test entry: run the Eevee evolution script.
extern "C" s32 dbgMenuMenuTestEeveeMenu(void)
{
    menuScriptEevee();
    return 0;
}

#pragma force_active reset
