#include <types.h>

s32 dbgMenuGetGsinputMsgId(s32 id) {
	if (id == 0) {
		return 0xED04;
	} else if (id == 1) {
		return 0xED05;
	} else if (id == 2) {
		return 0xED06;
	} else if (id == 3) {
		return 0xED07;
	} else if (id == 4) {
		return 0xED08;
	} else {
		return 0xED04;
	}
}
