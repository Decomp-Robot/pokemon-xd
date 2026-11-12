#ifndef INCLUDE_RELGLOBAL_H
#define INCLUDE_RELGLOBAL_H

#include <types.h>

struct RelivehallData {
    u8 mHologramAlpha;
    u8 unknown;
    u16 mSteps;
    u16 mModelScale;
    u8 mPartIndex[68];
};
typedef struct RelivehallData RelivehallData;
extern RelivehallData *relivehall_data;

struct DbgMenuItem;

extern u32 (*debugMenuDataGetNum)(void);
extern struct DbgMenuItem *(*debugMenuDataGetPtr)(long itemNo);

struct GSfloor_dd_ {
    u8 unknown;
    u8 kind;
    u16 floorID;
    u8 unknownData[0x3c];
};

// Usable from C as well as C++.
typedef struct GSfloor_dd_ GSfloor_dd_;

struct FloorListHeader {
    u32 floorCount;
};
typedef struct FloorListHeader FloorListHeader;

extern GSfloor_dd_* floordata;
extern FloorListHeader* floordata_number;

/* Count of entries in the item data index table. */
extern u32 *item_data_index_number;

/* Count of entries in the pokemon data table. */
extern u32 *pokemon_data_number;

/* Count of entries in the pokemon nature (seikaku) table. */
extern u32 *pokemon_seikaku_data_number;

#endif
