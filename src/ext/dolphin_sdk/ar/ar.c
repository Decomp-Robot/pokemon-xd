#include <dolphin.h>
#include <dolphin/ar.h>
#include "fake_tgmath.h"

#include "__ar.h"

#define DSP_STATUS_AI_INTERRUPT         0x0008
#define DSP_STATUS_ARAM_INTERRUPT       0x0020
#define DSP_STATUS_DSP_INTERRUPT        0x0080
#define DSP_STATUS_ARAM_DMA_IN_PROGRESS 0x0200
#define ARAM_DMA_DIRECTION_FLAG         0x8000

#ifdef DEBUG
const char* __ARVersion = "<< Dolphin SDK - AR\tdebug build: Apr  5 2004 03:56:19 (0x2301) >>";
#else
const char* __ARVersion = "<< Dolphin SDK - AR\trelease build: Apr  5 2004 04:15:03 (0x2301) >>";
#endif

static ARCallback __AR_Callback;
static u32 __AR_Size;
static u32 __AR_InternalSize;
static u32 __AR_ExpansionSize;
static u32 __AR_StackPointer;
static u32 __AR_FreeBlocks;
static u32*  __AR_BlockLength;
static BOOL __AR_init_flag;

// prototypes
static void __ARHandler(__OSInterrupt exception, OSContext* context);
static void __ARWaitForDMA(void);
static void __ARWriteDMA(u32 mmem_addr, u32 aram_addr, u32 length);
static void __ARReadDMA(u32 mmem_addr, u32 aram_addr, u32 length);
static void __ARChecksize(void);
static void __ARClearArea(u32 start_addr, u32 length);

ARCallback ARRegisterDMACallback(ARCallback callback) {
    ARCallback old_callback;
    BOOL old;

    old_callback = __AR_Callback;
    old = OSDisableInterrupts();
    __AR_Callback = callback;
    OSRestoreInterrupts(old);
    return old_callback;
}

u32 ARGetDMAStatus(void) {
    BOOL old;
    u32 val;
    
    old = OSDisableInterrupts();
    val = __DSPRegs[DSP_CONTROL_STATUS] & DSP_STATUS_ARAM_DMA_IN_PROGRESS;
    OSRestoreInterrupts(old);
    return val;
}

void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length) {
    BOOL old;

    old = OSDisableInterrupts();
    ASSERTMSGLINE(376, !(__DSPRegs[DSP_CONTROL_STATUS] & DSP_STATUS_ARAM_DMA_IN_PROGRESS), "ARAM DMA already in progress\n");
    ASSERTMSGLINE(377, !(mainmem_addr & 0x1F), "AR: Main memory address is not a multiple of 32 bytes!\n");
    ASSERTMSGLINE(378, !(length & 0x1F), "AR: DMA transfer length is not a multiple of 32 bytes!\n");
    __DSPRegs[DSP_ARAM_DMA_MM_HI] = (__DSPRegs[DSP_ARAM_DMA_MM_HI] & 0xFFFFFC00 | (mainmem_addr >> 16)); 
    __DSPRegs[DSP_ARAM_DMA_MM_LO] = (__DSPRegs[DSP_ARAM_DMA_MM_LO] & 0xFFFF001F | ((u16)mainmem_addr));
    __DSPRegs[DSP_ARAM_DMA_ARAM_HI] = (__DSPRegs[DSP_ARAM_DMA_ARAM_HI] & 0xFFFFFC00 | (aram_addr >> 16));
    __DSPRegs[DSP_ARAM_DMA_ARAM_LO] = (__DSPRegs[DSP_ARAM_DMA_ARAM_LO] & 0xFFFF001F | ((u16)aram_addr));
    __DSPRegs[DSP_ARAM_DMA_SIZE_HI] = __DSPRegs[DSP_ARAM_DMA_SIZE_HI] & ~ARAM_DMA_DIRECTION_FLAG | (type << 15);
    __DSPRegs[DSP_ARAM_DMA_SIZE_HI] = (__DSPRegs[DSP_ARAM_DMA_SIZE_HI] & 0xFFFFFC00) | (length >> 16);
    __DSPRegs[DSP_ARAM_DMA_SIZE_LO] = (__DSPRegs[DSP_ARAM_DMA_SIZE_LO] & 0xFFFF001F) | (length & 0x0000FFFF);
    OSRestoreInterrupts(old);
}

u32 ARAlloc(u32 length) {
    u32 tmp;
    BOOL old;

    old = OSDisableInterrupts();
    ASSERTMSGLINE(430, !(length & 0x1F), "ARAlloc(): length is not multiple of 32bytes!");
    ASSERTMSGLINE(434, length <= (__AR_Size - __AR_StackPointer), "ARAlloc(): Out of ARAM!");
    ASSERTMSGLINE(435, __AR_FreeBlocks, "ARAlloc(): No more free blocks!");

    tmp = __AR_StackPointer;
    __AR_StackPointer += length;
    *__AR_BlockLength = length;
    __AR_BlockLength += 1;
    __AR_FreeBlocks -= 1;
    OSRestoreInterrupts(old);
    return tmp;
}

u32 ARInit(u32* stack_index_addr, u32 num_entries) {
    BOOL old;
    u16 refresh;

    if (__AR_init_flag == TRUE) {
        return 0x4000;
    }

    OSRegisterVersion(__ARVersion);

    old = OSDisableInterrupts();
    __AR_Callback = NULL;
    __OSSetInterruptHandler(__OS_INTERRUPT_DSP_ARAM, __ARHandler);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_DSP_ARAM);
    __AR_StackPointer = 0x4000;
    __AR_FreeBlocks = num_entries;
    __AR_BlockLength = stack_index_addr;
    refresh = __DSPRegs[DSP_ARAM_REFRESH] & 0xFF;

    ASSERTMSGLINE(590, (refresh <= 196.0f), "ARInit(): ILLEGAL SDRAM REFRESH VALUE\n");
    __DSPRegs[DSP_ARAM_REFRESH] = (u16)((__DSPRegs[DSP_ARAM_REFRESH] & ~0xFF) | (refresh & 0xFF));

    __ARChecksize();
    __AR_init_flag = TRUE;
    OSRestoreInterrupts(old);
    return __AR_StackPointer;
}

void ARSetSize(void) {
#ifdef DEBUG
    OSReport("ARSetSize(): I don't do anything anymore!\n");
#endif
}

u32 ARGetBaseAddress(void) {
    return 0x4000;
}

u32 ARGetSize(void) {
    return __AR_Size;
}

void ARClear(u32 flag) {
    switch (flag) {
    case AR_CLEAR_INTERNAL_ALL:
        if (__AR_InternalSize != 0) {
            __ARClearArea(0, __AR_InternalSize);
        }
        return;
    case AR_CLEAR_INTERNAL_USER:
        if (__AR_InternalSize != 0) {
            __ARClearArea(0x4000, __AR_InternalSize - 0x4000);
        }
        break;
    case AR_CLEAR_EXPANSION:
        if (__AR_InternalSize != 0 && __AR_ExpansionSize != 0) {
            __ARClearArea(__AR_InternalSize, __AR_ExpansionSize);
        }
        break;
    default:
        ASSERTMSGLINE(774, 0, "ARClear(): Unknown flag.\n");
        break;
    }
}

static void __ARHandler(__OSInterrupt exception, OSContext* context) {
    OSContext exceptionContext;
    u16 tmp;

    tmp = __DSPRegs[DSP_CONTROL_STATUS];
    tmp = (tmp & ~(DSP_STATUS_AI_INTERRUPT | DSP_STATUS_DSP_INTERRUPT)) | DSP_STATUS_ARAM_INTERRUPT;
    __DSPRegs[DSP_CONTROL_STATUS] = (tmp);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (__AR_Callback) {
        __AR_Callback();
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void __ARClearInterrupt(void) {
    u16 tmp;

    tmp = __DSPRegs[DSP_CONTROL_STATUS];
    tmp = (tmp & ~(DSP_STATUS_AI_INTERRUPT | DSP_STATUS_DSP_INTERRUPT)) | DSP_STATUS_ARAM_INTERRUPT;
    __DSPRegs[DSP_CONTROL_STATUS] = (tmp);
}

u16 __ARGetInterruptStatus(void) {
    return __DSPRegs[DSP_CONTROL_STATUS] & DSP_STATUS_ARAM_INTERRUPT;
}

static void __ARWaitForDMA(void) {
    while (__DSPRegs[DSP_CONTROL_STATUS] & DSP_STATUS_ARAM_DMA_IN_PROGRESS);
}

static void __ARWriteDMA(u32 mmem_addr, u32 aram_addr, u32 length) {
	// Main mem address
	__DSPRegs[DSP_ARAM_DMA_MM_HI] = (u16)((__DSPRegs[DSP_ARAM_DMA_MM_HI] & ~0x03ff) | (u16)(mmem_addr >> 16));
	__DSPRegs[DSP_ARAM_DMA_MM_LO] = (u16)((__DSPRegs[DSP_ARAM_DMA_MM_LO] & ~0xffe0) | (u16)(mmem_addr & 0xffff));

	// ARAM address
	__DSPRegs[DSP_ARAM_DMA_ARAM_HI] = (u16)((__DSPRegs[DSP_ARAM_DMA_ARAM_HI] & ~0x03ff) | (u16)(aram_addr >> 16));
	__DSPRegs[DSP_ARAM_DMA_ARAM_LO] = (u16)((__DSPRegs[DSP_ARAM_DMA_ARAM_LO] & ~0xffe0) | (u16)(aram_addr & 0xffff));

	// DMA buffer size
	__DSPRegs[DSP_ARAM_DMA_SIZE_HI] = (u16)(__DSPRegs[DSP_ARAM_DMA_SIZE_HI] & ~ARAM_DMA_DIRECTION_FLAG);

	__DSPRegs[DSP_ARAM_DMA_SIZE_HI] = (u16)((__DSPRegs[DSP_ARAM_DMA_SIZE_HI] & ~0x03ff) | (u16)(length >> 16));
	__DSPRegs[DSP_ARAM_DMA_SIZE_LO] = (u16)((__DSPRegs[DSP_ARAM_DMA_SIZE_LO] & ~0xffe0) | (u16)(length & 0xffff));

	__ARWaitForDMA();
    __ARClearInterrupt();
}

static void __ARReadDMA(u32 mmem_addr, u32 aram_addr, u32 length) {
	// Main mem address
	__DSPRegs[DSP_ARAM_DMA_MM_HI] = (u16)((__DSPRegs[DSP_ARAM_DMA_MM_HI] & ~0x03ff) | (u16)(mmem_addr >> 16));
	__DSPRegs[DSP_ARAM_DMA_MM_LO] = (u16)((__DSPRegs[DSP_ARAM_DMA_MM_LO] & ~0xffe0) | (u16)(mmem_addr & 0xffff));

	// ARAM address
	__DSPRegs[DSP_ARAM_DMA_ARAM_HI] = (u16)((__DSPRegs[DSP_ARAM_DMA_ARAM_HI] & ~0x03ff) | (u16)(aram_addr >> 16));
	__DSPRegs[DSP_ARAM_DMA_ARAM_LO] = (u16)((__DSPRegs[DSP_ARAM_DMA_ARAM_LO] & ~0xffe0) | (u16)(aram_addr & 0xffff));

	// DMA buffer size
	__DSPRegs[DSP_ARAM_DMA_SIZE_HI] = (u16)(__DSPRegs[DSP_ARAM_DMA_SIZE_HI] | ARAM_DMA_DIRECTION_FLAG);

	__DSPRegs[DSP_ARAM_DMA_SIZE_HI] = (u16)((__DSPRegs[DSP_ARAM_DMA_SIZE_HI] & ~0x03ff) | (u16)(length >> 16));
	__DSPRegs[DSP_ARAM_DMA_SIZE_LO] = (u16)((__DSPRegs[DSP_ARAM_DMA_SIZE_LO] & ~0xffe0) | (u16)(length & 0xffff));

	__ARWaitForDMA();
    __ARClearInterrupt();
}

enum {
    ARAM_SIZE_LOW_MEMORY_ADDRESS = 0x00D0,
    ARAM_CONTROLLER_READY = 1,
    ARAM_SIZE_INTERNAL = 3,
    ARAM_SIZE_EXPANSION_2MB = 0 << 3,
    ARAM_SIZE_EXPANSION_4MB = 1 << 3,
    ARAM_SIZE_EXPANSION_8MB = 2 << 3,
    ARAM_SIZE_EXPANSION_16MB = 3 << 3,
    ARAM_SIZE_EXPANSION_32MB = 4 << 3
};

static void __ARChecksize(void) {
    u8 test_data_pad[63];
    u8 dummy_data_pad[63];
    u8 buffer_pad[63];
    u8 save_pad_1[63];
    u8 save_pad_2[63];
    u8 save_pad_3[63];
    u8 save_pad_4[63];
    u8 save_pad_5[63];
    u32* test_data;
    u32* dummy_data;
    u32* buffer;
    u32* save1;
    u32* save2;
    u32* save3;
    u32* save4;
    u32* save5;
    u16 ARAM_mode = 0;
    u32 ARAM_size = 0;
    u32 i;

    do {} while(!(__DSPRegs[DSP_ARAM_MODE] & ARAM_CONTROLLER_READY));

    ARAM_mode = ARAM_SIZE_INTERNAL;
    ARAM_size = __AR_InternalSize = 16777216;
    __DSPRegs[DSP_ARAM_SIZE] = ((__DSPRegs[DSP_ARAM_SIZE] & 0xFFFFFFC0) | ARAM_SIZE_INTERNAL) | ARAM_SIZE_EXPANSION_32MB;

    test_data = (u32*)(OSRoundUp32B((u32)(test_data_pad)));
    dummy_data = (u32*)(OSRoundUp32B((u32)(dummy_data_pad)));
    buffer = (u32*)(OSRoundUp32B((u32)(buffer_pad)));

    save1 = (u32*)(OSRoundUp32B((u32)(save_pad_1)));
    save2 = (u32*)(OSRoundUp32B((u32)(save_pad_2)));
    save3 = (u32*)(OSRoundUp32B((u32)(save_pad_3)));
    save4 = (u32*)(OSRoundUp32B((u32)(save_pad_4)));
    save5 = (u32*)(OSRoundUp32B((u32)(save_pad_5)));

    for (i = 0; i < 8; i++) {
        *(test_data + i) = 0xDEADBEEF;
        *(dummy_data + i) = 0xBAD0BAD0;
    }

    DCFlushRange((void*)test_data, 32);
    DCFlushRange((void*)dummy_data, 32);

    __AR_ExpansionSize = 0;

    DCInvalidateRange((void*)save1, 32);
    __ARReadDMA((u32)save1, ARAM_size + 0, 32);
    PPCSync();

    __ARWriteDMA((u32)test_data, ARAM_size + 0x0000000, 32);

    memset((void*)buffer, 0, 32);
    DCFlushRange((void*)buffer, 32);

    __ARReadDMA((u32)buffer, ARAM_size + 0x0000000, 32);
    PPCSync();

    if (buffer[0] == test_data[0]) {
        DCInvalidateRange((void*)save2, 32);
        __ARReadDMA((u32)save2, ARAM_size + 0x0200000, 32);
        PPCSync();

        DCInvalidateRange((void*)save3, 32);
        __ARReadDMA((u32)save3, ARAM_size + 0x1000000, 32);
        PPCSync();

        DCInvalidateRange((void*)save4, 32);
        __ARReadDMA((u32)save4, ARAM_size + 0x0000200, 32);
        PPCSync();

        DCInvalidateRange((void*)save5, 32);
        __ARReadDMA((u32)save5, ARAM_size + 0x0400000, 32);
        PPCSync();

        __ARWriteDMA((u32)dummy_data, ARAM_size + 0x0200000, 32);
        __ARWriteDMA((u32)test_data, ARAM_size + 0x0000000, 32);

        memset((void*)buffer, 0, 32);
        DCFlushRange((void*)buffer, 32);

        __ARReadDMA((u32)buffer, ARAM_size + 0x0200000, 32);
        PPCSync();

        if (buffer[0] == test_data[0]) {
            __ARWriteDMA((u32)save1, ARAM_size + 0x0000000, 32);

            ARAM_mode |= ARAM_SIZE_EXPANSION_2MB;
            ARAM_size += 2097152;
            __AR_ExpansionSize = 2097152;
        } else {
            __ARWriteDMA((u32)dummy_data, ARAM_size + 0x1000000, 32);
            __ARWriteDMA((u32)test_data, ARAM_size + 0x0000000, 32);

            memset((void*)buffer, 0, 32);
            DCFlushRange((void*)buffer, 32);

            __ARReadDMA((u32)buffer, ARAM_size + 0x1000000, 32);
            PPCSync();

            if (buffer[0] == test_data[0]) {
                __ARWriteDMA((u32)save1, ARAM_size + 0x0000000, 32);
                __ARWriteDMA((u32)save2, ARAM_size + 0x0200000, 32);

                ARAM_mode |= ARAM_SIZE_EXPANSION_4MB;
                ARAM_size += 4194304;
                __AR_ExpansionSize = 4194304;
            } else {
                __ARWriteDMA((u32)dummy_data, ARAM_size + 0x0000200, 32);
                __ARWriteDMA((u32)test_data, ARAM_size + 0x0000000, 32);

                memset((void*)buffer, 0, 32);
                DCFlushRange((void*)buffer, 32);

                __ARReadDMA((u32)buffer, ARAM_size + 0x0000200, 32);
                PPCSync();

                if (buffer[0] == test_data[0]) {
                    __ARWriteDMA((u32)save1, ARAM_size + 0x0000000, 32);
                    __ARWriteDMA((u32)save2, ARAM_size + 0x0200000, 32);
                    __ARWriteDMA((u32)save3, ARAM_size + 0x1000000, 32);

                    ARAM_mode |= ARAM_SIZE_EXPANSION_8MB;
                    ARAM_size += 8388608;
                    __AR_ExpansionSize = 8388608;
                } else {
                    __ARWriteDMA((u32)dummy_data, ARAM_size + 0x0400000, 32);

                    __ARWriteDMA((u32)test_data, ARAM_size + 0x0000000, 32);

                    memset((void*)buffer, 0, 32);
                    DCFlushRange((void*)buffer, 32);

                    __ARReadDMA((u32)buffer, ARAM_size + 0x0400000, 32);
                    PPCSync();

                    if (buffer[0] == test_data[0]) {
                        __ARWriteDMA((u32)save1, ARAM_size + 0x0000000, 32);
                        __ARWriteDMA((u32)save2, ARAM_size + 0x0200000, 32);
                        __ARWriteDMA((u32)save3, ARAM_size + 0x1000000, 32);
                        __ARWriteDMA((u32)save4, ARAM_size + 0x0000200, 32);

                        ARAM_mode |= ARAM_SIZE_EXPANSION_16MB;
                        ARAM_size += 16777216;
                        __AR_ExpansionSize = 16777216;
                    } else {
                        __ARWriteDMA((u32)save1, ARAM_size + 0x0000000, 32);
                        __ARWriteDMA((u32)save2, ARAM_size + 0x0200000, 32);
                        __ARWriteDMA((u32)save3, ARAM_size + 0x1000000, 32);
                        __ARWriteDMA((u32)save4, ARAM_size + 0x0000200, 32);
                        __ARWriteDMA((u32)save5, ARAM_size + 0x0400000, 32);

                        ARAM_mode |= ARAM_SIZE_EXPANSION_32MB;
                        ARAM_size += 33554432;
                        __AR_ExpansionSize = 33554432;
                    }
                }
            }
        }

#ifdef DEBUG
        OSReport("__ARChecksize(): ARAM Expansion present.\n");
#endif
        __DSPRegs[DSP_ARAM_SIZE] = (u16)((__DSPRegs[DSP_ARAM_SIZE] & ~(0x07 | 0x38)) | ARAM_mode);
    }

    *(u32*)OSPhysicalToUncached(ARAM_SIZE_LOW_MEMORY_ADDRESS) = ARAM_size;
    __AR_Size = ARAM_size;
}

static void __ARClearArea(u32 start_addr, u32 length) {
    u8 zero_buffer[2079];
    u8* ptr;
    u32 curr_addr;
    u32 curr_len;
    u32 end_addr;
    u32 remainder;

    ASSERTMSGLINE(1321, !(start_addr & 0x1F), "__ARClearArea(): Destination address not 32-byte aligned.\n");
    ASSERTMSGLINE(1322, !(length & 0x1F), "__ARClearArea(): Length not multiple of 32 bytes.\n");

    ptr = (u8*)(OSRoundUp32B((u32)(zero_buffer)));

    do {} while(!(__DSPRegs[DSP_ARAM_MODE] & ARAM_CONTROLLER_READY));

    memset(ptr, 0, 2048);
    DCFlushRange(ptr, 2048);

    curr_addr = start_addr;
    end_addr = start_addr + length;

    while (curr_addr < end_addr) {
        remainder = end_addr - curr_addr;
 
        curr_len = OSRoundUp32B(remainder < 2048 ? remainder : 2048);
        __ARWriteDMA((u32)ptr, curr_addr, curr_len);
        curr_addr += curr_len;
    }
}
