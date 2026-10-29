#ifndef APP_MENU_MENUBINGO_H
#define APP_MENU_MENUBINGO_H

#include <types.h>
#include <app/bingo/bingoBios.h>

class cBingo : public bingo_bios {
public:
    u8 unk_000[0x122];
    u8 mMode;
    u8 unk_123[0x6];
    u8 mMball;
    u8 unk_12A[0x22];
};

extern cBingo gBingo;

#endif
