#include <types.h>
#include <libc/memory.h>

#include <savedata.h>
#include <gamedata.h>
#include <hero.h>
#include <heroBios.h>
#include <itemData.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <waza.h>
#include <GSflag.h>
#include <GSmsg.h>
#include <darkPokemon.h>
#include <exribbon.h>
#include <mail.h>
#include <memo.h>
#include <menuCBBios.h>
#include <relivehall.h>
#include <sodateya.h>
#include <pokecolo.h>
#include <app/pcbox/pcbox.h>

extern "C" void heroCreate(Hero *hero, u16 *name, u8 sex);
extern "C" void heroInit(Hero *hero);
extern "C" void darkpokemonValInit(void *save);

// The whole save image; savedataBios hands out pointers into it.
u8 pokecolo_savedata[0x287D0];

// Save image the game is currently using; null until a save is loaded.
u8 *now_savedata_ptr;

// Slots of the save image that only the save-data provider addresses. The
// ones shared with other subsystems are the enumerators of SaveDataRegion.
enum SaveDataSlot {
    SAVE_SLOT_FLAG_LEVEL_0 = 4,
    SAVE_SLOT_FLAG_LEVEL_1 = 5,
    SAVE_SLOT_FLAG_LEVEL_2 = 6,
    SAVE_SLOT_FLAG_LEVEL_0_WORDS = 7,
    SAVE_SLOT_FLAG_LEVEL_1_WORDS = 8,
    SAVE_SLOT_FLAG_LEVEL_2_WORDS = 9,
    SAVE_SLOT_YOBI_1 = 17,
    SAVE_SLOT_YOBI_2 = 18,
    SAVE_SLOT_YOBI_3 = 19,
    SAVE_SLOT_YOBI_4 = 20,
    SAVE_SLOT_FLAG_SAVEDATA = 23,
    SAVE_SLOT_DISK_SELL_OUT = 24,
    SAVE_SLOT_DISK_SELL_OUT_WORDS = 25,
    SAVE_SLOT_SPARE_SIZE = 26,
    SAVE_SLOT_SPARE = 27,
    SAVE_SLOT_SPARE_WORDS = 28
};

// Slot numbers the dispatch below has a table entry for; the save image ends
// with the spare block.
static const int SAVE_SLOT_MAX = 29;

// Words in each of the three flag banks the dispatch hands out.
enum SaveDataFlagBankWords {
    SAVE_DATA_FLAG_LEVEL_0_WORDS = 8,
    SAVE_DATA_FLAG_LEVEL_1_WORDS = 32,
    SAVE_DATA_FLAG_LEVEL_2_WORDS = 384
};

// Bytes the spare block behind the Ex Ribbon flags can hold.
static const u32 SAVE_DATA_SPARE_SIZE = 51200;

// Battle disks one sell-out word records; one bit per disk.
static const u32 SAVE_DATA_DISK_SELL_OUT_BITS = 32;

// Byte offsets of the sell-out words and of the spare block behind the flags.
enum SaveDataImageOffset {
    SAVE_DATA_DISK_SELL_OUT_OFFSET = 0x10E30,
    SAVE_DATA_SPARE_OFFSET = 0x10E38
};

// Regional builds pokecoloGetCountryCode() reports.
enum CountryCode {
    COUNTRY_JAPAN = 0,
    COUNTRY_USA = 1,
    COUNTRY_EUROPE = 2
};

// Floor a new trainer starts on, the floor recorded as the one they arrived
// from, and the floor a demo game resumes on.
enum SavedataFloorId {
    SAVEDATA_START_FLOOR_ID = 138,
    SAVEDATA_START_PREVIOUS_FLOOR_ID = 143,
    SAVEDATA_DEMO_START_FLOOR_ID = 71
};

// Position inside the starting floor a new trainer is placed at.
static const int SAVEDATA_START_FLOOR_POSITION = 2;

// Builds a new game is created for: the retail disc and the demo disc, whose
// trainer starts with the four preset party records below.
enum SavedataRomVersion {
    SAVEDATA_ROM_VERSION_RETAIL = 0,
    SAVEDATA_ROM_VERSION_DEMO = 1
};

// Experience a new trainer's starter is booked with.
static const u32 SAVEDATA_STARTER_EXP = 1250;

// Save flag the report menu tests before it lets the trainer record the game;
// the etc-flag debug screen drives the same flag.
static const s32 SAVEDATA_FLAG_ETC_SAVE_ENABLE = 990;

// Let the inventory choose an available slot for each starting item.
static const s16 ITEM_POSITION_AUTO = -1;

// A party slot the shipped builds start a new trainer with.
typedef struct SavedataPartyInit {
    u8 level;
    u16 species;
    u16 shadowRecordId;
    u16 friendLevel;
    u16 waza[4];
    u32 catchFloorId;
    u32 exp;
    s8 sex;
    s8 nature;
    bool abilityFlag;
} SavedataPartyInit;

// The four records of the starting party as one copyable block.
typedef struct SavedataPartyInitList {
    SavedataPartyInit slots[4];
} SavedataPartyInitList;

// Starting party of the demo disc; the retail build starts the trainer with
// one Eevee instead.
const SavedataPartyInitList sDemoStartingParty = { {
    { 40, POKEMON_SPECIES_PARASECT, 0, 0,
      { WAZA_DATA_ID_SPORE, WAZA_DATA_ID_GIGA_DRAIN, WAZA_DATA_ID_SCRATCH, WAZA_DATA_ID_GROWTH },
      0, 64000, POKEMON_SEX_MALE, POKEMON_NATURE_HARDY, 0 },
    { 40, POKEMON_SPECIES_KIRLIA, 0, 0,
      { WAZA_DATA_ID_CONFUSION, WAZA_DATA_ID_SHOCK_WAVE, WAZA_DATA_ID_SHADOW_BALL, WAZA_DATA_ID_CALM_MIND },
      0, 80000, POKEMON_SEX_FEMALE, POKEMON_NATURE_BASHFUL, 0 },
    { 40, POKEMON_SPECIES_ELECTRODE, 0, 0,
      { WAZA_DATA_ID_SPARK, WAZA_DATA_ID_THUNDER_WAVE, WAZA_DATA_ID_SWIFT, WAZA_DATA_ID_SCREECH },
      0, 64000, POKEMON_SEX_MALE, POKEMON_NATURE_SERIOUS, 0 },
    { 40, POKEMON_SPECIES_SPHEAL, 0, 0,
      { WAZA_DATA_ID_AURORA_BEAM, WAZA_DATA_ID_SURF, WAZA_DATA_ID_BODY_SLAM, WAZA_DATA_ID_GROWL },
      0, 56660, POKEMON_SEX_FEMALE, POKEMON_NATURE_BASHFUL, 0 },
} };

// Marks the battle disk at index as sold out, so it cannot be sold again.
void savedataSetBattleDiskSellOut(u32 index) {
    u32 *sellOut = (u32 *)savedataGetStatus(NULL, SAVE_SLOT_DISK_SELL_OUT);

    sellOut[index / SAVE_DATA_DISK_SELL_OUT_BITS] |=
        1 << (index % SAVE_DATA_DISK_SELL_OUT_BITS);
}

// Whether the battle disk at index was already sold out.
u32 savedataGetBattleDiskSellOut(u32 index) {
    u32 *sellOut = (u32 *)savedataGetStatus(NULL, SAVE_SLOT_DISK_SELL_OUT);

    return (sellOut[index / SAVE_DATA_DISK_SELL_OUT_BITS] &
            (1 << (index % SAVE_DATA_DISK_SELL_OUT_BITS))) != 0;
}

// Fills the save image in saveData with a fresh trainer.
void savedataCreate(void *saveData, u16 *name) {
    GamedataSave *gamedata;
    Hero *hero;
    HeroPokemonGetParam param;
    u16 defaultName[10];

    memset(&param, 0, sizeof(param));
    savedataInit(saveData);

    gamedata = (GamedataSave *)savedataGetStatus(saveData, SAVE_DATA_GAME);
    switch (pokecoloGetCountryCode()) {
    case COUNTRY_JAPAN:
        gamedataCreate(gamedata, GAMEDATA_ATTEST_VERSION, GAMEDATA_ATTEST_GENERATION,
                       GAMEDATA_ATTEST_AREA_JAPAN, GAMEDATA_ATTEST_LANGAREA_JAPANESE);
        break;
    case COUNTRY_USA:
        gamedataCreate(gamedata, GAMEDATA_ATTEST_VERSION, GAMEDATA_ATTEST_GENERATION,
                       GAMEDATA_ATTEST_AREA_USA, GAMEDATA_ATTEST_LANGAREA_ENGLISH);
        break;
    case COUNTRY_EUROPE:
        gamedataCreate(gamedata, GAMEDATA_ATTEST_VERSION, GAMEDATA_ATTEST_GENERATION,
                       GAMEDATA_ATTEST_AREA_EUROPE, GAMEDATA_ATTEST_LANGAREA_EUROPE);
        break;
    }
    gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_PAR_CHECKSUM, pokecoloGetLanguage());

    hero = (Hero *)savedataGetStatus(saveData, SAVE_DATA_HERO);
    if (name == NULL) {
        // A trainer who is given no name of their own is called the blank
        // wide space, which reads as an empty name.
        GScharMakeFromSJIS(defaultName, "\x81\x40");
        name = defaultName;
    }
    heroCreate(hero, name, 0);

    if (pokecoloGetRomVersion() == SAVEDATA_ROM_VERSION_DEMO) {
        SavedataPartyInitList party = sDemoStartingParty;
        int i;
        Pokemon *pokemon;

        gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_WORLD_MAP_FLOOR, SAVEDATA_START_FLOOR_ID);
        gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_PREVIOUS_FLOOR, SAVEDATA_START_PREVIOUS_FLOOR_ID);
        gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_FLOOR_POSITION, SAVEDATA_START_FLOOR_POSITION);

        SavedataPartyInit *rec = party.slots;
        for (i = 0; i < 4; i++) {
            param.species = rec->species;
            param.shadowRecordId = rec->shadowRecordId;
            param.level = rec->level;
            param.friendLevel = rec->friendLevel;
            param.waza[0] = rec->waza[0];
            param.waza[1] = rec->waza[1];
            param.waza[2] = rec->waza[2];
            param.waza[3] = rec->waza[3];
            param.sexDataId = heroBiosGetSexDataId(hero);
            param.trainerId = heroBiosGetRnd(hero);
            param.catchTrainerName = heroBiosGetNamePtr(hero);
            param.catchFloorId = rec->catchFloorId;
            param.exp = rec->exp;
            rec++;
            hero->pokemonGet(&param, true);
        }

        rec = party.slots;
        for (int i = 0; i < 4; i++, rec++) {
            pokemon = hero->getPokemon(i);
            u32 rnd = heroBiosGetRnd(hero);
            u32 pid = pokemonCreateRndFit(pokemon, rec->sex, rec->nature,
                                          POKEMON_RARE_FIT_ORDINARY, rnd);

            pokemon_SetRnd(pokemon, pid);
            pokemonBiosSetTokuseiFlag(pokemon, rec->abilityFlag);
            pokemonResetBasisStatus(pokemon);
        }

        heroSetStatus(hero, HERO_STATUS_POKEDORU, 0);
        heroItemAddItemDataId(hero, ITEM_ULTRA_BALL, 20, ITEM_POSITION_AUTO);
        heroItemAddItemDataId(hero, ITEM_HYPER_POTION, 20, ITEM_POSITION_AUTO);
        heroItemAddItemDataId(hero, ITEM_FULL_HEAL, 10, ITEM_POSITION_AUTO);
        heroItemAddItemDataId(hero, ITEM_REVIVE, 5, ITEM_POSITION_AUTO);
        hero->setBattleResumeFloorID(SAVEDATA_DEMO_START_FLOOR_ID);
        hero->setBattleResumeFloorIndex(0);
        hero->setHeroStyle(HERO_STYLE_SNATCH, false);
        hero->setFollowerID(0);
    } else {
        gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_WORLD_MAP_FLOOR, SAVEDATA_START_FLOOR_ID);
        gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_PREVIOUS_FLOOR, SAVEDATA_START_PREVIOUS_FLOOR_ID);
        gamedatasaveSetStatus(gamedata, GAMEDATASAVE_FIELD_FLOOR_POSITION, SAVEDATA_START_FLOOR_POSITION);

        param.species = POKEMON_SPECIES_EEVEE;
        param.shadowRecordId = 0;
        param.level = 10;
        param.friendLevel = 215;
        param.waza[0] = WAZA_DATA_ID_TACKLE;
        param.waza[1] = WAZA_DATA_ID_TAIL_WHIP;
        param.waza[2] = WAZA_DATA_ID_BITE;
        param.waza[3] = WAZA_DATA_ID_SAND_ATTACK;
        param.sexDataId = heroBiosGetSexDataId(hero);
        param.trainerId = heroBiosGetRnd(hero);
        param.catchTrainerName = heroBiosGetNamePtr(hero);
        param.catchFloorId = 0;
        param.exp = SAVEDATA_STARTER_EXP;
        hero->pokemonGet(&param, true);

        heroSetStatus(hero, HERO_STATUS_POKEDORU, 5000);
        hero->setBattleResumeFloorID(SAVEDATA_START_FLOOR_ID);
        hero->setBattleResumeFloorIndex(SAVEDATA_START_FLOOR_POSITION);
        hero->setHeroStyle(HERO_STYLE_NORMAL, false);
        hero->setFollowerID(0);
    }
}

// Clears everything a new game does not carry over and rebuilds the
// subsystem state that lives in the save image.
void savedataInit(void *saveData) {
    u8 gamedataBackup[136];
    u8 rulesBackup[868];

    memcpy(gamedataBackup, savedataGetStatus(saveData, SAVE_DATA_GAME), sizeof(gamedataBackup));
    memcpy(rulesBackup, savedataGetStatus(saveData, SAVE_DATA_FIGHT_RULE), sizeof(rulesBackup));
    memset(savedataGetStatus(NULL, SAVE_DATA_IMAGE), 0, 163792);
    memcpy(savedataGetStatus(saveData, SAVE_DATA_GAME), gamedataBackup, sizeof(gamedataBackup));
    memcpy(savedataGetStatus(saveData, SAVE_DATA_FIGHT_RULE), rulesBackup, sizeof(rulesBackup));

    gamedataInit((GamedataSave *)savedataGetStatus(saveData, SAVE_DATA_GAME));
    heroInit((Hero *)savedataGetStatus(saveData, SAVE_DATA_HERO));
    pcboxInit((PCStorageRecord *)savedataGetStatus(saveData, SAVE_DATA_PCBOX));

    if (saveData == NULL || saveData == savedataGetStatus(NULL, SAVE_DATA_IMAGE)) {
        GSflagClear(1);
        GSflagClear(2);
        GSflagClear(3);
        GSflagOn(SAVEDATA_FLAG_ETC_SAVE_ENABLE);
    }

    memset(savedataGetStatus(saveData, SAVE_SLOT_DISK_SELL_OUT), 0,
           (u32)savedataGetStatus(saveData, SAVE_SLOT_DISK_SELL_OUT_WORDS) * sizeof(u32));
    memset(savedataGetStatus(saveData, SAVE_SLOT_SPARE), 0,
           (u32)savedataGetStatus(saveData, SAVE_SLOT_SPARE_WORDS) * sizeof(u32));

    mailInitMailbox((MailSaveData *)savedataGetStatus(saveData, SAVE_DATA_MAIL));
    sodateyaInit((SodateyaSave *)savedataGetStatus(saveData, SAVEDATA_BLOCK_SODATEYA));
    menuCB_RULE_INIT((_TOOL_SAVEDATA *)savedataGetStatus(saveData, SAVE_DATA_FIGHT_RULE));
    memoInit(savedataGetStatus(saveData, SAVEDATA_BLOCK_MEMO));
    darkpokemonValInit(savedataGetStatus(saveData, SAVE_DATA_DARK_POKEMON));
    exribbonInit((u8 *)savedataGetStatus(saveData, SAVE_DATA_EXRIBBON));
    ((CReliveHall *)savedataGetStatus(saveData, SAVE_DATA_RELIVE_HALL))->init();
}

// Resolves a slot of the save image to the address the caller works with.
void *savedataGetStatus(void *saveData, u16 region) {
    if (region >= SAVE_SLOT_MAX) {
        return NULL;
    }
    if (saveData == NULL) {
        saveData = savedataBiosGetNowSavedataPtr();
        if (saveData == NULL) {
            return NULL;
        }
    }

    switch (region) {
    case SAVE_DATA_IMAGE:
        return saveData;
    case SAVE_DATA_GAME:
        return savedataBiosGetGamedataPtr(saveData);
    case SAVE_DATA_HERO:
        return savedataBiosGetHeroPtr(saveData);
    case SAVE_DATA_PCBOX:
        return savedataBiosGetPcboxPtr(saveData);
    case SAVE_SLOT_FLAG_LEVEL_0:
        return savedataBiosGetFlagLevel0Ptr(saveData);
    case SAVE_SLOT_FLAG_LEVEL_1:
        return savedataBiosGetFlagLevel1Ptr(saveData);
    case SAVE_SLOT_FLAG_LEVEL_2:
        return savedataBiosGetFlagLevel2Ptr(saveData);
    case SAVE_SLOT_DISK_SELL_OUT:
        return (u8 *)saveData + SAVE_DATA_DISK_SELL_OUT_OFFSET;
    case SAVE_SLOT_SPARE:
        return (u8 *)saveData + SAVE_DATA_SPARE_OFFSET;
    case SAVE_SLOT_FLAG_LEVEL_0_WORDS:
        return (void *)SAVE_DATA_FLAG_LEVEL_0_WORDS;
    case SAVE_SLOT_FLAG_LEVEL_1_WORDS:
        return (void *)SAVE_DATA_FLAG_LEVEL_1_WORDS;
    case SAVE_SLOT_FLAG_LEVEL_2_WORDS:
        return (void *)SAVE_DATA_FLAG_LEVEL_2_WORDS;
    case SAVE_SLOT_DISK_SELL_OUT_WORDS:
        return (void *)2;
    case SAVE_SLOT_SPARE_WORDS:
        return (void *)2;
    case SAVE_DATA_MAIL:
        return savedataBiosGetMailboxPtr(saveData);
    case SAVEDATA_BLOCK_SODATEYA:
        return savedataBiosGetSodateyaPtr(saveData);
    case SAVEDATA_BLOCK_MEMO:
        return savedataBiosGetMemoDataPtr(saveData);
    case SAVE_DATA_FIGHT_RULE:
        return savedataBiosGetToolPtr(saveData);
    case SAVE_DATA_DARK_POKEMON:
        return savedataBiosGetDarkpokemonPtr(saveData);
    case SAVE_DATA_EXRIBBON:
        return savedataBiosGetExRibbonPtr(saveData);
    case SAVE_DATA_ANYWAY:
        return savedataBiosGetAnywaySavePtr(saveData);
    case SAVE_SLOT_SPARE_SIZE:
        return (void *)SAVE_DATA_SPARE_SIZE;
    case SAVE_SLOT_YOBI_1:
        return savedataBiosGetYobi1Ptr(saveData);
    case SAVE_SLOT_YOBI_2:
        return savedataBiosGetYobi2Ptr(saveData);
    case SAVE_SLOT_YOBI_3:
        return savedataBiosGetYobi3Ptr(saveData);
    case SAVE_SLOT_YOBI_4:
        return savedataBiosGetYobi4Ptr(saveData);
    case SAVE_DATA_RELIVE_HALL:
        return savedataBiosGetRelivePtr(saveData);
    case SAVE_SLOT_FLAG_SAVEDATA:
        return savedataBiosGetFlagSavedataPtr(saveData);
    default:
        return NULL;
    }
}
