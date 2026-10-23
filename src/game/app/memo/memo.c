#include <memo.h>

#include <hsdRandom.h>
#include <pokemonBios.h>
#include <Pokemon.h>
#include <savedata.h>

/*
 * The memo of the Pokemon the player caught: one list inside the game data
 * (memo block of the save) that holds, for a limited number of species, the random
 * number of the trainer the Pokemon was caught for and the random number it is
 * shown with.
 *
 * The menus that show one of those Pokemon -- the Pokespot list, the evolution
 * demo and the party status screen -- fill their record from the memo entry of
 * the species, and the debug menu fills the whole list with test entries.
 */

// Random number of a memo entry: the whole 32 bit range but its top value, which
// marks a Pokemon without one.
#define MEMO_RND_LIMIT 0xFFFFFFFFu

// Entries memoInit empties, which is as many Pokemon as the list can hold.
#define MEMO_ENTRY_LIMIT 500

// Blocks of species the debug menu fills the list with: the first block is the
// species of the Pokespot Pokemon, the second the ones above them.
#define MEMO_DEBUG_ID_FIRST 1
#define MEMO_DEBUG_ID_LAST 0xFB
#define MEMO_DEBUG_ID_FIRST_HIGH 0x115
#define MEMO_DEBUG_ID_LAST_HIGH 0x19B

// One Pokemon of the memo. The id holds the species of the Pokemon in its low
// bits; the high flag is consumed by memoDataSetMemoFlag.
#define MEMO_ID_MASK 0x3FFFu
#define MEMO_ID_FLAG 0x8000u

typedef struct MemoEntry {
    /* 0x00 */ u16 id;          // species of the Pokemon, with the flags on top
    /* 0x02 */ u16 pad;
    /* 0x04 */ u32 trainerRnd; // random number of the trainer the Pokemon was caught for
    /* 0x08 */ u32 pokemonRnd; // random number the Pokemon was shown with
} MemoEntry;

typedef struct MemoData {
    /* 0x00 */ u16 count;   // number of Pokemon the list holds
    /* 0x02 */ u16 pad;
    /* 0x04 */ MemoEntry entries[]; // Pokemon of the list
} MemoData;


// Empty the memo list and give every entry a fresh random number.
void memoInit(void *data) {
    MemoData *memo = (MemoData *)data;
    MemoEntry *entry;
    s32 i;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }

    memo->count = 0;
    for (i = 0; i < MEMO_ENTRY_LIMIT; i++) {
        entry = (MemoEntry *)((char *)memo->entries + i * sizeof(MemoEntry));
        entry->id = 0;
        entry->pokemonRnd = ((HSD_Rand() << 16) | HSD_Rand()) % MEMO_RND_LIMIT;
    }
}

// Add a Pokemon to the list; one of its species that is in the list already is
// left alone.
void memoDataSet(void *data, Pokemon *pokemon) {
    MemoData *memo = (MemoData *)data;
    u16 entry;
    u16 species;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }
    species = pokemonBiosGetPokemonDataId(pokemon);

    for (entry = 0; entry < memo->count; entry++) {
        u16 entrySpecies = memo->entries[entry].id & MEMO_ID_MASK;

        if (entrySpecies == species) {
            return;
        }
    }

    memo->entries[memo->count].id = (u32)species | MEMO_ID_FLAG;
    memo->entries[memo->count].pokemonRnd = pokemonBiosGetRnd(pokemon);
    memo->entries[memo->count].trainerRnd = pokemonBiosGetCatchTrainerRnd(pokemon);
    memo->count++;
}

// Add the Pokemon of a species to the list with a fresh random number; a species
// that is in the list already is left alone.
void memoDataSetPokemonID(void *data, u16 id) {
    MemoData *memo = (MemoData *)data;
    u16 entry;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }

    for (entry = 0; entry < memo->count; entry++) {
        u16 entrySpecies = memo->entries[entry].id & MEMO_ID_MASK;

        if (entrySpecies == id) {
            return;
        }
    }

    memo->entries[memo->count].id = (u32)id | MEMO_ID_FLAG;
    memo->entries[memo->count].pokemonRnd = ((HSD_Rand() << 16) | HSD_Rand()) % MEMO_RND_LIMIT;
    memo->count++;
}

// Clear the memo flag from each entry and report whether any entry had it set.
s32 memoDataSetMemoFlag(void *data) {
    MemoData *memo = (MemoData *)data;
    s32 any = 0;
    u16 entry;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }

    for (entry = 0; entry < memo->count; entry++) {
        if ((memo->entries[entry].id & MEMO_ID_FLAG) != 0) {
            any = 1;
        }
        memo->entries[entry].id &= MEMO_ID_MASK;
    }
    return any;
}

// Number of Pokemon the memo list holds.
u16 memoDataGetCount(void *data) {
    if (data == 0) {
        data = savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }
    return ((MemoData *)data)->count;
}

// Species of the Pokemon at an index into the list (0 while the list is empty).
u16 memoDataGetPokemonID(void *data, u16 index) {
    MemoData *memo = (MemoData *)data;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }

    if (memo->count != 0) {
        return memo->entries[index].id;
    }
    return 0;
}

// Random number the Pokemon of a memo entry is shown with (0 when the list holds
// no such Pokemon).
u32 memoDataGetPokemonRndFromID(void *data, u16 id) {
    MemoData *memo = (MemoData *)data;
    u16 entry;
    u16 count;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }
    count = memoDataGetCount(memo);

    for (entry = 0; entry < count; entry++) {
        if ((s32)(memo->entries[entry].id & MEMO_ID_MASK) == id) {
            return memo->entries[entry].pokemonRnd;
        }
    }
    return 0;
}

// Catch trainer random number of the memo entry of a species (0 when the list
// holds no such Pokemon).
u32 memoDataGetPokemonTrainerRndFromID(void *data, u16 id) {
    MemoData *memo = (MemoData *)data;
    u16 entry;
    u16 count;

    if (data == 0) {
        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }
    count = memoDataGetCount(memo);

    for (entry = 0; entry < count; entry++) {
        if ((s32)(memo->entries[entry].id & MEMO_ID_MASK) == id) {
            return memo->entries[entry].trainerRnd;
        }
    }
    return 0;
}

// Fill the list of the memo with one test Pokemon per species of the two debug
// blocks; the debug menu hands the Pokemon to the Pokespot list with it. A
// species the list holds already is left alone and the next one is taken.
void memoInitDebug(void *data) {
    MemoData *memo;
    u16 i;

    if (data == 0) {
        data = savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
    }

    for (i = MEMO_DEBUG_ID_FIRST; i <= MEMO_DEBUG_ID_LAST; i++) {
        u16 entry;
        u16 count;

        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
        count = memo->count;
        for (entry = 0; entry < count; entry++) {
            u16 entrySpecies = memo->entries[entry].id & MEMO_ID_MASK;

            if (entrySpecies == i) {
                goto nextLow;
            }
        }
        memo->entries[count].id = (u32)i | MEMO_ID_FLAG;
        memo->entries[memo->count].pokemonRnd = ((HSD_Rand() << 16) | HSD_Rand()) % MEMO_RND_LIMIT;
        memo->count++;
    nextLow:;
    }

    for (i = MEMO_DEBUG_ID_FIRST_HIGH; i <= MEMO_DEBUG_ID_LAST_HIGH; i++) {
        u16 entry;
        u16 count;

        memo = (MemoData *)savedataGetStatus(0, SAVEDATA_BLOCK_MEMO);
        count = memo->count;
        for (entry = 0; entry < count; entry++) {
            u16 entrySpecies = memo->entries[entry].id & MEMO_ID_MASK;

            if (entrySpecies == i) {
                goto nextHigh;
            }
        }
        memo->entries[count].id = (u32)i | MEMO_ID_FLAG;
        memo->entries[memo->count].pokemonRnd = ((HSD_Rand() << 16) | HSD_Rand()) % MEMO_RND_LIMIT;
        memo->count++;
    nextHigh:;
    }
}

// The scale a Pokemon is shown with in its memo, by species, and the angle it is
// turned to. The angle is the same for every Pokemon, and the scale of a species
// that is not listed stays at one.
f32 memoGetScaleAngle(u16 id, f32 *scale, f32 *angle) {
    f32 value = 1.0f;

    switch (id) {
    case 0x15C:
        value = 1.2f;
        break;
    case 0x26:
        value = 0.6f;
        break;
    case 0x4B:
        value = 0.8f;
        break;
    case 0x91:
        value = 0.75f;
        break;
    case 0x12C:
        value = 0.7f;
        break;
    case 0x8E:
        value = 0.85f;
        break;
    case 0x197:
        value = 0.65f;
        break;
    case 0x198:
        value = 0.65f;
        break;
    case 0x1A:
        value = 0.7f;
        break;
    case 0x06:
        value = 0.7f;
        break;
    case 0xE2:
        value = 0.75f;
        break;
    case 0xD9:
        value = 0.75f;
        break;
    case 0x14B:
        value = 0.78f;
        break;
    case 0x44:
        value = 0.78f;
        break;
    case 0x136:
        value = 0.75f;
        break;
    case 0x4A:
        value = 0.8f;
        break;
    case 0x16B:
        value = 0.8f;
        break;
    case 0x85:
        value = 0.8f;
        break;
    case 0x92:
        value = 0.6f;
        break;
    case 0x90:
        value = 0.6f;
        break;
    case 0x65:
        value = 1.2f;
        break;
    case 0xFA:
        value = 0.65f;
        break;
    case 0x194:
        value = 0.7f;
        break;
    case 0xF9:
        value = 0.85f;
        break;
    case 0x135:
        value = 0.75f;
        break;
    case 0x156:
        value = 1.25f;
        break;
    case 0x14C:
        value = 1.2f;
        break;
    case 0x6C:
        value = 1.3f;
        break;
    case 0x5C:
        value = 0.8f;
        break;
    case 0x57:
        value = 1.2f;
        break;
    case 0x13A:
        value = 1.18f;
        break;
    case 0x82:
        value = 1.1f;
        break;
    default:
        break;
    }

    if (angle != 0) {
        *angle = 0.2617994f;
    }
    if (scale != 0) {
        *scale = value;
    }
    return value;
}
