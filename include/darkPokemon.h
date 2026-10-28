#ifndef DARK_POKEMON_H
#define DARK_POKEMON_H

#include <types.h>

#ifdef __cplusplus
enum darkPokemonStatus {
    DARK_POKEMON_STATUS_NONE = 0,
    DARK_POKEMON_STATUS_FIGHT = 1,
    DARK_POKEMON_SNATCHED = 3,
    DARK_POKEMON_STATUS_RELIVED = 4
};
enum darkPokemonWhere {
    DARK_POKEMON_WHERE_NONE = 0,
    DARK_POKEMON_WHERE_PARTY = 1,
    DARK_POKEMON_WHERE_SODATEYA = 4,
    DARK_POKEMON_EXCHANGED = 5
};
enum darkPokemonSnatchWho { DARK_POKEMON_SNATCH_NONE = 0, DARK_POKEMON_SNATCH_HERO = 1 };
class DarkPokemon {
public:
    u16 getPokemonID() const;
    void setSnatchIndex(u8 index);
    u8 isWazaNew(u8 index) const;
    void setWazaNew(u8 index, bool value);
    darkPokemonStatus getStatus() const;
    void setStatus(darkPokemonStatus status);
    void setCatchFloorId(u16 floorID);
    void setUpdate(bool update);
    void setPlace(darkPokemonWhere where, u8 index);
    void addStatus(darkPokemonStatus status);
    darkPokemonSnatchWho getSnatchFromWho() const;
    void setSnatchFromWho(darkPokemonSnatchWho who);
private:
    u32 mStatusPlaceSnatchState; // 0x00: packed status, place, and snatch-source state.
    u32 mPoolExpState; // 0x04: packed pool experience value.
    u8 _08[3];
    u8 mStatusRandom[6]; // 0x0B: status random data.
    u8 mPlaceIndex; // 0x11: party/place index.
    u8 _12;
    u8 mSnatchIndex; // 0x13: snatch party index.
    u16 mOriginalTrainerId; // 0x14
    u16 mCatchFloorId; // 0x16: floor where it was caught.
    u16 mPoolFriendLevel; // 0x18
    u16 mPokemonID; // 0x1A: associated Pokemon data ID.
    unsigned long mRandom; // 0x1C
    u32 mSnatchTimeState; // 0x20: escape/snatch timing state.
    s32 mDarkPoint; // 0x24
    u16 mTrainerName[11]; // 0x28: ten characters and terminator.
    u8 _3E[0x0A];
};
extern u32 darkPokemonList_number;
#else
typedef struct DarkPokemon DarkPokemon;
#endif

#ifdef __cplusplus
extern "C" {
#endif

DarkPokemon *darkPokemonGetDarkPokemon(u32 darkID);
u32 darkPokemonGetSnatchSuccessNum(void);
s32 darkPokemonGetReliveNum(void);
void darkPokemonBiosSetInfomation(u16 pokemonID, u16 information);

#ifdef __cplusplus
}
#endif

#endif
