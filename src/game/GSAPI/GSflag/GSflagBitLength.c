/*
 * GSflagBitLength.c
 *
 * How many bits are needed to hold an unsigned value. A flag is kept in a run
 * of consecutive bits, and a value is checked against that run before it is
 * stored in it.
 */

#include <types.h>

u32 _flagGetBitLength(u32 value)
{
    return 32 - __cntlzw(value);
}
