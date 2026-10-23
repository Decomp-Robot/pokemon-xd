#include <types.h>

extern u8 gDbgMenuFightMsgSpeed;

/* Debug message speed setting; values outside 1..9 clamp to the nearest end. */
void dbgMenuFightSetMsgSpeed(u8 speed) {
	if (speed < 1) {
		speed = 1;
	}
	if (speed > 9) {
		speed = 9;
	}
	gDbgMenuFightMsgSpeed = speed;
}
