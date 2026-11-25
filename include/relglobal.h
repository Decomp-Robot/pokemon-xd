#ifndef INCLUDE_RELGLOBAL_H
#define INCLUDE_RELGLOBAL_H

#include <types.h>
#include <PokemonData.h>
#include <ribbon.h>
#include <roomData.h>

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

struct FloorTreasure;

struct FloorListHeader {
    u32 floorCount;
};
typedef struct FloorListHeader FloorListHeader;

extern GSfloor_dd_* floordata;
extern FloorListHeader* floordata_number;
extern struct FloorTreasure* floor_tresure_list;

struct MenuSeData;

/* Count of records in the menu sound-effect table. */
extern u32 *menu_se_number;

/* Records of the menu sound-effect table. */
extern struct MenuSeData *menu_se;

extern u32 *mball_data_number;
extern u16 *mball_data;

/* Count of entries in the item data index table. */
extern u32 *item_data_index_number;

/* Count of entries in the pokemon data table. */
extern u32 *pokemon_data_number;

/* Pokemon species record table. */
extern PokemonData *pokemon_data;

extern u32 *legendPokemonData_number;
extern u16 *legendPokemonData;

struct WazaData;

/* Count of loaded move records. */
extern u32 *waza_data_number;

/* Loaded move records. */
extern struct WazaData *waza_data;

/* Count of tutorial records. */
extern u32 *tutorialdata_number;

/* Tutorial records. */
extern u16 *tutorialdata;

/* Count of records in the move tutor table. */
extern s32 *wazaoshie_data_number;

/* Count of entries in the pokemon nature (seikaku) table. */
extern u32 *pokemon_seikaku_data_number;

typedef struct PokemonMenuAnimationEntry {
    u8 animationIndex;
    u16 species;
} PokemonMenuAnimationEntry;

extern u32 *pokemon_menu_animation_number;
extern PokemonMenuAnimationEntry *pokemon_menu_animation;

struct SHOP_DATA {
    u8 mMessageSet;
    u8 mShopType;
    u16 mGoodsIndex;
};
typedef struct SHOP_DATA SHOP_DATA;

struct SHOP_SALESMAN {
    u8 mSoundEffect;
    u32 mMessages[14];
};
typedef struct SHOP_SALESMAN SHOP_SALESMAN;

struct SHOP_EXCHANGER {
    u8 mSoundEffect;
    u32 mMessages[18];
};
typedef struct SHOP_EXCHANGER SHOP_EXCHANGER;

extern SHOP_SALESMAN *shopSalesman;
extern u16 *shopGoods;
extern SHOP_EXCHANGER *shopExchanger;
extern SHOP_DATA *shopData;

extern u32 *ribbon_data_number;
extern RibbonData *ribbon_data;

struct DeckDataVirtual;
extern u32 *deckdata_virtual_number;
extern struct DeckDataVirtual *deckdata_virtual;

#endif
