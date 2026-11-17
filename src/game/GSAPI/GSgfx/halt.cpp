#include <types.h>
#include <string.h>
#include <dolphin/os.h>
#include <dolphin/base/PPCArch.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSInterrupt.h>

#define DSISR_DABRMATCH       0x00400000
#define OS_PHYSICAL_ADDR_MASK 0x3FFFFFFF

void _haltProcessException();

extern "C" {
void GSgfxHalt(OSError error, OSContext* context, ...);
}

u8 _haltOldDSIHandler[116];
u8 _haltOldISIHandler[52];
u8 _haltOldAlignmentHandler[52];
u8 _haltOldProgramHandler[52];
u8 _haltTex[128];
OSContext _haltContext;
u8 _haltCallStack[60];

u8 _haltInitialized;
u32 _haltExceptionType;

asm void _haltDSIHandler() {
    nofralloc
    mtsprg 1, r3
    mtsprg 2, r4
    mfdsisr r3
    lis r4, (DSISR_DABRMATCH >> 16)
    and r3, r3, r4
    cmplwi r3, 0
    beq _haltDSIProcessException
    lis r3, _haltOldDSIHandler@h
    ori r3, r3, _haltOldDSIHandler@l
    lis r4, (OS_PHYSICAL_ADDR_MASK >> 16)
    ori r4, r4, (OS_PHYSICAL_ADDR_MASK & 0xffff)
    and r3, r3, r4
    mtctr r3
    mfsprg r3, 1
    mfsprg r4, 2
    bctr
_haltDSIProcessException:
    mfsprg r4, 2
    mfsrr0 r3
    mtsprg 2, r3
    mfsrr1 r3
    mtsprg 3, r3
    mfmsr r3
    ori r3, r3, (MSR_FP | MSR_IR | MSR_DR)
    mtsrr1 r3
    lis r3, _haltProcessException@h
    ori r3, r3, _haltProcessException@l
    mtsrr0 r3
    li r3, OS_ERROR_DSI
    rfi
}

asm void _haltISIHandler() {
    nofralloc
    mtsprg 1, r3
    mfsrr0 r3
    mtsprg 2, r3
    mfsrr1 r3
    mtsprg 3, r3
    mfmsr r3
    ori r3, r3, (MSR_FP | MSR_IR | MSR_DR)
    mtsrr1 r3
    lis r3, _haltProcessException@h
    ori r3, r3, _haltProcessException@l
    mtsrr0 r3
    li r3, OS_ERROR_ISI
    rfi
}

asm void _haltAlignmentHandler() {
    nofralloc
    mtsprg 1, r3
    mfsrr0 r3
    mtsprg 2, r3
    mfsrr1 r3
    mtsprg 3, r3
    mfmsr r3
    ori r3, r3, (MSR_FP | MSR_IR | MSR_DR)
    mtsrr1 r3
    lis r3, _haltProcessException@h
    ori r3, r3, _haltProcessException@l
    mtsrr0 r3
    li r3, OS_ERROR_ALIGNMENT
    rfi
}

asm void _haltProcessException() {
    nofralloc
    stw r3, _haltExceptionType(r0)
    lis r3, _haltContext@h
    ori r3, r3, _haltContext@l
    stw r0, OS_CONTEXT_R0(r3)
    stw r1, OS_CONTEXT_R1(r3)
    stw r2, OS_CONTEXT_R2(r3)
    stw r4, OS_CONTEXT_R4(r3)
    stw r5, OS_CONTEXT_R5(r3)
    stw r6, OS_CONTEXT_R6(r3)
    stw r7, OS_CONTEXT_R7(r3)
    stw r8, OS_CONTEXT_R8(r3)
    stw r9, OS_CONTEXT_R9(r3)
    stw r10, OS_CONTEXT_R10(r3)
    stw r11, OS_CONTEXT_R11(r3)
    stw r12, OS_CONTEXT_R12(r3)
    stw r13, OS_CONTEXT_R13(r3)
    stw r14, OS_CONTEXT_R14(r3)
    stw r15, OS_CONTEXT_R15(r3)
    stw r16, OS_CONTEXT_R16(r3)
    stw r17, OS_CONTEXT_R17(r3)
    stw r18, OS_CONTEXT_R18(r3)
    stw r19, OS_CONTEXT_R19(r3)
    stw r20, OS_CONTEXT_R20(r3)
    stw r21, OS_CONTEXT_R21(r3)
    stw r22, OS_CONTEXT_R22(r3)
    stw r23, OS_CONTEXT_R23(r3)
    stw r24, OS_CONTEXT_R24(r3)
    stw r25, OS_CONTEXT_R25(r3)
    stw r26, OS_CONTEXT_R26(r3)
    stw r27, OS_CONTEXT_R27(r3)
    stw r28, OS_CONTEXT_R28(r3)
    stw r29, OS_CONTEXT_R29(r3)
    stw r30, OS_CONTEXT_R30(r3)
    stw r31, OS_CONTEXT_R31(r3)
    mfsprg r4, 1
    stw r4, OS_CONTEXT_R3(r3)
    mfsprg r4, 2
    stw r4, OS_CONTEXT_SRR0(r3)
    mfsprg r4, 3
    stw r4, OS_CONTEXT_SRR1(r3)
    stfd f0, OS_CONTEXT_FPR0(r3)
    stfd f1, OS_CONTEXT_FPR1(r3)
    stfd f2, OS_CONTEXT_FPR2(r3)
    stfd f3, OS_CONTEXT_FPR3(r3)
    stfd f4, OS_CONTEXT_FPR4(r3)
    stfd f5, OS_CONTEXT_FPR5(r3)
    stfd f6, OS_CONTEXT_FPR6(r3)
    stfd f7, OS_CONTEXT_FPR7(r3)
    stfd f8, OS_CONTEXT_FPR8(r3)
    stfd f9, OS_CONTEXT_FPR9(r3)
    stfd f10, OS_CONTEXT_FPR10(r3)
    stfd f11, OS_CONTEXT_FPR11(r3)
    stfd f12, OS_CONTEXT_FPR12(r3)
    stfd f13, OS_CONTEXT_FPR13(r3)
    stfd f14, OS_CONTEXT_FPR14(r3)
    stfd f15, OS_CONTEXT_FPR15(r3)
    stfd f16, OS_CONTEXT_FPR16(r3)
    stfd f17, OS_CONTEXT_FPR17(r3)
    stfd f18, OS_CONTEXT_FPR18(r3)
    stfd f19, OS_CONTEXT_FPR19(r3)
    stfd f20, OS_CONTEXT_FPR20(r3)
    stfd f21, OS_CONTEXT_FPR21(r3)
    stfd f22, OS_CONTEXT_FPR22(r3)
    stfd f23, OS_CONTEXT_FPR23(r3)
    stfd f24, OS_CONTEXT_FPR24(r3)
    stfd f25, OS_CONTEXT_FPR25(r3)
    stfd f26, OS_CONTEXT_FPR26(r3)
    stfd f27, OS_CONTEXT_FPR27(r3)
    stfd f28, OS_CONTEXT_FPR28(r3)
    stfd f29, OS_CONTEXT_FPR29(r3)
    stfd f30, OS_CONTEXT_FPR30(r3)
    stfd f31, OS_CONTEXT_FPR31(r3)
    mffs f0
    stfd f0, OS_CONTEXT_FPSCR(r3)
    sync
    lfd f0, OS_CONTEXT_FPR0(r3)
    mtfsb0 FPSCR_FX_BIT
    mtfsb0 FPSCR_ZX_BIT
    mfspr r4, GQR0
    stw r4, OS_CONTEXT_GQR0(r3)
    mfspr r4, GQR1
    stw r4, OS_CONTEXT_GQR1(r3)
    mfspr r4, GQR2
    stw r4, OS_CONTEXT_GQR2(r3)
    mfspr r4, GQR3
    stw r4, OS_CONTEXT_GQR3(r3)
    mfspr r4, GQR4
    stw r4, OS_CONTEXT_GQR4(r3)
    mfspr r4, GQR5
    stw r4, OS_CONTEXT_GQR5(r3)
    mfspr r4, GQR6
    stw r4, OS_CONTEXT_GQR6(r3)
    mfspr r4, GQR7
    stw r4, OS_CONTEXT_GQR7(r3)
    mfcr r4
    stw r4, OS_CONTEXT_CR(r3)
    mflr r4
    stw r4, OS_CONTEXT_LR(r3)
    mfctr r4
    stw r4, OS_CONTEXT_CTR(r3)
    mfxer r4
    stw r4, OS_CONTEXT_XER(r3)
    mfdsisr r5
    mfdar r6
    lwz r4, OS_CONTEXT_SRR1(r3)
    sync
    mtmsr r4
    sync
    lwz r3, _haltExceptionType(r0)
    lis r4, _haltContext@h
    ori r4, r4, _haltContext@l
    b GSgfxHalt
}

void OSPanic(const char* file, int line, const char* msg, ...) {}

void GSgfxHalt(OSError error, OSContext* context, ...) {}

void GSgfxHaltInit() {
    memset(&_haltContext, 0xff, sizeof(_haltContext));
    OSSetErrorHandler(OS_ERROR_DSI, GSgfxHalt);
    OSSetErrorHandler(OS_ERROR_ISI, GSgfxHalt);
    OSSetErrorHandler(OS_ERROR_ALIGNMENT, GSgfxHalt);
    OSSetErrorHandler(__OS_EXCEPTION_MEMORY_PROTECTION, GSgfxHalt);
    __OSFpscrEnableBits = FPSCR_ZE;
    OSSetErrorHandler(__OS_EXCEPTION_FLOATING_POINT_EXCEPTION, GSgfxHalt);
    BOOL enabled = OSDisableInterrupts();
    memcpy(_haltOldDSIHandler, (void*)0x80000300, sizeof(_haltOldDSIHandler));
    memcpy(_haltOldISIHandler, (void*)0x80000400, sizeof(_haltOldISIHandler));
    memcpy(_haltOldAlignmentHandler, (void*)0x80000600, sizeof(_haltOldAlignmentHandler));
    memcpy(_haltOldProgramHandler, (void*)0x80000700, sizeof(_haltOldProgramHandler));
    memcpy((void*)0x80000300, (void*)_haltDSIHandler, sizeof(_haltOldDSIHandler));
    memcpy((void*)0x80000400, (void*)_haltISIHandler, sizeof(_haltOldISIHandler));
    memcpy((void*)0x80000600, (void*)_haltAlignmentHandler, sizeof(_haltOldAlignmentHandler));
    DCFlushRange((void*)0x80000300, 4096);
    _haltInitialized = 1;
    OSRestoreInterrupts(enabled);
}
