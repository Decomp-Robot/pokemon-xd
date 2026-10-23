#include <types.h>

/* True for the five slots saved in the ordinary ".DSV1" layout.  Slots 5-9 use
   the padded ".DSV2" layout instead, so they take the normal save path. */
u8 _memcard_debug_type_check(s32 type) {
	switch (type) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return 1;
	default:
		return 0;
	}
}
