#include "Pokemon.h"
#include "pokemonBios.h"
#include "gamedata.h"
#include "GSmsg.h"
#include <libc/memory.h>

extern "C" u16 gbaPokemonConditonFromGC(Pokemon *pokemon);
extern "C" void gbaPokemonConditonToGC(Pokemon *pokemon, u16 condition);
void _rotatepp(unsigned char *data, unsigned long selector);
void _revrotatepp(unsigned char *data, unsigned long selector);
u32 _swap32(u32 value);
u16 _swap16(u16 value);

// Length of the name a Pokemon or its trainer holds on the GBA side.
static const s32 GBA_NAME_SIZE = 10;
static const s32 GBA_TRAINER_NAME_SIZE = 7;
// Marks the end of a GBA name, the remaining slots are zero filled.
static const u8 GBA_NAME_END = 0xFF;
// Language ids the GBA uses for its own text tables.
static const u8 GBA_LANGUAGE_NONE = 0;
static const u8 GBA_LANGUAGE_JAPANESE = 1;
static const u8 GBA_LANGUAGE_ENGLISH = 2;
static const u8 GBA_LANGUAGE_FRENCH = 3;
static const u8 GBA_LANGUAGE_ITALIAN = 4;
static const u8 GBA_LANGUAGE_GERMAN = 5;
static const u8 GBA_LANGUAGE_SPANISH = 7;
// Versions the game records in a Pokemon's origin attestation.
static const u8 GAMEDATA_VERSION_NONE = 0;
static const u8 GAMEDATA_VERSION_FIRERED = 1;
static const u8 GAMEDATA_VERSION_LEAFGREEN = 2;
static const u8 GAMEDATA_VERSION_SAPPHIRE = 8;
static const u8 GAMEDATA_VERSION_RUBY = 9;
static const u8 GAMEDATA_VERSION_EMERALD = 10;
static const u8 GAMEDATA_VERSION_COLOSSEUM = 11;
// Regions a GBA cartridge is sold in, and the language areas of their text.
static const u8 GBA_AREA_NONE = 0;
static const u8 GBA_AREA_JAPAN = 1;
static const u8 GBA_AREA_AMERICA = 2;
static const u8 GBA_AREA_EUROPE = 3;
static const u8 GBA_LANGAREA_NONE = 0;
static const u8 GBA_LANGAREA_JAPAN = 1;
static const u8 GBA_LANGAREA_AMERICA = 2;
static const u8 GBA_LANGAREA_GERMANY = 3;
static const u8 GBA_LANGAREA_FRANCE = 4;
static const u8 GBA_LANGAREA_ITALY = 5;
static const u8 GBA_LANGAREA_SPAIN = 6;
// Two further language areas, which also give English text.
static const u8 GBA_LANGAREA_ENGLISH_8 = 8;
static const u8 GBA_LANGAREA_ENGLISH_9 = 9;
static const u8 GBA_GAME_NONE = 0;
static const u8 GBA_GAME_SAPPHIRE = 1;
static const u8 GBA_GAME_RUBY = 2;
static const u8 GBA_GAME_EMERALD = 3;
static const u8 GBA_GAME_FIRERED = 4;
static const u8 GBA_GAME_LEAFGREEN = 5;
static const u8 GBA_GAME_COLOSSEUM = 15;
static const s32 GBA_GENERATION = 3;
static const s32 GBA_NAME_SIZE_JAPANESE = 5;

// Number of 32 bit words of a record that are scrambled.
static const u32 GBA_SCRAMBLE_WORDS = 12;
// Number of 16 bit words the record checksum is calculated over.
static const s32 GBA_CHECKSUM_WORDS = 24;

// Bit layout of the origin word of a GBA Pokemon record.
struct GbaPokemonOrigins {
    u16 sex : 1;
    u16 ball : 4;
    u16 game : 4;
    u16 level : 7;
    u8 catchFloor;
    u8 pokerus;
};

// Bit layout of the individual values of a GBA Pokemon record.
struct GbaPokemonIvs {
    u32 tokusei : 1;
    u32 tamago : 1;
    u32 speDef : 5;
    u32 speAtk : 5;
    u32 nimbleness : 5;
    u32 phyDef : 5;
    u32 phyAtk : 5;
    u32 maxHp : 5;
};

// Bit layout of the ribbons and medals of a GBA Pokemon record.
struct GbaPokemonRibbons {
    u32 eventGetFlag : 1;
    u32 amari : 4;
    u32 world : 1;
    u32 earth : 1;
    u32 national : 1;
    u32 country : 1;
    u32 sky : 1;
    u32 land : 1;
    u32 marine : 1;
    u32 ganba : 1;
    u32 bromide : 1;
    u32 victory : 1;
    u32 winning : 1;
    u32 champ : 1;
    u32 strong : 3;
    u32 clever : 3;
    u32 cute : 3;
    u32 beautiful : 3;
    u32 style : 3;
};

// Bit layout of the flag byte of a GBA Pokemon record.
struct GbaPokemonFlags {
    u8 amari : 5;
    u8 tamago : 1;
    u8 valid : 1;
    u8 fusei : 1;
};

// The record the GBA stores for one Pokemon.
struct GbaPokemon {
    u32 rnd; // 0x00
    u32 catchTrainerRnd; // 0x04
    u8 nickname[GBA_NAME_SIZE]; // 0x08
    u8 language; // 0x12
    GbaPokemonFlags flags; // 0x13
    u8 catchTrainerName[GBA_TRAINER_NAME_SIZE]; // 0x14
    u8 pcboxMark; // 0x1B
    u16 checksum; // 0x1C
    u16 amari; // 0x1E
    u16 species; // 0x20
    u16 item; // 0x22
    u32 exp; // 0x24
    u8 ppCounts; // 0x28
    u8 friendLevel; // 0x29
    u16 para1Amari; // 0x2A
    u16 waza[4]; // 0x2C
    u8 pp[4]; // 0x34
    u8 maxHpEffort; // 0x38
    u8 phyAtkEffort; // 0x39
    u8 phyDefEffort; // 0x3A
    u8 nimblenessEffort; // 0x3B
    u8 speAtkEffort; // 0x3C
    u8 speDefEffort; // 0x3D
    u8 style; // 0x3E
    u8 beautiful; // 0x3F
    u8 cute; // 0x40
    u8 clever; // 0x41
    u8 strong; // 0x42
    u8 fur; // 0x43
    u32 origins; // 0x44
    u32 ivs; // 0x48
    u32 ribbons; // 0x4C
    u32 condition; // 0x50
    u8 level; // 0x54
    u8 mailId; // 0x55
    u16 hp; // 0x56
    u16 maxHp; // 0x58
    u16 phyAtk; // 0x5A
    u16 phyDef; // 0x5C
    u16 nimbleness; // 0x5E
    u16 speAtk; // 0x60
    u16 speDef; // 0x62
};

// Converts the Pokemon into the record the GBA stores for it.
extern "C" void gbaPokemonFromGC(Pokemon *pokemon, void *data)
{
    GamedataAttest *attest;
    GbaPokemon *record = (GbaPokemon *)data;
    GbaPokemonOrigins origins;
    GbaPokemonIvs ivs;
    GbaPokemonRibbons ribbons;
    u16 checksum = 0;
    u16 *checksumSource;
    u32 *scramble;
    s32 length;
    s32 i;

    memset(data, 0, sizeof(GbaPokemon));
    if (!pokemonCheckValid(pokemon)) {
        return;
    }

    record->rnd = _swap32(pokemonBiosGetRnd(pokemon));
    record->catchTrainerRnd = _swap32(pokemonBiosGetCatchTrainerRnd(pokemon));
    attest = pokemonBiosGetAttest(pokemon);

    switch (gamedataAttestBiosGetVerId(attest)) {
    case GAMEDATA_VERSION_SAPPHIRE:
        origins.game = GBA_GAME_SAPPHIRE;
        break;
    case GAMEDATA_VERSION_RUBY:
        origins.game = GBA_GAME_RUBY;
        break;
    case GAMEDATA_VERSION_EMERALD:
        origins.game = GBA_GAME_EMERALD;
        break;
    case GAMEDATA_VERSION_FIRERED:
        origins.game = GBA_GAME_FIRERED;
        break;
    case GAMEDATA_VERSION_LEAFGREEN:
        origins.game = GBA_GAME_LEAFGREEN;
        break;
    case GAMEDATA_VERSION_COLOSSEUM:
        origins.game = GBA_GAME_COLOSSEUM;
        break;
    default:
        origins.game = GBA_GAME_NONE;
        break;
    }

    switch (gamedataAttestBiosGetLangareaId(attest)) {
    case GBA_LANGAREA_JAPAN:
        record->language = GBA_LANGUAGE_JAPANESE;
        break;
    case GBA_LANGAREA_AMERICA:
        record->language = GBA_LANGUAGE_ENGLISH;
        break;
    case GBA_LANGAREA_GERMANY:
        record->language = GBA_LANGUAGE_GERMAN;
        break;
    case GBA_LANGAREA_FRANCE:
        record->language = GBA_LANGUAGE_FRENCH;
        break;
    case GBA_LANGAREA_ITALY:
        record->language = GBA_LANGUAGE_ITALIAN;
        break;
    case GBA_LANGAREA_SPAIN:
        record->language = GBA_LANGUAGE_SPANISH;
        break;
    case GBA_LANGAREA_ENGLISH_8:
        record->language = GBA_LANGUAGE_ENGLISH;
        break;
    case GBA_LANGAREA_ENGLISH_9:
        record->language = GBA_LANGUAGE_ENGLISH;
        break;
    default:
        record->language = GBA_LANGUAGE_NONE;
        break;
    }

    record->flags.valid = 1;
    record->flags.tamago = pokemonBiosGetTamagoFlag(pokemon) != 0;
    record->flags.amari = pokemonBiosGetFlagAmari(pokemon);
    record->flags.fusei = pokemonBiosGetFuseiFlag(pokemon) != 0;

    length = GScharConvToGBACode(record->nickname, pokemonBiosGetNicknameOrgPtr(pokemon),
                                 gamedataAttestBiosGetLangareaId(attest));
    if (length < GBA_NAME_SIZE) {
        record->nickname[length] = GBA_NAME_END;
        memset(record->nickname + length + 1, 0, GBA_NAME_SIZE - 1 - length);
    }

    length = GScharConvToGBACode(record->catchTrainerName, pokemonBiosGetCatchTrainerNamePtr(pokemon),
                                 gamedataAttestBiosGetLangareaId(attest));
    if (length < GBA_TRAINER_NAME_SIZE) {
        record->catchTrainerName[length] = GBA_NAME_END;
        memset(record->catchTrainerName + length + 1, 0, GBA_TRAINER_NAME_SIZE - 1 - length);
    }

    record->pcboxMark = pokemonBiosGetPcboxMark(pokemon);
    record->amari = _swap16(pokemonBiosGetAmari(pokemon));
    record->species = _swap16(pokemonBiosGetPokemonDataId(pokemon));
    record->item = _swap16(pokemonBiosGetItemDataId(pokemon));
    record->exp = _swap32(pokemonBiosGetExp(pokemon));
    record->friendLevel = pokemonBiosGetFriend(pokemon);
    record->para1Amari = pokemonBiosGetPara1Amari(pokemon);
    record->ppCounts = 0;
    for (i = 0; i < 4; i++) {
        record->waza[i] = _swap16(pokemonBiosGetPokemonWazaDataId(pokemon, i));
        record->ppCounts |= pokemonBiosGetPokemonWazaPpCount(pokemon, i) << (i * 2);
        record->pp[i] = pokemonBiosGetPokemonWazaPp(pokemon, i);
    }

    record->maxHpEffort = pokemonBiosGetMaxHpEffort(pokemon);
    record->phyAtkEffort = pokemonBiosGetPhyAtkEffort(pokemon);
    record->phyDefEffort = pokemonBiosGetPhyDefEffort(pokemon);
    record->nimblenessEffort = pokemonBiosGetNimblenessEffort(pokemon);
    record->speAtkEffort = pokemonBiosGetSpeAtkEffort(pokemon);
    record->speDefEffort = pokemonBiosGetSpeDefEffort(pokemon);
    record->style = pokemonBiosGetStyle(pokemon);
    record->beautiful = pokemonBiosGetBeautiful(pokemon);
    record->cute = pokemonBiosGetCute(pokemon);
    record->clever = pokemonBiosGetClever(pokemon);
    record->strong = pokemonBiosGetStrong(pokemon);
    record->fur = pokemonBiosGetFur(pokemon);

    origins.pokerus = pokemonBiosGetPokerus(pokemon);
    origins.catchFloor = pokemonBiosGetCatchFloorId(pokemon);
    origins.level = pokemonBiosGetCatchLevel(pokemon);
    origins.ball = pokemonBiosGetCatchBallId(pokemon);
    origins.sex = pokemonBiosGetCatchTrainerSex(pokemon);

    ivs.maxHp = pokemonBiosGetMaxHpRnd(pokemon);
    ivs.phyAtk = pokemonBiosGetPhyAtkRnd(pokemon);
    ivs.phyDef = pokemonBiosGetPhyDefRnd(pokemon);
    ivs.nimbleness = pokemonBiosGetNimblenessRnd(pokemon);
    ivs.speAtk = pokemonBiosGetSpeAtkRnd(pokemon);
    ivs.speDef = pokemonBiosGetSpeDefRnd(pokemon);
    ivs.tamago = pokemonBiosGetTamagoFlag(pokemon) != 0;
    ivs.tokusei = pokemonBiosGetTokuseiFlag(pokemon) != 0;

    ribbons.style = pokemonBiosGetStyleMedal(pokemon);
    ribbons.beautiful = pokemonBiosGetBeautifulMedal(pokemon);
    ribbons.cute = pokemonBiosGetCuteMedal(pokemon);
    ribbons.clever = pokemonBiosGetCleverMedal(pokemon);
    ribbons.strong = pokemonBiosGetStrongMedal(pokemon);
    ribbons.champ = pokemonBiosGetChampRibbon(pokemon) != 0;
    ribbons.winning = pokemonBiosGetWinningRibbon(pokemon) != 0;
    ribbons.victory = pokemonBiosGetVictoryRibbon(pokemon) != 0;
    ribbons.bromide = pokemonBiosGetBromideRibbon(pokemon) != 0;
    ribbons.ganba = pokemonBiosGetGanbaRibbon(pokemon) != 0;
    ribbons.marine = pokemonBiosGetMarineRibbon(pokemon) != 0;
    ribbons.land = pokemonBiosGetLandRibbon(pokemon) != 0;
    ribbons.sky = pokemonBiosGetSkyRibbon(pokemon) != 0;
    ribbons.country = pokemonBiosGetCountryRibbon(pokemon) != 0;
    ribbons.national = pokemonBiosGetNationalRibbon(pokemon) != 0;
    ribbons.earth = pokemonBiosGetEarthRibbon(pokemon) != 0;
    ribbons.world = pokemonBiosGetWorldRibbon(pokemon) != 0;
    ribbons.amari = pokemonBiosGetAmariRibbon(pokemon);
    ribbons.eventGetFlag = pokemonBiosGetEventGetFlag(pokemon);

    record->condition = _swap32(gbaPokemonConditonFromGC(pokemon) & 0xFFFF) |
                        (pokemonBiosGetConditionAmari(pokemon) & 0xFFFFF000);
    record->level = pokemonBiosGetLevel(pokemon);
    record->mailId = pokemonBiosGetMailId(pokemon);
    record->maxHp = _swap16(pokemonBiosGetMaxHp(pokemon));
    record->hp = _swap16(pokemonBiosGetHp(pokemon));
    record->phyAtk = _swap16(pokemonBiosGetPhyAtk(pokemon));
    record->phyDef = _swap16(pokemonBiosGetPhyDef(pokemon));
    record->nimbleness = _swap16(pokemonBiosGetNimbleness(pokemon));
    record->speAtk = _swap16(pokemonBiosGetSpeAtk(pokemon));
    record->speDef = _swap16(pokemonBiosGetSpeDef(pokemon));

    record->origins = _swap32(*(u32 *)&origins);
    record->ivs = _swap32(*(u32 *)&ivs);
    record->ribbons = _swap32(*(u32 *)&ribbons);

    scramble = (u32 *)&record->species;
    checksumSource = (u16 *)scramble;
    for (i = 0; i < GBA_CHECKSUM_WORDS; i++) {
        checksum += _swap16(checksumSource[i]);
    }
    record->checksum = _swap16(checksum);

    for (i = 0; i < GBA_SCRAMBLE_WORDS; i++) {
        *scramble ^= record->rnd ^ record->catchTrainerRnd;
        scramble++;
    }
    _rotatepp((u8 *)&record->species, _swap32(record->rnd));
}

// Restores the Pokemon from the record the GBA stored for it.
extern "C" void gbaPokemonToGC(Pokemon *pokemon, GbaPokemon *data)
{
    u32 origins;
    u32 ivs;
    u32 ribbons;
    GbaPokemonOrigins *gbaOrigins = (GbaPokemonOrigins *)&origins;
    GbaPokemonIvs *gbaIvs = (GbaPokemonIvs *)&ivs;
    GbaPokemonRibbons *gbaRibbons = (GbaPokemonRibbons *)&ribbons;
    u16 *checksumSource;
    u32 *scramble;
    u16 checksum = 0;
    u16 name[GBA_NAME_SIZE + 1];
    u8 versionId;
    u8 areaId;
    u8 langareaId;
    s32 i;

    pokemonInit(pokemon);
    if (!data->flags.valid && !data->flags.fusei) {
        return;
    }

    checksumSource = (u16 *)&data->species;
    scramble = (u32 *)checksumSource;
    for (i = 0; i < GBA_SCRAMBLE_WORDS; i++) {
        *scramble ^= data->rnd ^ data->catchTrainerRnd;
        scramble++;
    }

    _revrotatepp((u8 *)&data->species, _swap32(data->rnd));

    for (i = 0; i < GBA_CHECKSUM_WORDS; i++) {
        checksum += _swap16(*checksumSource);
        checksumSource++;
    }

    origins = _swap32(data->origins);
    ivs = _swap32(data->ivs);
    ribbons = _swap32(data->ribbons);
    pokemonBiosSetRnd(pokemon, _swap32(data->rnd));
    pokemonBiosSetCatchTrainerRnd(pokemon, _swap32(data->catchTrainerRnd));

    switch (gbaOrigins->game) {
    case GBA_GAME_SAPPHIRE:
        versionId = GAMEDATA_VERSION_SAPPHIRE;
        break;
    case GBA_GAME_RUBY:
        versionId = GAMEDATA_VERSION_RUBY;
        break;
    case GBA_GAME_EMERALD:
        versionId = GAMEDATA_VERSION_EMERALD;
        break;
    case GBA_GAME_FIRERED:
        versionId = GAMEDATA_VERSION_FIRERED;
        break;
    case GBA_GAME_LEAFGREEN:
        versionId = GAMEDATA_VERSION_LEAFGREEN;
        break;
    case GBA_GAME_COLOSSEUM:
        versionId = GAMEDATA_VERSION_COLOSSEUM;
        break;
    default:
        versionId = GAMEDATA_VERSION_NONE;
        break;
    }

    switch (data->language) {
    case GBA_LANGUAGE_JAPANESE:
        areaId = GBA_AREA_JAPAN;
        langareaId = GBA_LANGAREA_JAPAN;
        break;
    case GBA_LANGUAGE_ENGLISH:
        areaId = GBA_AREA_AMERICA;
        langareaId = GBA_LANGAREA_AMERICA;
        break;
    case GBA_LANGUAGE_FRENCH:
        areaId = GBA_AREA_EUROPE;
        langareaId = GBA_LANGAREA_FRANCE;
        break;
    case GBA_LANGUAGE_ITALIAN:
        areaId = GBA_AREA_EUROPE;
        langareaId = GBA_LANGAREA_ITALY;
        break;
    case GBA_LANGUAGE_GERMAN:
        areaId = GBA_AREA_EUROPE;
        langareaId = GBA_LANGAREA_GERMANY;
        break;
    case GBA_LANGUAGE_SPANISH:
        areaId = GBA_AREA_EUROPE;
        langareaId = GBA_LANGAREA_SPAIN;
        break;
    default:
        areaId = GBA_AREA_NONE;
        langareaId = GBA_LANGAREA_NONE;
        break;
    }

    gamedataAttestCreate(pokemonBiosGetAttest(pokemon), versionId, GBA_GENERATION, areaId,
                         langareaId);

    GScharConvToGCCode(name, data->nickname, GBA_NAME_SIZE, langareaId);
    pokemonBiosSetNicknamePtr(pokemon, name);
    if (langareaId == GBA_LANGAREA_JAPAN) {
        pokemon->mNickname[GBA_NAME_SIZE_JAPANESE] = 0;
    }

    pokemonBiosSetFlagAmari(pokemon, data->flags.amari);
    pokemonBiosSetFuseiFlag(pokemon, data->flags.fusei != 0);
    GScharConvToGCCode(pokemonBiosGetCatchTrainerNamePtr(pokemon), data->catchTrainerName,
                       GBA_TRAINER_NAME_SIZE, langareaId);
    pokemonBiosSetPcboxMark(pokemon, data->pcboxMark);
    pokemonBiosSetAmari(pokemon, _swap16(data->amari));
    pokemonBiosSetPokemonDataId(pokemon, _swap16(data->species));
    pokemonBiosSetItemDataId(pokemon, _swap16(data->item));
    pokemonBiosSetExp(pokemon, _swap32(data->exp));
    pokemonBiosSetFriend(pokemon, data->friendLevel);
    pokemonBiosSetPara1Amari(pokemon, data->para1Amari);
    for (i = 0; i < 4; i++) {
        pokemonBiosSetPokemonWazaDataId(pokemon, i, _swap16(data->waza[i]));
        pokemonBiosSetPokemonWazaPpCount(pokemon, i, (data->ppCounts >> (i * 2)) & 3);
        pokemonBiosSetPokemonWazaPp(pokemon, i, data->pp[i]);
    }
    pokemonBiosSetMaxHpEffort(pokemon, data->maxHpEffort);
    pokemonBiosSetPhyAtkEffort(pokemon, data->phyAtkEffort);
    pokemonBiosSetPhyDefEffort(pokemon, data->phyDefEffort);
    pokemonBiosSetNimblenessEffort(pokemon, data->nimblenessEffort);
    pokemonBiosSetSpeAtkEffort(pokemon, data->speAtkEffort);
    pokemonBiosSetSpeDefEffort(pokemon, data->speDefEffort);
    pokemonBiosSetStyle(pokemon, data->style);
    pokemonBiosSetBeautiful(pokemon, data->beautiful);
    pokemonBiosSetCute(pokemon, data->cute);
    pokemonBiosSetClever(pokemon, data->clever);
    pokemonBiosSetStrong(pokemon, data->strong);
    pokemonBiosSetFur(pokemon, data->fur);
    pokemonBiosSetPokerus(pokemon, gbaOrigins->pokerus);
    pokemonBiosSetCatchFloorId(pokemon, gbaOrigins->catchFloor);
    pokemonBiosSetCatchLevel(pokemon, gbaOrigins->level);
    pokemonBiosSetCatchBallId(pokemon, gbaOrigins->ball);
    pokemonBiosSetCatchTrainerSex(pokemon, gbaOrigins->sex);
    pokemonBiosSetMaxHpRnd(pokemon, gbaIvs->maxHp);
    pokemonBiosSetPhyAtkRnd(pokemon, gbaIvs->phyAtk);
    pokemonBiosSetPhyDefRnd(pokemon, gbaIvs->phyDef);
    pokemonBiosSetNimblenessRnd(pokemon, gbaIvs->nimbleness);
    pokemonBiosSetSpeAtkRnd(pokemon, gbaIvs->speAtk);
    pokemonBiosSetSpeDefRnd(pokemon, gbaIvs->speDef);
    pokemonBiosSetTamagoFlag(pokemon, gbaIvs->tamago != 0);
    pokemonBiosSetTokuseiFlag(pokemon, gbaIvs->tokusei != 0);
    pokemonBiosSetStyleMedal(pokemon, gbaRibbons->style);
    pokemonBiosSetBeautifulMedal(pokemon, gbaRibbons->beautiful);
    pokemonBiosSetCuteMedal(pokemon, gbaRibbons->cute);
    pokemonBiosSetCleverMedal(pokemon, gbaRibbons->clever);
    pokemonBiosSetStrongMedal(pokemon, gbaRibbons->strong);
    pokemonBiosSetChampRibbon(pokemon, gbaRibbons->champ != 0);
    pokemonBiosSetWinningRibbon(pokemon, gbaRibbons->winning != 0);
    pokemonBiosSetVictoryRibbon(pokemon, gbaRibbons->victory != 0);
    pokemonBiosSetBromideRibbon(pokemon, gbaRibbons->bromide != 0);
    pokemonBiosSetGanbaRibbon(pokemon, gbaRibbons->ganba != 0);
    pokemonBiosSetMarineRibbon(pokemon, gbaRibbons->marine != 0);
    pokemonBiosSetLandRibbon(pokemon, gbaRibbons->land != 0);
    pokemonBiosSetSkyRibbon(pokemon, gbaRibbons->sky != 0);
    pokemonBiosSetCountryRibbon(pokemon, gbaRibbons->country != 0);
    pokemonBiosSetNationalRibbon(pokemon, gbaRibbons->national != 0);
    pokemonBiosSetEarthRibbon(pokemon, gbaRibbons->earth != 0);
    pokemonBiosSetWorldRibbon(pokemon, gbaRibbons->world != 0);
    pokemonBiosSetAmariRibbon(pokemon, gbaRibbons->amari);
    pokemonBiosSetEventGetFlag(pokemon, gbaRibbons->eventGetFlag != 0);
    gbaPokemonConditonToGC(pokemon, _swap32(data->condition));
    pokemonBiosSetConditionAmari(pokemon, _swap32(data->condition) & 0xFFFFF000);
    pokemonBiosSetLevel(pokemon, data->level);
    pokemonBiosSetMailId(pokemon, data->mailId);
    pokemonBiosSetMaxHp(pokemon, _swap16(data->maxHp));
    pokemonBiosSetHp(pokemon, _swap16(data->hp));
    pokemonBiosSetPhyAtk(pokemon, _swap16(data->phyAtk));
    pokemonBiosSetPhyDef(pokemon, _swap16(data->phyDef));
    pokemonBiosSetNimbleness(pokemon, _swap16(data->nimbleness));
    pokemonBiosSetSpeAtk(pokemon, _swap16(data->speAtk));
    pokemonBiosSetSpeDef(pokemon, _swap16(data->speDef));
}
