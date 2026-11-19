#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#include <types.h>

typedef struct PokemonData PokemonData;
typedef PokemonData PokemonSpeciesStats;

enum PokemonDataLimits {
    POKEMON_DATA_TYPE_COUNT = 2,
    POKEMON_DATA_ABILITY_COUNT = 2,
    POKEMON_DATA_ITEM_COUNT = 2,
    POKEMON_DATA_MACHINE_COUNT = 58,
    POKEMON_DATA_EVOLUTION_COUNT = 5,
    POKEMON_DATA_LEVEL_UP_MOVE_COUNT = 20,
    POKEMON_DATA_BODY_COUNT = 2,
    POKEMON_DATA_KOWAZA_COUNT = 8
};

enum PokemonDataAbilityIndex {
    POKEMON_DATA_ABILITY_FIRST = 0,
    POKEMON_DATA_ABILITY_SECOND = 1
};

enum PokemonDataStatSet {
    POKEMON_DATA_BASE_STATS = 0,
    POKEMON_DATA_EFFORT_YIELD = 1,
    POKEMON_DATA_STAT_SET_COUNT = 2
};

typedef struct PokemonBaseStats {
    u16 mMaxHp;
    u16 mPhysicalAttack;
    u16 mPhysicalDefense;
    u16 mSpecialAttack;
    u16 mSpecialDefense;
    u16 mNimbleness;
} PokemonBaseStats;

typedef struct PokemonEvolution {
    u8 mKind;
    u8 _01;
    u16 mCondition;
    u16 mPokemonDataId;
} PokemonEvolution;

typedef struct PokemonLevelUpMove {
    u8 mLevel;
    u8 _01;
    u16 mWazaDataId;
} PokemonLevelUpMove;

typedef struct PokemonBody {
    u8 mColor;
    u8 _01;
    u16 mStatusFaceMenuSpriteId;
    u32 mPokebodyId;
} PokemonBody;

struct PokemonData {
    u8 mGrowDataId;
    u8 mCatchRate;
    u8 mSexRatio;
    u8 _03;
    u16 mGiveExp;
    u16 mInitFriend;
    u16 mHeight;
    u16 mWeight;
    u16 mVoice;
    u8 _0E[4];
    u16 mNumPokemon; // 0x12
    u16 mNumZukan; // 0x14
    u8 _16[2];
    u32 mNameMessageId;
    u32 mTypeNameMessageId;
    u32 mDescriptionMessageId;
    // id of the zukan seen flag.
    s32 mMitaFlagId;
    // id of the zukan caught flag.
    s32 mTukamaetaFlagId;
    u32 mPkxDataId; // 0x2C
    u8 mZokuseiDataId[POKEMON_DATA_TYPE_COUNT];
    u8 mTokuseiDataId[POKEMON_DATA_ABILITY_COUNT];
    u8 mWazaMachineCompatibility[POKEMON_DATA_MACHINE_COUNT];
    u8 _6E[12];
    u16 mItemDataId[POKEMON_DATA_ITEM_COUNT];
    u16 mKowaza[POKEMON_DATA_KOWAZA_COUNT]; // 0x7E
    PokemonBaseStats mBaseStatsAndEffortYield[POKEMON_DATA_STAT_SET_COUNT];
    PokemonEvolution mSinka[POKEMON_DATA_EVOLUTION_COUNT];
    PokemonLevelUpMove mGetWaza[POKEMON_DATA_LEVEL_UP_MOVE_COUNT];
    PokemonBody mBody[POKEMON_DATA_BODY_COUNT];
};

// Voice id reported for a species that has no voice data to play.
#define POKEMON_VOICE_ID_INVALID 0xFFFF

// Tutor selection, progression gate, and learned-state flag.
typedef struct PokemonTutorMove {
    u16 mMoveId;
    u8 _02[2];
    u32 mProgressionGateIndex;
    u32 mLearnedFlagId;
} PokemonTutorMove;

#ifdef __cplusplus
extern "C" {
#endif
extern const PokemonTutorMove *pokemonTutorMoveTable;
u16 pokemonDataBiosGetHeight(PokemonSpeciesStats *data);
u16 pokemonDataBiosGetWeight(PokemonSpeciesStats *data);
u32 pokemonDataBiosGetTypeName(PokemonSpeciesStats *data);
u8 pokemonDataBiosGetTokuseiDataId(PokemonSpeciesStats *data, u16 index);
PokemonSpeciesStats *pokemonDataBiosGetPtr(u16 species);
s32 pokemonDataBiosGetVoice(PokemonSpeciesStats *stats);
s8 *pokemonNakigoeDataBiosGetDataAddress(PokemonSpeciesStats *stats);
u32 pokemonDataBiosGetZokuseiDataId(PokemonSpeciesStats *data, u16 index);
u32 pokemonDB_GetName(u16 species);
u8 pokemonDataBiosGetWazaMcn(PokemonSpeciesStats *stats, u16 machineNo);


typedef struct PokemonDpFilterData PokemonDpFilterData;
typedef struct PokemonSeikakuData PokemonSeikakuData;
typedef struct PokemonSeikakuRateData PokemonSeikakuRateData;

PokemonSeikakuData *pokemonSeikakuDataBiosGetPtr(u8 id);

u32 pokemonSeikakuDataGetMessageId(PokemonSeikakuData *data);
#ifdef __cplusplus
}
#endif

#endif
