/*
 * GSflag.c
 *
 * The Boolean entry points of the flag store: setting a flag on or off with a
 * plain value and reducing a flag's stored value to true or false.
 */

#include <GSflag.h>

u8 GSflagTest(s32 flag)
{
    return GSflagGet(flag) != 0;
}

void GSflagOff(s32 flag)
{
    GSflagSet(flag, 0);
}

void GSflagOn(s32 flag)
{
    GSflagSet(flag, 1);
}
