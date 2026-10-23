#ifndef MENU_MODEL_H
#define MENU_MODEL_H

#include <types.h>
#include <GSvec.h>

// Work block of one rendered menu model. Only the species of the loaded model
// and its transform are read back; the remaining layout stays sized padding.
// nameEntryModel (menuNameEntry.cpp) fixes this one at 0xEC bytes.
struct MENU_MODEL {
    MENU_MODEL() {}

    u8 _00[0x24];
    u16 mSpecies;
    u8 _26[0x1A];
    GSvec mAngle;
    GSvec mOffset;
    u8 _58[0x94];
};

// The status screen's model block (menuPokemonStatusModel) is four bytes
// longer than the name entry one; nothing in the project reads the tail, so it
// stays sized padding.
struct MENU_MODEL_STATUS : MENU_MODEL {
    u8 _EC[4];
};

#endif
