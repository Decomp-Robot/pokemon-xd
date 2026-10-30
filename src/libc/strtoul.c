#include "strtoul.h"

#include <libc/limits.h>

#include "ctype_inline.h"
#include "errno.h"
#include "scanf.h"

/* Value the library leaves in errno when the converted value does not fit. */
#define ERANGE 34

enum scan_states {
    start = 0x01,
    check_for_zero = 0x02,
    leading_zero = 0x04,
    need_digit = 0x08,
    digit_loop = 0x10,
    finished = 0x20,
    failure = 0x40
};

#define final_state(scan_state) (scan_state & (finished | failure))
#define success(scan_state) (scan_state & (leading_zero | digit_loop | finished))
#define fetch() (count++, (*readCharacter)(context, 0, MSL_INPUT_READ))
#define unfetch(character) (*readCharacter)(context, character, MSL_INPUT_UNREAD)

int atoi(const char *text)
{
    MSLStringInput input;
    int overflow;
    int negative;
    int charsScanned;
    unsigned long value;

    input.cursor = text;
    input.endOfString = 0;
    value = __strtoul(10, INT_MAX, __StringRead, &input,
                      &charsScanned, &negative, &overflow);
    /* Out-of-range input saturates to the limits of the return type. */
    if (overflow || (!negative && value > INT_MAX) ||
        (negative && value > (unsigned long)INT_MAX + 1)) {
        value = negative ? (unsigned long)INT_MAX + 1 : INT_MAX;
        errno = ERANGE;
        return (int)value;
    }
    if (negative) {
        return -(int)value;
    }
    return (int)value;
}

unsigned long __strtoul(int base, int maxWidth,
                        int (*readCharacter)(void *context, int character, int operation),
                        void *context, int *charsScanned, int *negative, int *overflow)
{
    int scan_state = start;
    int count = 0;
    int spaces = 0;
    unsigned long value = 0;
    unsigned long value_max = 0;
    int c;

    *negative = *overflow = 0;

    if (base < 0 || base == 1 || base > 36 || maxWidth < 1) {
        scan_state = failure;
    } else {
        c = fetch();
    }

    if (base != 0) {
        value_max = ULONG_MAX / base;
    }

    while (count <= maxWidth && c != -1 && !final_state(scan_state)) {
        switch (scan_state) {
        case start:
            if (isspace(c)) {
                c = fetch();
                count--;
                spaces++;
                break;
            }

            if (c == '+') {
                c = fetch();
            } else if (c == '-') {
                c = fetch();
                *negative = 1;
            }

            scan_state = check_for_zero;
            break;

        case check_for_zero:
            if (base == 0 || base == 16) {
                if (c == '0') {
                    scan_state = leading_zero;
                    c = fetch();
                    break;
                }
            }

            scan_state = need_digit;
            break;

        case leading_zero:
            if (c == 'X' || c == 'x') {
                base = 16;
                scan_state = need_digit;
                c = fetch();
                break;
            }

            if (base == 0) {
                base = 8;
            }

            scan_state = digit_loop;
            break;

        case need_digit:
        case digit_loop:
            if (base == 0) {
                base = 10;
            }

            if (!value_max) {
                value_max = ULONG_MAX / base;
            }

            if (isdigit(c)) {
                if ((c -= '0') >= base) {
                    if (scan_state == digit_loop) {
                        scan_state = finished;
                    } else {
                        scan_state = failure;
                    }

                    c += '0';
                    break;
                }
            } else if (!isalpha(c) || (toupper(c) - 'A' + 10) >= base) {
                if (scan_state == digit_loop) {
                    scan_state = finished;
                } else {
                    scan_state = failure;
                }

                break;
            } else {
                c = toupper(c) - 'A' + 10;
            }

            if (value > value_max) {
                *overflow = 1;
            }

            value *= base;

            if (c > (ULONG_MAX - value)) {
                *overflow = 1;
            }

            value += c;
            scan_state = digit_loop;
            c = fetch();
            break;
        }
    }

    if (!success(scan_state)) {
        count = 0;
        value = 0;
        *charsScanned = 0;
    } else {
        count--;
        *charsScanned = count + spaces;
    }

    unfetch(c);
    return value;
}
