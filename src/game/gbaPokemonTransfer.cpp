// The transfer runs on the OS timers, so the SDK's OS header comes first: it
// brings in the SDK's scalar types, which the game headers use as well.
#include <dolphin/os.h>

#include <gbaCommand.h>
#include <types.h>

/*
 * Pokemon transfer over the GBA link.
 *
 * The transfer of one Pokemon runs as a chain of job callbacks: AGB_GetPokemon
 * resets the link and each stage below hands the channel on to the next one.
 * The stages with an "s" in their name poll GBAGetStatusAsync until the
 * channel is ready again, the others start the transfer itself and return;
 * the link library calls the next stage once it has finished.
 *
 * All stages share one work item per SI channel (AGBWorkArray). The download
 * half reads the answer of the GBA into the receive buffer of the channel
 * (agbRecvBuffers), which stage 9 points the work item at.
 */

// Stage numbers of the polling variants of a stage run as 1000 + the stage.
#define STAGE_POLL_BASE 1000
// Milliseconds a stage waits for the link channel before it gives up on it, and
// the period the download half polls the channel with.
#define STAGE_TIMEOUT_MSEC 125
#define STAGE_POLL_MSEC 100
// Size of the packet a channel exchanges: the received packet is assembled in
// one scratch buffer of this size per channel (agbRecvBuffers).
#define AGB_PACKET_SIZE 0x278
// Command byte of the download exchange, which the GBA echoes in the header of
// its reply.
#define AGB_DOWNLOAD_COMMAND 0x99

// Bits of the channel status byte the link library reports for a channel. The
// GBA sets the ready bit once it has put a word into the channel's receive
// register, and keeps the busy bit set while it is not ready for the next word:
// the reset handshake the link library runs for the channel waits for the busy
// bit to clear before it exchanges a word.
#define AGB_LINK_STATUS_BUSY 0x02
#define AGB_LINK_STATUS_DATA_READY 0x08

// Dolphin SDK Game Boy Advance link library (dolphin/gba).
typedef void (*GBACallback)(long chan, long result);

extern "C" long GBAGetStatusAsync(long chan, u8 *status, GBACallback callback);
extern "C" long GBAReadAsync(long chan, void *dst, u8 *status, GBACallback callback);
extern "C" long GBAWriteAsync(long chan, void *src, u8 *status, GBACallback callback);

void _timer_set(long chan, unsigned long msec);
void _job_type_clear(long chan, const char *jobName);
extern "C" void pokeconv(void *record, void *packet);

// Name each stage reports to the link job list when it releases the channel,
// kept in the string pool of the link library.
extern char agbJobName04[];   // "_getpokemon_04"
extern char agbJobName05[];   // "_getpokemon_05"
extern char agbJobName05s[];  // "_getpokemon_05s"
extern char agbJobName06[];   // "_getpokemon_06"
extern char agbJobName07[];   // "_getpokemon_07"
extern char agbJobName07s[];  // "_getpokemon_07s"
extern char agbJobName08[];   // "_getpokemon_08"
extern char agbJobName09[];   // "_getpokemon_09"
extern char agbJobName09s[];  // "_getpokemon_09s"
extern char agbJobName10[];   // "_getpokemon_10"
extern char agbJobName11[];   // "_getpokemon_11"

// Forward declarations of the transfer stages, each one hands the channel on to
// the stage that follows it in the chain. The second argument is the result the
// link library hands to its callback: 1 ends the job, anything else leaves the
// stage free to pass the next link call to the channel.
void _getpokemon_04(long chan, long result);
void _getpokemon_05(long chan, long result);
void _getpokemon_05s(long chan, long result);
void _getpokemon_06(long chan, long result);
void _getpokemon_07(long chan, long result);
void _getpokemon_07s(long chan, long result);
void _getpokemon_08(long chan, long result);
void _getpokemon_09(long chan, long result);
void _getpokemon_09s(long chan, long result);
void _getpokemon_10(long chan, long result);
void _getpokemon_11(long chan, long result);

// State of one GBA link channel: the status bytes the link library reports the
// channel state in, the stage the channel runs, the packet buffer the transfer
// is assembled in and the two destinations the packet is written to.
struct AGBWork {
    long jobType;            // 0x00: job the channel runs, 0 while none is active.
    u8 linkStatus[0x04];     // 0x04: channel state reported by the link library.
    long stage;              // 0x08: stage of the job the channel runs.
    long active;             // 0x0C: nonzero while a transfer owns the channel.
    long idleStage;          // 0x10: stage AGB_IsComplete resumes an idle channel.
    long result;             // 0x14: return value of the last link call.
    long status;             // 0x18: status AGB_GetStatus reports (0 = the packet
                             //       is in, 1 = the job ended in error, 2 = the
                             //       stage timed out, 3 = the job is running).
    unsigned long packet;    // 0x1C: word the current read receives into.
    u8 _20[0x1C];
    unsigned long request;   // 0x3C: word the current write sends out.
    unsigned long reply;     // 0x40: header word read back from the GBA.
    unsigned long period;    // 0x44: polling period of the stage timer, in msec.
    u8 _48[0x14];
    unsigned long offset;    // 0x5C: write position in the packet buffer.
    u8 _60[0x0C];
    unsigned long tick;      // 0x6C: start tick of the stage timer.
    unsigned long timeout;   // 0x70: stage timer length, in ticks.
    unsigned long pollTick;  // 0x74: start tick of the polling timer.
    unsigned long pollTimeout; // 0x78: polling timer length, in ticks.
    unsigned long resetTick;    // 0x7C: start tick of the GBA reset handshake.
    unsigned long resetTimeout; // 0x80: reset handshake length, in ticks.
    u8 _84[0x08];               // 0x84-0x8B: unknown.
    u8 *buffer;              // 0x8C: buffer the packet is assembled in.
    u8 _90[0x04];
    void *record;            // 0x94: Pokemon record the packet is converted into.
    unsigned long *dest;     // 0x98: destination of the packet header word.
};

extern AGBWork AGBWorkArray[4];
extern u8 agbRecvBuffers[];

// _getpokemon_11(long, long): stage 11, last stage of the download half. Appends
// the received word to the packet and keeps polling for the next one; once the
// whole packet is in, converts it and hands the header word that precedes it
// to the requester.
void _getpokemon_11(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 11;
    if (result == 1) {
        _job_type_clear(chan, agbJobName11);
        AGBWorkArray[chan].status = 1;
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
        return;
    }
    *(unsigned long *)(AGBWorkArray[chan].buffer + AGBWorkArray[chan].offset) =
        AGBWorkArray[chan].packet;
    AGBWorkArray[chan].offset += 4;
    if (AGBWorkArray[chan].offset < AGB_PACKET_SIZE) {
        _timer_set(chan, STAGE_TIMEOUT_MSEC);
        _getpokemon_09s(chan, result);
        return;
    }
    pokeconv(AGBWorkArray[chan].record, AGBWorkArray[chan].buffer);
    {
        u8 *packet = AGBWorkArray[chan].buffer;

        *AGBWorkArray[chan].dest =
            (packet[3] << 24) | (packet[2] << 16) | (packet[1] << 8) | packet[0];
    }
    _job_type_clear(chan, agbJobName11);
    AGBWorkArray[chan].status = 0;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
}

// _getpokemon_10(long, long): stage 10, reads one word of the packet. The channel
// state decides whether the download half is still running.
void _getpokemon_10(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 10;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName10);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_DATA_READY) != 0) {
        AGBWorkArray[chan].result =
            GBAReadAsync(chan, &AGBWorkArray[chan].packet, AGBWorkArray[chan].linkStatus,
                _getpokemon_11);
        if (AGBWorkArray[chan].result != 2) {
            return;
        }
        goto done;
    }
    _getpokemon_09s(chan, result);
}

// _getpokemon_09s(long, long): stage 1009, waits for the link channel to report
// ready and then reads the next word of the packet.
void _getpokemon_09s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 9;
    if (OSGetTick() - AGBWorkArray[chan].tick <= AGBWorkArray[chan].timeout) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName09s);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _getpokemon_10);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _getpokemon_09(long, long): stage 9, checks the header of the reply and either
// prepares the packet buffer or ends the transfer.
void _getpokemon_09(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 9;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName09);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    if ((AGBWorkArray[chan].reply >> 24) != AGB_DOWNLOAD_COMMAND) {
        goto done;
    }
    AGBWorkArray[chan].offset = 0;
    AGBWorkArray[chan].buffer = &agbRecvBuffers[chan * AGB_PACKET_SIZE];
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    _getpokemon_09s(chan, result);
}

// _getpokemon_08(long, long): stage 8, downloads the reply of the request.
void _getpokemon_08(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 8;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName08);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0]
            & (AGB_LINK_STATUS_BUSY | AGB_LINK_STATUS_DATA_READY))
            != AGB_LINK_STATUS_DATA_READY) {
        goto idle;
    }
    AGBWorkArray[chan].result =
        GBAReadAsync(chan, &AGBWorkArray[chan].reply, AGBWorkArray[chan].linkStatus,
            _getpokemon_09);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _getpokemon_07s(chan, result);
}

// _getpokemon_07s(long, long): stage 1007, waits for the link channel to report
// ready and then starts the download of the reply.
void _getpokemon_07s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 7;
    if (OSGetTick() - AGBWorkArray[chan].pollTick <= AGBWorkArray[chan].pollTimeout) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName07s);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _getpokemon_08);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _getpokemon_07(long, long): stage 7, arms the polling timer of the download
// half.
void _getpokemon_07(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 7;
    if (result == 1) {
        _job_type_clear(chan, agbJobName07);
        AGBWorkArray[chan].status = 1;
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
        return;
    }
    AGBWorkArray[chan].period = STAGE_POLL_MSEC;
    AGBWorkArray[chan].pollTimeout = OSMillisecondsToTicks(AGBWorkArray[chan].period);
    AGBWorkArray[chan].pollTick = OSGetTick();
    _getpokemon_07s(chan, result);
}

// _getpokemon_06(long, long): stage 6, sends the request word out to the GBA.
void _getpokemon_06(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 6;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName06);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_BUSY) != 0) {
        goto idle;
    }
    AGBWorkArray[chan].request = AGB_DOWNLOAD_COMMAND;
    AGBWorkArray[chan].result =
        GBAWriteAsync(chan, &AGBWorkArray[chan].request, AGBWorkArray[chan].linkStatus,
            _getpokemon_07);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _getpokemon_05s(chan, result);
}

// _getpokemon_05s(long, long): stage 1005, waits for the link channel to report
// ready and then starts the upload of the request.
void _getpokemon_05s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 5;
    if (OSGetTick() - AGBWorkArray[chan].tick <= AGBWorkArray[chan].timeout) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName05s);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _getpokemon_06);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _getpokemon_05(long, long): stage 5, arms the stage timer of the send half.
void _getpokemon_05(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 5;
    if (result == 1) {
        _job_type_clear(chan, agbJobName05);
        AGBWorkArray[chan].status = 1;
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
        return;
    }
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    _getpokemon_05s(chan, result);
}

// _getpokemon_04(long, long): stage 4, sends the first request of the transfer
// out.
void _getpokemon_04(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 4;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName04);
    AGBWorkArray[chan].status = 1;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAWriteAsync(chan, &AGBWorkArray[chan].reply, AGBWorkArray[chan].linkStatus,
            _getpokemon_05);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}
