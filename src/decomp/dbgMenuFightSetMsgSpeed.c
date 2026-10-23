#include <types.h>

extern u8 lbl_804E7E20;

void dbgMenuFightSetMsgSpeed(u8 speed) {
	if (speed < 1) {
		speed = 1;
	}
	if (speed > 9) {
		speed = 9;
	}
	lbl_804E7E20 = speed;
}
