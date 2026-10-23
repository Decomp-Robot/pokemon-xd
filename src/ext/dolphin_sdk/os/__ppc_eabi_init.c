#include <stdlib.h>
#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

__declspec(section ".ctors") extern void (* _ctors[])();

static void __init_cpp(void);

/* Startup routines linked in .init ahead of the TRK interrupt vector table.
 * __init_hardware enables the floating-point unit and brings up the processor support, FPR, and cache modules; __flush_cache pushes a freshly
 * copied ROM section out of the data cache and invalidates the matching
 * instruction-cache lines so the CPU executes what was just loaded
 * (__start.c:__copy_rom_section calls it after every ROM copy). */
__declspec(section ".init") asm void __init_hardware(void)
{ // clang-format off
    nofralloc
    mfmsr r0
    ori  r0,r0,MSR_FP
    mtmsr r0
    mflr    r31
    bl      __OSPSInit
    bl      __OSFPRInit
    bl      __OSCacheInit
    mtlr    r31
    blr
}

__declspec(section ".init") asm void __flush_cache(void *address, unsigned int size)
{ // clang-format off
    nofralloc
    lis     r5, 0xffff
    ori     r5, r5, 0xfff1
    and     r5, r5, r3
    subf    r3, r5, r3
    add     r4, r4, r3
rept:
    dcbst   0,r5
    sync
    icbi    0,r5
    addic   r5,r5,0x8
    subic.  r4,r4,0x8
    bge     rept
    isync
    blr
}

void __init_user(void) {
    __init_cpp();
}

static void __init_cpp(void) {
	void (* * constructor)();

	/*
	 *	call static initializers
	 */
	for (constructor = _ctors; *constructor; constructor++) {
		(*constructor)();
	}
}

void _ExitProcess(void) {
    PPCHalt();
}
