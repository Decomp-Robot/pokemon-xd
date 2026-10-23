#include <dolphin/os.h>
#include <dolphin/si.h>
#include <gbaAgb.h>
#include <gbaCommand.h>
#include <types.h>
#include <GSdvd.h>
#include <GSmem.h>

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
void _ReadJBCode(const char *path, u8 *dst, unsigned long *size);

// Hands the CPU on to the other threads of the game (the cooperative scheduler
// the link layer polls the other channels with).
extern "C" void GSthreadSwitch(void);

// Exchange layers the entry points of this unit build on. They live in the
// integrated link TUs, as do the upload resume stages AGB_IsComplete runs
// idle channels on; the SI probe the idle poke reads is the cached
// controller-type lookup of the integrated battle BIOS module.
long _InitialExchange(long chan);
long _SendRecv(long chan, unsigned long data, unsigned long *result);
long _BulkSend2(long chan, const u8 *data, int size);
long _AGB_SendWazaText(long chan, u8 *data);
void _upload_00s(long chan, long result);
void _upload_16s0(long chan, long result);
void _upload_16s(long chan, long result);

extern "C" u32 menuCBBios_PortIDtoControlerID(long portID);

// Number of SI channels a record of AGBWork is kept for.
#define AGB_CHANNEL_COUNT 4

// Module storage of the link library (job records, code path, packet scratch
// buffers, sleep alarm, file buffer, game code, sleeping thread) is owned by
// the integrated link TUs under the shared names of include/gbaAgb.h; this
// unit only runs the key, code and exchange entry points on it.

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


// AGB_Initialize(long): takes a channel over for a transfer, which only goes
// through while no other job is running on it.
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

// AGB_Terminate_init(void): gives all four channels back.
extern "C" void AGB_Terminate_init(void)
{
    long chan;

    for (chan = 0; chan < AGB_CHANNEL_COUNT; chan++) {
        _AGB_Work_Clear(chan);
        _job_type_clear(chan, __FUNCTION__);
    }
}

// AGB_Terminate(long): gives the channel of a transfer back, and reports that a
// transfer which was still running is over with an error.
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

// AGB_GetStatus(long): reports how the transfer of a channel ended, or that the
// channel it was asked for does not exist.
extern "C" long AGB_GetStatus(long chan)
{
    if (chan < 0 || chan > AGB_CHANNEL_COUNT - 1) {
        return AGB_STATUS_ERROR;
    }
    return AGBWorkArray[chan].status;
}

// AGB_IsComplete(long): runs the step a channel that was left idle has to
// resume on, and reports whether the channel is free again afterwards.
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

// Clear transfer state, leaving the channel free for the next job.
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

// _job_type_clear(long, const char *): reports that a stage leaves the channel
// behind and takes it out of the job it ran.
void _job_type_clear(long chan, const char *jobName)
{
    AGBWorkArray[chan].active = 0;
    AGBWorkArray[chan].jobType = 0;
    AGBWorkArray[chan].idleStage = 0;
}

// _timer_set(long, unsigned long): arms the stage timer of a channel from a
// length in milliseconds and starts it at the current tick.
void _timer_set(long chan, unsigned long msec)
{
    AGBWorkArray[chan].timeout = msec * (OS_TIMER_CLOCK / 1000);
    AGBWorkArray[chan].tick = OSGetTick();
}

// SwapEndian(unsigned long): turns the byte order of the GBA into the one of the
// GameCube, which is how the words that cross the link are coded.
unsigned long SwapEndian(unsigned long value)
{
    return ((value >> 24) & 0xFF) | ((value >> 8) & 0xFF00) |
           ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
}

// _PKJBUpGenerateKeyA(void): spins until the ticks of the GBA hand out a word
// with a balanced number of set bits; that word is the key the exchange codes
// the packets with.
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

// _PKJBUpValidateKeyB(unsigned long): checks the key the GBA answers the key
// exchange with. The key has to be tagged with the byte of the exchange and to
// carry a balanced number of set bits; the key without that byte is handed back
// for the coding of the packets.
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

// _AdjustGbaImageRegistry(unsigned char *, char *): stamps the game code of the
// disc into the header of the image of the GBA and checksums the header with
// it, so the GBA can tell whether the code it was handed runs on this game.
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

// _ReadJBCode(const char *, u8 *, unsigned long *): reads the code the GBA has
// to run out of the disc into the file buffer of the link. The length is handed
// back rounded up to the transfer unit of the link, and the game code of the
// disc is stamped into the header of the image.
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

// _PKJBUpCalcCrc(unsigned long, unsigned long): shifts the bits of the checksum
// out of the word, taking the polynomial in for every bit that was set.
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

// _PKJBUpFindBootKey(unsigned long, unsigned long): looks the boot key of the
// GBA up out of the checksum its code answered the reset of the upload with.
// The boot keys are one step apart, and the key that comes with one of them is
// handed back with its own byte in the top half of the word.
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

// Read the GBA input keys. Reject responses with an unexpected command byte.
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

// PKJB_IdleSend(long): pokes a GBA that sits on the port in its idle loop, so
// it notices that a link battle is about to start. Reports 1 while the port does
// not carry a GBA, 2 when the poke did not go through, and 0 when it did.
extern "C" long PKJB_IdleSend(long chan)
{
    u8 status;
    unsigned long poke;
    unsigned long id;

    id = menuCBBios_PortIDtoControlerID(chan);
    if (id - GBA_PORT_ID) {
        return 1;
    }
    poke = GBA_IDLE_POKE;
    if (!GBAWrite(chan, &poke, &status)) {
        return 0;
    }
    return 2;
}

// PKJB_SendWazaText(long, void *): sends the waza docs of a party out to the
// GBA of a link battle. The key layer of the game is set to the state a write
// takes, the exchange of the waza text runs, and the key goes back to the state
// a transfer waits in.
extern "C" long PKJB_SendWazaText(long chan, void *data)
{
    long keyChan = chan + 1;
    long result;

    gbaCommandSetKeyState(keyChan, GBA_KEY_COMMAND_TRANSFER);
    result = _AGB_SendWazaText(chan, (u8 *)data);
    gbaCommandSetKeyState(keyChan, GBA_KEY_COMMAND_READ);
    return result;
}

