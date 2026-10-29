#include <floorManager.h>
#include <types.h>
#include <app/menu/menuBingo.h>

static const u32 kFloorBattleBingo = 904;

extern "C" u8 menuBattleDeBingoGetNum(void)
{
    if (bingo_bios::getStaticSheet() != 0) {
        return gBingo.getNo(bingo_bios::getStaticSheet());
    }
    return 0;
}

extern "C" u8 menuBattleDeBingoGetLine(void)
{
    return bingo_bios::getStaticResult() & 15;
}

extern "C" u8 menuBattleDeBingoGetResult(void)
{
    u8 result = bingo_bios::getStaticResult();
    return (result >> 4) & 15;
}

extern "C" void menuBattleDeBingo(void)
{
    GSfloorPush(kFloorBattleBingo);
}
