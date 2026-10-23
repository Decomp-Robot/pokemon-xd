#ifndef MENU_MODEL_H
#define MENU_MODEL_H

#include <types.h>

// Work block of one rendered menu model. Only the species of the loaded model
// is read back; the rest of the layout is unrecovered and stays sized padding.
struct MENU_MODEL {
    u8 _00[0x24];
    u16 mSpecies; // 0x24: species of the loaded model.
    u8 _26[0xC6];
};

#endif
