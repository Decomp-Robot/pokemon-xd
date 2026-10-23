#ifndef TRK_NUB_H
#define TRK_NUB_H

#include <dolphin/os/OSContext.h>

/*
 * Definitions the MetroTRK nub shares: the debugger stub that runs on the
 * target, saves the machine state the debugger cannot read back, talks to the
 * host over the board's console transport, and installs the debugger's
 * exception handlers.
 */

/*
 * gTRKMemMap -- the memory ranges the debugger vouches for, one 16-byte record
 * per range: first and last address, then whether the range may be read and
 * written.  TRKValidMemory32 walks it.
 */
typedef struct TRKMemRange {
    u32 first;      /* 0x00 */
    u32 last;       /* 0x04 */
    s32 readable;   /* 0x08 */
    s32 writeable;  /* 0x0C */
} TRKMemRange;

/* Access kinds TRKValidMemory32 is asked about. */
typedef enum {
    TRK_MEMMAP_READ = 0,
    TRK_MEMMAP_WRITE = 1
} TRKAccess;

/* Result the nub reports for a range it will not vouch for. */
#define kTRKInvalidMemory 0x700

extern const TRKMemRange gTRKMemMap[1];

/*
 * gTRKCPUState -- the debugger's saved register block.  The nub addresses it
 * by field offset rather than by name, apart from the BAT entry the address
 * helpers read; the offsets and the total size are what the nub and the game
 * agree on.
 */
typedef struct TRKCPUState {
    u8 reserved0[0x238];
    u32 dbat3u;   /* 0x238: upper half of data BAT 3 */
    u8 reserved1[0x430 - 0x23C];
} TRKCPUState;

extern TRKCPUState gTRKCPUState;

/*
 * gTRKState -- the debugger's own state block.  Only the two words the target
 * initialisation touches are named.
 */
typedef struct TRKState {
    u8 reserved0[0x8C];
    u32 msr;      /* 0x8C: the machine state the debugger was entered with */
    u8 reserved1[8];
    u32 stopped;  /* 0x98: set while the target is held */
    u8 reserved2[0xA4 - 0x9C];
} TRKState;

extern TRKState gTRKState;

/* gTRKRestoreFlags -- the flags TRKRestoreExtended1Block consumes, byte 0 for
   the time base and byte 1 for the decrementer. */
extern u8 gTRKRestoreFlags[9];

/* Exception offsets used to select resident debugger handler templates. */
extern const u32 TRK_ISR_OFFSETS[15];

/* Resident debugger exception-handler templates, indexed by exception offset. */
extern u8 gTRKInterruptVectorTable[0x1F34];

/* gTRKLockedCacheBase -- the locked-cache window base the address helpers compare against. */
extern u32 gTRKLockedCacheBase;

/* TRK_Use_BBA -- nonzero once the Broadband Adapter has been chosen. */
extern u8 TRK_Use_BBA;

/* gTRKUseSerialIO -- the serial IO option the console paths read: GetUseSerialIO
   and TRKSetUseSerialIO access this byte. */
extern u8 gTRKUseSerialIO;

/* PPCHalt -- the idle loop the end of program trap is built from. */
void PPCHalt(void);

/* _db_stack_addr -- the debugger's own stack address, defined by the link. */
extern u8 _db_stack_addr[];

/* Address bit that selects the cached mirror of main memory, the window of
   hardware mapped addresses the debugger leaves as they stand, and the two
   bits of the data BAT entry mapping the locked-cache window that say it is live. */
#define TRK_CACHED_BASE 0x80000000u
#define TRK_MAPPED_WINDOW_START 0x7E000000u
#define TRK_MAPPED_WINDOW_END 0x80000000u
#define TRK_ADDRESS_MASK 0x3FFFFFFFu

#define TRK_LOCKED_CACHE_BASE 0xE0000000u
#define TRK_EXCEPTION_MASK_ADDR 0x44u
#define TRK_LOCKED_CACHE_SIZE 0x4000u
#define TRK_DBAT_VALID_MASK 0x3u

/* The last interrupt vector __TRK_copy_vectors copies, and the one it never
   copies: index 4 is the external-interrupt vector (0x500), serviced through
   TRKEXICallBack instead. */
#define TRK_ISR_LAST 14
#define TRK_ISR_EXTERNAL 4
#define TRK_EXTERNAL_INTERRUPT_CODE 0x500
#define TRK_VECTOR_SIZE 0x100

/* The debugger moves ARAM one 32-byte block at a time and leaves the first
   16 KB alone.  TRK_ARAM_LIMIT is the highest address TRK__write_aram stages
   through. */
#define TRK_ARAM_BLOCK_SIZE 32
#define TRK_ARAM_RESERVED 0x4000u
#define TRK_ARAM_LIMIT 0x8000000u

/* Direction words ARStartDMA takes, as <dolphin/ar.h> spells them. */
#define ARAM_DIR_MRAM_TO_ARAM 0
#define ARAM_DIR_ARAM_TO_MRAM 1

/*
 * gDBCommTable -- the console transport the nub picked at startup.  A NULL
 * slot means the transport does not implement that call.
 */
typedef void (*TRKCommCallback)(s32 id, OSContext* context);

typedef struct DBCommTable {
    s32  (*initialize)(volatile u8** inputPendingPtrRef, TRKCommCallback callback);     /* 0x00 */
    s32  (*enableInterrupts)(void);       /* 0x04 */
    s32  (*shutdown)(void);               /* 0x08 */
    s32  (*peek)(void);                   /* 0x0C */
    s32  (*read)(void*, u32);             /* 0x10 */
    s32  (*write)(void*, u32);            /* 0x14 */
    s32  (*open)(void);                   /* 0x18 */
    s32  (*close)(void);                  /* 0x1C */
    s32  (*unreservePort)(void);          /* 0x20 */
    s32  (*reservePort)(void);            /* 0x24 */
} DBCommTable;

extern DBCommTable gDBCommTable;

/* The board ids InitMetroTRKCommTable is handed. */
#define TRK_BOARD_AMC_DDH 0
#define TRK_BOARD_GDEV 1
#define TRK_BOARD_BBA 2

/* The trace level TRK_main announces the nub under. */
#define TRK_TRACE_LEVEL 1

/* The transports the board can be wired for. */
extern s32  udp_cc_initialize(volatile u8** inputPendingPtrRef, TRKCommCallback callback);
extern s32  udp_cc_open(void);
extern s32  udp_cc_close(void);
extern s32  udp_cc_read(void*, u32);
extern s32  udp_cc_write(void*, u32);
extern s32  udp_cc_shutdown(void);
extern s32  udp_cc_peek(void);
extern s32  udp_cc_pre_continue(void);
extern s32  udp_cc_post_stop(void);

extern s32  gdev_cc_initialize(volatile u8** inputPendingPtrRef, TRKCommCallback callback);
extern s32  gdev_cc_initinterrupts(void);
extern s32  gdev_cc_open(void);
extern s32  gdev_cc_close(void);
extern s32  gdev_cc_read(void*, u32);
extern s32  gdev_cc_write(void*, u32);
extern s32  gdev_cc_shutdown(void);
extern s32  gdev_cc_peek(void);
extern s32  gdev_cc_pre_continue(void);
extern s32  gdev_cc_post_stop(void);

extern s32  ddh_cc_initialize(volatile u8** inputPendingPtrRef, TRKCommCallback callback);
extern s32  ddh_cc_initinterrupts(void);
extern s32  ddh_cc_open(void);
extern s32  ddh_cc_close(void);
extern s32  ddh_cc_read(void*, u32);
extern s32  ddh_cc_write(void*, u32);
extern s32  ddh_cc_shutdown(void);
extern s32  ddh_cc_peek(void);
extern s32  ddh_cc_pre_continue(void);
extern s32  ddh_cc_post_stop(void);

/* SDK entry points the nub reaches, among them the board identification
   stubs a board that is not the one the link expects reports through. */
s32 AMC_IsStub(void);
s32 Hu_IsStub(void);
u32 ARGetDMAStatus(void);
void ARStartDMA(u32 type, u32 uMainMem, u32 uARAM, u32 uLength);
void DCFlushRange(void* addr, u32 nBytes);
void ICInvalidateRange(void* addr, u32 nBytes);
void MWTRACE(s32 level, const char* fmt, ...);
s32 OSEnableScheduler(void);
void OSReport(const char* message, ...);
void TRKInterruptHandler(void);
s32 TRKInitializeNub(void);
void TRKNubMainLoop(void);
void TRKNubWelcome(void);
void TRKSwapAndGo(void);
void TRKTargetSetStopped(s32 stopped);
s32 TRKTerminateNub(void);
void TRK_flush_cache(void*, u32);
void* TRK_memcpy(void* dst, const void* src, u32 count);
u32 TRKMfmsr(void);
void __ARClearInterrupt(void);
u16 __ARGetInterruptStatus(void);

/* Host-serviced file operations.  Arguments and results use the trap ABI. */
u32 TRKAccessFile(u8 command, u32 fileHandle, u32* length, u8* buffer);
u32 TRKOpenFile(u8 command, const char* pathname, u8 openFlags,
                u32* fileHandleOut);
u32 TRKCloseFile(u8 command, u32 fileHandle);
u32 TRKPositionFile(u8 command, u32 fileHandle, u32* position,
                    u8 positioningMode);

/* The nub's entry points and its target side, defined by the nub's own
   translation units. */
void InitMetroTRK(void);
void InitMetroTRK_BBA(void);
s32 InitMetroTRKCommTable(s32 hardware);
void EnableEXI2Interrupts(void);
void EnableMetroTRKInterrupts(void);
void InitializeProgramEndTrap(void);
void ReserveEXI2Port(void);
void TRKEXICallBack(s32 id, OSContext* context);
s32 TRKInitializeIntDrivenUART(u32 baudRate, u32 unused2, u32 unused3,
                              volatile u8** inputPendingPtrRef);
s32 TRKInitializeTarget(void);
void TRKLoadContext(OSContext* context, s32 interrupt);
s32 TRKPollUART(void);
s32 TRKReadUARTN(void* buffer, u32 length);
void TRKRestoreExtended1Block(void);
void TRKSaveExtended1Block(void);
u32 TRKTargetTranslate(u32 address);
void TRKUARTInterruptHandler(void);
s32 TRKValidMemory32(u32 address, u32 length, TRKAccess access);
s32 TRKWriteUARTN(void* buffer, u32 length);
void TRK__read_aram(register u32* buf, u32 addr, u32* size);
void TRK__write_aram(register u32* buf, u32 addr, u32* size);
void TRK_main(void);
void TRK_board_display(const char* msg);
void UnreserveEXI2Port(void);
void __TRK_copy_vectors(void);
u32 TRKTargetContinue(void);
u8 GetUseSerialIO(void);
void TRKSetUseSerialIO(u8 value);

#endif
