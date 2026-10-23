#include "nub.h"

/* File requests enter the host debugger through an illegal instruction. */
#pragma force_active on

asm u32 TRKAccessFile(u8 command, u32 fileHandle, u32* length,
                      u8* buffer) {
	nofralloc
	twi 31,r0,0
	blr
}

asm u32 TRKOpenFile(u8 command, const char* pathname, u8 openFlags,
                    u32* fileHandleOut) {
	nofralloc
	twi 31,r0,0
	blr
}

asm u32 TRKCloseFile(u8 command, u32 fileHandle) {
	nofralloc
	twi 31,r0,0
	blr
}

asm u32 TRKPositionFile(u8 command, u32 fileHandle, u32* position,
                        u8 positioningMode) {
	nofralloc
	twi 31,r0,0
	blr
}

#pragma force_active reset

