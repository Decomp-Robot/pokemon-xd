#ifndef APP_BINGO_BINGOBIOS_H
#define APP_BINGO_BINGOBIOS_H

#include <types.h>

struct BINGO_DATA_dd_;

class bingo_bios {
public:
    static BINGO_DATA_dd_* getStaticSheet();
    u8 getNo(BINGO_DATA_dd_* sheet);
    static u8 getStaticResult();
};

#endif
