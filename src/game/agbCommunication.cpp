#include <gbaAgb.h>
#include <gbaCommand.h>
#include <GSmem.h>
#include <GSdvd.h>
#include <menuCBBios.h>

#define GBA_FILE_BUFFER_SIZE 0x20000
#define GBA_FILE_BUFFER_ALIGN 0x20

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


// Handler of the sleep alarm of _sleep_thread, which resumes the thread that
// suspended itself there.
static void _alarm_handler(OSAlarm *alarm, OSContext *context);

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
long _AGB_ExchangeIsPrepared(long chan);
long _AGB_ExchangeEnd(long chan);
long _AGB_EntryRequest(long chan, unsigned long data);
long _AGB_BattleRequest(long chan, const u8 *data, int size);
long _AGB_ReturnTitle(long chan);
long _AGB_EntryGetStatus(long chan, unsigned long *data);
long _AGB_SendWazaText(long chan, u8 *data);

// Number of SI channels a record of AGBWork is kept for.
#define AGB_CHANNEL_COUNT 4

// Byte of the word a boot key is looked up under, and the byte the key the GBA
// answers the key exchange with is checked against.
#define PKJB_BOOT_KEY_BYTE 0xBBu
#define PKJB_KEY_B_BYTE 0xEE

// Input-key request and the high byte of its response.
#define PKJB_INPUT_KEY_REQUEST 0xAA

// Code the GBA answers a word it could not make sense of with.
#define PKJB_WORD_INVALID 0xF

// Polynomial the rolling checksum of the packet coding shifts its bits out
// with, and the number of bits it is run over.
#define PKJB_CRC_POLYNOMIAL 0xA1C1
#define PKJB_CRC_BITS 32

// Number of bits the key the GBA answers with is checked over, and the number
// of set bits it is accepted with: too few or too many of them make a key easy
// to guess.
#define PKJB_KEY_B_BITS 24
#define PKJB_KEY_B_MIN_ONES 7
#define PKJB_KEY_B_MAX_ONES 0xE

// Same for the key the exchange hands the GBA.
#define PKJB_KEY_A_BITS 32
#define PKJB_KEY_A_MIN_ONES 10
#define PKJB_KEY_A_MAX_ONES 0x18

// Ticks a word of the key exchange is built out of: the word keeps a code of
// its own in its top byte and the ticks the GBA was reset at below it, so the
// keys of two sessions never come out the same.
#define PKJB_KEY_A_TICK_MASK 0xFFFFFFu
#define PKJB_KEY_A_TICK_CODE 0xDD00u

// Word _PKJBUpGenerateKeyA falls back to once the ticks it started with have
// not handed out a balanced word in time.
#define PKJB_KEY_A_FALLBACK 0xDD654321u
#define PKJB_KEY_A_FALLBACK_TRIES 0x20

// Number of boot keys _PKJBUpFindBootKey tries, the step it takes from one of
// them to the next, and the byte the key it finds starts with.
#define PKJB_BOOT_KEY_COUNT 0x100
#define PKJB_BOOT_KEY_STEP 0x1000000
#define PKJB_BOOT_KEY_FOUND 0xBB000000u
#define PKJB_BOOT_KEY_NONE 0

// Length of the code file _ReadJBCode reads, and the unit it rounds that
// length up to: the code is sent out in blocks of 32 bytes.
#define GBA_CODE_FILE_SIZE 0x20000
#define GBA_CODE_ALIGN 0x20

// Status AGB_Terminate leaves on a channel whose transfer was still running,
// the word the cached controller-type lookup reports for the port
// a GBA answers
// on, and the word an idle poke of the GBA sends.
#define AGB_STATUS_TERMINATED 4
#define GBA_PORT_ID 0x00040000
#define GBA_IDLE_POKE 0x11

// GBA header game code and checksum subtraction range.
#define GBA_IMAGE_GAME_CODE 0xAC
#define GBA_IMAGE_CHECKSUM_BEGIN 0xA0
#define GBA_IMAGE_CHECKSUM_INIT 0xE7
#define GBA_IMAGE_CHECKSUM_END 0xBD



void _AdjustGbaImageRegistry(u8 *image, char *gameId);

// Alarm and thread _sleep_thread blocks the calling thread with: the handler of
// the alarm resumes the thread that suspended itself. Both live in the data of
// the link library, so the wake handler of the alarm can reach the thread.
extern OSAlarm agbSleepAlarm;
extern OSThread *agbSleepThread;

extern "C" void pokeconv(void *record, void *packet);

long AGB_Initialize(long chan)
{
    if (chan < 0 || chan > AGB_CHANNEL_COUNT - 1) {
        return 0;
    }
    if (AGBWorkArray[chan].jobType != 0) {
        return 0;
    }
    _AGB_Work_Clear(chan);
    AGBWorkArray[chan].jobType = 0;
    return 1;
}

extern "C" void AGB_Terminate_init(void)
{
    long chan;

    for (chan = 0; chan < AGB_CHANNEL_COUNT; chan++) {
        _AGB_Work_Clear(chan);
        _job_type_clear(chan, __FUNCTION__);
    }
}

extern "C" long AGB_Terminate(long chan)
{
    if (chan < 0 || chan > AGB_CHANNEL_COUNT - 1) {
        return 0;
    }
    AGBWorkArray[chan].active = 0;
    AGBWorkArray[chan].status = 0;
    AGBWorkArray[chan].idleStage = 0;
    if (AGBWorkArray[chan].jobType != 0) {
        AGBWorkArray[chan].status = AGB_STATUS_TERMINATED;
    }
    _job_type_clear(chan, __FUNCTION__);
    return 1;
}

extern "C" long AGB_GetStatus(long chan)
{
    if (chan < 0 || chan > AGB_CHANNEL_COUNT - 1) {
        return AGB_STATUS_ERROR;
    }
    return AGBWorkArray[chan].status;
}

extern "C" long AGB_IsComplete(long chan)
{
    if (chan < 0 || chan > AGB_CHANNEL_COUNT - 1) {
        return 0;
    }
    if (AGBWorkArray[chan].jobType != 0) {
        switch (AGBWorkArray[chan].idleStage) {
        case AGB_IDLE_UPLOAD_START:
            _upload_00s(chan, 0);
            break;
        case AGB_IDLE_RESET_16S0:
            _upload_16s0(chan, 0);
            break;
        case AGB_IDLE_RESET_16S:
            _upload_16s(chan, 0);
            break;
        }
    }
    return AGBWorkArray[chan].jobType == 0;
}

void _AGB_Work_Clear(long chan)
{
    AGBWorkArray[chan].packet = 0;
    AGBWorkArray[chan].syncCode = 0;
    AGBWorkArray[chan].key = 0;
    AGBWorkArray[chan].checksum = 0;
    AGBWorkArray[chan].linkWord = 0;
    AGBWorkArray[chan].keyB = 0;
    AGBWorkArray[chan].bootKey = 0;
    AGBWorkArray[chan].keyA = 0;
    AGBWorkArray[chan].request = 0;
    AGBWorkArray[chan].reply = 0;
    AGBWorkArray[chan].period = 0;
    AGBWorkArray[chan].reserved48 = 0;
    AGBWorkArray[chan].reserved4C = 0;
    AGBWorkArray[chan].reserved50 = 0;
    AGBWorkArray[chan].reserved54 = 0;
    AGBWorkArray[chan].reserved58 = 0;
    AGBWorkArray[chan].tick = 0;
    AGBWorkArray[chan].timeout = 0;
    AGBWorkArray[chan].pollTick = 0;
    AGBWorkArray[chan].pollTimeout = 0;
    AGBWorkArray[chan].resetTick = 0;
    AGBWorkArray[chan].resetTimeout = 0;
    AGBWorkArray[chan].reserved84 = 0;
    AGBWorkArray[chan].reserved88 = 0;
    AGBWorkArray[chan].offset = 0;
    AGBWorkArray[chan].codedWord = 0;
    AGBWorkArray[chan].plainWord = 0;
    AGBWorkArray[chan].reserved68 = 0;
    AGBWorkArray[chan].active = 0;
    AGBWorkArray[chan].status = 0;
    AGBWorkArray[chan].idleStage = 0;
}

void _job_type_clear(long chan, const char *jobName)
{
    AGBWorkArray[chan].active = 0;
    AGBWorkArray[chan].jobType = 0;
    AGBWorkArray[chan].idleStage = 0;
}

void _timer_set(long chan, unsigned long msec)
{
    AGBWorkArray[chan].timeout = msec * (OS_TIMER_CLOCK / 1000);
    AGBWorkArray[chan].tick = OSGetTick();
}

unsigned long SwapEndian(unsigned long value)
{
    return ((value >> 24) & 0xFF) | ((value >> 8) & 0xFF00) |
           ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
}

unsigned long _PKJBUpGenerateKeyA(void)
{
    unsigned long tries = 0;
    unsigned long word;
    unsigned long key;
    int bit;
    unsigned long ones;

    for (;;) {
        if (tries > PKJB_KEY_A_FALLBACK_TRIES) {
            key = PKJB_KEY_A_FALLBACK;
        }
        else {
            key = (OSGetTick() & PKJB_KEY_A_TICK_MASK) | (PKJB_KEY_A_TICK_CODE << 16);
        }
        word = key;
        ones = 0;
        for (bit = 0; bit < PKJB_KEY_A_BITS; bit++) {
            if (word & 1) {
                ones++;
            }
            word >>= 1;
        }
        tries++;
        if (ones < PKJB_KEY_A_MIN_ONES) {
            continue;
        }
        if (ones > PKJB_KEY_A_MAX_ONES) {
            continue;
        }
        return key;
    }
}

u32 _PKJBUpValidateKeyB(u32 key)
{
    u32 keyB;
    int bit;
    unsigned long ones;

    // The exchange tags its answer with a byte of its own in the low byte of
    // the word; the truncation brings that byte out for the compare.
    if ((u8)key != PKJB_KEY_B_BYTE) {
        OSReport("_PKJBUpValidateKeyB: invalid key (d7-d0): 0x%x\n", key);
        return 0;
    }
    // The check runs over the word without its tag byte.
    keyB = key & ~0xFF;
    ones = 0;
    for (bit = 0; bit < PKJB_KEY_B_BITS; bit++) {
        if (key & 0x80000000u) {
            ones++;
        }
        key <<= 1;
    }
    if (ones < PKJB_KEY_B_MIN_ONES || ones > PKJB_KEY_B_MAX_ONES) {
        OSReport("_PKJBUpValidateKeyB: invalid key (d31-d8): 0x%x\n", keyB);
        return 0;
    }
    return keyB;
}

void _AdjustGbaImageRegistry(unsigned char *image, char *gameId)
{
    long i;
    unsigned int checksum = GBA_IMAGE_CHECKSUM_INIT;

    image[GBA_IMAGE_GAME_CODE + 0] = gameId[0];
    image[GBA_IMAGE_GAME_CODE + 1] = gameId[1];
    image[GBA_IMAGE_GAME_CODE + 2] = gameId[2];
    image[GBA_IMAGE_GAME_CODE + 3] = gameId[3];
    i = GBA_IMAGE_CHECKSUM_BEGIN;
    while (i < GBA_IMAGE_CHECKSUM_END) {
        checksum -= image[i];
        i++;
    }
    image[i] = (u8)checksum;
}

void _ReadJBCode(const char *path, u8 *dst, unsigned long *size)
{
    GSdvdFile *file;

    file = GSdvdOpen(path);
    if (file == 0) {
        return;
    }
    *size = GSdvdGetLength(file);
    if (*size == 0 || *size > GBA_CODE_FILE_SIZE) {
        return;
    }
    *size = (*size + (GBA_CODE_ALIGN - 1)) & ~(GBA_CODE_ALIGN - 1);
    if (GSdvdReadAsync(file, dst, *size, 0, 0) == 0) {
        return;
    }
    do {
        GSthreadSwitch();
    } while (GSdvdGetFileInfoStatus(file) != 0);
    GSdvdClose(file);
    _AdjustGbaImageRegistry(dst, (char *)&gbaGameId);
}

unsigned long _PKJBUpCalcCrc(unsigned long word, unsigned long checksum)
{
    unsigned long crc = word ^ checksum;
    int bits;

    for (bits = PKJB_CRC_BITS; bits > 0; bits--) {
        if (crc & 1) {
            crc >>= 1;
            crc ^= PKJB_CRC_POLYNOMIAL;
        }
        else {
            crc >>= 1;
        }
    }
    return crc;
}

unsigned long _PKJBUpFindBootKey(unsigned long key, unsigned long checksum)
{
    unsigned long candidate;
    unsigned long keyByte;
    unsigned long tries;
    unsigned long bootKey;

    OSReport("from GBA recvData = 0x%08x, GC resultCrc = 0x%08x\n", key, checksum);
    keyByte = key >> 8;
    for (tries = 0, candidate = 0xFFFFFFFFUL; tries < PKJB_BOOT_KEY_COUNT; tries++) {
        if (_PKJBUpCalcCrc(candidate, checksum) == keyByte) {
            bootKey = _PKJBUpCalcCrc(candidate, PKJB_BOOT_KEY_BYTE) | PKJB_BOOT_KEY_FOUND;
            OSReport("KeyC found! 0x%08x, return bootKey = 0x%08x\n", candidate, bootKey);
            return bootKey;
        }
        candidate = candidate + PKJB_BOOT_KEY_STEP;
    }
    OSReport("*************** ERROR: KeyC NOT FOUND 0x%08x, 0x%08x ***************\n", key, checksum);
    return PKJB_BOOT_KEY_NONE;
}

extern "C" long PKJB_GetKey(long chan, u16 *key)
{
    unsigned long answer;
    long result;

    result = _InitialExchange(chan);
    if (result != 0) {
        return result;
    }
    result = _SendRecv(chan, PKJB_INPUT_KEY_REQUEST, &answer);
    if (result != 0) {
        return result;
    }
    if ((answer >> 24) != PKJB_INPUT_KEY_REQUEST) {
        return PKJB_WORD_INVALID;
    }
    *key = SwapEndian(answer) >> 16;
    return 0;
}

extern "C" long PKJB_IdleSend(long chan)
{
    u8 status;
    unsigned long poke;
    unsigned long id;

    id = gba_SIProbe(chan);
    if (id - GBA_PORT_ID) {
        return 1;
    }
    poke = GBA_IDLE_POKE;
    if (!GBAWrite(chan, &poke, &status)) {
        return 0;
    }
    return 2;
}

extern "C" long PKJB_SendWazaText(long chan, void *data)
{
    long keyChan = chan + 1;
    long result;

    gbaCommandSetKeyState(keyChan, GBA_KEY_COMMAND_TRANSFER);
    result = _AGB_SendWazaText(chan, (u8 *)data);
    gbaCommandSetKeyState(keyChan, GBA_KEY_COMMAND_READ);
    return result;
}

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

long PKJB_EntryRequest(long chan, unsigned long data)
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

long PKJB_EntryGetStatus(long chan, unsigned long *data)
{
    return _AGB_EntryGetStatus(chan, data);
}

long _AGB_EntryGetStatus(long chan, unsigned long *data)
{
    u8 status;
    u32 wait;
    u32 word;

    if (gba_SIProbe(chan) != AGB_SI_TYPE_GBA) {
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

long PKJB_BattleGetStatus(long chan, unsigned long *status)
{
    long result = _AGB_EntryGetStatus(chan, status);

    if (result >= 0) {
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    }
    return result;
}

long PKJB_ReturnTitle(long chan)
{
    long port = chan + 1;
    long result;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    result = _AGB_ReturnTitle(chan);
    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_READ);
    return result;
}

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

long _InitialExchange(long chan)
{
    u8 status;
    u32 word;
    u32 timeout;
    u32 start;
    long result;

    if (gba_SIProbe(chan) != AGB_SI_TYPE_GBA) {
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

long PKJB_ExchangeStart(long chan, void *data)
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
        if (gba_SIProbe(chan) != AGB_SI_TYPE_GBA) {
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

static void _alarm_handler(OSAlarm *alarm, OSContext *context)
{
    OSResumeThread(agbSleepThread);
}

void _sleep_thread(int msec)
{
    BOOL level;

    agbSleepThread = OSGetCurrentThread();
    OSCreateAlarm(&agbSleepAlarm);
    level = OSDisableInterrupts();
    OSSetAlarm(&agbSleepAlarm, OSMillisecondsToTicks(msec), _alarm_handler);
    OSSuspendThread(agbSleepThread);
    OSRestoreInterrupts(level);
}

long PKJB_ExchangeIsPrepared(long chan)
{
    long result = _AGB_ExchangeIsPrepared(chan);

    if (result == AGB_LINK_ANSWERED || result >= AGB_LINK_READ) {
        gbaCommandSetKeyState(chan + 1, GBA_KEY_COMMAND_READ);
    }
    return result;
}

long _AGB_ExchangeIsPrepared(long chan)
{
    u32 word;
    u8 status;

    _sleep_thread(AGB_PREPARED_SLEEP_MSEC);
    if (gba_SIProbe(chan) != AGB_SI_TYPE_GBA) {
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

long PKJB_ExchangeEnd(long chan)
{
    long port = chan + 1;
    long result;

    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_TRANSFER);
    result = _AGB_ExchangeEnd(chan);
    gbaCommandSetKeyState(port, GBA_KEY_COMMAND_READ);
    return result;
}

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

extern "C" void AGB_FileBuffAlloc(void)
{
    gbaFileBuffer = (u8 *)GSmemAllocAlign(GBA_FILE_BUFFER_SIZE, GBA_FILE_BUFFER_ALIGN);
}

extern "C" void AGB_FileBuffFree(void)
{
    GSmemFree(gbaFileBuffer);
    gbaFileBuffer = 0;
}
