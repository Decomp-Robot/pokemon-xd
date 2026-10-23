#include <LooseBallAnims.h>
#include <menuCBBios.h>

// Desperation info of port `index`: the controller ID mapped to the port.
s32 LooseBallAnims::GetDesperationInfo(s32 index)
{
    return staControlerMap[index];
}
