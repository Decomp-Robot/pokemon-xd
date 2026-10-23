#ifndef GBA_AGB_H
#define GBA_AGB_H

// The job record of the link layer holds OS ticks and the stages are driven by
// the OS timers, so the SDK's OS header comes first: it brings in the SDK's
// scalar types, which the game headers use as well.
#include <dolphin/os.h>
#include <dolphin/si.h>

#include <types.h>

/*
 * The AGB link layer of the GBA connection.
 *
 * A Pokemon transfer is one job per SI channel: AGB_GetPokemon starts the
 * download of a Pokemon (the GBA sends it over), AGB_Upload starts the upload
 * of the code the GBA has to run (the .jb file AGB_Prepare read into the file
 * buffer) together with the Pokemon the GBA is handed.
 *
 * A job runs as a chain of stage callbacks: each stage starts one link call and
 * hands the next stage to the link library, which calls that stage back once
 * the call has finished. The stages with an "s" in their name poll
 * GBAGetStatusAsync while the channel is not ready yet, the "s0" variant polls
 * the reset handshake of the GBA.
 *
 * All stages share one job record per channel (AGBWorkArray). The download half
 * assembles the received packet in the receive buffer of the channel
 * (agbRecvBuffers), which stage 9 points the record at; the upload half sends
 * the file buffer (gbaFileBuffer) out word by word, coded with the rolling key
 * and checksum the two sides agree on during the key exchange.
 */

// Stage numbers of the polling variants of a stage: the "s" variants run as
// 1000 + the stage, the "s0" variants as 200 + the stage.
#define STAGE_POLL_BASE 1000
#define STAGE_POLL0_BASE 200

// Milliseconds a stage waits for the link channel before it gives up on it, and
// the period the two halves poll the channel with: 100 ms while the GBA is not
// ready for a transfer yet, 5 ms while packet words are exchanged.
#define STAGE_TIMEOUT_MSEC 125
#define STAGE_POLL_MSEC 100
#define PACKET_POLL_MSEC 5

// Size of the packet a channel exchanges: the received packet is assembled in
// one scratch buffer of this size per channel (agbRecvBuffers).
#define AGB_PACKET_SIZE 0x278
// Length of the header the packet of an upload starts with: those words are
// sent as they are, the payload behind them is coded.
#define AGB_PACKET_HEADER_SIZE 0xA0

// Command byte of the exchanges, which the GBA echoes in the header of its
// reply: 0x99 for the download of a Pokemon, 0xAA for the upload.
#define AGB_DOWNLOAD_COMMAND 0x99
#define AGB_UPLOAD_COMMAND 0xAA

// Kind byte (the top byte of the word) the GBA answers with while the key
// exchange runs: it asks for the boot key, or reports that the exchange goes on
// with the packet that was sent.
#define AGB_WORD_KIND_BOOT_KEY 0xFF
#define AGB_WORD_KIND_ACCEPTED 0xCC

// Bits of the channel status byte the link library reports for a channel. The
// GBA sets the ready bit once it has put a word into the receive register of
// the channel, and keeps the busy bit set while it is not ready for the next
// word: the reset handshake the link library runs for the channel waits for the
// busy bit to clear before it exchanges a word.
#define AGB_LINK_STATUS_BUSY 0x02
#define AGB_LINK_STATUS_DATA_READY 0x08
// Bits 4 and 5 carry the state code of the exchange: the GBA reports that its
// code is up and waits for the transfer (0x10), that it is in the packet
// exchange (0x30, the code the first packet word is tagged with) or that it
// took the last word of the packet (0x20).
#define AGB_LINK_STATUS_SYNC_STATE 0x30
#define AGB_LINK_STATUS_SYNC_READY 0x10
#define AGB_LINK_STATUS_SYNC_TAKEN 0x20
#define AGB_LINK_STATUS_SYNC_PACKET 0x30

// State code a packet word is tagged with, and the step that tag is advanced by
// after every word: stage 12 checks the tag the GBA reports back against it.
#define AGB_SYNC_CODE_PACKET 0x30
#define AGB_SYNC_CODE_STEP 0x10

// Multiplier the rolling key of the packet coding advances with.
#define AGB_KEY_MULTIPLIER 0x6177614B

// Result SIProbe reports for a port that carries a Game Boy Advance.
#define AGB_SI_TYPE_GBA 0x00040000

// Jobs a channel runs (the jobType of its record).
#define AGB_JOB_TYPE_UPLOAD 2
#define AGB_JOB_TYPE_GET_POKEMON 4

// Codes of the resume step the job framework runs for an idle channel: the
// channel is put back to the start of the upload chain, or to one of the two
// stages that complete the GBA reset handshake.
#define AGB_IDLE_NONE 0
#define AGB_IDLE_UPLOAD_START 1
#define AGB_IDLE_RESET_16S0 2
#define AGB_IDLE_RESET_16S 3

// Status of a job AGB_GetStatus reports: the transfer it ran is in, it ended in
// error, the GBA it ran on stopped answering (the poll timers of the reset
// handshake ran out), or it is still running.
#define AGB_STATUS_COMPLETE 0
#define AGB_STATUS_ERROR 1
#define AGB_STATUS_NO_ANSWER 2
#define AGB_STATUS_RUNNING 3

// Game codes the code of the GBA is run on: the upload of the code accepts the
// Japanese and the American Pokemon Ruby only, which answer the reset of the
// upload with their code (the characters of the code are the value).
#define AGB_GAME_CODE_RUBY_JP 0x4158564A // "AXVJ"
#define AGB_GAME_CODE_RUBY_US 0x41585645 // "AXVE"

// The upload a link battle runs: the code is sent out together with the Pokemon
// the GameCube hands the GBA, and both modes arm the same poll timer for it.
#define AGB_UPLOAD_MODE_POKEMON 0

// Dolphin SDK Game Boy Advance link library (dolphin/gba).
typedef void (*GBACallback)(long chan, long result);

extern "C" long GBAGetStatusAsync(long chan, u8 *status, GBACallback callback);
extern "C" long GBAReadAsync(long chan, void *dst, u8 *status, GBACallback callback);
extern "C" long GBAWriteAsync(long chan, void *src, u8 *status, GBACallback callback);
extern "C" long GBAResetAsync(long chan, u8 *status, GBACallback callback);

// Blocking forms of the same calls, which wait for the GBA to answer and report
// whether it did; they are what the exchanges of the link library are built on.
extern "C" {
BOOL GBAInit(void);
BOOL GBAGetStatus(long chan, u8 *status);
BOOL GBARead(long chan, void *dst, u8 *status);
BOOL GBAWrite(long chan, const void *src, u8 *status);
BOOL GBAReset(long chan, u8 *status);
}

// Job framework and channel initialization.
void _timer_set(long chan, unsigned long msec);
void _job_type_clear(long chan, const char *jobName);
void _AGB_Work_Clear(long chan);
long AGB_Initialize(long chan);

// Reads the code the GBA has to run (the .jb file) into the file buffer of the
// link and reports the length it read through size, rounded up to the transfer
// unit of the link.
void _ReadJBCode(const char *path, u8 *dst, unsigned long *size);

// Code and key helpers of the link library the upload exchange is built on.
unsigned long SwapEndian(unsigned long value);
unsigned long _PKJBUpGenerateKeyA(void);
unsigned long _PKJBUpFindBootKey(unsigned long key, unsigned long checksum);
unsigned long _PKJBUpCalcCrc(unsigned long checksum, unsigned long word);
unsigned long _PKJBUpValidateKeyB(unsigned long key);

#ifdef __cplusplus
extern "C" {
#endif

// Entry points the game starts a transfer with.
long AGB_GetPokemon(long chan, void *record, unsigned long *dest);
long AGB_Upload(long chan, long mode);
long AGB_Prepare(const char *path);

// The request entry points of the link library the battle menus drive. Every
// one of them takes the channel over, runs its exchange and reports the code of
// the exchange (0 when the GBA answered); the caller polls the exchange with
// the matching status entry point until it is through.
long PKJB_ExchangeEnd(long chan);
long PKJB_ReturnTitle(long chan);
long PKJB_ExchangeIsPrepared(long chan);
long PKJB_ExchangeStart(long chan, void *data);
long PKJB_EntryRequest(long chan, unsigned long data);
long PKJB_EntryGetStatus(long chan, unsigned long *data);
long PKJB_BattleGetStatus(long chan, unsigned long *status);
long PKJB_BattleRequest(long chan, const u8 *data, int size);
long PKJB_GetKey(long chan, u16 *key);
long PKJB_SendWazaText(long chan, void *data);
long PKJB_IdleSend(long chan);
void AGB_FileBuffFree(void);
void AGB_FileBuffAlloc(void);

#ifdef __cplusplus
}
#endif

// The four bytes the AGB code of the GBA answers with: AGB_Prepare copies the
// game code of the running disc out of the disc ID into it, and both exchanges
// check that the GBA answers with it.
extern unsigned long gbaGameId;

// Name each stage reports to the link job list when it releases the channel,
// kept in the string pool of the link library.
extern char agbJobName01[];   // "_getpokemon_01"
extern char agbJobName01s[];  // "_getpokemon_01s"
extern char agbJobName02[];   // "_getpokemon_02"
extern char agbJobName03[];   // "_getpokemon_03"
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
extern char agbJobNameGetPokemon[]; // "AGB_GetPokemon"

extern char agbUploadJobName00s[];  // "_upload_00s"
extern char agbUploadJobName03[];   // "_upload_03"
extern char agbUploadJobName06[];   // "_upload_06"
extern char agbUploadJobName07[];   // "_upload_07"
extern char agbUploadJobName08[];   // "_upload_08"
extern char agbUploadJobName09[];   // "_upload_09"
extern char agbUploadJobName10[];   // "_upload_10"
extern char agbUploadJobName11[];   // "_upload_11"
extern char agbUploadJobName11s[];  // "_upload_11s"
extern char agbUploadJobName12[];   // "_upload_12"
extern char agbUploadJobName12s[];  // "_upload_12s"
extern char agbUploadJobName13[];   // "_upload_13"
extern char agbUploadJobName13s[];  // "_upload_13s"
extern char agbUploadJobName14[];   // "_upload_14"
extern char agbUploadJobName15[];   // "_upload_15"
extern char agbUploadJobName16[];   // "_upload_16"
extern char agbUploadJobName16s[];  // "_upload_16s"
extern char agbUploadJobName16s0[]; // "_upload_16s0"
extern char agbUploadJobName25[];   // "_upload_25"

// Stages of the download job, each one hands the channel on to the stage that
// follows it in the chain. The second argument is the result the link library
// hands to its callback: 1 ends the job, anything else leaves the stage free to
// pass the next link call to the channel.
void _getpokemon_01(long chan, long result);
void _getpokemon_01s(long chan, long result);
void _getpokemon_02(long chan, long result);
void _getpokemon_03(long chan, long result);
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

// Stages of the upload job. The stages 00s..6 check the code the GBA answers
// the reset with and give it the key of the packet coding; the stages 17..25 are
// the packet handshake, and every failure of those hands the channel to stage
// 16s, which resets the GBA and starts the packet exchange again at stage 17.
void _upload_00s(long chan, long result);
void _upload_01(long chan, long result);
void _upload_02(long chan, long result);
void _upload_03(long chan, long result);
void _upload_04(long chan, long result);
void _upload_05(long chan, long result);
void _upload_06(long chan, long result);
void _upload_07(long chan, long result);
void _upload_08(long chan, long result);
void _upload_09(long chan, long result);
void _upload_10(long chan, long result);
void _upload_11(long chan, long result);
void _upload_11s(long chan, long result);
void _upload_12(long chan, long result);
void _upload_12s(long chan, long result);
void _upload_13(long chan, long result);
void _upload_13s(long chan, long result);
void _upload_14(long chan, long result);
void _upload_15(long chan, long result);
void _upload_16(long chan, long result);
void _upload_16s(long chan, long result);
void _upload_16s0(long chan, long result);
void _upload_17(long chan, long result);
void _upload_17s(long chan, long result);
void _upload_18(long chan, long result);
void _upload_19(long chan, long result);
void _upload_20(long chan, long result);
void _upload_21(long chan, long result);
void _upload_21s(long chan, long result);
void _upload_22(long chan, long result);
void _upload_23(long chan, long result);
void _upload_23s(long chan, long result);
void _upload_24(long chan, long result);
void _upload_25(long chan, long result);

// One job record per SI channel: the channel state the link library reports,
// the stage the channel runs, the packet it assembles or sends and the timers
// the stages are driven by.
struct AGBWork {
    long jobType;            // 0x00: job the channel runs, 0 while none is active.
    u8 linkStatus[0x04];     // 0x04: channel state reported by the link library.
    long stage;              // 0x08: stage of the job the channel runs.
    long active;             // 0x0C: nonzero while a transfer owns the channel.
    long idleStage;          // 0x10: resume step AGB_IsComplete runs for an idle
                             //       channel (one of the AGB_IDLE_ codes).
    long result;             // 0x14: return value of the last link call.
    long status;             // 0x18: status AGB_GetStatus reports (0 = the transfer
                             //       is in, 1 = it ended in error, 3 = it runs).
    unsigned long packet;    // 0x1C: word a link call reads into or writes from.
    unsigned long syncCode;  // 0x20: state code the packet words are tagged with.
    unsigned long key;       // 0x24: rolling key of the packet coding.
    unsigned long checksum;  // 0x28: rolling checksum of the packet coding.
    unsigned long linkWord;  // 0x2C: packet word the exchange writes or reads.
    unsigned long keyB;      // 0x30: key the GBA answered the key exchange with.
    unsigned long bootKey;   // 0x34: boot key looked up for the GBA.
    unsigned long keyA;      // 0x38: key the exchange gives the GBA; the upload
                             //       stages reuse the same word for the code the
                             //       GBA answers the reset with.
    unsigned long request;   // 0x3C: word the current write sends out.
    unsigned long reply;     // 0x40: header word read back from the GBA.
    unsigned long period;    // 0x44: polling period of the stage timer, in msec.
    unsigned long reserved48;
    unsigned long reserved4C;
    unsigned long reserved50;
    unsigned long reserved54;
    unsigned long reserved58;
    unsigned long offset;    // 0x5C: write position in the packet buffer.
    unsigned long codedWord; // 0x60: packet word after the coding.
    unsigned long plainWord; // 0x64: packet word the checksum advances with.
    unsigned long reserved68;
    unsigned long tick;      // 0x6C: start tick of the stage timer.
    unsigned long timeout;   // 0x70: stage timer length, in ticks.
    unsigned long pollTick;  // 0x74: start tick of the polling timer.
    unsigned long pollTimeout; // 0x78: polling timer length, in ticks.
    unsigned long resetTick;    // 0x7C: start tick of the GBA reset handshake.
    unsigned long resetTimeout; // 0x80: reset handshake length, in ticks.
    unsigned long reserved84;
    unsigned long reserved88;
    u8 *buffer;              // 0x8C: buffer the packet is assembled in.
    unsigned long size;      // 0x90: length of the packet to upload, in bytes.
    void *record;            // 0x94: Pokemon record the packet is converted into.
    unsigned long *dest;     // 0x98: destination of the packet header word.
};

extern AGBWork AGBWorkArray[4];
extern u8 agbRecvBuffers[];

// Scratch buffer the code read out of the .jb file is loaded into, and the
// length of the code it holds (the upload sends that many bytes out). The
// buffer is handed out by AGB_FileBuffAlloc and released again by
// AGB_FileBuffFree, both in agbCommunication.cpp; the length is
// what _ReadJBCode read.
extern u8 *gbaFileBuffer;
extern unsigned long gbaFileSize;
// Path of the code file the last AGB_Prepare read, kept in the buffer the link
// library reads the file from (the name is copied out of the caller's string).
extern char gbaJbFilePath[0x100];
// Set while the link hardware still has to be initialized: AGB_Prepare
// initializes it once, so the sessions behind the first one reuse the link.
extern u8 gbaLinkUninitialized;

#endif
