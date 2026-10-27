#ifndef GBA_COMMAND_H
#define GBA_COMMAND_H

#include <types.h>
#include <hero.h>

// Per-channel SI probe error counters.
extern u8 gba_SIProbe_ErrorCnt[4];

// Command the key layer runs on a channel; gbaCommandKeyRead answers it
// with the matching link call.
enum {
    GBA_KEY_COMMAND_NONE = 0,     // no command; the channel is not answered.
    GBA_KEY_COMMAND_READ = 1,     // reads the key word the GBA answers with.
    GBA_KEY_COMMAND_TRANSFER = 2, // a transfer owns the channel, no key is read.
    GBA_KEY_COMMAND_IDLE = 3,     // sends the idle word and reads its answer.
};

#ifdef __cplusplus
extern "C" {
#endif

void gba_SIProbe_ErrorCntClear(s32 chan);
u32 gba_SIProbe(long chan);
void gbaCommandChangePokemonEnd(long channel);
long gbaCommandChangePokemonIsPrepared(long channel);
long gbaCommandChangePokemon(long channel, struct Pokemon *pokemon, u32 partyIndex, long species);
u16 gbaCommandGetGBAChangePokemon(u32 key);
u8 gbaCommandGetWazaTarget(u32 key);
u8 gbaCommandGetWazaDataId(u32 key);
u32 gbaCommandGetAction(u32 key);
long gbaCommandRequestAction(u32 channel, void *target, u32 slot, long flag);
long gbaCommandEntryPokemon(u32 packed, u8 *slots);
long gbaCommandEntryResult(long channel, u32 *result);
void gbaCommandEntryRequest(long channel, u32 pokemonCount, u32 minimumPokemonCount, u32 entryType);
void gbaCommandSendWazaText(long channel, Hero *hero);
u16 gbaCommandKeyPressed(long channel);
long gbaCommandKeyRead(long channel);

// Makes the channel run the given key command and clears the state the
// previous one left; returns the command that was running before.
long gbaCommandSetKeyState(long chan, long state);

// Whether the port carries a controller that answers the link commands and
// has answered the last one.
u8 gbaCommandIsConnected(s32 port);

#ifdef __cplusplus
}
#endif

#endif
