#include <hero.h>
#include <heroItem.h>
#include <heroBios.h>
#include <heroMove.h>
#include <people.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <itemData.h>
#include <item.h>
#include <savedata.h>
#include <GSmsg.h>
#include <GSlog.h>
#include <hsdRandom.h>
#include <app/pcbox/pcbox.h>
#include <app/floor/floorModule.h>
#include <floorManager.h>

Hero* _getHero();
void _heroExitFloor(unsigned int, FloorEnterMode);
void _heroEnterFloor(unsigned int, FloorEnterMode);
const char* _heroGetUniqueModuleName();
void _heroMakeStateData(FloorStateMode, void*, u32);
void _heroRestoreStateData(FloorStateMode, void*, u32);
u32 _heroGetStateSize(FloorStateMode);
u32 _heroGetUID();

static FloorModuleInterface _floorModuleHero;
static u32 _hModuleHero;


namespace {

const int HERO_PARTY_SIZE = 6;
const int HERO_ITEM_STOCK_MAX = 999;

enum HeroPokemonGetResult {
    HERO_POKEMON_TO_BOX = -1,
    HERO_POKEMON_PARTY_FULL = -2
};

const unsigned char HERO_MODULE_PRIORITY = 25;
const long HERO_MOTION_STAND = 3;
const u16 HERO_HIZUKI_NAME_MSG = 1;

} // namespace

extern "C" {

u32 heroGetPokeomCount(Hero* hero)
{
    u16 i;
    u16 count = 0;

    for (i = 0; i < HERO_PARTY_SIZE; i++) {
        if (pokemonCheckValid(heroBiosGetPokemonPtr(hero, i))) {
            count++;
        }
    }
    return count;
}

s32 heroGetEmptySlot(Hero* hero)
{
    s32 i;

    for (i = 0; i < HERO_PARTY_SIZE; i++) {
        Pokemon* pokemon = heroBiosGetPokemonPtr(hero, i);

        if (!pokemonCheckValid(pokemon)) {
            return i;
        }
        pokemonBiosGetDp(pokemon);
    }
    return -1;
}

u16 heroGetLivingPokemonCount(Hero* hero)
{
    s32 i;
    Pokemon* pokemon;
    u16 count = 0;

    for (i = 0; i < HERO_PARTY_SIZE; i++) {
        pokemon = heroBiosGetPokemonPtr(hero, i);

        if (pokemonCheckValid(pokemon) &&
            pokemonGetStatus(pokemon, 0, POKEMON_QUERY_IS_FAINTED, 0) == 0) {
            count++;
        }
    }
    return count;
}

void heroDecPokecoupon(Hero* hero, s32 point)
{
    heroSetStatus(hero, HERO_STATUS_POKECOUPON, heroGetStatus(hero, HERO_STATUS_POKECOUPON, 0) - point);

    if (point <= 0) {
        heroSetStatus(hero, HERO_STATUS_POKECOUPON_ALL,
                      heroGetStatus(hero, HERO_STATUS_POKECOUPON_ALL, 0) - point);
    }
}

void heroAddPokecoupon(Hero* hero, s32 point)
{
    u32 value = heroGetStatus(hero, HERO_STATUS_POKECOUPON, 0);

    value += point;
    heroSetStatus(hero, HERO_STATUS_POKECOUPON, value);

    if (point >= 0) {
        value = heroGetStatus(hero, HERO_STATUS_POKECOUPON_ALL, 0);
        value += point;
        heroSetStatus(hero, HERO_STATUS_POKECOUPON_ALL, value);
    }
}

void heroDecPokedoru(Hero* hero, u32 amount)
{
    heroSetStatus(hero, HERO_STATUS_POKEDORU, heroGetStatus(hero, HERO_STATUS_POKEDORU, 0) - amount);
}

void heroAddPokedoru(Hero* hero, u32 amount)
{
    u32 value = heroGetStatus(hero, HERO_STATUS_POKEDORU, 0);

    value += amount;
    heroSetStatus(hero, HERO_STATUS_POKEDORU, value);
}

ItemSlot* heroHizukiItemGetItemAryPtr(Hero* hero, u16* outSlotCount, u16* outStockMaximum,
                                     u8* outSortMode, u8* outInsertionMode)
{
    if (outSlotCount != 0) *outSlotCount = 10;
    if (outStockMaximum != 0) *outStockMaximum = 1;
    if (outSortMode != 0) *outSortMode = 0;
    if (outInsertionMode != 0) *outInsertionMode = 1;

    return (ItemSlot*)heroGetStatus(hero, HERO_STATUS_HIZUKI_ITEM, 0);
}

void heroCheckSetMonohiroiAllTemotiPokemon(Hero* hero)
{
    Pokemon* pokemon;
    u16 i;

    for (i = 0; i < HERO_PARTY_SIZE; i++) {
        pokemon = (Pokemon*)heroGetStatus(hero, HERO_STATUS_POKEMON, i);

        if (pokemonCheckValid(pokemon)) {
            pokemonCheckSetMonohiroi(pokemon);
        }
    }
}

s32 heroItemCheckAddItemDataId(Hero* hero, u16 itemID)
{
    u16 count = 0;
    u16 max = 0;
    u8 insertionMode;
    ItemSlot* items = heroItemGetItemDataIdToItemAryPtr(hero, itemID, &count, &max, 0, &insertionMode);

    if (items == 0) {
        return -1;
    }
    return itemCheckAddItemDataIdAry(items, count, itemID, max, insertionMode);
}

s32 heroItemDecItemDataId(Hero* hero, u16 itemID, u16 count, s16 itemPosition)
{
    u16 slotCount = 0;
    u16 max = 0;
    u8 sortMode;
    ItemSlot* items = heroItemGetItemDataIdToItemAryPtr(hero, itemID, &slotCount, &max, &sortMode, 0);

    if (items == 0) {
        return -1;
    }
    return itemDecItemDataIdAry(items, slotCount, itemID, count, itemPosition, max, sortMode);
}

s32 heroItemAddItemDataId(Hero* hero, u16 itemID, u16 count, s16 itemPosition)
{
    u16 slotCount = 0;
    u16 max = 0;
    u8 sortMode;
    u8 insertionMode;
    ItemSlot* items = heroItemGetItemDataIdToItemAryPtr(hero, itemID, &slotCount, &max,
                                                       &sortMode, &insertionMode);

    if (items == 0) {
        return -1;
    }
    return itemAddItemDataIdAry(items, slotCount, itemID, count, itemPosition, max,
                                sortMode, insertionMode);
}

u8 heroItemCheckHaveItemDataId(Hero* hero, u16 itemID)
{
    u16 slotCount = 0;
    u16 max = 0;
    ItemSlot* items = heroItemGetItemDataIdToItemAryPtr(hero, itemID, &slotCount, &max, 0, 0);

    if (items == 0) {
        return 0;
    }
    return itemSearchItemDataIdAry(items, slotCount, itemID, ITEM_SEARCH_MATCH, max) != 0;
}

ItemSlot* heroItemGetItemDataIdToItemAryPtr(Hero* hero, u16 itemID, u16* outSlotCount,
                                           u16* outStockMaximum, u8* outSortMode,
                                           u8* outInsertionMode)
{
    u8 kind = itemGetStatus(0, itemID, ITEM_STATUS_KIND, 0);

    return heroItemGetItemKindToItemAryPtr(hero, kind, outSlotCount, outStockMaximum,
                                           outSortMode, outInsertionMode);
}

ItemSlot* heroItemGetItemKindToItemAryPtr(Hero* hero, u8 kind, u16* outSlotCount,
                                         u16* outStockMaximum, u8* outSortMode,
                                         u8* outInsertionMode)
{
    ItemSlot* items = 0;
    u16 slotCount = 0;
    u16 stockMaximum = 0;
    u8 sortMode = 0;
    u8 insertionMode;

    switch (kind) {
    case ITEM_KIND_POKEMON_BALL:
        items = (ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_BALL, 0);
        slotCount = 16;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 0;
        insertionMode = 1;
        break;
    case ITEM_KIND_NORMAL:
        items = (ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_NORMAL, 0);
        slotCount = 30;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 0;
        insertionMode = 1;
        break;
    case ITEM_KIND_SEED:
        items = (ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_SEED, 0);
        slotCount = 46;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 1;
        insertionMode = 1;
        break;
    case ITEM_KIND_WAZA_MACHINE:
        items = (ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_SKILL, 0);
        slotCount = 64;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 1;
        insertionMode = 0;
        break;
    case ITEM_KIND_EXTRA:
        items = (ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_EXTRA, 0);
        slotCount = 43;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 0;
        insertionMode = 0;
        break;
    case ITEM_KIND_COLOGNE:
        items = (ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_KORON, 0);
        slotCount = 3;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 1;
        insertionMode = 0;
        break;
    case ITEM_KIND_BATTLE_DISK:
        if (hero == 0) {
            hero = _getHero();
        }
        items = hero->getDisk();
        slotCount = 60;
        stockMaximum = HERO_ITEM_STOCK_MAX;
        sortMode = 1;
        insertionMode = 0;
        break;
    }

    if (outSlotCount != 0) *outSlotCount = slotCount;
    if (outStockMaximum != 0) *outStockMaximum = stockMaximum;
    if (outSortMode != 0) *outSortMode = sortMode;
    if (outInsertionMode != 0) *outInsertionMode = insertionMode;

    return items;
}

bool heroIsMinePokemon(Hero* hero, Pokemon* pokemon)
{
    u32 heroTrainerID = heroGetStatus(hero, HERO_STATUS_TRAINER_RND, 0);
    u16* heroName = (u16*)heroGetStatus(hero, HERO_STATUS_NAME, 0);
    u32 pokemonTrainerID = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0);
    u16* pokemonName = (u16*)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_NAME, 0);

    if (heroTrainerID != pokemonTrainerID) {
        return false;
    }
    return GScharCmp(heroName, pokemonName) == 0;
}

s16 heroGetPokemon(Hero* hero, const Pokemon* pokemon, bool usePcbox)
{
    Pokemon temp;
    u32 slot;

    if (pokemon == 0) {
        return HERO_PARTY_SIZE;
    }
    pokemonBiosCopy(&temp, pokemon);
    slot = heroAddPokemon(hero, &temp);
    if (slot >= HERO_PARTY_SIZE) {
        if (!usePcbox) {
            return HERO_POKEMON_PARTY_FULL;
        }
        if (pcboxAddPokemon(0, &temp, -1) == 1) {
            return HERO_POKEMON_TO_BOX;
        }
        return HERO_POKEMON_PARTY_FULL;
    }
    return slot;
}

s16 heroCatchPokemon(Hero* hero, const Pokemon* pokemon, u16 catchFloorID, u8 catchBallID,
    bool usePcbox)
{
    Pokemon temp;
    u8 level;
    u8 sex;
    u32 trainerID;
    u16* name;
    u32 slot;

    if (pokemon == 0) {
        return HERO_PARTY_SIZE;
    }
    level = pokemonGetStatus((Pokemon*)pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0);
    sex = heroGetStatus(hero, HERO_STATUS_SEX, 0);
    trainerID = heroGetStatus(hero, HERO_STATUS_TRAINER_RND, 0);
    name = (u16*)heroGetStatus(hero, HERO_STATUS_NAME, 0);
    pokemonBiosCopy(&temp, pokemon);
    pokemonSetCatchStatus(&temp, catchFloorID, level, catchBallID, sex, trainerID, name);
    slot = heroAddPokemon(hero, &temp);
    if (slot >= HERO_PARTY_SIZE) {
        if (!usePcbox) {
            return HERO_POKEMON_PARTY_FULL;
        }
        if (pcboxAddPokemon(0, &temp, -1) == 1) {
            return HERO_POKEMON_TO_BOX;
        }
        return HERO_POKEMON_PARTY_FULL;
    }
    return slot;
}

u8 heroAddPokemon(Hero* hero, const Pokemon* pokemon)
{
    u8 slot;

    if (pokemon == 0) {
        return HERO_PARTY_SIZE;
    }
    for (slot = 0; slot < HERO_PARTY_SIZE; slot++) {
        if (pokemonCheckValid((Pokemon*)heroGetStatus(hero, HERO_STATUS_POKEMON, slot)) != 1) {
            if (hero == 0) {
                hero = _getHero();
            }
            hero->setPokemon(pokemon, slot);
            return slot;
        }
    }
    return HERO_PARTY_SIZE;
}

u8 heroCheckValid(Hero* hero)
{
    u16 emptyName = 0;

    if (GScharCmp((u16*)heroGetStatus(hero, HERO_STATUS_NAME, 0), &emptyName) == 0) {
        return 0;
    }
    return heroGetStatus(hero, HERO_STATUS_SEX, 0) != POKEMON_SEX_NONE;
}

void heroCreate(Hero* hero, u16* name, u8 sex)
{
    u32 rnd;

    heroInit(hero);
    rnd = heroCreateRnd();
    heroSetStatus(hero, HERO_STATUS_TRAINER_RND, rnd);
    heroSetStatus(hero, HERO_STATUS_NAME, (u32)name);
    heroSetStatus(hero, HERO_STATUS_SEX, sex);
    heroSetStatus(hero, HERO_STATUS_HIZUKI_NAME, (u32)GSmsgGetStringPtr(HERO_HIZUKI_NAME_MSG));
    if (hero == 0) {
        hero = Hero::getHeroPtr();
    }
}

u32 heroCreateRnd()
{
    u16 low = HSD_Rand();

    return low | (HSD_Rand() << 16);
}

void heroInit(Hero* hero)
{
    u16 emptyName = 0;

    if (hero == 0) {
        hero = Hero::getHeroPtr();
    }
    heroSetStatus(hero, HERO_STATUS_NAME, (u32)&emptyName);
    heroSetStatus(hero, HERO_STATUS_TRAINER_RND, 0);
    pokemonInitAry((Pokemon*)heroGetStatus(hero, HERO_STATUS_POKEMON, 0), HERO_PARTY_SIZE);
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_NORMAL, 0),
        sizeof(hero->mNormalItems) / sizeof(ItemSlot));
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_EXTRA, 0),
        sizeof(hero->mExtraItems) / sizeof(ItemSlot));
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_BALL, 0),
        sizeof(hero->mBallItems) / sizeof(ItemSlot));
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_SKILL, 0),
        sizeof(hero->mSkillItems) / sizeof(ItemSlot));
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_SEED, 0),
        sizeof(hero->mSeedItems) / sizeof(ItemSlot));
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_ITEM_KORON, 0),
        sizeof(hero->mKoronItems) / sizeof(ItemSlot));
    itemInitAry(hero->getDisk(), sizeof(hero->mDisk) / sizeof(ItemSlot));
    heroSetStatus(hero, HERO_STATUS_SEX, POKEMON_SEX_NONE);
    heroSetStatus(hero, HERO_STATUS_POKEDORU, 0);
    heroSetStatus(hero, HERO_STATUS_POKECOUPON, 0);
    heroSetStatus(hero, HERO_STATUS_POKECOUPON_ALL, 0);
    heroSetStatus(hero, HERO_STATUS_BADGE_01, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_02, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_03, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_04, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_05, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_06, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_07, 1);
    heroSetStatus(hero, HERO_STATUS_BADGE_08, 1);
    heroSetStatus(hero, HERO_STATUS_HIZUKI_NAME, (u32)&emptyName);
    heroSetStatus(hero, HERO_STATUS_HIZUKI_FLAG, 0);
    itemInitAry((ItemSlot*)heroGetStatus(hero, HERO_STATUS_HIZUKI_ITEM, 0),
        sizeof(hero->mHizukiItems) / sizeof(ItemSlot));
    heroSetStatus(hero, HERO_STATUS_HOME_PLACE, 0);
    hero->setHeroStyle(HERO_STYLE_NORMAL, false);
    hero->setFollowerID(HERO_FOLLOWER_NONE);
    if (_hModuleHero == 0) {
        FloorModuleInterface_clear(&_floorModuleHero);
        _floorModuleHero.initialize = _heroEnterFloor;
        _floorModuleHero.finalize = _heroExitFloor;
        _floorModuleHero.getModuleName = _heroGetUniqueModuleName;
        _floorModuleHero.makeStateData = _heroMakeStateData;
        _floorModuleHero.restoreStateData = _heroRestoreStateData;
        _floorModuleHero.getStateSize = _heroGetStateSize;
        _floorModuleHero.getUID = _heroGetUID;
        _hModuleHero = floorRegisterModule(HERO_MODULE_PRIORITY, 0, &_floorModuleHero);
    }
}

void heroSetStatus(Hero* hero, u16 statusID, u32 value)
{
    if (statusID == 0 || statusID >= HERO_STATUS_COUNT) {
        return;
    }
    if (hero == 0) {
        void* saveData = savedataGetStatus(0, SAVE_DATA_IMAGE);

        if (saveData == 0) {
            return;
        }
        hero = (Hero*)savedataGetStatus(saveData, SAVE_DATA_HERO);
        if (hero == 0) {
            return;
        }
    }
    switch (statusID) {
    case HERO_STATUS_NAME:
        heroBiosSetNamePtr(hero, (u16*)value);
        break;
    case HERO_STATUS_TRAINER_RND:
        heroBiosSetRnd(hero, value);
        break;
    case HERO_STATUS_SEX:
        heroBiosSetSexDataId(hero, value);
        break;
    case HERO_STATUS_POKEDORU:
        heroBiosSetPokedoru(hero, value);
        break;
    case HERO_STATUS_POKECOUPON:
        heroBiosSetPokecoupon(hero, value);
        break;
    case HERO_STATUS_POKECOUPON_ALL:
        heroBiosSetPokecouponAll(hero, value);
        break;
    case HERO_STATUS_BADGE_01:
        heroBiosSetBadge01Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_02:
        heroBiosSetBadge02Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_03:
        heroBiosSetBadge03Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_04:
        heroBiosSetBadge04Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_05:
        heroBiosSetBadge05Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_06:
        heroBiosSetBadge06Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_07:
        heroBiosSetBadge07Flag(hero, value != 0);
        break;
    case HERO_STATUS_BADGE_08:
        heroBiosSetBadge08Flag(hero, value != 0);
        break;
    case HERO_STATUS_HIZUKI_NAME:
        heroBiosSetHizukiNamePtr(hero, (u16*)value);
        break;
    case HERO_STATUS_HIZUKI_FLAG:
        heroBiosSetHizukiFlag(hero, value != 0);
        break;
    case HERO_STATUS_HOME_PLACE:
        heroBiosSetHomePlace(hero, value);
        break;
    }
}

u32 heroGetStatus(Hero* hero, u16 statusID, u16 slot)
{
    if (statusID == 0 || statusID >= HERO_STATUS_COUNT) {
        return 0;
    }
    if (hero == 0) {
        hero = _getHero();
    }
    switch (statusID) {
    case HERO_STATUS_NAME:
        return (u32)heroBiosGetNamePtr(hero);
    case HERO_STATUS_TRAINER_RND:
        return heroBiosGetRnd(hero);
    case HERO_STATUS_POKEMON:
        return (u32)heroBiosGetPokemonPtr(hero, slot);
    case HERO_STATUS_ITEM_NORMAL:
        return (u32)heroBiosGetItemNormalPtr(hero, slot);
    case HERO_STATUS_ITEM_EXTRA:
        return (u32)heroBiosGetExtraItemPtr(hero, slot);
    case HERO_STATUS_ITEM_BALL:
        return (u32)heroBiosGetItemBallPtr(hero, slot);
    case HERO_STATUS_ITEM_SKILL:
        return (u32)heroBiosGetItemSkillPtr(hero, slot);
    case HERO_STATUS_ITEM_SEED:
        return (u32)heroBiosGetItemSeedPtr(hero, slot);
    case HERO_STATUS_ITEM_KORON:
        return (u32)heroBiosGetItemKoronPtr(hero, slot);
    case HERO_STATUS_HIZUKI_ITEM:
        return (u32)heroBiosGetHizukiItemPtr(hero, slot);
    case HERO_STATUS_SEX:
        return heroBiosGetSexDataId(hero);
    case HERO_STATUS_POKEDORU:
        return heroBiosGetPokedoru(hero);
    case HERO_STATUS_POKECOUPON:
        return heroBiosGetPokecoupon(hero);
    case HERO_STATUS_POKECOUPON_ALL:
        return heroBiosGetPokecouponAll(hero);
    case HERO_STATUS_BADGE_01:
        return heroBiosGetBadge01Flag(hero);
    case HERO_STATUS_BADGE_02:
        return heroBiosGetBadge02Flag(hero);
    case HERO_STATUS_BADGE_03:
        return heroBiosGetBadge03Flag(hero);
    case HERO_STATUS_BADGE_04:
        return heroBiosGetBadge04Flag(hero);
    case HERO_STATUS_BADGE_05:
        return heroBiosGetBadge05Flag(hero);
    case HERO_STATUS_BADGE_06:
        return heroBiosGetBadge06Flag(hero);
    case HERO_STATUS_BADGE_07:
        return heroBiosGetBadge07Flag(hero);
    case HERO_STATUS_BADGE_08:
        return heroBiosGetBadge08Flag(hero);
    case HERO_STATUS_HIZUKI_NAME:
        return (u32)heroBiosGetHizukiNamePtr(hero);
    case HERO_STATUS_HIZUKI_FLAG:
        return heroBiosGetHizukiFlag(hero);
    case HERO_STATUS_HOME_PLACE:
        return heroBiosGetHomePlace(hero);
    }
    return 0;
}

s32 heroGetObjID(u8 heroType)
{
    Hero* hero = Hero::getHeroPtr();

    if (hero == 0) {
        return HERO_OBJ_ID_STYLE0;
    }
    return hero->getHeroObjID(heroType, -1);
}

} // extern "C"

Hero* _getHero()
{
    void* saveData = savedataGetStatus(0, SAVE_DATA_IMAGE);
    Hero* hero;

    if (saveData == 0) {
        return 0;
    }
    hero = (Hero*)savedataGetStatus(saveData, SAVE_DATA_HERO);
    if (hero == 0) {
        return 0;
    }
    return hero;
}

void _heroExitFloor(unsigned int, FloorEnterMode mode)
{
    if (mode == FLOOR_ENTER_PUSH) {
        return;
    }
    if (heroMoveIsSmoothFloorLink()) {
        return;
    }
    if (!peopleCheckFlag(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_HERO, PEOPLE_FLAG_DISABLE_MOTION_RESET)) {
        peopleSetMotion(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_HERO, HERO_MOTION_STAND, 0, true);
    }
    peopleSetMotion(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_MANA, HERO_MOTION_STAND, 0, true);
    peopleSetMotion(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_GIRL, HERO_MOTION_STAND, 0, true);
    peopleSetMotion(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_KLEIN, HERO_MOTION_STAND, 0, true);
}

void _heroEnterFloor(unsigned int, FloorEnterMode)
{
}

extern "C" {

void heroSetFollowerWithID(u8 followerID, u8 modelLevel)
{
    Hero* hero = Hero::getHeroPtr();

    hero->setFollowerID(followerID);
    hero->setFollowerModelLevel(HERO_MODEL_LEVEL_NORMAL);
    switch (followerID) {
    case HERO_FOLLOWER_NONE:
        heroMoveDismissMember(HERO_MOVE_COMPANION_1);
        heroMoveDismissMember(HERO_MOVE_COMPANION_2);
        heroMoveDismissMember(HERO_MOVE_COMPANION_3);
        break;
    case HERO_FOLLOWER_MANA:
        switch (modelLevel) {
        case HERO_MODEL_LEVEL_NORMAL:
            heroSetFollower(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_MANA, HERO_OBJ_ID_MANA);
            return;
        case HERO_MODEL_LEVEL_ALTERNATE:
            heroSetFollower(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_MANA, HERO_OBJ_ID_MANA_FOLLOWER);
            hero->setFollowerModelLevel(HERO_MODEL_LEVEL_ALTERNATE);
            return;
        }
        // fall through
    case HERO_FOLLOWER_GIRL:
        heroSetFollower(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_GIRL, HERO_OBJ_ID_GIRL_FOLLOWER);
        break;
    case HERO_FOLLOWER_KLEIN:
        heroSetFollower(PEOPLE_GROUP_GLOBAL, HERO_RESOURCE_KLEIN, HERO_OBJ_ID_KLEIN_FOLLOWER);
        break;
    }
}

void heroSetFollower(u32 groupID, u32 resourceID, u32 objectID)
{
    Hero* hero = Hero::getHeroPtr();
    tagPeopleWork* people;

    hero->setFollowerGrpID(groupID);
    hero->setFollowerResID(resourceID);
    hero->setFollowerObjID(objectID);
    peopleChangeFollower(objectID);
    people = peopleGet(groupID, resourceID);
    if (people == 0) {
        GSlogWrite("Illegal ID!\n");
        return;
    }
    peopleSetFollowerPtr(people);
    if (objectID == HERO_OBJ_ID_MANA_FOLLOWER) {
        hero->setFollowerModelLevel(HERO_MODEL_LEVEL_ALTERNATE);
    } else {
        hero->setFollowerModelLevel(HERO_MODEL_LEVEL_NORMAL);
    }
}

bool heroIsPlayerPeople(u32 groupID, u32 resourceID)
{
    Hero* hero;

    if (groupID == PEOPLE_GROUP_GLOBAL && resourceID == HERO_RESOURCE_HERO) {
        return true;
    }
    hero = Hero::getHeroPtr();
    if (groupID == hero->getFollowerGrpID() && resourceID == hero->getFollowerResID()) {
        return true;
    }
    return false;
}

} // extern "C"
