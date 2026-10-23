#include <types.h>

/* Colosseum battle connection flag.  The debug link menu raises it on entering
 * the connection menu, initialization clears it, and the GBA
 * link code polls it while the menu is open, so the read stays volatile. */
extern volatile u8 lbl_804EA688;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

/* Built the way the original object was: it carries the complement of the
 * previous state although the value is discarded, so dead-assignment
 * elimination and the peephole pass stay off for the functions below. */
#pragma peephole off
#pragma opt_dead_assignments off

/* Raises the connection flag and reports the state it was in before. */
void menuColosseumBattleConnectionExit(void)
{
}

s32 menuColosseumBattleConnectionMain(void)
{
    u8 prev = lbl_804EA688;
    u8 prevInverted;

    lbl_804EA688 = 1;
    prevInverted = (u8)(1 - prev);
    return prev;
}

#pragma opt_dead_assignments reset
#pragma peephole reset

void menuColosseumBattleConnectionInit(void)
{
    lbl_804EA688 = 0;
}

#pragma force_active reset
