#ifndef HERO_H
#define HERO_H

#include <types.h>
#include <item.h>

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
    HERO_RESOURCE_HERO = 100,
    HERO_RESOURCE_MANA = 101,
    HERO_RESOURCE_GIRL = 104,
    HERO_RESOURCE_KLEIN = 105
};

enum HeroFollowerID {
    HERO_FOLLOWER_NONE = 0,
    HERO_FOLLOWER_MANA = 1,
    HERO_FOLLOWER_GIRL = 2,
    HERO_FOLLOWER_KLEIN = 3
};

// Object IDs of the hero and companion models.
enum HeroObjID {
    HERO_OBJ_ID_STYLE0 = 0x16C90400,
    HERO_OBJ_ID_STYLE1 = 0x20250400,
    HERO_OBJ_ID_STYLE2 = 0x169F0400,
    HERO_OBJ_ID_STYLE0_FOLLOWER = 0x169E0400,
    HERO_OBJ_ID_STYLE1_FOLLOWER = 0x1EE20400,
    HERO_OBJ_ID_STYLE2_FOLLOWER = 0x16A00400,
    HERO_OBJ_ID_MANA = 0x16C70400,
    HERO_OBJ_ID_MANA_FOLLOWER = 0x16C80400,
    HERO_OBJ_ID_GIRL_FOLLOWER = 0x195B0400,
    HERO_OBJ_ID_KLEIN = 0x1A7C0400,
    HERO_OBJ_ID_KLEIN_FOLLOWER = 0x19540400
};

// Model level a companion model is drawn with.
enum HeroFollowerModelLevel {
    HERO_MODEL_LEVEL_NORMAL = 0,
    HERO_MODEL_LEVEL_ALTERNATE = 1
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
    Hero &operator=(const Hero &other);

    // Party access.
    const Pokemon *getPokemon(long index) const;
    Pokemon *getPokemon(long index, bool &isHatena);
    Pokemon *getPokemon(long index);
    long addPokemon(const Pokemon *pokemon);
    bool setPokemon(const Pokemon *pokemon, long index);
    bool deletePokemon(long index);
    s16 pokemonGet(HeroPokemonGetParam *param, bool usePcbox);
    s32 getLegendPokemonSize() const;

    // Items.
    bool itemSort(u8 kind);
    int itemQuantity(u16 itemID);

    // Map state.
    u8 getFollowerID();
    void setFollowerID(u8 followerID);
    u8 getFollowerModelLevel();
    void setFollowerModelLevel(u8 level);
    u32 getFollowerGrpID();
    u32 getFollowerResID();
    void setFollowerObjID(u32 objID);
    void setFollowerGrpID(u32 grpID);
    void setFollowerResID(u32 resID);
    u8 getHeroStyle() const;
    void setHeroStyle(u8 style, bool readObject);
    long getHeroObjID(long followerModelLevel, long heroStyle);
    static Hero *getHeroPtr();
    void getRestertPos(GSvec *pos, s16 &angle) const;
    void setRestertPos();
    void addFootStep(unsigned long step);
    u32 getFootStep() const;

    // Persistent trainer identity (save data).
    u32 getHeroRnd() const;

    // Where a suspended battle is resumed.
    unsigned int getBattleResumeFloorID() const;
    void setBattleResumeFloorID(unsigned int floorID);
    u8 getBattleResumeFloorIndex() const;
    void setBattleResumeFloorIndex(u8 index);

    u8 getMeetDarkPokemonCount() const;
    void setMeetDarkPokemonCount(u8 count);
    ItemSlot *getDisk();

public:
    u16 mName[11]; // 0x00: hero name.
    u8 _16[0x16];
    u32 mRandomIdentity; // 0x2C
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
private:
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
public:
    u8 mHeroStyle;
private:
    u16 mBattleResumeFloorID;
    u8 mBattleResumeFloorIndex;
    u8 _95D[0x1B];
};
#else
typedef struct Hero {
    u16 mName[11];
    u8 _16[0x16];
    u32 mRandomIdentity;
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
void heroInit(Hero *hero);
#endif
enum HeroStatusID {
    HERO_STATUS_NONE = 0,
    HERO_STATUS_NAME,
    HERO_STATUS_TRAINER_RND,
    HERO_STATUS_POKEMON,
    HERO_STATUS_ITEM_NORMAL,
    HERO_STATUS_ITEM_EXTRA,
    HERO_STATUS_ITEM_BALL,
    HERO_STATUS_ITEM_SKILL,
    HERO_STATUS_ITEM_SEED,
    HERO_STATUS_ITEM_KORON,
    HERO_STATUS_HIZUKI_ITEM,
    HERO_STATUS_SEX,
    HERO_STATUS_POKEDORU,
    HERO_STATUS_POKECOUPON,
    HERO_STATUS_POKECOUPON_ALL,
    HERO_STATUS_BADGE_01,
    HERO_STATUS_BADGE_02,
    HERO_STATUS_BADGE_03,
    HERO_STATUS_BADGE_04,
    HERO_STATUS_BADGE_05,
    HERO_STATUS_BADGE_06,
    HERO_STATUS_BADGE_07,
    HERO_STATUS_BADGE_08,
    HERO_STATUS_HIZUKI_NAME,
    HERO_STATUS_HIZUKI_FLAG,
    HERO_STATUS_HOME_PLACE,
    HERO_STATUS_COUNT
};

s32 heroGetObjID(u8 heroType);
u8 heroMoveIsMember(s32 member);
u8 heroMoveGetResID(u32* group, u32* id, s32 member);
void heroMoveTermEvent(void);
void heroMoveSetLockFrame(s32 frames);
void heroAddPokedoru(Hero* hero, u32 amount);
u32 heroGetPokeomCount(Hero* hero);
void heroDecPokedoru(Hero *hero, u32 amount);
u32 heroGetStatus(Hero* hero, u16 statusID, u16 slot);
u8 heroAddPokemon(Hero *hero, const Pokemon *pokemon);
void heroSetStatus(Hero* hero, u16 statusID, u32 value);
u16* heroBiosGetNamePtr(Hero* hero);
Pokemon* heroBiosGetPokemonPtr(Hero* hero, u16 slot);
void heroSetFollowerWithID(u8 followerID, u8 modelLevel);
u32 heroCreateRnd(void);
void heroSetFollower(u32 groupID, u32 resourceID, u32 objectID);

#ifdef __cplusplus
bool heroIsPlayerPeople(u32 groupID, u32 resourceID);
s16 heroGetPokemon(Hero *hero, const Pokemon *pokemon, bool usePcbox);
s16 heroCatchPokemon(Hero *hero, const Pokemon *pokemon, u16 catchFloorID, u8 catchBallID, bool usePcbox);
#endif
ItemSlot *heroItemGetItemKindToItemAryPtr(Hero *hero, u8 kind, u16 *count, u16 *maxQuantity, u8 *sortMode, u8 *insertionMode);
ItemSlot *heroItemGetItemDataIdToItemAryPtr(Hero *hero, u16 itemID, u16 *num, u16 *max, u8 *sortMode, u8 *insertionMode);

void heroDecPokecoupon(Hero *hero, s32 point);
s32 heroItemCheckAddItemDataId(Hero* hero, u16 itemID);
s32 heroItemAddItemDataId(Hero* hero, u16 itemID, u16 count, s16 menuPos);
s32 heroItemDecItemDataId(Hero* hero, u16 itemID, u16 count, s16 menuPos);
#ifdef __cplusplus
}
#endif

#endif
