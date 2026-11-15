#include "strtold.h"

#include <libc/limits.h>
#include <math.h>

#include "ansi_fp.h"
#include "ctype_inline.h"
#include "errno.h"
#include "float_data.h"
#include "locale.h"
#include "scanf.h"

#define ERANGE 34

enum scan_states {
    start = 0x0001,
    sig_start = 0x0002,
    leading_sig_zeroes = 0x0004,
    int_digit_loop = 0x0008,
    frac_start = 0x0010,
    frac_digit_loop = 0x0020,
    sig_end = 0x0040,
    exp_start = 0x0080,
    leading_exp_digit = 0x0100,
    leading_exp_zeroes = 0x0200,
    exp_digit_loop = 0x0400,
    finished = 0x0800,
    failure = 0x1000,
    nan_state = 0x2000,
    infin_state = 0x4000,
    hex_state = 0x8000
};

enum hex_scan_states {
    not_hex = 0,
    hex_start = 1,
    hex_leading_sig_zeroes = 2,
    hex_int_digit_loop = 4,
    hex_frac_digit_loop = 8,
    hex_sig_end = 16,
    hex_exp_start = 32,
    hex_leading_exp_digit = 64,
    hex_leading_exp_zeroes = 128,
    hex_exp_digit_loop = 256
};

enum {
    decimal_digit_limit = 20,
    nan_payload_limit = 32,
    result_byte_count = 8,
    first_hex_nibble = 2,
    hex_nibble_limit = 17,
    bits_per_nibble = 4,
    normalization_mask = 0xF0,
    normalized_leading_bit = 0x10,
    significand_head_mask = 0xF,
    double_exponent_bias = 1023,
    double_sign_mask = 0x8000
};

#define final_state(scan_state) (scan_state & (finished | failure))
#define success(scan_state) \
    (scan_state &           \
     (leading_sig_zeroes | int_digit_loop | frac_digit_loop | leading_exp_zeroes | exp_digit_loop | finished))
#define fetch() (count++, (*ReadProc)(ReadProcArg, 0, MSL_INPUT_READ))
#define unfetch(c) (*ReadProc)(ReadProcArg, c, MSL_INPUT_UNREAD)

long double __strtold(int max_width, int (*ReadProc)(void *, int, int), void *ReadProcArg,
                     int *chars_scanned, int *overflow) {
    int scan_state = start;
    int hex_scan_state = not_hex;
    int count = 0;
    int spaces = 0;
    int c;
    Dec d = {0, 0, 0, {0}};
    int sig_negative = 0;
    int exp_negative = 0;
    long exp_value = 0;
    int exp_adjust = 0;
    long double result;
    int sign_detected = 0;
    unsigned char *significand_bytes = (unsigned char *)&result;
    unsigned char significand_byte, digit;
    int digit_value;
    int byte_index;
    int nibble_index;
    int hex_exp_negative = 0;
    int exp_digits = 0;
    int integer_digits = 0;
    short hex_exponent = 0;
    int radix_marker = *(unsigned char *)__lconv.decimal_point;

    *overflow = 0;
    c = fetch();

    while (count <= max_width && c != -1 && !final_state(scan_state)) {
        switch (scan_state) {
            case start:
                if (isspace(c)) {
                    c = fetch();
                    count--;
                    spaces++;
                    break;
                }

                switch (toupper(c)) {
                    case '-':
                        sig_negative = 1;

                    case '+':
                        c = fetch();
                        sign_detected = 1;
                        break;
                    case 'I':
                        c = fetch();
                        scan_state = infin_state;
                        break;

                    case 'N':
                        c = fetch();
                        scan_state = nan_state;
                        break;

                    default:
                        scan_state = sig_start;
                        break;
                }
                break;

            case infin_state: {
                int i = 1;
                char model[] = "INFINITY";

                while ((i < 8) && (toupper(c) == model[i])) {
                    i++;
                    c = fetch();
                }

                if ((i == 3) || (i == 8)) {
                    if (sig_negative) {
                        result = -__float_huge[0];
                    } else {
                        result = __float_huge[0];
                    }

                    *chars_scanned = spaces + i + sign_detected;
                    return result;
                } else {
                    scan_state = failure;
                }

                break;
            }

            case nan_state: {
                int i = 1, j = 0;
                char model[] = "NAN(";

                while ((i < 4) && (toupper(c) == model[i])) {
                    i++;
                    c = fetch();
                }

                if ((i == 3) || (i == 4)) {
                    if (i == 4) {
                        while ((j < nan_payload_limit) && (isdigit(c) || isalpha(c))) {
                            j++;
                            c = fetch();
                        }

                        if (c != ')') {
                            scan_state = failure;
                            break;
                        } else {
                            j++;
                        }
                    }

                    if (sig_negative) {
                        result = -__float_nan[0];
                    } else {
                        result = __float_nan[0];
                    }

                    *chars_scanned = spaces + i + j + sign_detected;
                    return result;
                } else {
                    scan_state = failure;
                }
                break;
            }

            case sig_start:
                if (c == radix_marker) {
                    scan_state = frac_start;
                    c = fetch();
                    break;
                }
                if (!isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                if (c == '0') {
                    c = fetch();
                    if (toupper(c) == 'X') {
                        scan_state = hex_state;
                        hex_scan_state = hex_start;
                    } else {
                        scan_state = leading_sig_zeroes;
                    }
                    break;
                }

                scan_state = int_digit_loop;
                break;

            case leading_sig_zeroes:
                if (c == '0') {
                    c = fetch();

                    break;
                }
                scan_state = int_digit_loop;
                break;

            case int_digit_loop:
                if (!isdigit(c)) {
                    if (c == radix_marker) {
                        scan_state = frac_digit_loop;
                        c = fetch();
                    } else {
                        scan_state = sig_end;
                    }
                    break;
                }
                if (d.len < decimal_digit_limit) {
                    d.digits[d.len++] = (unsigned char)c;
                } else {
                    exp_adjust++;
                }

                c = fetch();
                break;

            case frac_start:
                if (!isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                scan_state = frac_digit_loop;
                break;

            case frac_digit_loop:
                if (!isdigit(c)) {
                    scan_state = sig_end;
                    break;
                }

                if (d.len < decimal_digit_limit) {
                    if (c != '0' || d.len) {
                        d.digits[d.len++] = (unsigned char)c;
                    }

                    exp_adjust--;
                }
                c = fetch();
                break;

            case sig_end:
                if (toupper(c) == 'E') {
                    scan_state = exp_start;
                    c = fetch();
                    break;
                }
                scan_state = finished;
                break;

            case exp_start:
                if (c == '+') {
                    c = fetch();
                } else if (c == '-') {
                    c = fetch();
                    exp_negative = 1;
                }

                scan_state = leading_exp_digit;
                break;

            case leading_exp_digit:
                if (!isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                if (c == '0') {
                    scan_state = leading_exp_zeroes;
                    c = fetch();
                    break;
                }

                scan_state = exp_digit_loop;
                break;

            case leading_exp_zeroes:
                if (c == '0') {
                    c = fetch();
                    break;
                }

                scan_state = exp_digit_loop;
                break;

            case exp_digit_loop:
                if (!isdigit(c)) {
                    scan_state = finished;
                    break;
                }

                exp_value = exp_value * 10 + (c - '0');
                if (exp_value > SHRT_MAX) {
                    *overflow = 1;
                }

                c = fetch();
                break;

            case hex_state: {
                switch (hex_scan_state) {
                    case hex_start:
                        for (byte_index = 0; byte_index < result_byte_count; byte_index++)
                            significand_bytes[byte_index] = '\0';
                        nibble_index = first_hex_nibble;
                        hex_scan_state = hex_leading_sig_zeroes;
                        c = fetch();
                        break;

                    case hex_leading_sig_zeroes:
                        if (c == '0') {
                            c = fetch();
                            break;
                        }

                        hex_scan_state = hex_int_digit_loop;
                        break;

                    case hex_int_digit_loop:
                        if (!isxdigit(c)) {
                            if (c == radix_marker) {
                                hex_scan_state = hex_frac_digit_loop;
                                c = fetch();
                            }

                            else {
                                hex_scan_state = hex_sig_end;
                            }
                            break;
                        }

                        if (nibble_index < hex_nibble_limit) {
                            integer_digits++;
                            significand_byte = *(significand_bytes + nibble_index / 2);

                            digit_value = toupper(c);
                            if (digit_value >= 'A') {
                                digit_value = digit_value - 'A' + 10;
                            } else {
                                digit_value -= '0';
                            }

                            digit = (unsigned char)digit_value;

                            if ((nibble_index % 2) != 0) {
                                significand_byte |= digit;
                            } else {
                                significand_byte |= digit << bits_per_nibble;
                            }

                            *(significand_bytes + nibble_index++ / 2) = significand_byte;
                            c = fetch();
                        }

                        else {
                            c = fetch();
                        }

                        break;

                    case hex_frac_digit_loop:
                        if (!isxdigit(c)) {
                            hex_scan_state = hex_sig_end;
                            break;
                        }

                        if (nibble_index < hex_nibble_limit) {
                            significand_byte = *(significand_bytes + nibble_index / 2);
                            digit_value = toupper(c);

                            if (digit_value >= 'A') {
                                digit_value = digit_value - 'A' + 10;
                            } else {
                                digit_value -= '0';
                            }

                            digit = (unsigned char)digit_value;

                            if ((nibble_index % 2) != 0) {
                                significand_byte |= digit;
                            } else {
                                significand_byte |= digit << bits_per_nibble;
                            }

                            *(significand_bytes + nibble_index++ / 2) = significand_byte;
                            c = fetch();
                        } else {
                            c = fetch();
                        }
                        break;

                    case hex_sig_end:
                        if (toupper(c) == 'P') {
                            hex_scan_state = hex_exp_start;
                            exp_digits++;
                            c = fetch();
                        } else {
                            scan_state = finished;
                        }

                        break;

                    case hex_exp_start:
                        exp_digits++;
                        if (c == '-') {
                            hex_exp_negative = 1;
                        } else if (c != '+') {
                            c = unfetch(c);
                            exp_digits--;
                        }

                        hex_scan_state = hex_leading_exp_digit;
                        c = fetch();
                        break;

                    case hex_leading_exp_digit:
                        if (!isdigit(c)) {
                            scan_state = failure;
                            break;
                        }

                        if (c == '0') {
                            exp_digits++;
                            hex_scan_state = hex_leading_exp_zeroes;
                            c = fetch();
                            break;
                        }

                        hex_scan_state = hex_exp_digit_loop;
                        break;

                    case hex_exp_digit_loop:
                        if (!isdigit(c)) {
                            scan_state = finished;
                            break;
                        }

                        hex_exponent = hex_exponent * 10 + (c - '0');

                        if (exp_value > SHRT_MAX) {
                            *overflow = 1;
                        }

                        exp_digits++;
                        c = fetch();

                        break;
                }
            } break;
        }
    }

    if (!success(scan_state)) {
        count = 0;
        *chars_scanned = 0;
    } else {
        count--;
        *chars_scanned = count + spaces;
    }
    unfetch(c);

    if (hex_scan_state == not_hex) {
        if (exp_negative)
            exp_value = -exp_value;
        {
            int n = d.len;
            unsigned char *p = &d.digits[n];
            while (n-- && *--p == '0')
                exp_adjust++;
            d.len = (unsigned char)(n + 1);
            if (d.len == 0)
                d.digits[d.len++] = '0';
        }
        exp_value += exp_adjust;
        if (exp_value < SHRT_MIN || exp_value > SHRT_MAX)
            *overflow = 1;
        if (*overflow) {
            if (exp_negative)
                return 0.0;
            else
                return sig_negative ? -conversionInfinity[0] : conversionInfinity[0];
        }
        d.exp = (short)exp_value;
        result = __dec2num(&d);
        if (result != 0.0 && result < conversionMinNormal[0])
            *overflow = 1;
        else if (result > conversionMaxFinite[0]) {
            *overflow = 1;
            result = conversionInfinity[0];
        }
        if (sig_negative && success(scan_state))
            result = -result;
        return result;
    } else {
        if (result) {
            if (hex_exp_negative)
                hex_exponent = -hex_exponent;
            while ((*(short *)&result & normalization_mask) != normalized_leading_bit) {
                *(unsigned long long *)&result >>= 1;
                hex_exponent++;
            }
            hex_exponent += (integer_digits - 1) * bits_per_nibble;
            *(short *)&result &= significand_head_mask;
            *(short *)&result |= (hex_exponent + double_exponent_bias) << bits_per_nibble;
            *chars_scanned = spaces + sign_detected + nibble_index + 1 + exp_digits;
            if (result != 0.0 && result < conversionMinNormal[0]) {
                *overflow = 1;
                result = 0.0;
            } else if (result > conversionMaxFinite[0]) {
                *overflow = 1;
                result = conversionInfinity[0];
            }
            if (sig_negative)
                *(short *)&result |= double_sign_mask;
        } else {
            result = 0.0;
        }
        return result;
    }
}

double atof(const char *str) {
    long double value, abs_value;
    int overflow, count;
    MSLStringInput isc;
    isc.cursor = str;
    isc.endOfString = 0;
    value = __strtold(INT_MAX, __StringRead, &isc, &count, &overflow);
    abs_value = fabs(value);
    if (overflow || (value != 0.0 && (abs_value < doubleMinNormal[0] || abs_value > doubleMaxFinite[0])))
        errno = ERANGE;
    return value;
}

