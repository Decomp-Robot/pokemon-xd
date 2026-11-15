#include <app/bingo/bingoBios.h>

u16 bingo_bios::getScore(BINGO_DATA_dd_* sheet, u8 index)
{
    if (index >= BINGO_SCORE_COUNT) {
        return 0;
    }
    return sheet->scores[index];
}

u8 bingo_bios::getGridPos(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_GRID_COUNT) {
        return 0;
    }
    return sheet->grids[index].position;
}

u8 bingo_bios::getGridGrid(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_GRID_COUNT) {
        return 0;
    }
    return sheet->grids[index].kind;
}

u32 bingo_bios::getPanelHoseiVal(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_PANEL_COUNT) {
        return 0;
    }
    return sheet->panels[index].correction & BINGO_HOSEI_VALUE_MASK;
}

u32 bingo_bios::getPanelHoseiKind(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_PANEL_COUNT) {
        return 0;
    }
    return (sheet->panels[index].correction >> BINGO_HOSEI_KIND_SHIFT) & BINGO_HOSEI_KIND_MASK;
}

u32 bingo_bios::getPanelWaza(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_PANEL_COUNT) {
        return 0;
    }
    return sheet->panels[index].move;
}

u32 bingo_bios::getPanelNo(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_PANEL_COUNT) {
        return 0;
    }
    return sheet->panels[index].species;
}

u8 bingo_bios::getPanelTokusei(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_PANEL_COUNT) {
        return 0;
    }
    return sheet->panels[index].ability;
}

u8 bingo_bios::getPanelType(BINGO_DATA_dd_* sheet, u32 index)
{
    if (index >= BINGO_PANEL_COUNT) {
        return 0;
    }
    return sheet->panels[index].type;
}

u8 bingo_bios::getBall(BINGO_DATA_dd_* sheet)
{
    if (sheet != 0) {
        return sheet->balls;
    }
    return 0;
}

u32 bingo_bios::getLevel(BINGO_DATA_dd_* sheet)
{
    if (sheet != 0) {
        return sheet->level;
    }
    return 0;
}

u8 bingo_bios::getNo(BINGO_DATA_dd_* sheet)
{
    if (sheet != 0) {
        return sheet->number;
    }
    return 0;
}
