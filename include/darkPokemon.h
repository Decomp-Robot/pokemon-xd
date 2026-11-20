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
    u32 getWalkCount() const;
    void setWalkCount(u32 count);
    void setReversPattern(u8 pattern);
    u8 getReversPattern() const;
    s32 getInitDarkPointDirect() const;
    void setCatchBallID(u16 ballID);
    u8 getCatchBallID() const;
    void setDarkPokemonID(u16 darkID);
    u16 getDarkPokemonID() const;
    void setPokemonID(u16 id);
    u16 getPokemonID() const;
    bool isSnach() const;
    bool isRelive() const;
    void setTrainerName(const u16 *name);
    const u16 *getTrainerName() const;
    void setStatusRnd(u8 status, u8 rnd);
    u8 getStatusRnd(u8 status) const;
    void setInformation(bool information);
    bool isInformation() const;
    void setUpdate(bool update);
    bool isUpdate() const;
    void setEscapeTime(u32 time);
    u32 getEscapeTime() const;
    void setSnatchTime(u32 time);
    void setRnd(u32 rnd);
    u32 getRnd() const;
    void setCatchFloorId(u16 floorID);
    u16 getCatchFloorId() const;
    void setOrgTrainer(u16 trainer);
    u16 getOrgTrainer() const;
    void setSnatchIndex(u8 index);
    u8 getSnatchIndex() const;
    void getPlace(darkPokemonWhere &where, u8 &index) const;
    void setPlace(darkPokemonWhere where, u8 index);
    void setWazaNew(u8 index, bool value);
    bool isWazaNew(u8 index) const;
    void setPoolFriendLevel(u16 level);
    u16 getPoolFriendLevel() const;
    void setPoolExp(unsigned long exp);
    unsigned long getPoolExp() const;
    void setDarkPointDirect(long point);
    long getDarkPointDirect() const;
    void setReversMode(bool mode);
    bool isReversMode() const;
    void setSnatchFromWho(darkPokemonSnatchWho who);
    darkPokemonSnatchWho getSnatchFromWho() const;
    void setSnatchInfoLevel(u8 level);
    darkPokemonStatus getStatus() const;
    void addStatus(darkPokemonStatus status);
    void setStatus(darkPokemonStatus status);
    void setDarkPoint(float point);
    float getDarkPoint() const;
    void init();
private:
    u32 mStatus : 3;
    u32 mUnused : 1;
    u32 mSnatchInfoLevel : 3;
    u32 mSnatchFromWho : 2;
    u32 mReversMode : 1;
    u32 mUpdate : 1;
    u32 mWazaNew : 3;
    u32 mPlace : 3;
    u32 mPoolExp : 20;
    u32 mCatchBallID : 4;
    u32 mInformation : 1;
    u8 _08[3];
    u8 mStatusRandom[6];
    u8 mPlaceIndex;
    u8 _12;
    u8 mSnatchIndex;
    u16 mOrgTrainer;
    u16 mCatchFloorId;
    u16 mPoolFriendLevel;
    u16 mPokemonID;
    u32 mRandom;
    u32 mSnatchTimeState;
    s32 mDarkPoint;
    u16 mTrainerName[11];
    u16 mDarkPokemonID;
    u8 mReversPattern;
    u8 _41[3];
    u32 mWalkCount;
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
bool darkPokemonBiosGetInformationFlag(u16 darkID);
void darkPokemonBiosSetInfomation(u16 darkID, u16 information);
void darkPokemonBiosSetInformationFlag(u16 darkID, bool information);
u8 darkpokemonCheckUnderTaker(void);
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
void darkPokemonBiosSetReliveFlag(u16 darkID, u8 reliveFlag);
u8 darkPokemonBiosGetLevel(u16 darkID);
u8 darkPokemonBiosGetHokakuritu(u16 darkID);
u16 darkPokemonBiosGetInitDarkPoint(u16 darkID);

#ifdef __cplusplus
}
#endif

#endif
