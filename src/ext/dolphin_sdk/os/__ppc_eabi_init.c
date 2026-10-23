#include <stdlib.h>
#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

__declspec(section ".ctors") extern void (* _ctors[])();

static void __init_cpp(void);

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
