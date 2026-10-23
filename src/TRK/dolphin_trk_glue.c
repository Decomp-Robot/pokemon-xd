/*
 * dolphin_trk_glue.c -- the console transport the nub talks to the host over.
 * The board the debugger was entered on is identified once, at startup, and
 * picked from the transports this link carries; everything the rest of the
 * nub sends or receives goes through the table that identification fills in.
 */

#include "nub.h"
#include <dolphin/base/PPCArch.h>

/* The instruction the debugger leaves behind when the program ends: the
   target stops here and the host sees the END marker instead of a crash. */
static const char EndofProgramInstruction[4] = "\0END";

/* Bit position of OS_CONTEXT_STATE_EXC in a PowerPC word. */
#define TRK_CONTEXT_EXCEPTION_BIT 30

/*
 * Hand control back to the code the debugger interrupted: load the GPRs, the
 * CR, LR, CTR and XER the debugger saved, set the machine state up for the
 * exception return, stash the SPRGs, and jump into the interrupt handler's
 * return path.
 */
asm void TRKLoadContext(OSContext* context, s32 interrupt) {
	nofralloc
	lwz r0,OS_CONTEXT_R0(r3)
	lwz r1,OS_CONTEXT_R1(r3)
	lwz r2,OS_CONTEXT_R2(r3)
	lhz r5,OS_CONTEXT_STATE(r3)
	rlwinm. r6,r5,0,TRK_CONTEXT_EXCEPTION_BIT,TRK_CONTEXT_EXCEPTION_BIT
	beq load_low_regs
	rlwinm r5,r5,0,TRK_CONTEXT_EXCEPTION_BIT+1,TRK_CONTEXT_EXCEPTION_BIT-1
	sth r5,OS_CONTEXT_STATE(r3)
	lmw r5,OS_CONTEXT_R5(r3)
	b load_special_regs
load_low_regs:
	lmw r13,OS_CONTEXT_R13(r3)
load_special_regs:
	mr r31,r3
	mr r3,r4
	lwz r4,OS_CONTEXT_CR(r31)
	mtcr r4
	lwz r4,OS_CONTEXT_LR(r31)
	mtlr r4
	lwz r4,OS_CONTEXT_CTR(r31)
	mtctr r4
	lwz r4,OS_CONTEXT_XER(r31)
	mtxer r4
	mfmsr r4
	rlwinm r4,r4,0,MSR_EE_BIT+1,MSR_EE_BIT-1
	rlwinm r4,r4,0,MSR_RI_BIT+1,MSR_RI_BIT-1
	mtmsr r4
	mtsprg 1,r2
	lwz r4,OS_CONTEXT_R3(r31)
	mtsprg 2,r4
	lwz r4,OS_CONTEXT_R4(r31)
	mtsprg 3,r4
	lwz r2,OS_CONTEXT_SRR0(r31)
	lwz r4,OS_CONTEXT_SRR1(r31)
	lwz r31,OS_CONTEXT_R31(r31)
	b TRKInterruptHandler
}

void TRKUARTInterruptHandler(void)
{
}

/* Build the end of program trap out of PPCHalt: the word after the halt
   instruction is replaced by the END marker, and the new instruction is
   forced out to memory and back into the instruction cache. */
void InitializeProgramEndTrap(void)
{
    u8* halt = (u8*)PPCHalt;

    TRK_memcpy(halt + 4, &EndofProgramInstruction, 4);
    ICInvalidateRange(halt + 4, 4);
    DCFlushRange(halt + 4, 4);
}

/* Report the state of the board's transport to the console. */
void TRK_board_display(const char* msg)
{
    OSReport("%s\n", msg);
}

void UnreserveEXI2Port(void)
{
    gDBCommTable.unreservePort();
}

void ReserveEXI2Port(void)
{
    gDBCommTable.reservePort();
}

/* Both report 0 once the transport has taken the bytes, and -1 otherwise. */
s32 TRKWriteUARTN(void* buffer, u32 length)
{
    return gDBCommTable.write(buffer, length) ? -1 : 0;
}

s32 TRKReadUARTN(void* buffer, u32 length)
{
    return gDBCommTable.read(buffer, length) ? -1 : 0;
}

s32 TRKPollUART(void)
{
    return gDBCommTable.peek();
}

/* The Broadband Adapter raises its own interrupts, so the transport only
   gets asked to do it when the nub was entered some other way. */
void EnableEXI2Interrupts(void)
{
    if (TRK_Use_BBA == 0 && gDBCommTable.enableInterrupts != 0)
        gDBCommTable.enableInterrupts();
}

s32 TRKInitializeIntDrivenUART(u32 baudRate, u32 unused2, u32 unused3,
                              volatile u8** inputPendingPtrRef)
{
    gDBCommTable.initialize(inputPendingPtrRef, TRKEXICallBack);
    gDBCommTable.open();
    return 0;
}

/* Point the comm table at the transport the board id names, and report the
   board the debugger is on.  A board id from outside the three arms above
   gets only the diagnostic: no transport is set up for it, and the
   initializer's starting result is returned. */
s32 InitMetroTRKCommTable(s32 hardware)
{
    s32 result = 1;

    OSReport("Devkit set to : %ld\n", hardware);
    TRK_Use_BBA = 0;

    if (hardware == TRK_BOARD_BBA) {
        OSReport("MetroTRK : Set to BBA\n");
        TRK_Use_BBA = 1;
        gDBCommTable.initialize = udp_cc_initialize;
        gDBCommTable.open = udp_cc_open;
        gDBCommTable.close = udp_cc_close;
        gDBCommTable.read = udp_cc_read;
        gDBCommTable.write = udp_cc_write;
        gDBCommTable.shutdown = udp_cc_shutdown;
        gDBCommTable.peek = udp_cc_peek;
        gDBCommTable.unreservePort = udp_cc_pre_continue;
        gDBCommTable.reservePort = udp_cc_post_stop;
        gDBCommTable.enableInterrupts = 0;
        return 0;
    }

    if (hardware == TRK_BOARD_GDEV) {
        OSReport("MetroTRK : Set to GDEV hardware\n");
        result = Hu_IsStub();
        gDBCommTable.initialize = gdev_cc_initialize;
        gDBCommTable.open = gdev_cc_open;
        gDBCommTable.close = gdev_cc_close;
        gDBCommTable.read = gdev_cc_read;
        gDBCommTable.write = gdev_cc_write;
        gDBCommTable.shutdown = gdev_cc_shutdown;
        gDBCommTable.peek = gdev_cc_peek;
        gDBCommTable.unreservePort = gdev_cc_pre_continue;
        gDBCommTable.reservePort = gdev_cc_post_stop;
        gDBCommTable.enableInterrupts = gdev_cc_initinterrupts;
    } else if (hardware == TRK_BOARD_AMC_DDH) {
        OSReport("MetroTRK : Set to AMC DDH hardware\n");
        result = AMC_IsStub();
        gDBCommTable.initialize = ddh_cc_initialize;
        gDBCommTable.open = ddh_cc_open;
        gDBCommTable.close = ddh_cc_close;
        gDBCommTable.read = ddh_cc_read;
        gDBCommTable.write = ddh_cc_write;
        gDBCommTable.shutdown = ddh_cc_shutdown;
        gDBCommTable.peek = ddh_cc_peek;
        gDBCommTable.unreservePort = ddh_cc_pre_continue;
        gDBCommTable.reservePort = ddh_cc_post_stop;
        gDBCommTable.enableInterrupts = ddh_cc_initinterrupts;
    } else {
        OSReport("MetroTRK : Set to UNKNOWN hardware. (%ld)\n", hardware);
        OSReport("MetroTRK : Invalid hardware ID passed from OS\n");
        OSReport("MetroTRK : Defaulting to GDEV Hardware\n");
    }

    return result;
}

/* The transport calls back once the console has data: the target has to be
   schedulable again before its context is handed back. */
void TRKEXICallBack(s32 id, OSContext* context)
{
    OSEnableScheduler();
    TRKLoadContext(context, TRK_EXTERNAL_INTERRUPT_CODE);
}
