#ifndef POKEMON_H
#define POKEMON_H

#include <types.h>
#include <gamedata.h>
#include <darkPokemon.h>

// Pokemon -- party/battle pokemon record.
typedef struct Pokemon Pokemon;
typedef struct PokemonData PokemonData;
struct Pokemon {
    u16 mPokemonDataId; // 0x00: species data ID.
    u16 mItemDataId; // 0x02: held item data ID.
    u16 mHp; // 0x04: current HP.
    u16 mFriendLevel; // 0x06: friendship.
    u16 mCatchFloorId; // 0x08: floor where the Pokemon was caught.
    u8 _0A[4];
    u8 mCatchLevel; // 0x0E: level when caught.
    u8 mCatchBallId; // 0x0F: ball used to catch it.
    u8 mCatchTrainerSex; // 0x10: sex of the catching trainer.
    u8 mLevel; // 0x11: current level.
    u8 _12[2];
    u8 mPcBoxMark; // 0x14
    u8 _15;
    u8 mCondition; // 0x16
    u8 mConditionCount; // 0x17
    u8 mConditionTurn; // 0x18
    u8 mConditionTurnNow; // 0x19
    u8 _1A[6];
    u32 mExp; // 0x20: total experience.
    unsigned long mCatchTrainerRnd; // 0x24: catch trainer random identity value.
    unsigned long mPersonality; // 0x28: personality value.
    u8 _2C[8];
    GamedataAttest mAttest; // 0x34: origin/version attestation.
    u16 mTrainerName[11]; // 0x38: trainer name copy buffer.
    u16 mNickname[11]; // 0x4E: nickname copy buffer.
    u16 mOriginalNickname[11]; // 0x64: original nickname copy buffer.
    u8 _7A[2];
    u32 mRibbonData; // 0x7C
    u32 mWaza[4]; // 0x80: four move records.
    u16 mHpStat; // 0x90
    u16 mAttackStat; // 0x92
    u16 mDefenseStat; // 0x94
    u16 mSpecialAttackStat; // 0x96
    u16 mSpecialDefenseStat; // 0x98
    u16 mSpeedStat; // 0x9A
    u16 mHpEffort; // 0x9C
    u16 mAttackEffort; // 0x9E
    u16 mDefenseEffort; // 0xA0
    u16 mSpecialAttackEffort; // 0xA2
    u16 mSpecialDefenseEffort; // 0xA4
    u16 mSpeedEffort; // 0xA6
    u8 _A8[0x12];
    u16 mDarkPokemonDataId; // 0xBA
    u8 _BC[8];

#ifdef __cplusplus
    bool isHatena() const;
    bool isFuseiFlag() const;
    GamedataAttest *getAttest();
    void setFriendLevel(u16 level);
    void setExp(unsigned long exp);
    void setNicknamePtr(u16 *name);
    void setCatchTrainerNamePtr(u16 *name);
    void setCatchFloorId(u16 catchFloorId);
    void setCatchLevel(u8 catchLevel);
    void setCatchBallId(u8 catchBallId);
    void setCatchTrainerSex(unsigned char sex);
    void clear();
    bool checkValid() const;
    void setDarkPoint(float point);
    u16 getPokemonDataId() const;
    u16 getHp() const;
    u8 getLevel() const;
    u16* getNicknamePtr();
    const DarkPokemon* getDarkPokemon() const;
    DarkPokemon* getDarkPokemon();
    PokemonData* getPokemonDataPtr() const;
    u16 getItemDataId() const;
    bool isLegend() const;
    u8 isDarkPokemon() const;
    unsigned long getCatchTrainerRnd() const;
    void setCatchTrainerRnd(unsigned long rnd);
#endif
};

// Friendship changes selected by the event that caused them.
enum POKEMON_FRIEND_FILTER_ID {
    POKEMON_FRIEND_FILTER_LEARN_MOVE = 4
};

// Fields read out of a pokemon with pokemonGetStatus.
enum POKEMON_STATUS_FIELD {
    POKEMON_STATUS_FIELD_HELD_ITEM = 0x82,
    POKEMON_STATUS_FIELD_DARK_POINTS = 0xC5,
    POKEMON_STATUS_FIELD_REVERSE_MODE = 0xC8,
    POKEMON_STATUS_FIELD_CHAMP_RIBBON = 0xA3,
    POKEMON_STATUS_FIELD_WINNING_RIBBON = 0xA4,
    POKEMON_STATUS_FIELD_VICTORY_RIBBON = 0xA5,
    POKEMON_STATUS_FIELD_BROMIDE_RIBBON = 0xA6,
    POKEMON_STATUS_FIELD_GANBA_RIBBON = 0xA7,
    POKEMON_STATUS_FIELD_MARINE_RIBBON = 0xA8,
    POKEMON_STATUS_FIELD_LAND_RIBBON = 0xA9,
    POKEMON_STATUS_FIELD_SKY_RIBBON = 0xAA,
    POKEMON_STATUS_FIELD_COUNTRY_RIBBON = 0xAB,
    POKEMON_STATUS_FIELD_NATIONAL_RIBBON = 0xAC,
    POKEMON_STATUS_FIELD_EARTH_RIBBON = 0xAD,
    POKEMON_STATUS_FIELD_WORLD_RIBBON = 0xAE,
    POKEMON_STATUS_FIELD_STYLE_MEDAL = 0xB0,
    POKEMON_STATUS_FIELD_BEAUTIFUL_MEDAL = 0xB1,
    POKEMON_STATUS_FIELD_CUTE_MEDAL = 0xB2,
    POKEMON_STATUS_FIELD_CLEVER_MEDAL = 0xB3,
    POKEMON_STATUS_FIELD_STRONG_MEDAL = 0xB4,
    POKEMON_STATUS_FIELD_WAZA_PP = 0x80, // current PP of the given waza slot.
    POKEMON_STATUS_FIELD_WAZA_ID = 0x7F, // waza id of the given waza slot.
    POKEMON_STATUS_FIELD_TRAINER_ID = 0x75, // id of the trainer that met the pokemon.
    POKEMON_STATUS_FIELD_TRAINER_NAME = 0x76, // name of that trainer.
    POKEMON_STATUS_FIELD_NICKNAME = 0x77, // name the pokemon is given.
    POKEMON_STATUS_FIELD_SPECIES = 0x6E, // species of the pokemon.
    POKEMON_STATUS_FIELD_PID = 0x6F, // personality value used to fit the pokemon's data.
    POKEMON_STATUS_FIELD_ATTEST = 0x70, // attest record; gamedataGetStatus(record, 2) selects the origin text.
    POKEMON_STATUS_FIELD_MET_PLACE = 0x71, // place the pokemon was caught in.
    POKEMON_STATUS_FIELD_MET_LEVEL = 0x72, // level the pokemon was caught at.
    POKEMON_STATUS_FIELD_LEVEL = 0x7A, // level the pokemon is on.
    POKEMON_STATUS_FIELD_EXP = 0x79, // total experience of the pokemon.
    POKEMON_STATUS_FIELD_FRIEND_LEVEL = 0x99, // friendship level of the pokemon.
    POKEMON_STATUS_FIELD_HP = 0x83, // current HP.
    POKEMON_STATUS_FIELD_HP_MAX = 0x87, // maximum HP.
    POKEMON_STATUS_FIELD_ATTACK = 0x88, // battle stats shown on the summary page.
    POKEMON_STATUS_FIELD_DEFENSE = 0x89, // physical defense.
    POKEMON_STATUS_FIELD_SP_ATTACK = 0x8A, // special attack.
    POKEMON_STATUS_FIELD_SP_DEFENSE = 0x8B, // special defense.
    POKEMON_STATUS_FIELD_SPEED = 0x8C, // nimbleness/speed.
    POKEMON_STATUS_FIELD_POKERUS = 0xB5, // pokerus of the pokemon.
    POKEMON_STATUS_FIELD_MARKINGS = 0xBB, // markings the pokemon wears.
    POKEMON_STATUS_FIELD_NATURE = 0xBF, // nature index, into the nature table.
    POKEMON_STATUS_FIELD_WAZA_SEALED = 0xC2, // non-zero while the waza pages stay closed.
};

// Marking bits of Pokemon field POKEMON_STATUS_FIELD_MARKINGS (0xBB). Slot N
// is visible when its bit is set; the bit drives the slot sprite of the same slot.
enum POKEMON_MARKING {
    POKEMON_MARKING_SLOT0 = 8, // marking slot 0.
    POKEMON_MARKING_SLOT1 = 4, // marking slot 1.
    POKEMON_MARKING_SLOT2 = 2, // marking slot 2.
    POKEMON_MARKING_SLOT3 = 1, // marking slot 3.
};

// Pokerus active-infection mask of Pokemon field POKEMON_STATUS_FIELD_POKERUS (0xB5).
// A non-zero low nibble draws the infected icon; a zero nibble with a non-zero
// byte draws the cured icon.
enum POKEMON_POKERUS {
    POKEMON_POKERUS_ACTIVE_MASK = 0xF, // active pokerus infection in the low nibble.
};

enum { POKEMON_STATUS_CONDITION = 0x7C, POKEMON_STATUS_HP = 0x83 };

#ifdef __cplusplus
extern "C" {
#endif
u16 pokemon_GetHp(Pokemon *pokemon);
u16 pokemon_GetMaxHp(Pokemon *pokemon);
void pokemon_SetHp(Pokemon *pokemon, u16 hp);
void pokemonInitJoutai(Pokemon *pokemon);
void pokemonInitJoutaiDataId(Pokemon *pokemon, u16 joutaiID);

/* pokemonGetStatus query IDs. POKEMON_STATUS_SPECIES reads the record's
   species halfword (zero for an empty slot); POKEMON_STATUS_MOVE reads the
   move ID of the slot the fourth argument selects. */
enum PokemonStatusQuery {
    POKEMON_STATUS_SPECIES = 0x6E,
    POKEMON_STATUS_MOVE = 0x7F,
    POKEMON_QUERY_LEVEL = 0x7A,
    POKEMON_QUERY_SPECIES = 0x6E,
    POKEMON_QUERY_MOVE_PP = 0x80,
    POKEMON_QUERY_ATTACK = 0x88,
    POKEMON_QUERY_DEFENSE = 0x89,
    POKEMON_QUERY_SPECIAL_ATTACK = 0x8A,
    POKEMON_QUERY_SPECIAL_DEFENSE = 0x8B,
    POKEMON_QUERY_SPEED = 0x8C,
    POKEMON_QUERY_IS_FAINTED = 0x7B,
    POKEMON_QUERY_MOVE_ID = 0x7F,
    POKEMON_QUERY_PARTY_RECORD = 0xCC
};

enum PokemonSex {
    POKEMON_SEX_MALE = 0,
    POKEMON_SEX_FEMALE = 1,
    POKEMON_SEX_NONE = 2,
};

enum PokemonAilment {
    POKEMON_CONDITION_NONE = 0,
    POKEMON_CONDITION_POISON = 3,
    POKEMON_CONDITION_BAD_POISON = 4,
    POKEMON_CONDITION_BURN = 5,
    POKEMON_CONDITION_FREEZE = 6,
    POKEMON_CONDITION_PARALYSIS = 7,
    POKEMON_CONDITION_SLEEP = 8,
};

#ifdef __cplusplus
bool pokemonCheckValid(Pokemon *pokemon);
#else
u8 pokemonCheckValid(Pokemon *pokemon);
#endif
u16 pokemonGetTokuseiDataId(Pokemon *pokemon);
u16 pokemon_GetPokemonDataId(Pokemon *pokemon);
u16 battlePokemonStatusGetCurrentHp(Pokemon *pokemon);
// The field ID may select party data or deployed fight-record data.
s32 pokemonGetStatus(void *record, u16 speciesId, s32 field, u16 elementIndex);
void pokemonSetStatus(void *record, unsigned short pokemonDataID, int fieldID, int index, unsigned long value);
void pokemon_SetOriginalTrainerRandom(Pokemon *pokemon, u32 rnd);
void pokemon_SetPersonalityRandom(Pokemon *pokemon, u32 rnd);
u8 pokemonIsDarkPokemon(Pokemon *pokemon);
u16 pokemonGetJoutaiMenuSpriteId(Pokemon *pokemon);
f32 pokemonGetDp(Pokemon *pokemon);
u8 pokemonIsJoutaiDataId(Pokemon *pokemon, u16 joutaiID);
void pokemonInitJoutaiDataId(Pokemon *pokemon, u16 joutaiID);
void pokemonAddDpFormPokemonDpFilterId(Pokemon *pokemon, u16 itemID, u32 filterID);
void pokemonWazaCreate(Pokemon *pokemon, u16 slot, u16 wazaID);
s8 pokemonSearchWazaDataId(Pokemon *pokemon, u16 wazaID);
void pokemonReplace(Pokemon *dst, Pokemon *src);
u8 pokemonIsShadowPokemon(Pokemon *pokemon);
// Species and held item of a party Pokemon record.
u16 pokemonBiosGetPokemonDataId(Pokemon *pokemon);
u16 pokemonBiosGetItemDataId(Pokemon *pokemon);
u16 pokemonGetSoubiItemDataId(Pokemon *pokemon);
// Held item's equipment effect id.
u16 pokemonGetSoubiItemSoubiDataId(Pokemon *pokemon);
void pokemonGetFriendFormPokemonFriendFilterId(Pokemon *pokemon, u16 heldEffectID, u32 filterID);
// Removes and returns the held item when apply is zero; otherwise equips itemID.
u16 pokemonDoItemSoubi(Pokemon *pokemon, u16 itemID, u8 apply);

void pokemonAllKaihuku(Pokemon *pokemon);
void pokemonResetBasisStatus(Pokemon *pokemon);
void pokemonSetCatchStatus(Pokemon *pokemon, u16 catchFloorId, u8 catchLevel, u8 catchBallId,
                          u8 sexDataId, unsigned int trainerId, const u16 *name);
void pokemonBiosCopy(Pokemon *pokemon, const Pokemon *src);
void pokemonInit(Pokemon *pokemon);
void pokemonCreate(Pokemon *pokemon, u16 species, int level, void *attest);
void pokemonSetDarkPokemonStatus(Pokemon *pokemon, u16 shadowID);
u16 pokemonBiosGetInitDp(Pokemon *pokemon);
void pokemonSetDp(Pokemon *pokemon, f32 dp);
// Level of a Pokemon record.
u8 pokemonBiosGetLevel(Pokemon *pokemon);
// Next move learned at this level; advances the learnset index.
u16 pokemonGetOboeWazaDataId(Pokemon *pokemon, u8 level, u8 *outIndex);
// Gender and nickname of a Pokemon. Nicknames are UTF-16 strings of at most
// ten characters; the name entry mode, the message layer and the link
// exchange each copy them into their own name buffers.
u8 pokemonGetSex(Pokemon *pokemon);
u16 *pokemon_GetNickname(Pokemon *pokemon);

s32 pokemon_GetSeikaku(Pokemon *pokemon);
s32 pokemonAdjustValueBySeikaku(s32 nature, s32 fieldID, s32 value);
u32 pokemonCreateRndFit(Pokemon *pokemon, s8 sex, s8 nature, s8 rare, u32 trainerRnd);
void *pokemonCreateSequence(struct FightPokemon *pokemon);
void pokemonSetSequenceStatus(Pokemon *pokemon, void *sequence);

u8 pokemonWazaCheckValid(Pokemon *pokemon, u16 slot);

u8 pokemonWazaGetMaxPP(Pokemon *pokemon, u16 slot);

void pokemonWazaReplace(Pokemon *pokemon, s32 prev, s32 slot);

s32 pokemonGetLevelToExp(Pokemon *pokemon, u8 level);

u8 pokemonGetDarkPokemonLevel(Pokemon *pokemon);

u8 pokemonCheckRare2(Pokemon *pokemon);

void pokemonPlayVoice(Pokemon *pokemon);
void pokemonWazaForget(Pokemon *pokemon, int slot);
int pokemonGetWazaCount(Pokemon *pokemon);
s8 pokemonSetWazaStatus(Pokemon *pokemon, u16 wazaID, u8 replace);
void pokemonWazaCopy(Pokemon *pokemon, u16 destination, u16 source);
u16 *pokemon_GetName(Pokemon *pokemon);
u8 pokemonGetDarkPokemonCallKakuritu(Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
