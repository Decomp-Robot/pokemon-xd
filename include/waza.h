#ifndef WAZA_H
#define WAZA_H

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

#endif
