#include <fightSeq.h>


void WS_ZUKAN_CHECK(void)
{
    fightSeq_GetWazaSeq();
    fightSeq_NextWazaSeq(5);
}


void WS_ZUKAN(void)
{
    fightSeq_GetWazaSeq();
    fightSeq_NextWazaSeq(1);
}


void WS_NICKNAME(void)
{
    fightSeq_GetWazaSeq();
    fightSeq_NextWazaSeq(5);
}


void WS_ITEM_HP_KAIFUKU(void)
{
    fightSeq_GetWazaSeq();
    fightSeq_NextWazaSeq(1);
}


void WS_ITEM_CONDITION_RECOVER(void)
{
    fightSeq_GetWazaSeq();
    fightSeq_NextWazaSeq(1);
}
