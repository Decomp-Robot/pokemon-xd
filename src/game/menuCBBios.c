#include <types.h>

// menuCBBios_PortIDtoControlerID -- map an SI port ID to the cached
// controller ID probed on that port.
//
// gbaCommandKeyRead probes each of the 4 SI channels with SIProbe and caches
// the resulting controller type words in _SIProbe_Result (data 0x803226C0,
// 0x10 bytes, one word per channel); this accessor returns the cached word
// for the given port. The AGB-exchange callers (_AGB_ExchangeIsPrepared,
// _AGB_ExchangeStart, _InitialExchange, _AGB_EntryGetStatus, ...) pass their
// long channel straight through and test the result against the 0x40000
// controller-type word (subis/cmplwi), hence the long parameter and u32
// result. Groups with the other menuCBBios helpers
// (menuCBBios_ControlerIDtoPortID, menuCBBios_CopyTrainer,
// menuCBBios_InitTrainer, ...), which will join this module as they are
// recovered.
extern u32 _SIProbe_Result[4];

u32 menuCBBios_PortIDtoControlerID(long portID)
{
    return _SIProbe_Result[portID];
}
