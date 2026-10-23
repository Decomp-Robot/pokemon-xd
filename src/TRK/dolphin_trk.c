/*
 * dolphin_trk.c -- entering the debugger on a Dolphin board.  The nub is
 * handed control with everything the CPU was holding at the time, so the
 * routines here save the parts of the machine state the debugger cannot read
 * back, keep the nub's own copy of the interrupt vectors, and hand the target
 * over to TRK_main.
 */

#include "nub.h"
#include <dolphin/base/PPCArch.h>

/*
 * Enter the nub.  The GPRs, the link register, the condition register and the
 * machine state are staged in gTRKCPUState, with the stack pointer and the
 * register the caller happened to be holding put back over the values the
 * register save itself left there; TRKSaveExtended1Block completes the block.
 * The hardware breakpoints are cleared and the stack moves to the debugger's
 * own before the console transport is set up.
 *
 * A result other than 1 jumps straight to TRK_main.  The result-1 path
 * attempts to restore registers and return, but uses the initializer result
 * as the load base rather than the saved-state pointer.
 */
asm void InitMetroTRK(void) {
	nofralloc
	subi r1,r1,4
	stw r3,0(r1)
	lis r3,gTRKCPUState@h
	ori r3,r3,gTRKCPUState@l
	stmw r0,0(r3)
	lwz r4,0(r1)
	addi r1,r1,4
	stw r1,4(r3)
	stw r4,12(r3)
	mflr r4
	stw r4,132(r3)
	stw r4,128(r3)
	mfcr r4
	stw r4,136(r3)
	mfmsr r4
	ori r3,r4,MSR_EE
	xori r3,r3,MSR_EE
	mtmsr r3
	mtsrr1 r4
	bl TRKSaveExtended1Block
	lis r3,gTRKCPUState@h
	ori r3,r3,gTRKCPUState@l
	lmw r0,0(r3)
	li r0,0
	mtspr IABR,r0
	mtspr DABR,r0
	lis r1,_db_stack_addr@h
	ori r1,r1,_db_stack_addr@l
	mr r3,r5
	bl InitMetroTRKCommTable
	cmpwi r3,1
	bne run_nub
	lwz r4,132(r3)
	mtlr r4
	lmw r0,0(r3)
	blr
run_nub:
	b TRK_main
	blr
}

/*
 * The same entry point on a board whose transport is the Broadband Adapter.
 * The board is known here, so the transport is named outright, and the
 * adapter raises its own interrupts, so they are left enabled.
 */
asm void InitMetroTRK_BBA(void) {
	nofralloc
	subi r1,r1,4
	stw r3,0(r1)
	lis r3,gTRKCPUState@h
	ori r3,r3,gTRKCPUState@l
	stmw r0,0(r3)
	lwz r4,0(r1)
	addi r1,r1,4
	stw r1,4(r3)
	stw r4,12(r3)
	mflr r4
	stw r4,132(r3)
	stw r4,128(r3)
	mfcr r4
	stw r4,136(r3)
	mfmsr r4
	ori r3,r4,MSR_EE
	mtmsr r3
	mtsrr1 r4
	bl TRKSaveExtended1Block
	lis r3,gTRKCPUState@h
	ori r3,r3,gTRKCPUState@l
	lmw r0,0(r3)
	li r0,0
	mtspr IABR,r0
	mtspr DABR,r0
	lis r1,_db_stack_addr@h
	ori r1,r1,_db_stack_addr@l
	li r3,TRK_BOARD_BBA
	bl InitMetroTRKCommTable
	cmpwi r3,1
	bne run_nub_bba
	lwz r4,132(r3)
	mtlr r4
	lmw r0,0(r3)
	blr
run_nub_bba:
	b TRK_main
	blr
}

/*
 * Move a block of the buffer out to ARAM.  The DMA engine moves whole 32-byte
 * blocks, so a request that does not start or end on a block boundary needs
 * the rest of that block kept: ARAM is read back into a staging block and the
 * bytes the request does not cover are copied into the buffer the transfer
 * runs out of.
 */
void TRK__write_aram(register u32* buf, u32 addr, u32* size)
{
    register u8* block;
    u8 blockData[32] __attribute__((aligned(32)));
    u32 aligned;
    u32 length;
    u16 status;
    u32 staged;
    register u32 offset;

    if (addr < TRK_ARAM_RESERVED || addr + *size > TRK_ARAM_LIMIT)
        return;

    aligned = addr & ~(TRK_ARAM_BLOCK_SIZE - 1);
    length = *size + (addr & (TRK_ARAM_BLOCK_SIZE - 1));
    length = (length + TRK_ARAM_BLOCK_SIZE - 1) & ~(TRK_ARAM_BLOCK_SIZE - 1);

    for (offset = 0; offset < length; offset += TRK_ARAM_BLOCK_SIZE)
        asm { dcbf offset, buf }

    while (ARGetDMAStatus() != 0)
        ;

    status = __ARGetInterruptStatus();
    staged = TRK_ARAM_LIMIT;

    offset = addr & (TRK_ARAM_BLOCK_SIZE - 1);
    if (offset != 0) {
        block = blockData;
        staged = aligned;
        asm { dcbi 0, block }
        __ARClearInterrupt();
        ARStartDMA(ARAM_DIR_ARAM_TO_MRAM, (u32)block, aligned,
                   TRK_ARAM_BLOCK_SIZE);
        while (__ARGetInterruptStatus() == 0)
            ;
        TRK_memcpy(buf, blockData, offset);
        __dcbf(buf, 0);
    }

    addr += *size;
    offset = addr & (TRK_ARAM_BLOCK_SIZE - 1);
    if (offset != 0) {
        u8* dst;
        u32 endAligned = addr & ~(TRK_ARAM_BLOCK_SIZE - 1);

        if (endAligned != staged) {
            block = blockData;
            asm { dcbi 0, block }
            __ARClearInterrupt();
            ARStartDMA(ARAM_DIR_ARAM_TO_MRAM, (u32)block, endAligned,
                       TRK_ARAM_BLOCK_SIZE);
            while (__ARGetInterruptStatus() == 0)
                ;
        }
        /* The bytes behind the request travel to the same offset in the
           buffer the transfer runs out of. */
        dst = (u8*)buf + addr;
        TRK_memcpy(dst, blockData + offset, TRK_ARAM_BLOCK_SIZE - offset);
        __dcbf(dst, 0);
    }

    asm { sync }
    __ARClearInterrupt();
    ARStartDMA(ARAM_DIR_MRAM_TO_ARAM, (u32)buf, aligned, length);

    if (status == 0) {
        while (__ARGetInterruptStatus() == 0)
            ;
        __ARClearInterrupt();
    }
}

/*
 * Move a block of ARAM into the buffer.  Same 32-byte granularity: a request
 * that starts mid-block leaves the bytes in front of it at the head of the
 * buffer, and the transfer runs up to the end of the block past the request.
 */
void TRK__read_aram(register u32* buf, u32 addr, u32* size)
{
    register u32 offset;
    u32 tail;
    u32 aligned;
    u32 length;
    u16 status;

    if (addr < TRK_ARAM_RESERVED || addr + *size > TRK_ARAM_LIMIT)
        return;

    tail = addr & (TRK_ARAM_BLOCK_SIZE - 1);
    aligned = addr & ~(TRK_ARAM_BLOCK_SIZE - 1);
    length = *size + tail;
    length = (length + TRK_ARAM_BLOCK_SIZE - 1) & ~(TRK_ARAM_BLOCK_SIZE - 1);

    for (offset = 0; offset < length; offset += TRK_ARAM_BLOCK_SIZE)
        asm { dcbi offset, buf }

    while (ARGetDMAStatus() != 0)
        ;

    status = __ARGetInterruptStatus();
    __ARClearInterrupt();
    ARStartDMA(ARAM_DIR_ARAM_TO_MRAM, (u32)buf, aligned, length);

    while (__ARGetInterruptStatus() == 0)
        ;

    if (status != 0)
        return;

    __ARClearInterrupt();
}

/* The target starts halted, with the machine state the debugger was entered
   with, and with the locked-cache window based where the hardware puts it. */
s32 TRKInitializeTarget(void)
{
    gTRKState.stopped = 1;
    gTRKState.msr = TRKMfmsr();
    gTRKLockedCacheBase = TRK_LOCKED_CACHE_BASE;
    return 0;
}

/* Bring an address the debugger was handed into a form the CPU can fetch
   through: a locked-cache address stays put while its BAT mapping is valid,
   an address already in the mirrored window stays put, and anything else is
   forced into the cached mirror. */
static inline u32 TRKTranslateAddress(u32 address)
{
    u32 lockedCacheBase = gTRKLockedCacheBase;

    if (address >= lockedCacheBase && address < lockedCacheBase + TRK_LOCKED_CACHE_SIZE &&
        (gTRKCPUState.dbat3u & TRK_DBAT_VALID_MASK))
        return address;
    if (address >= TRK_MAPPED_WINDOW_START && address <= TRK_MAPPED_WINDOW_END)
        return address;
    return (address & TRK_ADDRESS_MASK) | TRK_CACHED_BASE;
}

/*
 * Install resident debugger exception-handler templates into the active
 * vectors selected by the exception mask, excluding external interrupts.
 */
void __TRK_copy_vectors(void)
{
    u32* window = (u32*)TRKTranslateAddress(TRK_EXCEPTION_MASK_ADDR);
    s32 i;
    u32 enabled;

    enabled = *window;
    for (i = 0; i <= TRK_ISR_LAST; i++) {
        u32 vector;
        u32 destination;

        if ((enabled & (1u << i)) == 0 || i == TRK_ISR_EXTERNAL)
            continue;
        vector = TRK_ISR_OFFSETS[i];
        destination = TRKTranslateAddress(vector);
        TRK_memcpy((void*)destination, &gTRKInterruptVectorTable[vector],
                   TRK_VECTOR_SIZE);
        TRK_flush_cache((void*)destination, TRK_VECTOR_SIZE);
    }
}

/* Map an address the debugger was handed onto one the CPU can fetch
   through. */
u32 TRKTargetTranslate(u32 address)
{
    return TRKTranslateAddress(address);
}

void EnableMetroTRKInterrupts(void)
{
    EnableEXI2Interrupts();
}
