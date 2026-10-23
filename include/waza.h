#ifndef WAZA_H
#define WAZA_H

#include <types.h>

/* Move-table record; fields are accessed through the move-data API. */
typedef struct WazaData WazaData;

#ifdef __cplusplus
extern "C" {
#endif
long wazaGetStatus(long battleMoveData, unsigned long wazaID, unsigned short fieldID, long index);
u8 wazaDB_GetCompatible(u32 wazaID);
WazaData *wazaDataBiosGetPtr(u32 wazaID);
u32 wazaDataBiosGetName(WazaData *data);
#ifdef __cplusplus
}
#endif

enum { WAZA_STATUS_TYPE = 0x1A };

enum { WAZA_ID_MASK = 0xFFFF };

// Waza (move) data fields read with wazaGetStatus. The status menu asks for
// the move by waza number: its name message id, its base PP, its type id,
// its accuracy and power shown as numbers, and its description message id.
enum WAZA_FIELD {
    WAZA_FIELD_NAME = 1,        // name message id, via GSmsgGetStringPtr.
    WAZA_FIELD_PP = 2,          // base PP of the move.
    WAZA_FIELD_TYPE = 3,        // type id, drawn as the type icon.
    WAZA_FIELD_ACCURACY = 6,    // accuracy, shown as a number.
    WAZA_FIELD_POWER = 7,       // power, shown as a number.
    WAZA_FIELD_HIDEN_FLAG = 0x19, // hiden (HM) flag: non-zero when the move cannot be forgotten.
    WAZA_FIELD_DESCRIPTION = 0x22, // description message id of the move.
};

// Shadow moves of a dark pokemon. Moves in [WAZA_SHADOW_FIRST,
// WAZA_SHADOW_SPECIAL) stay hidden until purification; WAZA_SHADOW_SPECIAL
// is the shadow move kept on display with its own icon and PP text.
enum WAZA_SHADOW {
    WAZA_SHADOW_FIRST = 0x164,   // first hidden shadow move.
    WAZA_SHADOW_LAST = 0x175,    // last hidden shadow move.
    WAZA_SHADOW_SPECIAL = 0x176, // shadow move with special display.
    WAZA_SHADOW_SPECIAL_ICON = 0x7B, // type icon drawn for the special shadow move.
};

enum {
    WAZA_STATUS_NAME_MESSAGE_ID = WAZA_FIELD_NAME,
    WAZA_STATUS_TYPE_ID = WAZA_FIELD_TYPE
};

enum WAZA_COMPATIBILITY {
    WAZA_COMPATIBILITY_UNDISPLAYED = 0,
    WAZA_COMPATIBILITY_TYPE_CHART = 1,
    WAZA_COMPATIBILITY_UNDISPLAYED_ALTERNATE = 2,
    WAZA_COMPATIBILITY_UNDISPLAYED_OTHER = 3,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GHOST = 4,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GRASS = 5,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_POISON_OR_STEEL = 6,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GROUND = 7,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FLYING = 8,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_NORMAL = 9,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY = 10,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_ALTERNATE = 11,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_DARK = 12,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FIRE = 13,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_OTHER = 14,
    WAZA_COMPATIBILITY_ALL_TYPES_NORMAL = 15,
};

#endif
