#ifndef DARK_POKEMON_H
#define DARK_POKEMON_H

#include <types.h>
#include <bool.h>

#ifdef __cplusplus
enum darkPokemonStatus {
    DARK_POKEMON_STATUS_NONE = 0,
    DARK_POKEMON_STATUS_FIGHT = 1,
    DARK_POKEMON_STATUS_UNKNOWN = 2,
    DARK_POKEMON_STATUS_SNATCHED = 3,
    DARK_POKEMON_STATUS_RELIVED = 4
};
enum darkPokemonWhere {
    DARK_POKEMON_WHERE_NONE = 0,
    DARK_POKEMON_WHERE_PARTY = 1,
    DARK_POKEMON_WHERE_PCBOX = 2,
    DARK_POKEMON_WHERE_RELIVE_HALL = 3,
    DARK_POKEMON_WHERE_SODATEYA = 4,
    DARK_POKEMON_WHERE_EXCHANGED = 5,
    DARK_POKEMON_WHERE_RELEASED = 6
};
enum darkPokemonSnatchWho { DARK_POKEMON_SNATCH_NONE = 0, DARK_POKEMON_SNATCH_HERO = 1 };

class DarkPokemon {
public:
    u8 getReversPattern() const;
    void setDarkPokemonID(u16 darkID);
    u8 getCatchBallID() const;
    u16 getPokemonID() const;
    bool isSnach() const;
    bool isRelive() const;
    const u16 *getTrainerName() const;
    bool isUpdate() const;
    u32 getRnd() const;
    u16 getCatchFloorId() const;
    u16 getOrgTrainer() const;
    void setPoolFriendLevel(u16 level);
    u16 getPoolFriendLevel() const;
    void setPoolExp(u32 exp);
    u32 getPoolExp() const;
    void setDarkPoint(float point);
    void setDarkPointDirect(s32 point);
    s32 getDarkPointDirect() const;
    s32 getInitDarkPointDirect() const;
    float getDarkPoint() const;
    u8 getSnatchIndex() const;
    void setSnatchIndex(u8 index);
    u8 isWazaNew(u8 index) const;
    void setWazaNew(u8 index, bool value);
    darkPokemonStatus getStatus() const;
    void setStatus(darkPokemonStatus status);
    void setCatchFloorId(u16 floorID);
    void setUpdate(bool update);
    u8 isInformation() const;
    void setInformation(bool information);
    void setRnd(u32 rnd);
    void setSnatchInfoLevel(u8 level);
    u32 getWalkCount() const;
    void setWalkCount(u32 count);
    s8 getStatusRnd(u8 status) const;
    void setStatusRnd(u8 status, u8 rnd);
    void getPlace(darkPokemonWhere &where, u8 &index) const;
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
    u32 mRandom; // 0x1C
    u32 mSnatchTimeState; // 0x20: escape/snatch timing state.
    s32 mDarkPoint; // 0x24
    u16 mTrainerName[11]; // 0x28: ten characters and terminator.
    u8 _3E[0x0A];
};
#else
typedef struct DarkPokemon DarkPokemon;
#endif

/* Pokemon data IDs indexed by dark Pokemon ID. */
extern u16 darkPokemonList[];

/* Number of entries in the dark pokemon list. */
extern u32 darkPokemonList_number;

#ifdef __cplusplus
extern "C" {
#endif

DarkPokemon *darkPokemonGetDarkPokemon(u32 darkID);
u16 darkPokemonBiosGetDarkPoint(u16 darkID);
u16 darkPokemonBiosGetTemotiDarkWaza(u16 darkID, u16 slot);
u32 darkPokemonGetSnatchSuccessNum(void);
u32 darkPokemonGetMeetNum(void);
s32 darkPokemonGetReliveNum(void);
u8 darkPokemonBiosGetReliveFlag(u16 darkID);
u16 darkPokemonBiosGetSteps(u16 darkID);
void darkPokemonBiosSetSteps(u16 darkID, u16 steps);
u8 darkPokemonBiosGetInformationFlag(u16 darkID);
void darkPokemonBiosSetInfomation(u16 darkID, u16 information);
void darkPokemonBiosSetInformationFlag(u16 darkID, bool information);
u16 darkPokemonListBiosGetPokemonID(u16 darkID);
u16 *darkPokemonListBiosGetDPList(u16 darkID);
u32 darkpokemonBiosGetScenarioNum(void);
u32 darkPokemonBiosGetRnd(u16 darkID);
void darkPokemonBiosSetRnd(u16 darkID, u32 rnd);
s32 darkPokemonBiosGetStatusRnd(u16 darkID, u16 status);
void darkPokemonBiosSetStatusRnd(u16 darkID, u16 status, u16 rnd);
u8 darkPokemonBiosGetFightFlag(u16 darkID);
void darkPokemonBiosSetFightFlag(u16 darkID);
u8 darkPokemonBiosGetSnatchSuccessFlag(u16 darkID);
void darkPokemonBiosSetReliveFlag(u16 darkID);
u8 darkPokemonBiosGetLevel(u16 darkID);
u8 darkPokemonBiosGetHokakuritu(u16 darkID);
u16 darkPokemonBiosGetInitDarkPoint(u16 darkID);

#ifdef __cplusplus
}
#endif

#endif
