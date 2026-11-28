#ifndef POKEMONDB_H
#define POKEMONDB_H

#include <types.h>
#include <Pokemon.h>
#include <fightPokemon.h>
#include <fightWaza.h>
#include <itemFightItem.h>

#ifdef __cplusplus
extern "C" {
#endif

enum TOKUSEI_DATA_ID {
    TOKUSEI_DATA_ID_NONE = 0,
    TOKUSEI_DATA_ID_STENCH,
    TOKUSEI_DATA_ID_DRIZZLE,
    TOKUSEI_DATA_ID_SPEED_BOOST,
    TOKUSEI_DATA_ID_BATTLE_ARMOR,
    TOKUSEI_DATA_ID_STURDY,
    TOKUSEI_DATA_ID_DAMP,
    TOKUSEI_DATA_ID_LIMBER,
    TOKUSEI_DATA_ID_SAND_VEIL,
    TOKUSEI_DATA_ID_STATIC,
    TOKUSEI_DATA_ID_VOLT_ABSORB,
    TOKUSEI_DATA_ID_WATER_ABSORB,
    TOKUSEI_DATA_ID_OBLIVIOUS,
    TOKUSEI_DATA_ID_CLOUD_NINE,
    TOKUSEI_DATA_ID_COMPOUND_EYES,
    TOKUSEI_DATA_ID_INSOMNIA,
    TOKUSEI_DATA_ID_COLOR_CHANGE,
    TOKUSEI_DATA_ID_IMMUNITY,
    TOKUSEI_DATA_ID_FLASH_FIRE,
    TOKUSEI_DATA_ID_SHIELD_DUST,
    TOKUSEI_DATA_ID_OWN_TEMPO,
    TOKUSEI_DATA_ID_SUCTION_CUPS,
    TOKUSEI_DATA_ID_INTIMIDATE,
    TOKUSEI_DATA_ID_SHADOW_TAG,
    TOKUSEI_DATA_ID_ROUGH_SKIN,
    TOKUSEI_DATA_ID_WONDER_GUARD,
    TOKUSEI_DATA_ID_LEVITATE,
    TOKUSEI_DATA_ID_EFFECT_SPORE,
    TOKUSEI_DATA_ID_SYNCHRONIZE,
    TOKUSEI_DATA_ID_CLEAR_BODY,
    TOKUSEI_DATA_ID_NATURAL_CURE,
    TOKUSEI_DATA_ID_LIGHTNING_ROD,
    TOKUSEI_DATA_ID_SERENE_GRACE,
    TOKUSEI_DATA_ID_SWIFT_SWIM,
    TOKUSEI_DATA_ID_CHLOROPHYLL,
    TOKUSEI_DATA_ID_ILLUMINATE,
    TOKUSEI_DATA_ID_TRACE,
    TOKUSEI_DATA_ID_HUGE_POWER,
    TOKUSEI_DATA_ID_POISON_POINT,
    TOKUSEI_DATA_ID_INNER_FOCUS,
    TOKUSEI_DATA_ID_MAGMA_ARMOR,
    TOKUSEI_DATA_ID_WATER_VEIL,
    TOKUSEI_DATA_ID_MAGNET_PULL,
    TOKUSEI_DATA_ID_SOUNDPROOF,
    TOKUSEI_DATA_ID_RAIN_DISH,
    TOKUSEI_DATA_ID_SAND_STREAM,
    TOKUSEI_DATA_ID_PRESSURE,
    TOKUSEI_DATA_ID_THICK_FAT,
    TOKUSEI_DATA_ID_EARLY_BIRD,
    TOKUSEI_DATA_ID_FLAME_BODY,
    TOKUSEI_DATA_ID_RUN_AWAY,
    TOKUSEI_DATA_ID_KEEN_EYE,
    TOKUSEI_DATA_ID_HYPER_CUTTER,
    TOKUSEI_DATA_ID_PICKUP,
    TOKUSEI_DATA_ID_TRUANT,
    TOKUSEI_DATA_ID_HUSTLE,
    TOKUSEI_DATA_ID_CUTE_CHARM,
    TOKUSEI_DATA_ID_PLUS,
    TOKUSEI_DATA_ID_MINUS,
    TOKUSEI_DATA_ID_FORECAST,
    TOKUSEI_DATA_ID_STICKY_HOLD,
    TOKUSEI_DATA_ID_SHED_SKIN,
    TOKUSEI_DATA_ID_GUTS,
    TOKUSEI_DATA_ID_MARVEL_SCALE,
    TOKUSEI_DATA_ID_LIQUID_OOZE,
    TOKUSEI_DATA_ID_OVERGROW,
    TOKUSEI_DATA_ID_BLAZE,
    TOKUSEI_DATA_ID_TORRENT,
    TOKUSEI_DATA_ID_SWARM,
    TOKUSEI_DATA_ID_ROCK_HEAD,
    TOKUSEI_DATA_ID_DROUGHT,
    TOKUSEI_DATA_ID_ARENA_TRAP,
    TOKUSEI_DATA_ID_VITAL_SPIRIT,
    TOKUSEI_DATA_ID_WHITE_SMOKE,
    TOKUSEI_DATA_ID_PURE_POWER,
    TOKUSEI_DATA_ID_SHELL_ARMOR,
    TOKUSEI_DATA_ID_UNUSED,
    TOKUSEI_DATA_ID_AIR_LOCK
};

u16 pokemon_GetInitDp(Pokemon *pokemon);
u16 pokemon_GetPoolFriend(Pokemon *pokemon);
u16 pokemon_GetDarkpokemonDataId(Pokemon *pokemon);
void pokemon_SetDarkpokemonDataId(Pokemon *pokemon, u16 darkID);
u8 pokemon_GetDarkFlag(Pokemon *pokemon);

enum PokemonDataID {
    POKEMON_DATA_ID_NONE,
    POKEMON_DATA_ID_BULBASAUR,
    POKEMON_DATA_ID_IVYSAUR,
    POKEMON_DATA_ID_VENUSAUR,
    POKEMON_DATA_ID_CHARMANDER,
    POKEMON_DATA_ID_CHARMELEON,
    POKEMON_DATA_ID_CHARIZARD,
    POKEMON_DATA_ID_SQUIRTLE,
    POKEMON_DATA_ID_WARTORTLE,
    POKEMON_DATA_ID_BLASTOISE,
    POKEMON_DATA_ID_CATERPIE,
    POKEMON_DATA_ID_METAPOD,
    POKEMON_DATA_ID_BUTTERFREE,
    POKEMON_DATA_ID_WEEDLE,
    POKEMON_DATA_ID_KAKUNA,
    POKEMON_DATA_ID_BEEDRILL,
    POKEMON_DATA_ID_PIDGEY,
    POKEMON_DATA_ID_PIDGEOTTO,
    POKEMON_DATA_ID_PIDGEOT,
    POKEMON_DATA_ID_RATTATA,
    POKEMON_DATA_ID_RATICATE,
    POKEMON_DATA_ID_SPEAROW,
    POKEMON_DATA_ID_FEAROW,
    POKEMON_DATA_ID_EKANS,
    POKEMON_DATA_ID_ARBOK,
    POKEMON_DATA_ID_PIKACHU,
    POKEMON_DATA_ID_RAICHU,
    POKEMON_DATA_ID_SANDSHREW,
    POKEMON_DATA_ID_SANDSLASH,
    POKEMON_DATA_ID_NIDORAN_F,
    POKEMON_DATA_ID_NIDORINA,
    POKEMON_DATA_ID_NIDOQUEEN,
    POKEMON_DATA_ID_NIDORAN_M,
    POKEMON_DATA_ID_NIDORINO,
    POKEMON_DATA_ID_NIDOKING,
    POKEMON_DATA_ID_CLEFAIRY,
    POKEMON_DATA_ID_CLEFABLE,
    POKEMON_DATA_ID_VULPIX,
    POKEMON_DATA_ID_NINETALES,
    POKEMON_DATA_ID_JIGGLYPUFF,
    POKEMON_DATA_ID_WIGGLYTUFF,
    POKEMON_DATA_ID_ZUBAT,
    POKEMON_DATA_ID_GOLBAT,
    POKEMON_DATA_ID_ODDISH,
    POKEMON_DATA_ID_GLOOM,
    POKEMON_DATA_ID_VILEPLUME,
    POKEMON_DATA_ID_PARAS,
    POKEMON_DATA_ID_PARASECT,
    POKEMON_DATA_ID_VENONAT,
    POKEMON_DATA_ID_VENOMOTH,
    POKEMON_DATA_ID_DIGLETT,
    POKEMON_DATA_ID_DUGTRIO,
    POKEMON_DATA_ID_MEOWTH,
    POKEMON_DATA_ID_PERSIAN,
    POKEMON_DATA_ID_PSYDUCK,
    POKEMON_DATA_ID_GOLDUCK,
    POKEMON_DATA_ID_MANKEY,
    POKEMON_DATA_ID_PRIMEAPE,
    POKEMON_DATA_ID_GROWLITHE,
    POKEMON_DATA_ID_ARCANINE,
    POKEMON_DATA_ID_POLIWAG,
    POKEMON_DATA_ID_POLIWHIRL,
    POKEMON_DATA_ID_POLIWRATH,
    POKEMON_DATA_ID_ABRA,
    POKEMON_DATA_ID_KADABRA,
    POKEMON_DATA_ID_ALAKAZAM,
    POKEMON_DATA_ID_MACHOP,
    POKEMON_DATA_ID_MACHOKE,
    POKEMON_DATA_ID_MACHAMP,
    POKEMON_DATA_ID_BELLSPROUT,
    POKEMON_DATA_ID_WEEPINBELL,
    POKEMON_DATA_ID_VICTREEBEL,
    POKEMON_DATA_ID_TENTACOOL,
    POKEMON_DATA_ID_TENTACRUEL,
    POKEMON_DATA_ID_GEODUDE,
    POKEMON_DATA_ID_GRAVELER,
    POKEMON_DATA_ID_GOLEM,
    POKEMON_DATA_ID_PONYTA,
    POKEMON_DATA_ID_RAPIDASH,
    POKEMON_DATA_ID_SLOWPOKE,
    POKEMON_DATA_ID_SLOWBRO,
    POKEMON_DATA_ID_MAGNEMITE,
    POKEMON_DATA_ID_MAGNETON,
    POKEMON_DATA_ID_FARFETCHD,
    POKEMON_DATA_ID_DODUO,
    POKEMON_DATA_ID_DODRIO,
    POKEMON_DATA_ID_SEEL,
    POKEMON_DATA_ID_DEWGONG,
    POKEMON_DATA_ID_GRIMER,
    POKEMON_DATA_ID_MUK,
    POKEMON_DATA_ID_SHELLDER,
    POKEMON_DATA_ID_CLOYSTER,
    POKEMON_DATA_ID_GASTLY,
    POKEMON_DATA_ID_HAUNTER,
    POKEMON_DATA_ID_GENGAR,
    POKEMON_DATA_ID_ONIX,
    POKEMON_DATA_ID_DROWZEE,
    POKEMON_DATA_ID_HYPNO,
    POKEMON_DATA_ID_KRABBY,
    POKEMON_DATA_ID_KINGLER,
    POKEMON_DATA_ID_VOLTORB,
    POKEMON_DATA_ID_ELECTRODE,
    POKEMON_DATA_ID_EXEGGCUTE,
    POKEMON_DATA_ID_EXEGGUTOR,
    POKEMON_DATA_ID_CUBONE,
    POKEMON_DATA_ID_MAROWAK,
    POKEMON_DATA_ID_HITMONLEE,
    POKEMON_DATA_ID_HITMONCHAN,
    POKEMON_DATA_ID_LICKITUNG,
    POKEMON_DATA_ID_KOFFING,
    POKEMON_DATA_ID_WEEZING,
    POKEMON_DATA_ID_RHYHORN,
    POKEMON_DATA_ID_RHYDON,
    POKEMON_DATA_ID_CHANSEY,
    POKEMON_DATA_ID_TANGELA,
    POKEMON_DATA_ID_KANGASKHAN,
    POKEMON_DATA_ID_HORSEA,
    POKEMON_DATA_ID_SEADRA,
    POKEMON_DATA_ID_GOLDEEN,
    POKEMON_DATA_ID_SEAKING,
    POKEMON_DATA_ID_STARYU,
    POKEMON_DATA_ID_STARMIE,
    POKEMON_DATA_ID_MR_MIME,
    POKEMON_DATA_ID_SCYTHER,
    POKEMON_DATA_ID_JYNX,
    POKEMON_DATA_ID_ELECTABUZZ,
    POKEMON_DATA_ID_MAGMAR,
    POKEMON_DATA_ID_PINSIR,
    POKEMON_DATA_ID_TAUROS,
    POKEMON_DATA_ID_MAGIKARP,
    POKEMON_DATA_ID_GYARADOS,
    POKEMON_DATA_ID_LAPRAS,
    POKEMON_DATA_ID_DITTO,
    POKEMON_DATA_ID_EEVEE,
    POKEMON_DATA_ID_VAPOREON,
    POKEMON_DATA_ID_JOLTEON,
    POKEMON_DATA_ID_FLAREON,
    POKEMON_DATA_ID_PORYGON,
    POKEMON_DATA_ID_OMANYTE,
    POKEMON_DATA_ID_OMASTAR,
    POKEMON_DATA_ID_KABUTO,
    POKEMON_DATA_ID_KABUTOPS,
    POKEMON_DATA_ID_AERODACTYL,
    POKEMON_DATA_ID_SNORLAX,
    POKEMON_DATA_ID_ARTICUNO,
    POKEMON_DATA_ID_ZAPDOS,
    POKEMON_DATA_ID_MOLTRES,
    POKEMON_DATA_ID_DRATINI,
    POKEMON_DATA_ID_DRAGONAIR,
    POKEMON_DATA_ID_DRAGONITE,
    POKEMON_DATA_ID_MEWTWO,
    POKEMON_DATA_ID_MEW,
    POKEMON_DATA_ID_CHIKORITA,
    POKEMON_DATA_ID_BAYLEEF,
    POKEMON_DATA_ID_MEGANIUM,
    POKEMON_DATA_ID_CYNDAQUIL,
    POKEMON_DATA_ID_QUILAVA,
    POKEMON_DATA_ID_TYPHLOSION,
    POKEMON_DATA_ID_TOTODILE,
    POKEMON_DATA_ID_CROCONAW,
    POKEMON_DATA_ID_FERALIGATR,
    POKEMON_DATA_ID_SENTRET,
    POKEMON_DATA_ID_FURRET,
    POKEMON_DATA_ID_HOOTHOOT,
    POKEMON_DATA_ID_NOCTOWL,
    POKEMON_DATA_ID_LEDYBA,
    POKEMON_DATA_ID_LEDIAN,
    POKEMON_DATA_ID_SPINARAK,
    POKEMON_DATA_ID_ARIADOS,
    POKEMON_DATA_ID_CROBAT,
    POKEMON_DATA_ID_CHINCHOU,
    POKEMON_DATA_ID_LANTURN,
    POKEMON_DATA_ID_PICHU,
    POKEMON_DATA_ID_CLEFFA,
    POKEMON_DATA_ID_IGGLYBUFF,
    POKEMON_DATA_ID_TOGEPI,
    POKEMON_DATA_ID_TOGETIC,
    POKEMON_DATA_ID_NATU,
    POKEMON_DATA_ID_XATU,
    POKEMON_DATA_ID_MAREEP,
    POKEMON_DATA_ID_FLAAFFY,
    POKEMON_DATA_ID_AMPHAROS,
    POKEMON_DATA_ID_BELLOSSOM,
    POKEMON_DATA_ID_MARILL,
    POKEMON_DATA_ID_AZUMARILL,
    POKEMON_DATA_ID_SUDOWOODO,
    POKEMON_DATA_ID_POLITOED,
    POKEMON_DATA_ID_HOPPIP,
    POKEMON_DATA_ID_SKIPLOOM,
    POKEMON_DATA_ID_JUMPLUFF,
    POKEMON_DATA_ID_AIPOM,
    POKEMON_DATA_ID_SUNKERN,
    POKEMON_DATA_ID_SUNFLORA,
    POKEMON_DATA_ID_YANMA,
    POKEMON_DATA_ID_WOOPER,
    POKEMON_DATA_ID_QUAGSIRE,
    POKEMON_DATA_ID_ESPEON,
    POKEMON_DATA_ID_UMBREON,
    POKEMON_DATA_ID_MURKROW,
    POKEMON_DATA_ID_SLOWKING,
    POKEMON_DATA_ID_MISDREAVUS,
    POKEMON_DATA_ID_UNOWN,
    POKEMON_DATA_ID_WOBBUFFET,
    POKEMON_DATA_ID_GIRAFARIG,
    POKEMON_DATA_ID_PINECO,
    POKEMON_DATA_ID_FORRETRESS,
    POKEMON_DATA_ID_DUNSPARCE,
    POKEMON_DATA_ID_GLIGAR,
    POKEMON_DATA_ID_STEELIX,
    POKEMON_DATA_ID_SNUBBULL,
    POKEMON_DATA_ID_GRANBULL,
    POKEMON_DATA_ID_QWILFISH,
    POKEMON_DATA_ID_SCIZOR,
    POKEMON_DATA_ID_SHUCKLE,
    POKEMON_DATA_ID_HERACROSS,
    POKEMON_DATA_ID_SNEASEL,
    POKEMON_DATA_ID_TEDDIURSA,
    POKEMON_DATA_ID_URSARING,
    POKEMON_DATA_ID_SLUGMA,
    POKEMON_DATA_ID_MAGCARGO,
    POKEMON_DATA_ID_SWINUB,
    POKEMON_DATA_ID_PILOSWINE,
    POKEMON_DATA_ID_CORSOLA,
    POKEMON_DATA_ID_REMORAID,
    POKEMON_DATA_ID_OCTILLERY,
    POKEMON_DATA_ID_DELIBIRD,
    POKEMON_DATA_ID_MANTINE,
    POKEMON_DATA_ID_SKARMORY,
    POKEMON_DATA_ID_HOUNDOUR,
    POKEMON_DATA_ID_HOUNDOOM,
    POKEMON_DATA_ID_KINGDRA,
    POKEMON_DATA_ID_PHANPY,
    POKEMON_DATA_ID_DONPHAN,
    POKEMON_DATA_ID_PORYGON2,
    POKEMON_DATA_ID_STANTLER,
    POKEMON_DATA_ID_SMEARGLE,
    POKEMON_DATA_ID_TYROGUE,
    POKEMON_DATA_ID_HITMONTOP,
    POKEMON_DATA_ID_SMOOCHUM,
    POKEMON_DATA_ID_ELEKID,
    POKEMON_DATA_ID_MAGBY,
    POKEMON_DATA_ID_MILTANK,
    POKEMON_DATA_ID_BLISSEY,
    POKEMON_DATA_ID_RAIKOU,
    POKEMON_DATA_ID_ENTEI,
    POKEMON_DATA_ID_SUICUNE,
    POKEMON_DATA_ID_LARVITAR,
    POKEMON_DATA_ID_PUPITAR,
    POKEMON_DATA_ID_TYRANITAR,
    POKEMON_DATA_ID_LUGIA,
    POKEMON_DATA_ID_HO_OH,
    POKEMON_DATA_ID_CELEBI,
    POKEMON_DATA_ID_OLD_UNOWN_B,
    POKEMON_DATA_ID_OLD_UNOWN_C,
    POKEMON_DATA_ID_OLD_UNOWN_D,
    POKEMON_DATA_ID_OLD_UNOWN_E,
    POKEMON_DATA_ID_OLD_UNOWN_F,
    POKEMON_DATA_ID_OLD_UNOWN_G,
    POKEMON_DATA_ID_OLD_UNOWN_H,
    POKEMON_DATA_ID_OLD_UNOWN_I,
    POKEMON_DATA_ID_OLD_UNOWN_J,
    POKEMON_DATA_ID_OLD_UNOWN_K,
    POKEMON_DATA_ID_OLD_UNOWN_L,
    POKEMON_DATA_ID_OLD_UNOWN_M,
    POKEMON_DATA_ID_OLD_UNOWN_N,
    POKEMON_DATA_ID_OLD_UNOWN_O,
    POKEMON_DATA_ID_OLD_UNOWN_P,
    POKEMON_DATA_ID_OLD_UNOWN_Q,
    POKEMON_DATA_ID_OLD_UNOWN_R,
    POKEMON_DATA_ID_OLD_UNOWN_S,
    POKEMON_DATA_ID_OLD_UNOWN_T,
    POKEMON_DATA_ID_OLD_UNOWN_U,
    POKEMON_DATA_ID_OLD_UNOWN_V,
    POKEMON_DATA_ID_OLD_UNOWN_W,
    POKEMON_DATA_ID_OLD_UNOWN_X,
    POKEMON_DATA_ID_OLD_UNOWN_Y,
    POKEMON_DATA_ID_OLD_UNOWN_Z,
    POKEMON_DATA_ID_TREECKO,
    POKEMON_DATA_ID_GROVYLE,
    POKEMON_DATA_ID_SCEPTILE,
    POKEMON_DATA_ID_TORCHIC,
    POKEMON_DATA_ID_COMBUSKEN,
    POKEMON_DATA_ID_BLAZIKEN,
    POKEMON_DATA_ID_MUDKIP,
    POKEMON_DATA_ID_MARSHTOMP,
    POKEMON_DATA_ID_SWAMPERT,
    POKEMON_DATA_ID_POOCHYENA,
    POKEMON_DATA_ID_MIGHTYENA,
    POKEMON_DATA_ID_ZIGZAGOON,
    POKEMON_DATA_ID_LINOONE,
    POKEMON_DATA_ID_WURMPLE,
    POKEMON_DATA_ID_SILCOON,
    POKEMON_DATA_ID_BEAUTIFLY,
    POKEMON_DATA_ID_CASCOON,
    POKEMON_DATA_ID_DUSTOX,
    POKEMON_DATA_ID_LOTAD,
    POKEMON_DATA_ID_LOMBRE,
    POKEMON_DATA_ID_LUDICOLO,
    POKEMON_DATA_ID_SEEDOT,
    POKEMON_DATA_ID_NUZLEAF,
    POKEMON_DATA_ID_SHIFTRY,
    POKEMON_DATA_ID_NINCADA,
    POKEMON_DATA_ID_NINJASK,
    POKEMON_DATA_ID_SHEDINJA,
    POKEMON_DATA_ID_TAILLOW,
    POKEMON_DATA_ID_SWELLOW,
    POKEMON_DATA_ID_SHROOMISH,
    POKEMON_DATA_ID_BRELOOM,
    POKEMON_DATA_ID_SPINDA,
    POKEMON_DATA_ID_WINGULL,
    POKEMON_DATA_ID_PELIPPER,
    POKEMON_DATA_ID_SURSKIT,
    POKEMON_DATA_ID_MASQUERAIN,
    POKEMON_DATA_ID_WAILMER,
    POKEMON_DATA_ID_WAILORD,
    POKEMON_DATA_ID_SKITTY,
    POKEMON_DATA_ID_DELCATTY,
    POKEMON_DATA_ID_KECLEON,
    POKEMON_DATA_ID_BALTOY,
    POKEMON_DATA_ID_CLAYDOL,
    POKEMON_DATA_ID_NOSEPASS,
    POKEMON_DATA_ID_TORKOAL,
    POKEMON_DATA_ID_SABLEYE,
    POKEMON_DATA_ID_BARBOACH,
    POKEMON_DATA_ID_WHISCASH,
    POKEMON_DATA_ID_LUVDISC,
    POKEMON_DATA_ID_CORPHISH,
    POKEMON_DATA_ID_CRAWDAUNT,
    POKEMON_DATA_ID_FEEBAS,
    POKEMON_DATA_ID_MILOTIC,
    POKEMON_DATA_ID_CARVANHA,
    POKEMON_DATA_ID_SHARPEDO,
    POKEMON_DATA_ID_TRAPINCH,
    POKEMON_DATA_ID_VIBRAVA,
    POKEMON_DATA_ID_FLYGON,
    POKEMON_DATA_ID_MAKUHITA,
    POKEMON_DATA_ID_HARIYAMA,
    POKEMON_DATA_ID_ELECTRIKE,
    POKEMON_DATA_ID_MANECTRIC,
    POKEMON_DATA_ID_NUMEL,
    POKEMON_DATA_ID_CAMERUPT,
    POKEMON_DATA_ID_SPHEAL,
    POKEMON_DATA_ID_SEALEO,
    POKEMON_DATA_ID_WALREIN,
    POKEMON_DATA_ID_CACNEA,
    POKEMON_DATA_ID_CACTURNE,
    POKEMON_DATA_ID_SNORUNT,
    POKEMON_DATA_ID_GLALIE,
    POKEMON_DATA_ID_LUNATONE,
    POKEMON_DATA_ID_SOLROCK,
    POKEMON_DATA_ID_AZURILL,
    POKEMON_DATA_ID_SPOINK,
    POKEMON_DATA_ID_GRUMPIG,
    POKEMON_DATA_ID_PLUSLE,
    POKEMON_DATA_ID_MINUN,
    POKEMON_DATA_ID_MAWILE,
    POKEMON_DATA_ID_MEDITITE,
    POKEMON_DATA_ID_MEDICHAM,
    POKEMON_DATA_ID_SWABLU,
    POKEMON_DATA_ID_ALTARIA,
    POKEMON_DATA_ID_WYNAUT,
    POKEMON_DATA_ID_DUSKULL,
    POKEMON_DATA_ID_DUSCLOPS,
    POKEMON_DATA_ID_ROSELIA,
    POKEMON_DATA_ID_SLAKOTH,
    POKEMON_DATA_ID_VIGOROTH,
    POKEMON_DATA_ID_SLAKING,
    POKEMON_DATA_ID_GULPIN,
    POKEMON_DATA_ID_SWALOT,
    POKEMON_DATA_ID_TROPIUS,
    POKEMON_DATA_ID_WHISMUR,
    POKEMON_DATA_ID_LOUDRED,
    POKEMON_DATA_ID_EXPLOUD,
    POKEMON_DATA_ID_CLAMPERL,
    POKEMON_DATA_ID_HUNTAIL,
    POKEMON_DATA_ID_GOREBYSS,
    POKEMON_DATA_ID_ABSOL,
    POKEMON_DATA_ID_SHUPPET,
    POKEMON_DATA_ID_BANETTE,
    POKEMON_DATA_ID_SEVIPER,
    POKEMON_DATA_ID_ZANGOOSE,
    POKEMON_DATA_ID_RELICANTH,
    POKEMON_DATA_ID_ARON,
    POKEMON_DATA_ID_LAIRON,
    POKEMON_DATA_ID_AGGRON,
    POKEMON_DATA_ID_CASTFORM,
    POKEMON_DATA_ID_VOLBEAT,
    POKEMON_DATA_ID_ILLUMISE,
    POKEMON_DATA_ID_LILEEP,
    POKEMON_DATA_ID_CRADILY,
    POKEMON_DATA_ID_ANORITH,
    POKEMON_DATA_ID_ARMALDO,
    POKEMON_DATA_ID_RALTS,
    POKEMON_DATA_ID_KIRLIA,
    POKEMON_DATA_ID_GARDEVOIR,
    POKEMON_DATA_ID_BAGON,
    POKEMON_DATA_ID_SHELGON,
    POKEMON_DATA_ID_SALAMENCE,
    POKEMON_DATA_ID_BELDUM,
    POKEMON_DATA_ID_METANG,
    POKEMON_DATA_ID_METAGROSS,
    POKEMON_DATA_ID_REGIROCK,
    POKEMON_DATA_ID_REGICE,
    POKEMON_DATA_ID_REGISTEEL,
    POKEMON_DATA_ID_KYOGRE,
    POKEMON_DATA_ID_GROUDON,
    POKEMON_DATA_ID_RAYQUAZA,
    POKEMON_DATA_ID_LATIAS,
    POKEMON_DATA_ID_LATIOS,
    POKEMON_DATA_ID_JIRACHI,
    POKEMON_DATA_ID_DEOXYS,
    POKEMON_DATA_ID_CHIMECHO
};
u32 pokemonDB_GetName(u16);
u32 pokemonDB_GetShout(u16 pokemonDataId);
u32 pokemonDB_GetVoice(u16 pokemonDataId);
bool pokemonDB_GetTeachWaza(u16, u16);
u32 pokemonDB_GetPokebodyId(u16 species, u8 shiny);

void fightOutPokemon_SetSequencePtr(FightOutPokemon *pokemon, struct NullSequence *sequence);
FightPokemon *fightOutPokemon_GetMotoFightPokemonPtr(FightOutPokemon *pokemon);
FightPokemon *fightOutPokemon_GetFightPokemonPtr(FightOutPokemon *pokemon);
FIGHT_WAZA *fightOutPokemon_GetFightWazaPtr(FightOutPokemon *pokemon);
FIGHT_ITEM *fightOutPokemon_GetFightItemPtr(FightOutPokemon *pokemon);
FIGHT_ACTION *fightOutPokemon_GetFightActionBuffPtr(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetUseWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonGetUseWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonGetMotoWazaDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetMotoWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetWazaDamageValue(FightOutPokemon *pokemon, u8 damage);
void fightOutPokemon_SetWazaCritical(FightOutPokemon *pokemon, u32 critical);
void fightOutPokemon_SetWazaJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId);
void fightOutPokemon_SetWazaInitJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId);
void fightOutPokemon_SetWazaDamage(FightOutPokemon *pokemon, s32 damage);
void fightOutPokemon_SetWazaIryoku(FightOutPokemon *pokemon, u16 iryoku);
void fightOutPokemon_SetWazaTargetId(FightOutPokemon *pokemon, u16 targetId);
void fightOutPokemon_SetWazaHitDamage(FightOutPokemon *pokemon, s32 hitDamage);
u8 fightOutPokemon_GetDamageValue(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetCritical(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId);
u8 fightOutPokemon_GetIsHit(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetDamage(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetIryoku(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetTargetId(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetHitDamage(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageAtkValue(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageSpeValue(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetKaigaraDamageValue(FightOutPokemon *pokemon);
void fightOutPokemon_SetKaigaraDamageValue(FightOutPokemon *pokemon, s32 damage);
u16 fightOutPokemon_GetFightoutTurnCount(FightOutPokemon *pokemon);
void fightOutPokemon_SetFightoutTurnCount(FightOutPokemon *pokemon, u16 count);
FIGHT_WAZA *fightOutPokemon_GetKeepFightWazaPtr(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetNamakeFlag(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetTameWazaFlag(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetDamageSpeTargetId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetMotoWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaDataId(FightOutPokemon *pokemon);
struct FightOutPokemonEnemy *fightOutPokemon_GetFightOutPokemonEnemyPtr(FightOutPokemon *pokemon,
                                                                       u16 index);
void fightOutPokemon_SetOumuWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
u16 fightOutPokemon_GetOumuWazaAllyDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetNoPressureFlag(FightOutPokemon *pokemon, u32 flag);
u16 fightOutPokemon_GetLastUseWazaDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetSuccessCnt(FightOutPokemon *pokemon);
void fightOutPokemon_SetSuccessCnt(FightOutPokemon *pokemon, u8 count);

void fightOutPokemon_SetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId);
void fightOutPokemon_SetMeetEnemyFightPokemonEntryId(FightOutPokemon *pokemon, u8 index, u16 entryId);
void fightOutPokemon_SetUsedItemDataId(FightOutPokemon *pokemon, u16 itemDataId);
void fightOutPokemon_SetNamakeFlag(FightOutPokemon *pokemon, u32 flag);
void fightOutPokemon_SetGamanDamageTargetId(FightOutPokemon *pokemon, u16 targetId);
void fightOutPokemon_SetGamanDamageValue(FightOutPokemon *pokemon, s16 damage);
void fightOutPokemon_SetFightPokemonPtr(FightOutPokemon *pokemon, FightPokemon *fightPokemon);
void fightOutPokemon_SetMotoFightPokemonPtr(FightOutPokemon *pokemon, FightPokemon *fightPokemon);
void fightPokemon_SetAiDamage(FightPokemon *pokemon, u16 index, u16 target, s32 damage);
void fightPokemon_SetHokakuFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetDarkOutFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetLevelUpFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetCatchEntryFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetEntryId(FightPokemon *pokemon, s16 entryId);
void fightPokemon_SetMotoPokemonPtr(FightPokemon *pokemon, Pokemon *original);
void pokemon_SetComboPartnerAll(Pokemon *pokemon, u8 value);
void pokemon_SetPoolFriend(Pokemon *pokemon, u16 level);
void pokemon_SetPoolExp(Pokemon *pokemon, u32 exp);
void pokemon_SetFightTrainerPokemonDataId(Pokemon *pokemon, u16 dataId);
void pokemon_SetAmari(Pokemon *pokemon, u16 value);
void pokemon_SetPara1Amari(Pokemon *pokemon, u16 value);
void pokemon_SetMailId(Pokemon *pokemon, u8 value);
void pokemon_SetPcboxMark(Pokemon *pokemon, u8 value);
void pokemon_SetFlagAmari(Pokemon *pokemon, u8 value);
void pokemon_SetFuseiFlag(Pokemon *pokemon, u8 flag);
void pokemon_SetTokuseiFlag(Pokemon *pokemon, u8 flag);
void pokemon_SetTamagoFlag(Pokemon *pokemon, u8 flag);
void pokemon_SetPokerus(Pokemon *pokemon, u8 value);

// Contest medals and ribbons of a Pokemon; each setter writes one field of the
// medal record or of the ribbon word and leaves the others alone.
void pokemon_SetStyleMedal(Pokemon *pokemon, u8 value);
void pokemon_SetBeautifulMedal(Pokemon *pokemon, u8 value);
void pokemon_SetCuteMedal(Pokemon *pokemon, u8 value);
void pokemon_SetCleverMedal(Pokemon *pokemon, u8 value);
void pokemon_SetStrongMedal(Pokemon *pokemon, u8 value);
void pokemon_SetAmariRibbon(Pokemon *pokemon, u8 value);
void pokemon_SetWorldRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetEarthRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetNationalRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetCountryRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetSkyRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetLandRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetMarineRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetGanbaRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetBromideRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetVictoryRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetWinningRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetChampRibbon(Pokemon *pokemon, bool flag);

// Contest condition of a Pokemon: the fur sheen and the five contest
// categories of the condition record.
void pokemon_SetFur(Pokemon *pokemon, u8 value);
void pokemon_SetStrong(Pokemon *pokemon, u8 value);
void pokemon_SetClever(Pokemon *pokemon, u8 value);
void pokemon_SetCute(Pokemon *pokemon, u8 value);
void pokemon_SetBeautiful(Pokemon *pokemon, u8 value);
void pokemon_SetStyle(Pokemon *pokemon, u8 value);
void pokemon_SetFriend(Pokemon *pokemon, u16 value);

// Individual values, effort values and calculated battle stats of a Pokemon,
// one setter per stat.
void pokemon_SetNimblenessRnd(Pokemon *pokemon, u16 value);
void pokemon_SetSpeDefRnd(Pokemon *pokemon, u16 value);
void pokemon_SetSpeAtkRnd(Pokemon *pokemon, u16 value);
void pokemon_SetPhyDefRnd(Pokemon *pokemon, u16 value);
void pokemon_SetPhyAtkRnd(Pokemon *pokemon, u16 value);
void pokemon_SetMaxHpRnd(Pokemon *pokemon, u16 value);
void pokemon_SetNimblenessEffort(Pokemon *pokemon, u16 value);
void pokemon_SetSpeDefEffort(Pokemon *pokemon, u16 value);
void pokemon_SetSpeAtkEffort(Pokemon *pokemon, u16 value);
void pokemon_SetPhyDefEffort(Pokemon *pokemon, u16 value);
void pokemon_SetPhyAtkEffort(Pokemon *pokemon, u16 value);
void pokemon_SetMaxHpEffort(Pokemon *pokemon, u16 value);
void pokemon_SetNimbleness(Pokemon *pokemon, u16 value);
void pokemon_SetSpeDef(Pokemon *pokemon, u16 value);
void pokemon_SetSpeAtk(Pokemon *pokemon, u16 value);
void pokemon_SetPhyDef(Pokemon *pokemon, u16 value);
void pokemon_SetPhyAtk(Pokemon *pokemon, u16 value);
void pokemon_SetMaxHp(Pokemon *pokemon, u16 value);

u16 pokemon_GetPokemonWazaDataId(Pokemon *pokemon, u16 slot);
u16 pokemon_GetFightTrainerPokemonDataId(Pokemon *pokemon);
u8 pokemon_GetComboPartner(Pokemon *pokemon, u8 index, u8 mode);
u8 pokemon_GetPokemonWazaPp(Pokemon *pokemon, u16 slot);
void pokemon_SetPokemonWazaDataId(Pokemon *pokemon, u16 slot, u16 dataId);
void pokemon_SetPokemonWazaPpCount(Pokemon *pokemon, u16 slot, u8 count);
void pokemon_SetPokemonWazaPp(Pokemon *pokemon, u16 slot, u8 pp);

void pokemon_SetConditionAmari(Pokemon *pokemon, u32 value);
void pokemon_SetLevel(Pokemon *pokemon, u8 level);
void pokemon_SetExp(Pokemon *pokemon, u32 exp);
void pokemon_SetNicknamePtr(Pokemon *pokemon, const u16 *name);
void pokemon_SetCatchTrainerNamePtr(Pokemon *pokemon, const u16 *name);
void pokemon_SetCatchTrainerSex(Pokemon *pokemon, u8 sex);
void pokemon_SetCatchBallId(Pokemon *pokemon, u8 ballId);
void pokemon_SetCatchLevel(Pokemon *pokemon, u8 level);
void pokemon_SetCatchFloorId(Pokemon *pokemon, u16 floorId);
void pokemon_SetPokemonDataId(Pokemon *pokemon, u16 dataId);

u32 pokemon_GetTokuseiFlag(void *pokemonData);
u16 pokemonDB_GetBasisPhyAtk(u16 pokemonDataId);
u16 pokemonDB_GetBasisPhyDef(u16 pokemonDataId);
u8 pokemonDB_GetTokuseiDataId(u16 species, u16 index);
s16 fightPokemon_GetEntryId(FightPokemon *pokemon);
Pokemon *fightPokemon_GetMotoPokemonPtr(FightPokemon *pokemon);
Pokemon *fightPokemon_GetPokemonBuffPtr(FightPokemon *pokemon);
u8 fightPokemon_GetCatchEntryFlag(FightPokemon *pokemon);
void pokemon_SetItemDataId(Pokemon *pokemon, u16 itemDataId);

void pokemonDB_SetTukamaetaFlag(u16 species, u32 value);
void pokemonDB_SetMitaFlag(u16 species, u32 value);

#ifdef __cplusplus
}
#endif

#endif
