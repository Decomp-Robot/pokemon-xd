#include <types.h>

/* Colosseum battle connection flag.  The debug link menu raises it on entering
 * the connection menu, connection initialization clears it again, and the GBA
 * link code polls it while the menu is open, so the read stays volatile. */
extern volatile u8 menuColosseumBattleConnectionActive;

#pragma force_active on

void menuColosseumBattleConnectionExit(void)
{
}

#pragma peephole off
#pragma opt_dead_assignments off

s32 menuColosseumBattleConnectionMain(void)
{
    u8 prev = menuColosseumBattleConnectionActive;
    u8 prevInverted;

    menuColosseumBattleConnectionActive = 1;
    prevInverted = (u8)(1 - prev);
    return prev;
}

#pragma opt_dead_assignments reset
#pragma peephole reset

void menuColosseumBattleConnectionInit(void)
{
    menuColosseumBattleConnectionActive = 0;
}

#pragma force_active reset
