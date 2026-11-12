#include <pcbox.h>
#include <darkPokemon.h>
#include <GSmsg.h>
#include <itemData.h>
#include <msgctrl.h>
#include <menuPcBoxPokemon.h>
#include <menuPocketTool.h>
#include <savedata.h>
#include <string.h>

enum {
    MSG_PCBOX_DEFAULT_BOX_NAME = 15161,
    PCBOX_BOX_NAME_MAX_LENGTH = 8,
    PCBOX_ITEM_SLOT_COUNT = 235,
    PCBOX_ITEM_NUM_MAX = 999
};

inline void PCBOX::initBoxes()
{
    int box;
    PCStorageBox* pokemonBox;
    for (box = 0, pokemonBox = mStorage.boxes; box < PokemonBoxCount; box++, pokemonBox++) {
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, box + 1);
        GSmsgMakeGScharStr(pokemonBox->name, BoxNameMaxLength + 1, MSG_PCBOX_DEFAULT_BOX_NAME);
        pokemonInitAry(pokemonBox->pokemon, PokemonPerBox);
    }
    itemInitAry(mStorage.items, ItemSlotCount);
    mStorage.reserved = 0;
}

static inline PCStorageRecord* pcboxGetStorage(PCStorageRecord* storage)
{
    if (storage == NULL) {
        storage = (PCStorageRecord*)savedataGetStatus(NULL, SAVE_DATA_PCBOX);
    }
    return storage;
}

static inline Pokemon* pcboxGetBoxPokemon(PCStorageRecord* storage, s8 box, s8 slot)
{
    Pokemon* pokemon;
    if (box < 0 || box >= PCBOX::PokemonBoxCount) {
        pokemon = NULL;
    } else if (slot < 0 || slot >= PCBOX::PokemonPerBox) {
        pokemon = NULL;
    } else {
        pokemon = &storage->boxes[box].pokemon[slot];
    }
    return pokemon;
}

static inline s8 pcboxGetPokemonBoxEmptySlot(PCStorageRecord* storage, s8 box)
{
    int slot;
    for (slot = 0; slot < PCBOX::PokemonPerBox; slot++) {
        Pokemon* pokemon = pcboxGetBoxPokemon(storage, box, slot);
        if (pokemon != NULL && !pokemonCheckValid(pokemon)) {
            break;
        }
    }
    if (slot >= PCBOX::PokemonPerBox) {
        return -1;
    }
    return slot;
}

static inline Pokemon* pcboxGetPokemon(PCStorageRecord* storage, s8 box, s8 slot)
{
    storage = pcboxGetStorage(storage);
    return pcboxGetBoxPokemon(storage, box, slot);
}

static inline s32 pcboxSetPokemon(PCStorageRecord* storage, Pokemon* pokemon, s8 box, s8 slot)
{
    Pokemon* dest = pcboxGetPokemon(storage, box, slot);
    if (dest == NULL) {
        return false;
    }
    memcpy(dest, pokemon, sizeof(Pokemon));
    pokemonAllKaihuku(dest);
    DarkPokemon* dark = dest->getDarkPokemon();
    if (dark != NULL) {
        dark->setPlace(DARK_POKEMON_WHERE_PCBOX, box);
    }
    return true;
}

static inline s16 pcboxSearchItem(PCStorageRecord* storage, u16 itemID)
{
    int index;
    for (index = 0; index < PCBOX_ITEM_SLOT_COUNT; index++) {
        if (itemCheckValid(&storage->items[index]) &&
            itemGetStatus(&storage->items[index], 0, ITEM_STATUS_ITEM_DATA_ID, 0) == itemID) {
            break;
        }
    }
    if (index >= PCBOX_ITEM_SLOT_COUNT) {
        return -1;
    }
    return index;
}

static inline ItemSlot* pcboxGetItemSlot(PCStorageRecord* storage, s16 index)
{
    ItemSlot* item;
    if (index < 0 || index >= PCBOX_ITEM_SLOT_COUNT) {
        item = NULL;
    } else {
        item = &storage->items[index];
    }
    return item;
}

static inline u16 pcboxGetItemNum(PCStorageRecord* storage, s16 index)
{
    ItemSlot* item = pcboxGetItemSlot(storage, index);
    if (item == NULL) {
        return 0xFFFF;
    }
    if (!itemCheckValid(item)) {
        return 0xFFFF;
    }
    return itemGetStatus(item, 0, ITEM_STATUS_NUM, 0);
}

static inline s8 pcboxGetPokemonBoxNbUsedSlot(PCStorageRecord* storage, s8 box)
{
    s8 count = 0;
    storage = pcboxGetStorage(storage);
    if (box < 0 || box >= PCBOX::PokemonBoxCount) {
        count = -1;
    } else {
        for (s8 slot = 0; slot < PCBOX::PokemonPerBox; slot++) {
            Pokemon* pokemon = pcboxGetBoxPokemon(storage, box, slot);
            if (pokemon != NULL && pokemonCheckValid(pokemon)) {
                count++;
            }
        }
    }
    return count;
}

extern "C" s32 pcboxGetStatus(void)
{
    return 0;
}

extern "C" void pcboxSetStatus(void)
{
}

static inline void pcboxInitialize(PCStorageRecord* storage)
{
    storage = pcboxGetStorage(storage);
    int box;
    PCStorageBox* pokemonBox;
    for (box = 0, pokemonBox = storage->boxes; box < PCBOX::PokemonBoxCount; box++, pokemonBox++) {
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, box + 1);
        GSmsgMakeGScharStr(pokemonBox->name, PCBOX_BOX_NAME_MAX_LENGTH + 1, MSG_PCBOX_DEFAULT_BOX_NAME);
        pokemonInitAry(pokemonBox->pokemon, PCBOX::PokemonPerBox);
    }
    itemInitAry(storage->items, PCBOX_ITEM_SLOT_COUNT);
    storage->reserved = 0;
}

extern "C" void pcboxInit(PCStorageRecord* storage)
{
    storage = pcboxGetStorage(storage);
    int box;
    PCStorageBox* pokemonBox;
    for (box = 0, pokemonBox = storage->boxes; box < PCBOX::PokemonBoxCount; box++, pokemonBox++) {
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, box + 1);
        GSmsgMakeGScharStr(pokemonBox->name, PCBOX_BOX_NAME_MAX_LENGTH + 1, MSG_PCBOX_DEFAULT_BOX_NAME);
        pokemonInitAry(pokemonBox->pokemon, PCBOX::PokemonPerBox);
    }
    itemInitAry(storage->items, PCBOX_ITEM_SLOT_COUNT);
    storage->reserved = 0;
}

extern "C" s32 pcboxAddPokemon(PCStorageRecord* storage, Pokemon* pokemon, s8 box)
{
    s8 slot;
    storage = pcboxGetStorage(storage);
    if (box < -1 || box >= PCBOX::PokemonBoxCount) {
        return false;
    }
    if (pokemon == NULL) {
        return false;
    }
    if (box == -1) {
        bool found = false;
        for (box = menuPcBoxPokemonGetCurrentBox(); box < PCBOX::PokemonBoxCount; box++) {
            slot = pcboxGetPokemonBoxEmptySlot(storage, box);
            if (slot >= 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (box = 0; box < menuPcBoxPokemonGetCurrentBox(); box++) {
                slot = pcboxGetPokemonBoxEmptySlot(storage, box);
                if (slot >= 0) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            return false;
        }
    } else {
        slot = pcboxGetPokemonBoxEmptySlot(storage, box);
        if (slot < 0) {
            return false;
        }
    }
    return pcboxSetPokemon(storage, pokemon, box, slot);
}

extern "C" u16* pcboxGetPokemonBoxName(PCStorageRecord* storage, s8 box)
{
    storage = pcboxGetStorage(storage);
    if (box < 0 || box >= PCBOX::PokemonBoxCount) {
        return NULL;
    }
    return storage->boxes[box].name;
}

extern "C" s32 pcboxSetPokemonBoxName(PCStorageRecord* storage, s8 box, const u16* name)
{
    storage = pcboxGetStorage(storage);
    if (box < 0 || box >= PCBOX::PokemonBoxCount) {
        return 0;
    }
    if (name == NULL) {
        return 0;
    }
    int length = 0;
    const u16* p = name;
    while (*p != 0) {
        p++;
        length++;
    }
    if (length > PCBOX_BOX_NAME_MAX_LENGTH) {
        return 0;
    }
    GScharCpy(storage->boxes[box].name, name);
    return 1;
}

static inline s8 pcboxCountEmptySlots(PCStorageRecord* storage, s8 box)
{
    s8 used = pcboxGetPokemonBoxNbUsedSlot(storage, box);
    if (used < 0) {
        return -1;
    }
    return PCBOX::PokemonPerBox - used;
}

extern "C" s8 pcboxGetPokemonBoxNbEmptySlot(PCStorageRecord* storage, s8 box)
{
    s8 used = pcboxGetPokemonBoxNbUsedSlot(storage, box);
    if (used < 0) {
        return -1;
    }
    return PCBOX::PokemonPerBox - used;
}

extern "C" s32 pcboxGetNbPokemonBox(void)
{
    return PCBOX::PokemonBoxCount;
}

extern "C" s32 pcboxGetNbItemSlot(PCStorageRecord*)
{
    return PCBOX_ITEM_SLOT_COUNT;
}

static inline ItemSlot* pcboxGetStorageItem(PCStorageRecord* storage, s16 index)
{
    storage = pcboxGetStorage(storage);
    return pcboxGetItemSlot(storage, index);
}

extern "C" ItemSlot* pcboxGetItem(PCStorageRecord* storage, s16 index)
{
    return pcboxGetStorageItem(storage, index);
}

extern "C" u16 pcboxAddItem(PCStorageRecord* storage, u16 itemID, u16 num)
{
    storage = pcboxGetStorage(storage);
    if (num == 0) {
        return num;
    }
    if (itemDataBiosGetPtr(itemID) == NULL) {
        return num;
    }
    return itemAddItemDataIdAry(storage->items, PCBOX_ITEM_SLOT_COUNT, itemID, num,
                                pcboxSearchItem(storage, itemID), PCBOX_ITEM_NUM_MAX, 0, 0);
}

extern "C" u16 pcboxDelItem(PCStorageRecord* storage, u16 itemID, u16 num)
{
    storage = pcboxGetStorage(storage);
    if (num == 0) {
        return num;
    }
    if (itemDataBiosGetPtr(itemID) == NULL) {
        return num;
    }
    s16 index = pcboxSearchItem(storage, itemID);
    if (index < 0) {
        return num;
    }
    return itemDecItemDataIdAry(storage->items, PCBOX_ITEM_SLOT_COUNT, itemID, num, index,
                                PCBOX_ITEM_NUM_MAX, 0);
}

extern "C" u16 pcboxGetItemCapacity(PCStorageRecord* storage, u16 itemID)
{
    storage = pcboxGetStorage(storage);
    if (itemDataBiosGetPtr(itemID) == NULL) {
        return 0;
    }
    s16 index = pcboxSearchItem(storage, itemID);
    u16 num;
    if (index >= 0) {
        num = pcboxGetItemNum(storage, index);
        if (num > PCBOX_ITEM_NUM_MAX) {
            num = 0;
        }
    } else {
        num = 0;
    }
    return PCBOX_ITEM_NUM_MAX - num;
}

extern "C" bool pcboxSwapItemSlot(PCStorageRecord* storage, s16 index1, s16 index2)
{
    ItemSlot* item1 = pcboxGetStorageItem(storage, index1);
    if (item1 == NULL) {
        return false;
    }
    ItemSlot* item2 = pcboxGetStorageItem(storage, index2);
    if (item2 == NULL) {
        return false;
    }
    itemReplace(item1, item2);
    return true;
}

void PCBOX::init()
{
    PCBOX* pcbox = this;
    if (pcbox == NULL) {
        pcbox = (PCBOX*)pcboxGetStorage(NULL);
    }
    pcbox->initBoxes();
}

Pokemon* PCBOX::getPokemon(s8 box, s8 slot)
{
    return pcboxGetPokemon(&mStorage, box, slot);
}

bool PCBOX::setPokemon(Pokemon* pokemon, s8 box, s8 slot)
{
    return pcboxSetPokemon(&mStorage, pokemon, box, slot);
}

bool PCBOX::addPokemon(Pokemon* pokemon, s8 box)
{
    return pcboxAddPokemon(&mStorage, pokemon, box);
}

bool PCBOX::addPokemon(Pokemon* pokemon)
{
    return addPokemon(pokemon, -1);
}

bool PCBOX::delPokemon(s8 box, s8 slot)
{
    int result;
    Pokemon* pokemon = pcboxGetPokemon(&mStorage, box, slot);
    if (pokemon == NULL) {
        result = false;
    } else if (!pokemonCheckValid(pokemon)) {
        result = false;
    } else {
        pokemonInit(pokemon);
        result = true;
    }
    return result;
}

u16* PCBOX::getPokemonBoxName(s8 box)
{
    PCBOX* pcbox = this;
    if (pcbox == NULL) {
        pcbox = (PCBOX*)pcboxGetStorage(NULL);
    }
    if (box < 0 || box >= PokemonBoxCount) {
        return NULL;
    }
    return pcbox->mStorage.boxes[box].name;
}

s8 PCBOX::getPokemonBoxNbUsedSlot(s8 box)
{
    return pcboxGetPokemonBoxNbUsedSlot(&mStorage, box);
}

s8 PCBOX::getPokemonBoxNbEmptySlot(s8 box)
{
    s8 used = pcboxGetPokemonBoxNbUsedSlot(&mStorage, box);
    s8 empty;
    if (used < 0) {
        empty = -1;
    } else {
        empty = PokemonPerBox - used;
    }
    return empty;
}

bool PCBOX::isPokemonFull()
{
    for (int box = 0; box < PokemonBoxCount; box++) {
        if (pcboxCountEmptySlots(pcboxGetStorage(NULL), box) != 0) {
            return false;
        }
    }
    return true;
}

inline void PCBOX::debugPokemonSetup()
{
    int serial = 0;
    for (int box = 0; box < PCBOX::PokemonBoxCount; box++) {
        if (box == 7) {
            for (int slot = 0; slot < PCBOX::PokemonPerBox; slot++) {
                Pokemon pokemon;
                menuPocketDebugMakePokemonBySN(pokemon, serial++, false, 0.0f);
                pcboxSetPokemon(pcboxGetStorage(NULL), &pokemon, box, slot);
            }
        } else {
            for (int slot = 0; slot < 3; slot++) {
                Pokemon pokemon;
                menuPocketDebugMakePokemonBySN(pokemon, serial++, slot % 10 == 0, 0.1f);
                pcboxSetPokemon(pcboxGetStorage(NULL), &pokemon, box, slot);
            }
        }
    }
}

extern "C" void pcboxDebugPokemonSetup(void)
{
    pcboxInitialize(NULL);
    PCBOX* pcbox = (PCBOX*)pcboxGetStorage(NULL);
    pcbox->debugPokemonSetup();
}

inline void PCBOX::debugPokemonSetupFull()
{
    int serial = 0;
    for (int box = 0; box < PCBOX::PokemonBoxCount; box++) {
        for (int slot = 0; slot < PCBOX::PokemonPerBox; slot++) {
            Pokemon pokemon;
            menuPocketDebugMakePokemonBySN(pokemon, serial++, slot == 1, 0.5f);
            pcboxSetPokemon(pcboxGetStorage(NULL), &pokemon, box, slot);
        }
    }
}

extern "C" void pcboxDebugPokemonSetupFull(void)
{
    pcboxInitialize(NULL);
    PCBOX* pcbox = (PCBOX*)pcboxGetStorage(NULL);
    pcbox->debugPokemonSetupFull();
}

PCBOX* PCBOX::getPointer()
{
    return (PCBOX*)savedataGetStatus(NULL, SAVE_DATA_PCBOX);
}
