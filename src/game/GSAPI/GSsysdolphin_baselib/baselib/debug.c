#include <dolphin/os.h>
#include <hsd/debug.h>

/* CPU state captured for the panic callback. */
static OSContext debug_context;

/* Optional crash handler run before calling OSPanic. */
static void (*panic_func)(OSContext *context, ...);

static asm void HSD_SaveContext(void)
{
    nofralloc
    // Park the caller's r3 in SPRG0 before r3 becomes the context base.
    mtsprg 0, r3
    lis r3, debug_context@ha
    addi r3, r3, debug_context@l
    stmw r0, OS_CONTEXT_R0(r3)
    mfsprg r4, 0
    stw r4, OS_CONTEXT_R3(r3)
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
    mfsrr0 r4
    stw r4, OS_CONTEXT_SRR0(r3)
    mfsrr1 r4
    stw r4, OS_CONTEXT_SRR1(r3)
    // The floating point registers are captured by OSFillFPUContext.
    lhz r4, OS_CONTEXT_STATE(r3)
    ori r4, r4, OS_CONTEXT_STATE_FPSAVED
    sth r4, OS_CONTEXT_STATE(r3)
    b OSFillFPUContext
}

void HSD_Panic(const char *file, int line, const char *message)
{
    if (panic_func != NULL) {
        HSD_SaveContext();
        OSReport("%s in %s on line %d.\n", message, file, line);
        panic_func(&debug_context);
    }
    OSPanic(file, line, message);
}

void __assert(const char *file, int line, const char *expression)
{
    OSReport("assertion \"%s\" failed", expression);
    HSD_Panic(file, line, "");
}
