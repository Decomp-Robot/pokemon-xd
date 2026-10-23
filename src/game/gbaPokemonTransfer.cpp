// The transfer runs on the OS timers, so the SDK's OS header comes first: it
// brings in the SDK's scalar types, which the game headers use as well.
#include <dolphin/os.h>

#include <gbaCommand.h>
#include <types.h>
#include <gbaAgb.h>

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

// The packet stage hands the received Pokemon to the Pokemon module.
extern "C" void pokeconv(void *record, void *packet);

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
