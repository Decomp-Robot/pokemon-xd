#ifndef DARK_POKEMON_H
#define DARK_POKEMON_H

#include <types.h>

// Shadow-data status thresholds: 1 allows fighting, 3 marks a successful
// snatch, and 4 additionally marks the Pokemon as relived.
enum darkPokemonStatus {
    DARK_POKEMON_STATUS_NONE = 0,
    DARK_POKEMON_STATUS_FIGHT = 1,
    DARK_POKEMON_STATUS_SNATCH_SUCCESS = 3,
    DARK_POKEMON_STATUS_RELIVED = 4,
};

typedef struct DarkPokemon DarkPokemon;

extern unsigned long darkPokemonList_number;

// Saved shadow Pokemon record.
#ifdef __cplusplus
struct DarkPokemon {
    /* 0x00 */ u8 mStatusFlags;
    /* 0x01 */ u8 mBattleFlags;
    /* 0x02 */ u8 mUnresolvedHeader[0x13 - 0x02];
    /* 0x13 */ u8 mSnatchIndex;
    /* 0x14 */ u16 mOrgTrainer;
    /* 0x16 */ u16 mCatchFloorId;
    /* 0x18 */ u16 mPoolFriendLevel;
    /* 0x1A */ u16 mPokemonID;
    /* 0x1C */ unsigned long mRnd;
    /* 0x20 */ unsigned long mEscapeOrSnatchTime;
    /* 0x24 */ long mDarkPoint;
    /* 0x28 */ u16 mTrainerName[11];
    /* 0x3E */ u16 mDarkPokemonID;
    /* 0x40 */ u8 mUnresolvedTail[0x48 - 0x40];

    u16 getPokemonID() const;
    void setSnatchIndex(u8 index);
    u8 isWazaNew(u8 index) const;
    void setWazaNew(u8 index, bool value);
    void setStatus(darkPokemonStatus status);
    darkPokemonStatus getStatus(void) const;
    void setUpdate(bool update);
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
DarkPokemon *darkPokemonGetDarkPokemon(unsigned long shadowID);
s32 darkPokemonGetSnaggedCount(void);
s32 darkPokemonGetMeetNum(void);
u32 darkPokemonCountSnaggedScenario(void);
#ifdef __cplusplus
}
#endif

#endif
