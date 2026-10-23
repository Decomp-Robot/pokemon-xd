#ifndef HERO_H
#define HERO_H

#include <types.h>
#include <item.h>

enum { HERO_STATUS_TRAINER_ID = 2,
HERO_STATUS_ITEM_NORMAL = 4,
HERO_STATUS_ITEM_EXTRA = 5,
HERO_STATUS_ITEM_BALL = 6,
HERO_STATUS_ITEM_SKILL = 7,
HERO_STATUS_ITEM_SEED = 8,
HERO_STATUS_ITEM_KORON = 9,
HERO_STATUS_HIZUKI_ITEM = 10,
HERO_STATUS_SEX = 11,
HERO_STATUS_POKEDORU = 12,
HERO_STATUS_POKECOUPON = 13,
HERO_STATUS_POKECOUPON_ALL = 14,
HERO_STATUS_BADGE_01 = 15,
HERO_STATUS_BADGE_02 = 16,
HERO_STATUS_BADGE_03 = 17,
HERO_STATUS_BADGE_04 = 18,
HERO_STATUS_BADGE_05 = 19,
HERO_STATUS_BADGE_06 = 20,
HERO_STATUS_BADGE_07 = 21,
HERO_STATUS_BADGE_08 = 22,
HERO_STATUS_HIZUKI_FLAG = 24,
HERO_STATUS_HOME_PLACE = 25 };

enum { HERO_PTR_FLAG = 0x30D };

enum { HERO_STYLE_NORMAL = 0,
HERO_STYLE_VISOR = 1,
HERO_STYLE_SNATCH = 2 };
#include <Pokemon.h>
#include <item.h>

// Player inventory, money and party.
#ifdef __cplusplus
struct GSvec;

enum HERO_RESOURCE_ID {
    HERO_RESOURCE_MANA = 0x65,
    HERO_RESOURCE_FOLLOWER_SLOT_2 = 0x68,
    HERO_RESOURCE_FOLLOWER_SLOT_3 = 0x69
};

extern "C" void heroMoveGetHeroPos(GSvec* out);
/* Member code of the selected follower, or 0 when the hero walks alone. */
extern "C" u32 heroGetFollower(void);

struct HeroPokemonGetParam {
    u8 sexDataId;          // 0x00: sex data ID the pokemon is booked with.
    u8 level;              // 0x01
    u16 species;           // 0x02
    u16 shadowRecordId;    // 0x04: shadow data record ID; zero for a non-shadow pokemon.
    u16 friendLevel;       // 0x06
    u16 waza[4];           // 0x08
    unsigned int trainerId;         // 0x10: trainer ID the pokemon is booked to.
    u16 *catchTrainerName; // 0x14: name of the trainer that caught it.
    u16 *nickname;         // 0x18: nickname the pokemon is given, 0 to keep the species name.
    unsigned int catchFloorId;      // 0x1C: catch location; stored as u16 in the pokemon record.
    unsigned int exp;               // 0x20: experience the pokemon is created with.
};

class Hero {
public:
    Hero() {}

    // Party access.
    const Pokemon *getPokemon(long index) const;
    Pokemon *getPokemon(long index, bool &isHatena);
    Pokemon *getPokemon(long index);
    long addPokemon(const Pokemon *pokemon);
    bool setPokemon(const Pokemon *pokemon, long index);
    bool deletePokemon(long index);
    s16 pokemonGet(HeroPokemonGetParam *param, bool allowStorageFallback);
    s32 getLegendPokemonSize() const;

    // Items.
    bool itemSort(u8 kind);
    u16 itemQuantity(u16 itemID);

    // Map state.
    u8 getFollowerID();
    u8 getFollowerModelLevel();
    u8 getHeroStyle() const;
    void setHeroStyle(u8 style, bool readObject);
    long getHeroObjID(long followerModelLevel, long heroStyle);
    static Hero *getHeroPtr();
    void getRestertPos(GSvec *pos, s16 &angle) const;
    void setRestertPos();
    void addFootStep(unsigned long step);
    u32 getFootStep() const;

    // Where a suspended battle is resumed.
    unsigned int getBattleResumeFloorID() const;
    void setBattleResumeFloorID(unsigned int floorID);
    u8 getBattleResumeFloorIndex() const;
    void setBattleResumeFloorIndex(u8 index);

    u8 getMeetDarkPokemonCount() const;
    void setMeetDarkPokemonCount(u8 count);
    void *getDisk();

private:
    u16 mName[11]; // 0x00: hero name.
    u8 _16[0x16];
    unsigned long mRandomIdentity; // 0x2C
    Pokemon mPokemon[6];
    ItemSlot mNormalItems[30]; // 0x4C8
    ItemSlot mExtraItems[43]; // 0x540
    ItemSlot mBallItems[16]; // 0x5EC
    ItemSlot mSkillItems[64]; // 0x62C
    ItemSlot mSeedItems[46]; // 0x72C
    ItemSlot mKoronItems[3]; // 0x7E4
    ItemSlot mDisk[60];
    u8 mSex; // 0x8E0
    u8 mHomePlace; // 0x8E1
    u8 _8E2[2];
    u32 mMoney; // 0x8E4
    u32 mCoupon; // 0x8E8
    u32 mCouponTotal; // 0x8EC
    u8 mBadges[8]; // 0x8F0
    u8 mHizukiFlag; // 0x8F8
    ItemSlot mHizukiItems[10]; // 0x8FA
    u16 mHizukiName[11]; // 0x922
    u8 mMeetDarkPokemonCount;
    u8 mFollowerID;
    s16 mRestertAngle;
    f32 mRestertX;
    f32 mRestertY;
    f32 mRestertZ;
    unsigned int mFootStep;
    u32 mFollowerResource; // 0x94C
    u32 mFollowerGroup; // 0x950
    u32 mFollowerObject; // 0x954
    u8 mFollowerModelLevel; // 0x958
    u8 mHeroStyle;
    u16 mBattleResumeFloorID;
    u8 mBattleResumeFloorIndex;
    u8 _95D[0x1B];
};
#else
typedef struct Hero {
    u16 mName[11];
    u8 _16[0x16];
    unsigned long mRandomIdentity;
    Pokemon mPokemon[6];
    ItemSlot mNormalItems[30];
    ItemSlot mExtraItems[43];
    ItemSlot mBallItems[16];
    ItemSlot mSkillItems[64];
    ItemSlot mSeedItems[46];
    ItemSlot mKoronItems[3];
    ItemSlot mDisk[60];
    u8 mSex;
    u8 mHomePlace;
    u8 _8E2[2];
    u32 mMoney;
    u32 mCoupon;
    u32 mCouponTotal;
    u8 mBadges[8];
    u8 mHizukiFlag;
    ItemSlot mHizukiItems[10];
    u16 mHizukiName[11];
    u8 mMeetDarkPokemonCount;
    u8 mFollowerID;
    s16 mRestertAngle;
    f32 mRestertX;
    f32 mRestertY;
    f32 mRestertZ;
    unsigned int mFootStep;
    u32 mFollowerResource;
    u32 mFollowerGroup;
    u32 mFollowerObject;
    u8 mFollowerModelLevel;
    u8 mHeroStyle;
    u16 mBattleResumeFloorID;
    u8 mBattleResumeFloorIndex;
    u8 _95D[0x1B];
} Hero;
#endif

#ifdef __cplusplus
extern "C" {
#endif
enum HeroStatusID {
    HERO_STATUS_NAME = 1,
    HERO_STATUS_TRAINER_RND = 2,
    HERO_STATUS_POKEMON = 3,
    HERO_STATUS_MONEY = 12,
    HERO_STATUS_HIZUKI_NAME = 23
};

s32 heroGetObjID(s32 heroType);
u8 heroMoveIsMember(s32 member);
u8 heroMoveGetResID(u32* group, u32* id, s32 member);
void heroMoveTermEvent(void);
unsigned long heroGetPokeomCount(Hero* hero);
u32 heroGetStatus(Hero* hero, u16 statusID, u16 slot);
void heroSetStatus(Hero* hero, u16 statusID, u32 value);
u16* heroBiosGetNamePtr(Hero* hero);
Pokemon* heroBiosGetPokemonPtr(Hero* hero, u16 slot);

#ifdef __cplusplus
s16 heroGetPokemon(Hero *hero, const Pokemon *pokemon, bool allowStorageFallback);
s16 heroCatchPokemon(Hero *hero, const Pokemon *pokemon, u16 catchFloorID, u8 catchBallID, bool allowStorageFallback);
#endif
ItemSlot *heroItemGetItemKindToItemAryPtr(Hero *hero, u8 kind, u16 *count, u16 *maxQuantity, u8 *sortMode, u8 *insertionMode);
ItemSlot *heroItemGetItemDataIdToItemAryPtr(Hero *hero, u16 itemID, u16 *num, u16 *max, u8 *sortMode, u8 *insertionMode);

s32 heroItemCheckAddItemDataId(Hero* hero, u16 itemID);
s32 heroItemAddItemDataId(Hero* hero, u16 itemID, u16 count, s16 menuPos);
s32 heroItemDecItemDataId(Hero* hero, u16 itemID, u16 count, s16 menuPos);
#ifdef __cplusplus
}
#endif

#endif
