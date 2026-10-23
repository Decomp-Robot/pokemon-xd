#include <types.h>

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
