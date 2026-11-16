/* PowerPC EABI variadic argument and aggregate-copy helpers. */

#include <stdarg.h>

/* The variadic function's prologue saves the argument registers into the
 * register-save area: one 4-byte slot for each general-purpose register,
 * followed by the floating-point registers at the next 8-byte boundary.  A
 * double-word argument occupies an even-aligned pair of general registers, so
 * slot 6 is the last usable pair start and slot 7 is excluded because its pair
 * would run past the saved general registers.  A counter that reaches the slot
 * count marks the bank as exhausted. */
#define ARG_REG_SLOTS        8
#define ARG_GPR_SLOT_SIZE    4
#define ARG_FPR_SLOT_SIZE    8
#define ARG_DOUBLEWORD_ALIGN (2 * ARG_GPR_SLOT_SIZE)
#define ARG_PAIR_SLOT_LIMIT  (ARG_REG_SLOTS - 1)
#define ARG_FPR_SAVE_OFFSET  (ARG_REG_SLOTS * ARG_GPR_SLOT_SIZE)

void* __va_arg(va_list v_list, ArgumentClass type) {
    char* counter = &v_list->gpr;
    int registerIndex = v_list->gpr;
    char* result;
    int limit = ARG_REG_SLOTS;
    int alignment = ARG_GPR_SLOT_SIZE;
    int increment = 1;
    int adjustment = 0;
    int offset = 0;
    int slotSize = ARG_GPR_SLOT_SIZE;
    if (type == ARG_CLASS_FLOATING_POINT) {
        registerIndex = v_list->fpr;
        counter = &v_list->fpr;
        alignment = ARG_FPR_SLOT_SIZE;
        offset = ARG_FPR_SAVE_OFFSET;
        slotSize = ARG_FPR_SLOT_SIZE;
    }
    if (type == ARG_CLASS_DOUBLE_WORD) {
        alignment = ARG_DOUBLEWORD_ALIGN;
        limit = ARG_PAIR_SLOT_LIMIT;
        if (registerIndex & 1) {
            adjustment = 1;
        }
        increment = 2;
    }
    if (registerIndex < limit) {
        registerIndex += adjustment;
        result = v_list->reg_save_area + offset + registerIndex * slotSize;
        *counter = registerIndex + increment;
    } else {
        *counter = ARG_REG_SLOTS;
        result = (char*)(((unsigned long)v_list->input_arg_area + (alignment - 1)) & ~(alignment - 1));
        v_list->input_arg_area = result + alignment;
    }
    if (type == ARG_CLASS_INDIRECT) {
        result = *(char**)result;
    }
    return result;
}

void* __copy(void* dest, const void* source, unsigned long size) {
    char* d;
    const char* s;

    if (dest == 0) {
        return dest;
    }
    if (size == 0) {
        return dest;
    }

    d = dest;
    s = source;
    do {
        *d++ = *s++;
    } while (--size != 0);

    return dest;
}
