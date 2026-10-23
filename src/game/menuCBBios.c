#include <types.h>

// SI controller probe cache filled by gbaCommandKeyRead (gbaCommandLink.cpp), one
// cached controller-type word per SI channel.
extern u32 _SIProbe_Result[4];

// Look up the controller type last probed on the given SI port. The AGB exchange
// entry points in gbaAgbLink.cpp (_AGB_ExchangeIsPrepared, _AGB_ExchangeStart,
// _InitialExchange, _AGB_EntryGetStatus) pass their link channel straight through
// and test the result against the GBA controller-type word.
u32 menuCBBios_PortIDtoControlerID(long portID)
{
    return _SIProbe_Result[portID];
}
