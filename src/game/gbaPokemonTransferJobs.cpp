// The stages poll the link through the OS timers and the job record holds OS
// ticks, so the shared header of the link layer comes first: it brings in the
// OS header of the SDK and the scalar types of the game.
#include <gbaAgb.h>

#include <gbaCommand.h>

/*
 * The Pokemon transfer jobs of the AGB link layer.
 *
 * Every stage below is one step of a job: it starts one link call for its
 * channel, hands the stage that follows the call to the link library and
 * returns; the library calls that stage back once the call has finished, and
 * the failure paths release the channel again. The download job (the stages
 * _getpokemon_01..11 with the entry point AGB_GetPokemon) receives a Pokemon
 * from the GBA; the upload job (the stages _upload_00s..25 with the entry point
 * AGB_Upload) sends the code together with the Pokemon the GBA is handed.
 *
 * This unit holds the closing stages of the download job and the stages of the
 * upload job down to stage 7, which is the address range the transfer jobs
 * occupy here; the download stages 4..11 that run before them are in
 * game/gbaPokemonTransfer.cpp, and the work record the stages share is declared
 * in include/gbaAgb.h.
 */

// _getpokemon_03(long, long): stage 3, the header word of the reply of the GBA
// is in: check that it is the word the exchange runs with, and let the status
// stage of the header follow.
void _getpokemon_03(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 3;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName03);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    if (gbaGameId != AGBWorkArray[chan].reply) {
        goto done;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _getpokemon_04);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _getpokemon_02(long, long): stage 2, reads the data word of the reply.
void _getpokemon_02(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 2;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName02);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
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
            _getpokemon_03);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _getpokemon_01s(chan, result);
}

// _getpokemon_01s(long, long): stage 1001, waits for the link channel to report
// ready and then starts the read of the reply.
void _getpokemon_01s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 1;
    if (OSGetTick() - AGBWorkArray[chan].pollTick <= AGBWorkArray[chan].pollTimeout) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbJobName01s);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _getpokemon_02);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _getpokemon_01(long, long): stage 1, arms the polling timer the reply of the
// transfer is received with.
void _getpokemon_01(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 1;
    if (result == 1) {
        _job_type_clear(chan, agbJobName01);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
        return;
    }
    AGBWorkArray[chan].period = PACKET_POLL_MSEC;
    AGBWorkArray[chan].pollTimeout = OSMillisecondsToTicks(AGBWorkArray[chan].period);
    AGBWorkArray[chan].pollTick = OSGetTick();
    _getpokemon_01s(chan, result);
}

// AGB_GetPokemon(long, void*, unsigned long*): starts the download job of a
// channel: the record takes the channel over, the GBA is reset and the first
// stage runs once the code of the GBA answers. The Pokemon the GBA sends is
// converted into record and the header word that precedes it is stored through
// dest.
long AGB_GetPokemon(long chan, void *record, unsigned long *dest)
{
    // The key layer of the link drives the four SI ports, the job runs per channel.
    long port;

    if (AGB_Initialize(chan) == 0) {
        return 0;
    }
    port = chan + 1;
    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    _AGB_Work_Clear(chan);
    AGBWorkArray[chan].jobType = AGB_JOB_TYPE_GET_POKEMON;
    AGBWorkArray[chan].status = AGB_STATUS_RUNNING;
    AGBWorkArray[chan].active = 1;
    AGBWorkArray[chan].record = record;
    AGBWorkArray[chan].dest = dest;
    AGBWorkArray[chan].result = SIProbe(chan);
    if (AGBWorkArray[chan].result != AGB_SI_TYPE_GBA) {
failed:
        // The job that could not start releases the channel the way every stage
        // releases it when it ends in error.
        _job_type_clear(chan, agbJobNameGetPokemon);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        gbaCommandSetKeyState(port, GBA_KEY_COMMAND_READ);
        return 0;
    }
    AGBWorkArray[chan].result =
        GBAResetAsync(chan, AGBWorkArray[chan].linkStatus, _getpokemon_01);
    if (AGBWorkArray[chan].result == 2) {
        goto failed;
    }
    return 1;
}

// _upload_25(long, long): stage 25, last stage of the upload job. The GBA
// answers the command byte of the upload: the packet is in, and the channel goes
// back to the key layer. A job that ended here, or an answer of another kind,
// waits for the GBA to be ready again instead.
void _upload_25(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 25;
    if (result != 1) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    if ((AGBWorkArray[chan].reply >> 24) != AGB_UPLOAD_COMMAND) {
        goto done;
    }
    _job_type_clear(chan, agbUploadJobName25);
    AGBWorkArray[chan].status = AGB_STATUS_COMPLETE;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
}

// _upload_24(long, long): stage 24, reads the command byte of the answer.
void _upload_24(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 24;
    if (result != 1) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0]
            & (AGB_LINK_STATUS_BUSY | AGB_LINK_STATUS_DATA_READY))
            != AGB_LINK_STATUS_DATA_READY) {
        goto idle;
    }
    AGBWorkArray[chan].result =
        GBAReadAsync(chan, &AGBWorkArray[chan].reply, AGBWorkArray[chan].linkStatus,
            _upload_25);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _upload_23s(chan, result);
}

// _upload_23s(long, long): stage 1023, waits for the link channel to report
// ready and then starts the read of the answer.
void _upload_23s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 23;
    if (OSGetTick() - AGBWorkArray[chan].pollTick <= AGBWorkArray[chan].pollTimeout) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_24);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_23(long, long): stage 23, arms the polling timer of the answer.
void _upload_23(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 23;
    if (result == 1) {
        _upload_16s(chan, result);
        return;
    }
    AGBWorkArray[chan].period = STAGE_POLL_MSEC;
    AGBWorkArray[chan].pollTimeout = OSMillisecondsToTicks(AGBWorkArray[chan].period);
    AGBWorkArray[chan].pollTick = OSGetTick();
    _upload_23s(chan, result);
}

// _upload_22(long, long): stage 22, sends the command byte of the upload out.
void _upload_22(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 22;
    if (result != 1) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_BUSY) != 0) {
        goto idle;
    }
    AGBWorkArray[chan].request = AGB_UPLOAD_COMMAND;
    AGBWorkArray[chan].result =
        GBAWriteAsync(chan, &AGBWorkArray[chan].request, AGBWorkArray[chan].linkStatus,
            _upload_23);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _upload_21s(chan, result);
}

// _upload_21s(long, long): stage 1021, waits for the link channel to report
// ready and then starts the write of the command byte.
void _upload_21s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 21;
    if (OSGetTick() - AGBWorkArray[chan].tick <= AGBWorkArray[chan].timeout) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_22);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_21(long, long): stage 21, arms the stage timer of the command byte.
void _upload_21(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 21;
    if (result == 1) {
        _upload_16s(chan, result);
        return;
    }
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    _upload_21s(chan, result);
}

// _upload_20(long, long): stage 20, sends the answer word of the exchange out.
void _upload_20(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 20;
    if (result != 1) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAWriteAsync(chan, &AGBWorkArray[chan].reply, AGBWorkArray[chan].linkStatus,
            _upload_21);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_19(long, long): stage 19, checks the answer word of the GBA against
// the game code the exchange runs with.
void _upload_19(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 19;
    if (result != 1) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    if (gbaGameId != AGBWorkArray[chan].reply) {
        goto done;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_20);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_18(long, long): stage 18, reads the answer word of the GBA.
void _upload_18(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 18;
    if (result != 1) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0]
            & (AGB_LINK_STATUS_BUSY | AGB_LINK_STATUS_DATA_READY))
            != AGB_LINK_STATUS_DATA_READY) {
        goto idle;
    }
    AGBWorkArray[chan].result =
        GBAReadAsync(chan, &AGBWorkArray[chan].reply, AGBWorkArray[chan].linkStatus,
            _upload_19);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _upload_17s(chan, result);
}

// _upload_17s(long, long): stage 1017, waits for the link channel to report
// ready and then starts the read of the answer word.
void _upload_17s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 17;
    if (OSGetTick() - AGBWorkArray[chan].pollTick <= AGBWorkArray[chan].pollTimeout) {
        goto transfer;
    }
done:
    _upload_16s(chan, result);
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_18);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_17(long, long): stage 17, arms the polling timer of the answer word
// and starts the exchange again.
void _upload_17(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 17;
    if (result == 1) {
        _upload_16s(chan, result);
        return;
    }
    AGBWorkArray[chan].period = PACKET_POLL_MSEC;
    AGBWorkArray[chan].pollTimeout = OSMillisecondsToTicks(AGBWorkArray[chan].period);
    AGBWorkArray[chan].pollTick = OSGetTick();
    _upload_17s(chan, result);
}

// _upload_16s(long, long): stage 1016, waits for the reset handshake of the GBA
// to come in and starts the exchange at stage 17 once the code of the GBA is
// up. A handshake that ran into the reset timer ends the job.
void _upload_16s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 16;
    AGBWorkArray[chan].idleStage = AGB_IDLE_NONE;
    if (OSGetTick() - AGBWorkArray[chan].resetTick > AGBWorkArray[chan].resetTimeout) {
        _job_type_clear(chan, agbUploadJobName16s);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        return;
    }
    AGBWorkArray[chan].result = SIProbe(chan);
    if (AGBWorkArray[chan].result != AGB_SI_TYPE_GBA) {
        AGBWorkArray[chan].idleStage = AGB_IDLE_RESET_16S;
        return;
    }
    AGBWorkArray[chan].result =
        GBAResetAsync(chan, AGBWorkArray[chan].linkStatus, _upload_17);
    if (AGBWorkArray[chan].result == 2) {
        AGBWorkArray[chan].idleStage = AGB_IDLE_RESET_16S;
    }
}

// _upload_16s0(long, long): stage 216, waits for the code of the GBA to answer
// the probe and then starts the reset handshake; a reset that is still in
// progress is continued by stage 1016. A port the probe still reports as a Game
// Boy Advance keeps the channel idle here instead (the record is left to the job
// framework, which runs this stage again), so the reset runs for a port that no
// longer answers as one.
void _upload_16s0(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL0_BASE + 16;
    AGBWorkArray[chan].idleStage = AGB_IDLE_NONE;
    if (OSGetTick() - AGBWorkArray[chan].tick > AGBWorkArray[chan].timeout) {
        _job_type_clear(chan, agbUploadJobName16s0);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        return;
    }
    AGBWorkArray[chan].result = SIProbe(chan);
    if (AGBWorkArray[chan].result != AGB_SI_TYPE_GBA) {
        AGBWorkArray[chan].resetTimeout = OSSecondsToTicks(8);
        AGBWorkArray[chan].resetTick = OSGetTick();
        AGBWorkArray[chan].result =
            GBAResetAsync(chan, AGBWorkArray[chan].linkStatus, _upload_17);
        if (AGBWorkArray[chan].result == 2) {
            _upload_16s(chan, result);
        }
        return;
    }
    AGBWorkArray[chan].idleStage = AGB_IDLE_RESET_16S0;
}

// _upload_16(long, long): stage 16, arms the timer the code of the GBA is given
// to answer the probe and starts the reset handshake.
void _upload_16(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 16;
    if (result == 1) {
        _job_type_clear(chan, agbUploadJobName16);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        return;
    }
    AGBWorkArray[chan].timeout = OSSecondsToTicks(1);
    AGBWorkArray[chan].tick = OSGetTick();
    _upload_16s0(chan, result);
}

// _upload_15(long, long): stage 15, checks the kind of the word the GBA answered
// with: the GBA asking for its boot key is answered with the key looked up for
// it, and a word the GBA took lets the packet exchange run on.
void _upload_15(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 15;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName15);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    if ((AGBWorkArray[chan].linkWord >> 24) == AGB_WORD_KIND_BOOT_KEY) {
        AGBWorkArray[chan].bootKey =
            _PKJBUpFindBootKey(SwapEndian(AGBWorkArray[chan].linkWord),
                AGBWorkArray[chan].checksum);
        if (AGBWorkArray[chan].bootKey == 0) {
            goto done;
        }
        AGBWorkArray[chan].bootKey = SwapEndian(AGBWorkArray[chan].bootKey);
        AGBWorkArray[chan].result =
            GBAWriteAsync(chan, &AGBWorkArray[chan].bootKey,
                AGBWorkArray[chan].linkStatus, _upload_16);
        if (AGBWorkArray[chan].result != 2) {
            return;
        }
        goto done;
    }
    if ((AGBWorkArray[chan].linkWord >> 24) != AGB_WORD_KIND_ACCEPTED) {
        goto done;
    }
    _upload_13s(chan, result);
}

// _upload_14(long, long): stage 14, reads the word the GBA answers the packet
// with.
void _upload_14(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 14;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName14);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_SYNC_STATE)
            != AGB_LINK_STATUS_SYNC_PACKET) {
        goto done;
    }
    if ((AGBWorkArray[chan].linkStatus[0]
            & (AGB_LINK_STATUS_BUSY | AGB_LINK_STATUS_DATA_READY))
            != AGB_LINK_STATUS_DATA_READY) {
        goto idle;
    }
    AGBWorkArray[chan].result =
        GBAReadAsync(chan, &AGBWorkArray[chan].linkWord,
            AGBWorkArray[chan].linkStatus, _upload_15);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
idle:
    _upload_13s(chan, result);
}

// _upload_13s(long, long): stage 1013, waits for the packet the GBA is given to
// be written out.
void _upload_13s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 13;
    if (OSGetTick() - AGBWorkArray[chan].tick <= AGBWorkArray[chan].timeout) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName13s);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_14);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_13(long, long): stage 13, sends the packet the GBA runs out word by
// word; the packet word that closes it is handed to stage 1013 once the whole
// packet has been written.
void _upload_13(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 13;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName13);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_SYNC_TAKEN) == 0) {
        goto done;
    }
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_BUSY) != 0) {
        goto idle;
    }
    AGBWorkArray[chan].offset += 4;
    if (AGBWorkArray[chan].offset < AGBWorkArray[chan].size) {
        _upload_11s(chan, result);
        return;
    }
    AGBWorkArray[chan].timeout = OSSecondsToTicks(10);
    AGBWorkArray[chan].tick = OSGetTick();
    _upload_13s(chan, result);
    return;
idle:
    _upload_12s(chan, result);
}

// _upload_12s(long, long): stage 1012, polls the link channel until the word
// that was written has been taken by the GBA.
void _upload_12s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 12;
retry:
    if (OSGetTick() - AGBWorkArray[chan].tick > AGBWorkArray[chan].timeout) {
        _job_type_clear(chan, agbUploadJobName12s);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        return;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_13);
    if (AGBWorkArray[chan].result == 2) {
        goto retry;
    }
}

// _upload_12(long, long): stage 12, checks whether the GBA echoes the state
// code the word was tagged with, advances the tag and writes the next word.
void _upload_12(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 12;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName12);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_SYNC_STATE)
            != AGBWorkArray[chan].syncCode) {
        goto done;
    }
    AGBWorkArray[chan].syncCode ^= AGB_SYNC_CODE_STEP;
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    _upload_12s(chan, result);
}

// _upload_11s(long, long): stage 1011, sends one word of the packet out. The
// payload behind the header of the packet is coded with the rolling key and
// checksum of the exchange, and the word is written again while the stage timer
// runs.
void _upload_11s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_POLL_BASE + 11;
    AGBWorkArray[chan].codedWord =
        SwapEndian(*(unsigned long *)(AGBWorkArray[chan].buffer
            + AGBWorkArray[chan].offset));
    if (AGBWorkArray[chan].offset >= AGB_PACKET_HEADER_SIZE) {
        AGBWorkArray[chan].plainWord = AGBWorkArray[chan].codedWord;
        AGBWorkArray[chan].codedWord -= AGBWorkArray[chan].checksum;
        AGBWorkArray[chan].codedWord ^= AGBWorkArray[chan].key;
        AGBWorkArray[chan].checksum =
            _PKJBUpCalcCrc(AGBWorkArray[chan].checksum, AGBWorkArray[chan].plainWord);
        AGBWorkArray[chan].key =
            AGBWorkArray[chan].key * AGB_KEY_MULTIPLIER + 1;
    }
    AGBWorkArray[chan].linkWord = SwapEndian(AGBWorkArray[chan].codedWord);
send:
    AGBWorkArray[chan].result =
        GBAWriteAsync(chan, &AGBWorkArray[chan].linkWord,
            AGBWorkArray[chan].linkStatus, _upload_12);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    if (OSGetTick() - AGBWorkArray[chan].tick <= AGBWorkArray[chan].timeout) {
        goto send;
    }
    _job_type_clear(chan, agbUploadJobName11s);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
}

// _upload_11(long, long): stage 11, starts the packet exchange: the key the GBA
// gave back seeds the rolling key and checksum of the coding, the packet is
// written from its start.
void _upload_11(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 11;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName11);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_SYNC_STATE)
            != AGB_LINK_STATUS_SYNC_PACKET) {
        goto done;
    }
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_BUSY) != 0) {
        goto idle;
    }
    // Both seeds of the packet coding are derived from the two keys of the
    // exchange, the checksum from their sum and the rolling key from it stepped
    // once.
    AGBWorkArray[chan].checksum = AGBWorkArray[chan].keyA ^ AGBWorkArray[chan].keyB;
    AGBWorkArray[chan].key =
        (AGBWorkArray[chan].keyA ^ AGBWorkArray[chan].keyB) * AGB_KEY_MULTIPLIER + 1;
    AGBWorkArray[chan].syncCode = AGB_SYNC_CODE_PACKET;
    AGBWorkArray[chan].offset = 0;
    _upload_11s(chan, result);
    return;
idle:
    if (OSGetTick() - AGBWorkArray[chan].tick > AGBWorkArray[chan].timeout) {
        goto done;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_11);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_10(long, long): stage 10, arms the stage timer of the packet
// exchange.
void _upload_10(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 10;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName10);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_11);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}

// _upload_09(long, long): stage 9, checks the key the GBA answered with and
// tells it how many blocks of the packet follow.
void _upload_09(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 9;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName09);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    AGBWorkArray[chan].keyB =
        _PKJBUpValidateKeyB(SwapEndian(AGBWorkArray[chan].bootKey));
    if (AGBWorkArray[chan].keyB == 0) {
        goto done;
    }
    {
        // Number of 8-byte blocks the packet is made of, less the block the
        // length word itself occupies; the length is aligned to a block first.
        unsigned long blocks = (AGBWorkArray[chan].size + 7) & ~7;

        AGBWorkArray[chan].size = blocks;
        blocks = SwapEndian(blocks / 8 - 1);
        AGBWorkArray[chan].result =
            GBAWriteAsync(chan, &blocks, AGBWorkArray[chan].linkStatus, _upload_10);
        if (AGBWorkArray[chan].result != 2) {
            return;
        }
    }
    goto done;
}

// _upload_08(long, long): stage 8, waits for the GBA to ask for the key word
// and reads it into the boot key.
void _upload_08(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 8;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName08);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    if (AGBWorkArray[chan].linkStatus[0]
            == (AGB_LINK_STATUS_SYNC_PACKET | AGB_LINK_STATUS_DATA_READY)) {
        AGBWorkArray[chan].result =
            GBAReadAsync(chan, &AGBWorkArray[chan].bootKey,
                AGBWorkArray[chan].linkStatus, _upload_09);
        if (AGBWorkArray[chan].result != 2) {
            return;
        }
        goto done;
    }
    if (OSGetTick() - AGBWorkArray[chan].tick > AGBWorkArray[chan].timeout) {
        goto done;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_08);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_07(long, long): stage 7, arms the stage timer the GBA is given to
// ask for the key word.
void _upload_07(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 7;
    if (result != 1) {
        goto transfer;
    }
done:
    _job_type_clear(chan, agbUploadJobName07);
    AGBWorkArray[chan].status = AGB_STATUS_ERROR;
    return;
transfer:
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_08);
    if (AGBWorkArray[chan].result != 2) {
        return;
    }
    goto done;
}
