#ifndef INCLUDE_RELGLOBAL_H
#define INCLUDE_RELGLOBAL_H

#include <types.h>

struct GSfloor_dd_ {
    u8 unknown;
    u8 kind;
    u16 floorID;
    u8 unknownData[0x3c];
};


struct FloorListHeader {
    u32 floorCount;
};

extern GSfloor_dd_* floordata;
extern FloorListHeader* floordata_number;

#endif
