#ifndef APP_BINGO_BINGOBIOS_H
#define APP_BINGO_BINGOBIOS_H

#include <types.h>

enum {
    BINGO_SCORE_COUNT = 10,
    BINGO_PANEL_COUNT = 14,
    BINGO_GRID_COUNT = 3,
    BINGO_HOSEI_VALUE_MASK = 0xFF,
    BINGO_HOSEI_KIND_MASK = 0xF,
    BINGO_HOSEI_KIND_SHIFT = 12
};

struct BINGO_DATA_dd_ {
    struct Panel {
        u8 type;
        u8 ability;
        u8 nature;
        u8 sex;
        u16 species;
        u16 move;
        u16 correction;
    };

    struct Grid {
        u8 kind;
        u8 position;
    };

    u8 number;
    u8 unknownHeader[2];
    u8 level;
    u8 unknownBeforeBalls;
    u8 balls;
    u8 unknownBeforeScores[10];
    u16 scores[BINGO_SCORE_COUNT];
    Panel panels[BINGO_PANEL_COUNT];
    Grid grids[BINGO_GRID_COUNT];
    u8 unknownTail[2];
};

class bingo_bios {
public:
    u16 getScore(BINGO_DATA_dd_* sheet, u8 index);
    u8 getGridPos(BINGO_DATA_dd_* sheet, u32 index);
    u8 getGridGrid(BINGO_DATA_dd_* sheet, u32 index);
    static u32 getPanelHoseiVal(BINGO_DATA_dd_* sheet, u32 index);
    static u32 getPanelHoseiKind(BINGO_DATA_dd_* sheet, u32 index);
    u32 getPanelWaza(BINGO_DATA_dd_* sheet, u32 index);
    u32 getPanelNo(BINGO_DATA_dd_* sheet, u32 index);
    u8 getPanelTokusei(BINGO_DATA_dd_* sheet, u32 index);
    u8 getPanelType(BINGO_DATA_dd_* sheet, u32 index);
    u8 getBall(BINGO_DATA_dd_* sheet);
    u32 getLevel(BINGO_DATA_dd_* sheet);
    u8 getNo(BINGO_DATA_dd_* sheet);

    static BINGO_DATA_dd_* getStaticSheet();
    static u8 getStaticResult();
};

#endif
