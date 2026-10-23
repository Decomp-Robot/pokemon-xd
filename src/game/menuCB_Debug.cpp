#include <menuCBDebug.h>

u8 _DBG = 1;

void menuCBDebug_SkipAGBConnection(u8 connected)
{
    _DBG = (u8)(!connected);
}
