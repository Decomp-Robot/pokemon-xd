#include <hero.h>
#include <heroItem.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <itemData.h>

Hero* _getHero();

namespace {

const int HERO_PARTY_SIZE = 6;
const int HERO_ITEM_STOCK_MAX = 999;

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

} // extern "C"
