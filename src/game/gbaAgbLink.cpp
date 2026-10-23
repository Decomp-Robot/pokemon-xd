// The link library sits on the SI ports and drives the OS timers, so the SDK's
// OS and SI headers come first: they bring in the alarm and thread calls the
// exchanges block on, and the scalar types the game headers use as well.
#include <dolphin/os.h>
#include <dolphin/si.h>

#include <gbaAgb.h>
#include <gbaCommand.h>
#include <menuCBBios.h>
#include <types.h>

/*
 * The AGB link library of the GBA connection.
 *
 * The library owns the SI channels the Game Boy Advances answer on. A session
 * with one GBA runs in three steps: AGB_Prepare reads the code the GBA has to
 * run (the .jb file) into the file buffer, AGB_Upload starts the job that sends
 * that code and the Pokemon that goes with it out, and the link battle menus
 * hand their turn requests to the PKJB_ entry points until the session ends
 * with the return-title exchange.
 *
 * Every request drives the same exchange, so the library is built in layers:
 *
 *   _GBATimerRead  polls a channel and reads the word the GBA answers with.
 *   _SendRecv      writes one request word and reads its answer word.
 *   _BulkSend2     writes a block of the request out word by word.
 *   _InitialExchange  takes the channel over for a request: the GBA is reset,
 *                  the code it answers with is checked against the game code of
 *                  the running disc and the handshake word is exchanged.
 *   _AGB_ExchangeStart / _AGB_ExchangeEnd  the handshake word of a request and
 *                  the end of the exchange, _AGB_ReturnTitle the end of a
 *                  session.
 *
 * The PKJB_ entry points wrap those layers with the key layer of the game
 * (gbaCommandSetKeyState) and start an exchange whose handshake ran into its
 * timeout again, so the menus only see whether the turn they sent went through.
 *
 * The upload job the library starts runs as a chain of stage callbacks, like
 * the download job: this unit holds the stages 6 down to 00s, which check the
 * code the GBA answers the reset with, hand it the key of the packet coding and
 * reset it; the stages the packet exchange runs in are in
 * game/gbaPokemonTransferJobs.cpp.
 */

// Stage number of the polling variant of stage 00, which waits for the port to
// report a Game Boy Advance before the reset handshake goes out.
#define STAGE_UPLOAD_00S 10000

// Link status the upload waits for before it reads the code of the GBA: its
// code is up and it has put the answer word into the channel.
#define AGB_LINK_STATUS_RESET_ANSWER (AGB_LINK_STATUS_SYNC_READY | AGB_LINK_STATUS_DATA_READY)

// Command byte each exchange opens with: the exchange that ends a session, the
// upload of the code, the entry request that hands one Pokemon over, the battle
// request of one turn and the waza text.
#define AGB_COMMAND_END 0x44
#define AGB_COMMAND_LOAD 0x55
#define AGB_COMMAND_BATTLE 0x66
#define AGB_COMMAND_ENTRY 0x77
#define AGB_COMMAND_WAZA_TEXT 0x88

// Command byte of the exchange that leaves the title screen of the GBA, which
// is not opened by a handshake of its own: the word is written out on its own
// and the GBA answers it with the key its title screen sends.
#define AGB_COMMAND_RETURN_TITLE 0x60

// Word the entry-status exchange sends while the GBA has no answer ready yet.
#define AGB_ENTRY_STATUS_WAIT 0x11

// Length of the entry request the load exchange sends out (the request the
// command layer builds for the Pokemon it hands over) and the length of the
// waza text the battle exchange carries for one Pokemon.
#define AGB_ENTRY_REQUEST_SIZE 0x78
#define AGB_WAZA_TEXT_SIZE 0x780

// Milliseconds an exchange gives the GBA to answer one of its calls; the end of
// a session reads the answer of the GBA with the last of them.
#define AGB_EXCHANGE_MSEC 100
#define AGB_END_READ_MSEC 30000

// Seconds the handshake of an upload waits for the GBA to report itself, and
// the milliseconds _AGB_ExchangeIsPrepared leaves to the link between two polls.
#define AGB_UPLOAD_WAIT_SECS 7
#define AGB_PREPARED_SLEEP_MSEC 1

// Seconds the return-title exchange spends on the key the GBA answers with
// before it gives up on it, and the milliseconds the waza text waits for the
// code of the GBA.
#define AGB_RETURN_TITLE_SECS 3
#define AGB_WAZA_TEXT_MSEC 5

// Codes the primitives of the exchange report: the call ran through, the port
// does not carry a Game Boy Advance, the call the exchange made to the channel
// did not go through, and the word of the GBA could not be read. One code per
// step of an exchange that gave up is numbered on from these, and the steps of
// two exchanges are numbered one after the other, so their numbering can meet
// at the same value.
#define AGB_LINK_ANSWERED 0
#define AGB_LINK_NO_GBA 1
#define AGB_LINK_NOT_ANSWERING 2
#define AGB_LINK_READ 3

// Codes the two ready checks report on top of those: the GBA has no word of its
// own to hand over yet, so the caller comes back for it, or it answered with a
// word of its own instead of waiting for the request.
#define AGB_LINK_NOT_READY (-1)
#define AGB_LINK_WORD_HELD 4

// Steps of the reset exchange _InitialExchange runs before a session: the code
// of the GBA is reset and read back and the handshake word that opens the
// session is taken.
#define AGB_LINK_GAME_CODE 6          // the GBA answered with another game code.
#define AGB_LINK_GET_STATUS 7         // the status of the channel did not come.
#define AGB_LINK_WRITE 8              // the game code could not be written out.
#define AGB_LINK_READY_TIMEOUT 9      // the GBA did not take the game code.
#define AGB_LINK_READY_STATUS 0xA     // the status of that last check did not come.
#define AGB_LINK_HANDSHAKE_WRITE 0xB  // the handshake word could not be written.

// Steps of the session itself: a request goes out as a word that carries its
// command in its top byte and the GBA answers it, and the exchange ends with a
// word that tells whether the GBA still holds one of its own.
#define AGB_LINK_ANSWER 0xF           // the GBA answered another command.
#define AGB_LINK_WORD_LEFT 0x13       // the GBA still holds a word of the end.

// Codes the read of a word reports for itself: nothing of the GBA came before
// the time of the read ran out, the status of the channel did not come, or the
// word could not be read. An exchange reports them counted on from the code of
// the step the answer belongs to, so one code tells both the read and the call
// it ran into.
#define AGB_LINK_READ_TIMEOUT 1
#define AGB_LINK_READ_STATUS 2

// Codes of a word that is handed to the GBA and waited for, in the two calls
// that do it: the words of a block _BulkSend and _BulkSend2 write out and the
// request word _AGB_EntryRequest and _AGB_ReturnTitle wait for.
#define AGB_LINK_BLOCK_WRITE 0x10     // a word of the block could not be written.
#define AGB_LINK_BLOCK_TAKEN 0x11     // the GBA did not take one of them.
#define AGB_LINK_BLOCK_STATUS 0x12    // the status of a block word did not come.
#define AGB_LINK_WAIT_TIMEOUT 0x10    // the GBA did not take the word in time.
#define AGB_LINK_WAIT_STATUS 0x11     // the status of the word that waits did not come.

// Bytes a block exchange writes out before it hands the CPU on, so the polling
// of the other channels does not starve.
#define AGB_BLOCK_SWITCH 64

// Longest path AGB_Prepare accepts for the code file.
#define AGB_JB_PATH_MAX 0x7F

// Reads the code the GBA has to run out of the file the path names into dst and
// reports its length through size, rounded up to the transfer unit of the link.
long _ReadJBCode(const char *path, u8 *dst, unsigned long *size);

// Handler of the sleep alarm of _sleep_thread, which resumes the thread that
// suspended itself there.
extern "C" void agbSleepWakeThread(OSAlarm *alarm, OSContext *context);

// Hands the CPU on to the other threads of the game (the cooperative scheduler
// the link layer polls the other channels with).
extern "C" void GSthreadSwitch(void);

// Suspends the calling thread for the given number of milliseconds; the alarm
// the thread is woken by runs after them.
void _sleep_thread(int msec);

// The exchange layers the request wrappers of the battle menus build on: the
// handshake of a request, the two kinds of blocks a request is written out in,
// the word exchange the requests open with and the poll every read works on.
long _InitialExchange(long chan);
long _SendRecv(long chan, unsigned long data, unsigned long *result);
long _GBATimerRead(long chan, u8 *dst, u8 *status, int msec);
long _BulkSend(long chan, const u8 *data, int size);
long _BulkSend2(long chan, const u8 *data, int size);
long _AGB_ExchangeStart(long chan, u8 *data);
long _AGB_ExchangeEnd(long chan);
long _AGB_EntryRequest(long chan, unsigned long data);
long _AGB_BattleRequest(long chan, const u8 *data, int size);
long _AGB_ReturnTitle(long chan);
long _AGB_EntryGetStatus(long chan, unsigned long *data);
long _AGB_SendWazaText(long chan, u8 *data);

// Alarm and thread _sleep_thread blocks the calling thread with: the handler of
// the alarm resumes the thread that suspended itself. Both live in the data of
// the link library, so the wake handler of the alarm can reach the thread.
extern OSAlarm agbSleepAlarm;
extern OSThread *agbSleepThread;

// _upload_06(long, long): stage 6, starts the key exchange. The key the GBA is
// given is generated and written out as the packet word the GBA answers with
// its own key; a GBA whose code is not up yet waits for it instead.
void _upload_06(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 6;
    if (result != 1) {
        goto transfer;
    }
done:
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
    return;
transfer:
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_SYNC_STATE)
            != AGB_LINK_STATUS_SYNC_READY) {
        goto done;
    }
    if ((AGBWorkArray[chan].linkStatus[0] & AGB_LINK_STATUS_BUSY) != 0) {
        goto wait;
    }
    AGBWorkArray[chan].keyA = _PKJBUpGenerateKeyA();
    AGBWorkArray[chan].packet = SwapEndian(AGBWorkArray[chan].keyA);
    AGBWorkArray[chan].result = GBAWriteAsync(chan, &AGBWorkArray[chan].packet,
        AGBWorkArray[chan].linkStatus, _upload_07);
    if (AGBWorkArray[chan].result == 2) {
        _job_type_clear(chan, agbUploadJobName06);
        AGBWorkArray[chan].status = AGB_STATUS_ERROR;
        return;
    }
wait:
    if (OSGetTick() - AGBWorkArray[chan].tick > AGBWorkArray[chan].timeout) {
        goto done;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_06);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_05(long, long): stage 5, arms the stage timer of the word the GBA
// answers the key with and waits for the channel to report it.
void _upload_05(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 5;
    if (result != 1) {
        goto transfer;
    }
done:
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
    return;
transfer:
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_06);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_04(long, long): stage 4, gives the code the GBA answered the reset
// with back to it, so the GBA knows that its code was accepted.
void _upload_04(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 4;
    if (result != 1) {
        goto transfer;
    }
done:
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
    return;
transfer:
    if (AGBWorkArray[chan].linkStatus[0] != AGB_LINK_STATUS_SYNC_READY) {
        goto done;
    }
    AGBWorkArray[chan].result = GBAWriteAsync(chan, &AGBWorkArray[chan].keyA,
        AGBWorkArray[chan].linkStatus, _upload_05);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_03(long, long): stage 3, checks the code the GBA answered the reset
// with against the two GBA games the code runs on; any other answer leaves the
// channel idle until the poll timer of the stage runs out.
void _upload_03(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 3;
    if (result != 1) {
        goto transfer;
    }
done:
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
    return;
transfer:
    if (AGBWorkArray[chan].keyA == AGB_GAME_CODE_RUBY_JP
        || AGBWorkArray[chan].keyA == AGB_GAME_CODE_RUBY_US) {
        goto read;
    }
    if (OSGetTick() - AGBWorkArray[chan].pollTick < AGBWorkArray[chan].pollTimeout) {
        goto done;
    }
    _job_type_clear(chan, agbUploadJobName03);
    AGBWorkArray[chan].status = AGB_STATUS_NO_ANSWER;
    return;
read:
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_04);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_02(long, long): stage 2, reads the code the GBA answers the reset
// with once its code is up, and polls the channel until it is.
void _upload_02(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 2;
    if (result != 1) {
        goto transfer;
    }
done:
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
    return;
transfer:
    if (AGBWorkArray[chan].linkStatus[0] == AGB_LINK_STATUS_RESET_ANSWER) {
        AGBWorkArray[chan].result = GBAReadAsync(chan, &AGBWorkArray[chan].keyA,
            AGBWorkArray[chan].linkStatus, _upload_03);
        if (AGBWorkArray[chan].result == 2) {
            goto done;
        }
        return;
    }
    if (OSGetTick() - AGBWorkArray[chan].tick > AGBWorkArray[chan].timeout) {
        goto done;
    }
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_02);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_01(long, long): stage 1, arms the stage timer of the reset answer and
// waits for the channel to report it.
void _upload_01(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = 1;
    if (result != 1) {
        goto transfer;
    }
done:
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
    return;
transfer:
    _timer_set(chan, STAGE_TIMEOUT_MSEC);
    AGBWorkArray[chan].result =
        GBAGetStatusAsync(chan, AGBWorkArray[chan].linkStatus, _upload_02);
    if (AGBWorkArray[chan].result == 2) {
        goto done;
    }
}

// _upload_00s(long, long): stage 10000, resets the code of the GBA once its
// port reports a Game Boy Advance, and keeps the channel idle until it does or
// the poll timer of the stage runs out.
void _upload_00s(long chan, long result)
{
    if (AGBWorkArray[chan].active == 0) {
        return;
    }
    AGBWorkArray[chan].stage = STAGE_UPLOAD_00S;
    AGBWorkArray[chan].idleStage = AGB_IDLE_NONE;
    AGBWorkArray[chan].result = SIProbe(chan);
    if (AGBWorkArray[chan].result == AGB_SI_TYPE_GBA) {
        AGBWorkArray[chan].result =
            GBAResetAsync(chan, AGBWorkArray[chan].linkStatus, _upload_01);
        if (AGBWorkArray[chan].result == 2) {
            AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
        }
        return;
    }
    if (OSGetTick() - AGBWorkArray[chan].pollTick
            >= AGBWorkArray[chan].pollTimeout) {
        _job_type_clear(chan, agbUploadJobName00s);
        AGBWorkArray[chan].status = AGB_STATUS_NO_ANSWER;
        return;
    }
    AGBWorkArray[chan].idleStage = AGB_IDLE_UPLOAD_START;
}

// AGB_Upload(long, long): starts the upload job of a channel: the library takes
// the channel over and arms the poll timer the reset handshake runs with, then
// hands the code AGB_Prepare read to the first stage of the job, which sends it
// out together with the Pokemon the GBA is handed.
long AGB_Upload(long chan, long mode)
{
    if (AGB_Initialize(chan) == 0) {
        return 0;
    }
    _AGB_Work_Clear(chan);
    AGBWorkArray[chan].jobType = AGB_JOB_TYPE_UPLOAD;
    AGBWorkArray[chan].status = AGB_STATUS_RUNNING;
    AGBWorkArray[chan].active = 1;
    if (mode == AGB_UPLOAD_MODE_POKEMON) {
        AGBWorkArray[chan].pollTimeout = OSSecondsToTicks(AGB_UPLOAD_WAIT_SECS);
    } else {
        AGBWorkArray[chan].pollTimeout = OSSecondsToTicks(AGB_UPLOAD_WAIT_SECS);
    }
    AGBWorkArray[chan].pollTick = OSGetTick();
    AGBWorkArray[chan].size = gbaFileSize;
    AGBWorkArray[chan].buffer = gbaFileBuffer;
    gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_NONE);
    _upload_00s(chan, 0);
    return 1;
}

// AGB_Prepare(const char *path): reads the code the GBA has to run out of the
// file the path names and gets the link ready for the upload. The four bytes
// the GBA has to answer the exchange with are the game code of the running
// disc. Returns whether the code could be prepared.
long AGB_Prepare(const char *path)
{
    DVDDiskID *id;

    if (strlen(path) >= AGB_JB_PATH_MAX) {
        return 0;
    }
    strcpy(gbaJbFilePath, path);
    id = DVDGetCurrentDiskID();
    memcpy(&gbaGameId, id, 4);
    if (gbaLinkUninitialized) {
        GBAInit();
        gbaLinkUninitialized = 0;
    }
    _ReadJBCode(path, gbaFileBuffer, &gbaFileSize);
    return 1;
}

// _AGB_ExchangeEnd(long): ends the exchange of a session: the GBA is told that
// the GameCube ends it and the answer it gives is read.
long _AGB_ExchangeEnd(long chan)
{
    long result;
    u32 word;
    u8 status;
    long ret;

    result = _InitialExchange(chan);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    result = _SendRecv(chan, AGB_COMMAND_END, &word);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    if ((word >> 24) != AGB_COMMAND_END) {
        return AGB_LINK_ANSWER;
    }
    ret = _GBATimerRead(chan, (u8 *)&word, &status, AGB_END_READ_MSEC);
    if (ret != AGB_LINK_ANSWERED) {
        return ret + AGB_LINK_ANSWER;
    }
    if (word != 0) {
        return AGB_LINK_WORD_LEFT;
    }
    return AGB_LINK_ANSWERED;
}

// PKJB_ReturnTitle(long): ends the link session of a channel: the GBA is told
// that the GameCube returns to the title and the channel goes back to the key
// layer.
long PKJB_ReturnTitle(long chan)
{
    long port = chan + 1;
    long result;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    result = _AGB_ExchangeEnd(chan);
    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_READ);
    return result;
}

// _AGB_ExchangeIsPrepared(long): whether the GBA is ready to take the request
// of a session: its port is probed, the channel is asked whether it has an
// answer ready and the word is read. A report that is in and left nothing
// behind says the GBA waits for the request.
long _AGB_ExchangeIsPrepared(long chan)
{
    u32 word;
    u8 status;

    _sleep_thread(AGB_PREPARED_SLEEP_MSEC);
    if (menuCBBios_PortIDtoControlerID(chan) != AGB_SI_TYPE_GBA) {
        return AGB_LINK_NO_GBA;
    }
    if (GBAGetStatus(chan, &status)) {
        return AGB_LINK_NOT_ANSWERING;
    }
    if ((status & AGB_LINK_STATUS_DATA_READY) == 0) {
        return AGB_LINK_NOT_READY;
    }
    if (GBARead(chan, &word, &status)) {
        return AGB_LINK_READ;
    }
    if (word != 0) {
        return AGB_LINK_WORD_HELD;
    }
    return AGB_LINK_ANSWERED;
}

// PKJB_ExchangeIsPrepared(long): whether the exchange of the channel is ready
// to take its request. The channel goes back to the key layer as soon as the
// GBA answered, or when the exchange reported a failure.
long PKJB_ExchangeIsPrepared(long chan)
{
    long result = _AGB_ExchangeIsPrepared(chan);

    if (result == AGB_LINK_ANSWERED || result >= AGB_LINK_READ) {
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    }
    return result;
}

// _sleep_thread(int): suspends the calling thread for the given number of
// milliseconds. The alarm the thread is woken by runs the handler, which
// resumes it; the interrupt level of the caller is kept for that.
void _sleep_thread(int msec)
{
    BOOL level;

    agbSleepThread = OSGetCurrentThread();
    OSCreateAlarm(&agbSleepAlarm);
    level = OSDisableInterrupts();
    OSSetAlarm(&agbSleepAlarm, OSMillisecondsToTicks(msec), agbSleepWakeThread);
    OSSuspendThread(agbSleepThread);
    OSRestoreInterrupts(level);
}

// The handler of the sleep alarm: resumes the thread that suspended itself in
// _sleep_thread.
extern "C" void agbSleepWakeThread(OSAlarm *alarm, OSContext *context)
{
    OSResumeThread(agbSleepThread);
}

// _AGB_ExchangeStart(long, u8 *): starts the exchange of a request: the channel
// is taken over, the request is opened with its command word and the request
// itself is written out as a block.
long _AGB_ExchangeStart(long chan, u8 *data)
{
    u32 timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
    u32 start;
    u32 word;
    long result;

    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return AGB_LINK_NO_GBA;
        }
        if (menuCBBios_PortIDtoControlerID(chan) != AGB_SI_TYPE_GBA) {
            continue;
        }
        result = _InitialExchange(chan);
        if (result != AGB_LINK_ANSWERED) {
            return result;
        }
        result = _SendRecv(chan, AGB_COMMAND_LOAD, &word);
        if (result != AGB_LINK_ANSWERED) {
            return result;
        }
        if ((word >> 24) != AGB_COMMAND_LOAD) {
            return AGB_LINK_ANSWER;
        }
        _BulkSend(chan, data, AGB_ENTRY_REQUEST_SIZE);
        return AGB_LINK_ANSWERED;
    }
}

// PKJB_EntryRequest(long, void *): hands one Pokemon of the GameCube over. An
// exchange whose handshake ran into its timeout is started again; the channel
// goes back to the key layer once it is through.
long PKJB_EntryRequest(long chan, void *data)
{
    long port = chan + 1;
    long result;
    u32 timeout;
    u32 start;
    u32 elapsed;
    BOOL expired;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
    start = OSGetTick();
    do {
        elapsed = OSGetTick() - start;
        expired = elapsed > timeout;
        result = _AGB_ExchangeStart(chan, (u8 *)data);
    } while (result == AGB_LINK_NO_GBA && !expired);
    gbaCommandSetKeyState(port, result != AGB_LINK_ANSWERED
            ? GBA_KEY_COMMAND_READ : GBA_KEY_COMMAND_IDLE);
    return result;
}

// _BulkSend2(long, const u8 *, int): writes one block of a request out word by
// word. The GBA is given a timeout for every word, and the CPU is handed on
// between the blocks of a word, so the polling of the other channels runs on.
long _BulkSend2(long chan, const u8 *data, int size)
{
    u32 timeout;
    u32 start;
    u32 word;
    u8 status;
    int i;

    for (i = 0; i < size; i += 4) {
        word = *(unsigned long *)(data + i);
        if (GBAWrite(chan, &word, &status)) {
            return AGB_LINK_BLOCK_WRITE;
        }
        timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return AGB_LINK_BLOCK_TAKEN;
            }
            if (GBAGetStatus(chan, &status)) {
                return AGB_LINK_BLOCK_STATUS;
            }
            if ((status & AGB_LINK_STATUS_BUSY) == 0) {
                break;
            }
        }
        if ((i % AGB_BLOCK_SWITCH) == 0) {
            GSthreadSwitch();
        }
    }
    return AGB_LINK_ANSWERED;
}

// _BulkSend(long, const u8 *, int): writes one block of a request out word by
// word the way _BulkSend2 does, without handing the CPU on between the words.
long _BulkSend(long chan, const u8 *data, int size)
{
    u32 timeout;
    u32 start;
    u32 word;
    u8 status;
    int i;

    for (i = 0; i < size; i += 4) {
        word = *(unsigned long *)(data + i);
        if (GBAWrite(chan, &word, &status)) {
            return AGB_LINK_BLOCK_WRITE;
        }
        timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return AGB_LINK_BLOCK_TAKEN;
            }
            if (GBAGetStatus(chan, &status)) {
                return AGB_LINK_BLOCK_STATUS;
            }
            if ((status & AGB_LINK_STATUS_BUSY) == 0) {
                break;
            }
        }
    }
    return AGB_LINK_ANSWERED;
}

// _SendRecv(long, unsigned long, unsigned long *): writes one request word out
// and reads the word the GBA answers with.
long _SendRecv(long chan, unsigned long data, unsigned long *result)
{
    u8 status;
    long ret;

    if (GBAWrite(chan, &data, &status)) {
        return AGB_LINK_HANDSHAKE_WRITE;
    }
    ret = _GBATimerRead(chan, (u8 *)result, &status, AGB_EXCHANGE_MSEC);
    if (ret != AGB_LINK_ANSWERED) {
        return ret + AGB_LINK_HANDSHAKE_WRITE;
    }
    return AGB_LINK_ANSWERED;
}

// _GBATimerRead(long, u8 *, u8 *, int): polls the channel over the given number
// of milliseconds and reads the word the GBA answers with once it has one
// ready. Reports the code of the read.
long _GBATimerRead(long chan, u8 *dst, u8 *status, int msec)
{
    u32 timeout = OSMillisecondsToTicks(msec);
    u32 start = OSGetTick();
    u8 pollStatus;

    for (;;) {
        if (OSGetTick() - start > timeout) {
            return AGB_LINK_READ_TIMEOUT;
        }
        if (GBAGetStatus(chan, &pollStatus)) {
            return AGB_LINK_READ_STATUS;
        }
        if ((pollStatus & (AGB_LINK_STATUS_BUSY | AGB_LINK_STATUS_DATA_READY))
                != AGB_LINK_STATUS_DATA_READY) {
            continue;
        }
        if (GBARead(chan, dst, status)) {
            return AGB_LINK_READ;
        }
        return AGB_LINK_ANSWERED;
    }
}

// _InitialExchange(long): takes the channel over for a session with the GBA:
// its port is probed, its code is reset and asked for the game code of the
// running disc, and the handshake word that opens the exchange is written and
// taken.
long _InitialExchange(long chan)
{
    u8 status;
    u32 word;
    u32 timeout;
    u32 start;
    long result;

    if (menuCBBios_PortIDtoControlerID(chan) != AGB_SI_TYPE_GBA) {
        return AGB_LINK_NO_GBA;
    }
    if (GBAReset(chan, &status)) {
        return AGB_LINK_NOT_ANSWERING;
    }
    result = _GBATimerRead(chan, (u8 *)&word, &status, AGB_EXCHANGE_MSEC);
    if (result != AGB_LINK_ANSWERED) {
        return result + AGB_LINK_NOT_ANSWERING;
    }
    if (word != gbaGameId) {
        return AGB_LINK_GAME_CODE;
    }
    if (GBAGetStatus(chan, &status)) {
        return AGB_LINK_GET_STATUS;
    }
    if (GBAWrite(chan, &word, &status)) {
        return AGB_LINK_WRITE;
    }
    timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return AGB_LINK_READY_TIMEOUT;
        }
        if (GBAGetStatus(chan, &status)) {
            return AGB_LINK_READY_STATUS;
        }
        if ((status & AGB_LINK_STATUS_BUSY) == 0) {
            return AGB_LINK_ANSWERED;
        }
    }
}

// _AGB_ReturnTitle(long): returns to the title of the GBA: the exchange is
// taken over, the GBA is told to leave and the key it answers with is read
// until it has answered one.
long _AGB_ReturnTitle(long chan)
{
    u32 word;
    long written;
    u32 timeout;
    u32 start;
    u8 status;
    u16 key;
    long result;

    result = _InitialExchange(chan);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    word = AGB_COMMAND_RETURN_TITLE;
    written = GBAWrite(chan, &word, &status);
    if (written != 0) {
        return AGB_LINK_HANDSHAKE_WRITE;
    }
    timeout = OSSecondsToTicks(AGB_RETURN_TITLE_SECS);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return AGB_LINK_WAIT_TIMEOUT;
        }
        result = PKJB_GetKey(chan, &key);
        GSthreadSwitch();
        if (result == 0) {
            return written;
        }
    }
}

// PKJB_ReturnTitleWait(long): returns to the title and leaves the channel to
// the key layer once the GBA answered.
long PKJB_ReturnTitleWait(long chan)
{
    long port = chan + 1;
    long result;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    result = _AGB_ReturnTitle(chan);
    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_READ);
    return result;
}

// PKJB_BattleGetStatus(long, unsigned long *): the state the GBA reports for
// the running battle, read out of the answer of the entry exchange. A channel
// whose GBA answered goes back to the key layer.
long PKJB_BattleGetStatus(long chan, unsigned long *status)
{
    long result = _AGB_EntryGetStatus(chan, status);

    if (result >= 0) {
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    }
    return result;
}

// _AGB_EntryGetStatus(long, unsigned long *): reads the answer of the entry
// exchange. A GBA that has no answer ready yet is given the wait word and the
// caller is told to come back for it.
long _AGB_EntryGetStatus(long chan, unsigned long *data)
{
    u8 status;
    u32 wait;
    u32 word;

    if (menuCBBios_PortIDtoControlerID(chan) != AGB_SI_TYPE_GBA) {
        return AGB_LINK_NO_GBA;
    }
    if (GBAGetStatus(chan, &status)) {
        return AGB_LINK_NOT_ANSWERING;
    }
    if ((status & AGB_LINK_STATUS_DATA_READY) == 0) {
        wait = AGB_ENTRY_STATUS_WAIT;
        GBAWrite(chan, &wait, &status);
        return AGB_LINK_NOT_READY;
    }
    if (GBARead(chan, &word, &status)) {
        return AGB_LINK_READ;
    }
    *data = word;
    return AGB_LINK_ANSWERED;
}

// PKJB_EntryGetStatus(long, unsigned long *): the answer of the entry exchange
// of the channel.
long PKJB_EntryGetStatus(long chan, unsigned long *data)
{
    return _AGB_EntryGetStatus(chan, data);
}

// _AGB_BattleRequest(long, const u8 *, int): sends the request of one battle
// turn out: the exchange is opened with its command word and the packet behind
// it is written out as a block.
long _AGB_BattleRequest(long chan, const u8 *data, int size)
{
    u32 word;
    long result;

    result = _InitialExchange(chan);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    result = _SendRecv(chan, AGB_COMMAND_BATTLE, &word);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    if ((word >> 24) != AGB_COMMAND_BATTLE) {
        return AGB_LINK_ANSWER;
    }
    return _BulkSend2(chan, data, size);
}

// PKJB_BattleRequest(long, void *, long): sends the request of one battle turn
// out. An exchange whose handshake ran into its timeout is started again; the
// channel goes back to the key layer once it is through.
long PKJB_BattleRequest(long chan, const u8 *data, int size)
{
    long port = chan + 1;
    long result;
    u32 timeout;
    u32 start;
    u32 elapsed;
    BOOL expired;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
    start = OSGetTick();
    do {
        elapsed = OSGetTick() - start;
        expired = elapsed > timeout;
        result = _AGB_BattleRequest(chan, data, size);
    } while (result == AGB_LINK_NO_GBA && !expired);
    gbaCommandSetKeyState(port, result != AGB_LINK_ANSWERED
            ? GBA_KEY_COMMAND_READ : GBA_KEY_COMMAND_IDLE);
    return result;
}

// _AGB_EntryRequest(long, unsigned long): hands the request word of the entry
// exchange to the GBA and waits for it to take the word.
long _AGB_EntryRequest(long chan, unsigned long data)
{
    u32 answer;
    u32 request;
    u32 timeout;
    u32 start;
    u8 status;
    long result;

    result = _InitialExchange(chan);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    request = AGB_COMMAND_ENTRY;
    result = _SendRecv(chan, request, &answer);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    if ((answer >> 24) != AGB_COMMAND_ENTRY) {
        return AGB_LINK_ANSWER;
    }
    request = SwapEndian(data);
    GBAWrite(chan, &request, &status);
    timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return AGB_LINK_WAIT_TIMEOUT;
        }
        if (GBAGetStatus(chan, &status)) {
            return AGB_LINK_WAIT_STATUS;
        }
        if ((status & AGB_LINK_STATUS_BUSY) == 0) {
            return AGB_LINK_ANSWERED;
        }
    }
}

// PKJB_EntryRequestWait(long, unsigned long): hands the request word of the
// entry exchange over. An exchange whose handshake ran into its timeout is
// started again; the channel goes back to the key layer once it is through.
long PKJB_EntryRequestWait(long chan, unsigned long data)
{
    long port = chan + 1;
    long result;
    u32 timeout;
    u32 start;
    u32 elapsed;
    BOOL expired;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    timeout = OSMillisecondsToTicks(AGB_EXCHANGE_MSEC);
    start = OSGetTick();
    do {
        elapsed = OSGetTick() - start;
        expired = elapsed > timeout;
        result = _AGB_EntryRequest(chan, data);
    } while (result == AGB_LINK_NO_GBA && !expired);
    gbaCommandSetKeyState(port, result != AGB_LINK_ANSWERED
            ? GBA_KEY_COMMAND_READ : GBA_KEY_COMMAND_IDLE);
    return result;
}

// _AGB_SendWazaText(long, u8 *): sends the waza docs of a party out: the
// exchange is opened with its command word and the text is written out as a
// block. Reports the code of the exchange.
long _AGB_SendWazaText(long chan, u8 *data)
{
    u32 timeout = OSMillisecondsToTicks(AGB_WAZA_TEXT_MSEC);
    u32 start;
    u32 word;
    u32 elapsed;
    BOOL expired;
    long result;

    start = OSGetTick();
    do {
        elapsed = OSGetTick() - start;
        expired = elapsed > timeout;
        result = _InitialExchange(chan);
    } while (result == AGB_LINK_NO_GBA && !expired);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    result = _SendRecv(chan, AGB_COMMAND_WAZA_TEXT, &word);
    if (result != AGB_LINK_ANSWERED) {
        return result;
    }
    if ((word >> 24) != AGB_COMMAND_WAZA_TEXT) {
        return AGB_LINK_ANSWER;
    }
    return _BulkSend2(chan, data, AGB_WAZA_TEXT_SIZE);
}
