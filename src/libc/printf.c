/* Formatted text output: the printf family and the conversions behind it. */

#include <stdlib.h>
#include <string.h>

#include <libc/stdarg.h>
#include <libc/stdio.h>

#include "ansi_fp.h"
#include "critical_regions.h"
#include "ctype_inline.h"
#include "file.h"
#include "mem.h"

/* Longest conversion this implementation produces. */
#define CONVERSION_BUFF_SIZE 509

/* Conversion character stored for a format the parser rejects. */
#define BAD_CONVERSION 0xFF

/* How a converted value sits inside its field. */
enum justification_options {
    left_justification,
    right_justification,
    zero_fill
};

/* Sign shown in front of a converted value. */
enum sign_options {
    only_minus,
    sign_always,
    space_holder
};

/* Length modifier of a conversion. */
enum argument_options {
    normal_argument,
    char_argument,
    short_argument,
    long_argument,
    long_long_argument,
    long_double_argument,
    wchar_argument
};

/* One parsed conversion specification. */
typedef struct PrintfFormat {
    unsigned char pad;
    unsigned char sign;
    unsigned char precision_given;
    unsigned char alternate;
    unsigned char arg_type;
    unsigned char conversion;
    int field_width;
    int precision;
} PrintfFormat;

/* Destination handed to __StringWrite by sprintf and vsprintf. */
typedef struct StringWriteRecord {
    char* CharBuffer;
    unsigned int MaxChars;
    unsigned int NumChars;
} StringWriteRecord;

/* Writes count bytes of buffer, returns 0 when nothing could be written. */
typedef void* (*WriteProc)(void* arg, const char* buffer, int count);

static int __pformatter(WriteProc write_proc, void* write_arg, const char* format, va_list arg);
static char* float2str(double x, char* buff, PrintfFormat info);
static char* double2hex(double x, char* buff, PrintfFormat info);

void* __StringWrite(void* arg, const char* buffer, int count);
void* __FileWrite(void* arg, const char* buffer, int count);
void round_decimal(Dec* d, int digits);
static char* longlong2str(long long num, char* buff, PrintfFormat info);
static char* long2str(long num, char* buff, PrintfFormat info);
static const char* parse_format(const char* format, va_list arg, PrintfFormat* out);

int sprintf(char* dest, const char* format, ...)
{
    va_list args;
    StringWriteRecord rec;
    unsigned int count;

    va_start(args, format);

    rec.CharBuffer = dest;
    rec.MaxChars = (unsigned int)-1;
    rec.NumChars = 0;
    count = (unsigned int)__pformatter(__StringWrite, &rec, format, args);
    va_end(args);

    if (dest != 0) {
        unsigned int index = (unsigned int)-2;

        if (count < (unsigned int)-1) {
            index = count;
        }
        dest[index] = 0;
    }

    return count;
}

int vsprintf(char* dest, const char* format, va_list arg)
{
    StringWriteRecord rec;
    unsigned int count;

    rec.CharBuffer = dest;
    rec.MaxChars = (unsigned int)-1;
    rec.NumChars = 0;

    count = (unsigned int)__pformatter(__StringWrite, &rec, format, arg);

    if (dest != 0) {
        unsigned int index = (unsigned int)-2;

        if (count < (unsigned int)-1) {
            index = count;
        }
        dest[index] = 0;
    }

    return count;
}

int vprintf(const char* format, va_list arg)
{
    FILE* stream = &__files[1];
    int count;

    if (fwide(stream, -1) >= 0) {
        return -1;
    }

    __begin_critical_region(stdio_access);
    count = __pformatter(__FileWrite, stream, format, arg);
    __end_critical_region(stdio_access);

    return count;
}

int printf(const char* format, ...)
{
    va_list args;
    FILE* stream = &__files[1];
    int count;

    if (fwide(stream, -1) >= 0) {
        return -1;
    }

    __begin_critical_region(stdio_access);
    va_start(args, format);
    count = __pformatter(__FileWrite, stream, format, args);
    __end_critical_region(stdio_access);
    va_end(args);

    return count;
}

/* Appends the formatted text to the caller's buffer. */
void* __StringWrite(void* arg, const char* buffer, int count)
{
    StringWriteRecord* rec = (StringWriteRecord*)arg;
    unsigned int num;

    num = rec->MaxChars - rec->NumChars;

    if (rec->NumChars + count <= rec->MaxChars) {
        num = count;
    }

    memcpy(rec->CharBuffer + rec->NumChars, buffer, num);
    rec->NumChars += num;

    return (void*)1;
}

/* Sends the formatted text to a stream. */
void* __FileWrite(void* arg, const char* buffer, int count)
{
    if (fwrite(buffer, 1, count, (FILE*)arg) == (unsigned int)count) {
        return arg;
    }

    return 0;
}

static int __pformatter(WriteProc write_proc, void* write_arg, const char* format, va_list arg)
{
    char pad_char = ' ';
    char space;
    PrintfFormat info;
    char buf[512];
    const char* s = format;
    const char* q;
    char* dest;
    int count;
    int count_local;
    long num;
    long long long_num;
    int total = 0;

    while (*s != 0) {
        q = strchr(s, '%');

        if (q == 0) {
            count = strlen(s);
            total += count;

            if (count != 0 && !write_proc(write_arg, s, count)) {
                return -1;
            }

            break;
        }

        count = q - s;
        total += count;

        if (count != 0 && !write_proc(write_arg, s, count)) {
            return -1;
        }

        s = parse_format(q, arg, &info);

        switch (info.conversion) {
        case 'd':
        case 'i': {
            if (info.arg_type == long_argument) {
                num = va_arg(arg, long);
            } else if (info.arg_type == long_long_argument) {
                long_num = va_arg(arg, long long);
            } else {
                num = va_arg(arg, int);
            }

            if (info.arg_type == short_argument) {
                num = (short)num;
            }

            if (info.arg_type == char_argument) {
                num = (signed char)num;
            }

            if (info.arg_type == long_long_argument) {
                if ((dest = longlong2str(long_num, buf + sizeof(buf), info)) == 0) {
                    goto bad_conversion;
                }
            } else {
                if ((dest = long2str(num, buf + sizeof(buf), info)) == 0) {
                    goto bad_conversion;
                }
            }

            count = buf + sizeof(buf) - 1 - dest;
            break;
        }

        case 'X':
        case 'o':
        case 'u':
        case 'x': {
            if (info.arg_type == long_argument) {
                num = va_arg(arg, unsigned long);
            } else if (info.arg_type == long_long_argument) {
                long_num = va_arg(arg, unsigned long long);
            } else {
                num = va_arg(arg, unsigned int);
            }

            if (info.arg_type == short_argument) {
                num = (unsigned short)num;
            }

            if (info.arg_type == char_argument) {
                num = (unsigned char)num;
            }

            if (info.arg_type == long_long_argument) {
                if ((dest = longlong2str(long_num, buf + sizeof(buf), info)) == 0) {
                    goto bad_conversion;
                }
            } else {
                if ((dest = long2str(num, buf + sizeof(buf), info)) == 0) {
                    goto bad_conversion;
                }
            }

            count = buf + sizeof(buf) - 1 - dest;
            break;
        }

        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G': {
            double num;

            if (info.arg_type == long_double_argument) {
                num = va_arg(arg, long double);
            } else {
                num = va_arg(arg, double);
            }

            if ((dest = float2str(num, buf + sizeof(buf), info)) == 0) {
                goto bad_conversion;
            }

            count = buf + sizeof(buf) - 1 - dest;
            break;
        }

        case 'a':
        case 'A': {
            double num;

            if (info.arg_type == long_double_argument) {
                num = va_arg(arg, long double);
            } else {
                num = va_arg(arg, double);
            }

            if ((dest = double2hex(num, buf + sizeof(buf), info)) == 0) {
                goto bad_conversion;
            }

            count = buf + sizeof(buf) - 1 - dest;
            break;
        }

        case 's': {
            if (info.arg_type == wchar_argument) {
                const wchar_t* wide = va_arg(arg, wchar_t*);
                int length;

                if (wide == 0) {
                    wide = L"";
                }

                length = wcstombs(buf, wide, sizeof(buf));

                if (length < 0) {
                    goto bad_conversion;
                }

                dest = buf;
            } else {
                dest = va_arg(arg, char*);
            }

            if (dest == 0) {
                dest = "";
            }

            if (info.alternate) {
                count = (unsigned char)*dest++;

                if (info.precision_given && count > info.precision) {
                    count = info.precision;
                }
            } else if (info.precision_given) {
                const char* found;

                count = info.precision;
                found = memchr(dest, 0, count);

                if (found != 0) {
                    count = found - dest;
                }
            } else {
                count = strlen(dest);
            }

            break;
        }

        case 'n':
            dest = va_arg(arg, char*);

            switch (info.arg_type) {
            case normal_argument:
                *(int*)dest = total;
                break;

            case short_argument:
                *(short*)dest = total;
                break;

            case long_argument:
                *(long*)dest = total;
                break;

            case long_long_argument:
                *(long long*)dest = total;
                break;
            }

            continue;

        case 'c':
            dest = buf;
            *dest = va_arg(arg, int);
            count = 1;
            break;

        case '%':
            buf[0] = '%';
            dest = buf;
            count = 1;
            break;

        case BAD_CONVERSION:
        default:
        bad_conversion:
            count = strlen(q);
            total += count;

            if (count != 0 && !write_proc(write_arg, q, count)) {
                return -1;
            }

            return total;
        }

        count_local = count;

        if (info.pad != left_justification) {
            pad_char = (info.pad == zero_fill) ? '0' : ' ';

            if (*dest == '+' || *dest == '-' || *dest == ' ') {
                if (pad_char == '0') {
                    if (!write_proc(write_arg, dest, 1)) {
                        return -1;
                    }

                    ++dest;
                    --count;
                }
            }

            while (count_local < info.field_width) {
                if (!write_proc(write_arg, &pad_char, 1)) {
                    return -1;
                }

                ++count_local;
            }
        }

        if (count != 0 && !write_proc(write_arg, dest, count)) {
            return -1;
        }

        if (info.pad == left_justification) {
            while (count_local < info.field_width) {
                space = ' ';

                if (!write_proc(write_arg, &space, 1)) {
                    return -1;
                }

                ++count_local;
            }
        }

        total += count_local;
    }

    return total;
}

static char* float2str(double x, char* buff, PrintfFormat info)
{
    Dec dec;
    DecFormat form;
    char* p;
    char* q;
    int n, digits, sign;
    int int_digits, frac_digits;

    if (info.precision > CONVERSION_BUFF_SIZE) {
        return 0;
    }

    form.style = 0;
    form.digits = 32;
    __num2dec(&form, x, &dec);

    p = (char*)dec.digits + dec.len;

    while (dec.len > 1 && *--p == '0') {
        --dec.len;
        ++dec.exp;
    }

    switch (*dec.digits) {
    case '0':
        dec.exp = 0;
        break;

    case 'I':
        if (x < 0) {
            p = buff - 5;

            if (isupper(info.conversion)) {
                strcpy(p, "-INF");
            } else {
                strcpy(p, "-inf");
            }
        } else {
            p = buff - 4;

            if (isupper(info.conversion)) {
                strcpy(p, "INF");
            } else {
                strcpy(p, "inf");
            }
        }

        return p;

    case 'N':
        if (dec.sign) {
            p = buff - 5;

            if (isupper(info.conversion)) {
                strcpy(p, "-NAN");
            } else {
                strcpy(p, "-nan");
            }
        } else {
            p = buff - 4;

            if (isupper(info.conversion)) {
                strcpy(p, "NAN");
            } else {
                strcpy(p, "nan");
            }
        }

        return p;
    }

    dec.exp += dec.len - 1;

    p = buff;
    *--p = 0;

    switch (info.conversion) {
    case 'g':
    case 'G':
        if (dec.len > info.precision) {
            round_decimal(&dec, info.precision);
        }

        if (dec.exp < -4 || dec.exp >= info.precision) {
            if (info.alternate) {
                --info.precision;
            } else {
                info.precision = dec.len - 1;
            }

            if (info.conversion == 'g') {
                info.conversion = 'e';
            } else {
                info.conversion = 'E';
            }

            goto e_format;
        }

        if (info.alternate) {
            info.precision -= dec.exp + 1;
        } else if ((info.precision = dec.len - (dec.exp + 1)) < 0) {
            info.precision = 0;
        }

        goto f_format;

    case 'e':
    case 'E':
    e_format:
        if (dec.len > info.precision + 1) {
            round_decimal(&dec, info.precision + 1);
        }

        n = dec.exp;
        sign = '+';

        if (n < 0) {
            n = -n;
            sign = '-';
        }

        for (digits = 0; n != 0 || digits < 2; ++digits) {
            *--p = n % 10 + '0';
            n /= 10;
        }

        *--p = sign;
        *--p = info.conversion;

        if (buff - p + info.precision > CONVERSION_BUFF_SIZE) {
            return 0;
        }

        if (dec.len < info.precision + 1) {
            for (n = info.precision + 1 - dec.len + 1; --n;) {
                *--p = '0';
            }
        }

        for (n = dec.len, q = (char*)dec.digits + dec.len; --n;) {
            *--p = *--q;
        }

        if (info.precision || info.alternate) {
            *--p = '.';
        }

        *--p = *dec.digits;

        if (dec.sign) {
            *--p = '-';
        } else if (info.sign == sign_always) {
            *--p = '+';
        } else if (info.sign == space_holder) {
            *--p = ' ';
        }
        break;

    case 'f':
    case 'F':
    f_format:
        if ((frac_digits = -dec.exp + dec.len - 1) < 0) {
            frac_digits = 0;
        }

        if (frac_digits > info.precision) {
            round_decimal(&dec, dec.len - (frac_digits - info.precision));

            if ((frac_digits = -dec.exp + dec.len - 1) < 0) {
                frac_digits = 0;
            }
        }

        if ((int_digits = dec.exp + 1) < 0) {
            int_digits = 0;
        }

        if (int_digits + frac_digits > CONVERSION_BUFF_SIZE) {
            return 0;
        }

        q = (char*)dec.digits + dec.len;

        for (digits = 0; digits < (info.precision - frac_digits); ++digits) {
            *--p = '0';
        }

        for (digits = 0; digits < frac_digits && digits < dec.len; ++digits) {
            *--p = *--q;
        }

        for (; digits < frac_digits; ++digits) {
            *--p = '0';
        }

        if (info.precision || info.alternate) {
            *--p = '.';
        }

        if (int_digits) {
            for (digits = 0; digits < int_digits - dec.len; ++digits) {
                *--p = '0';
            }

            for (; digits < int_digits; ++digits) {
                *--p = *--q;
            }
        } else {
            *--p = '0';
        }

        if (dec.sign) {
            *--p = '-';
        } else if (info.sign == sign_always) {
            *--p = '+';
        } else if (info.sign == space_holder) {
            *--p = ' ';
        }
        break;
    }

    return p;
}

void round_decimal(Dec* d, int digits)
{
    char c;
    char* p;
    int carry;

    if (digits < 0) {
    return_zero:
        d->exp = 0;
        d->len = 1;
        d->digits[0] = '0';
        return;
    }

    if (digits >= d->len) {
        return;
    }

    p = (char*)d->digits + digits + 1;
    c = *--p - '0';

    if (c == 5) {
        char* q = (char*)d->digits + d->len;

        while (--q > p && *q == '0') {
            ;
        }

        carry = (q == p) ? p[-1] & 1 : 1;
    } else {
        carry = (c > 5);
    }

    while (digits) {
        c = *--p - '0' + carry;

        if ((carry = (c > 9)) != 0 || c == 0) {
            --digits;
        } else {
            *p = c + '0';
            break;
        }
    }

    if (carry) {
        d->exp += 1;
        d->len = 1;
        d->digits[0] = '1';
        return;
    } else if (digits == 0) {
        goto return_zero;
    }

    d->len = digits;
}

static char* double2hex(double x, char* buff, PrintfFormat info)
{
    char* p;
    unsigned char* q;
    signed char working_byte;
    unsigned char mantissa_byte;
    double ld;
    long exp;
    PrintfFormat exp_info;
    int hex_precision;
    DecFormat form;
    Dec dec;

    p = buff;
    ld = x;

    if (info.precision > CONVERSION_BUFF_SIZE) {
        return 0;
    }

    form.style = 0;
    form.digits = 32;
    __num2dec(&form, x, &dec);

    if (*dec.digits == 'I') {
        if (*(short*)&ld & 0x8000) {
            p = buff - 5;

            if (info.conversion == 'A') {
                strcpy(p, "-INF");
            } else {
                strcpy(p, "-inf");
            }
        } else {
            p = buff - 4;

            if (info.conversion == 'A') {
                strcpy(p, "INF");
            } else {
                strcpy(p, "inf");
            }
        }

        return p;
    } else if (*dec.digits == 'N') {
        if (*(unsigned char*)&x & 0x80) {
            p = buff - 5;

            if (info.conversion == 'A') {
                strcpy(p, "-NAN");
            } else {
                strcpy(p, "-nan");
            }
        } else {
            p = buff - 4;

            if (info.conversion == 'A') {
                strcpy(p, "NAN");
            } else {
                strcpy(p, "nan");
            }
        }

        return p;
    }

    exp_info.pad = right_justification;
    exp_info.sign = sign_always;
    exp_info.precision_given = 0;
    exp_info.alternate = 0;
    exp_info.arg_type = normal_argument;
    exp_info.conversion = 'd';
    exp_info.field_width = 0;
    exp_info.precision = 1;

    exp = (short)((*(short*)&ld & 0x7FF0) >> 4) - 1023;
    p = long2str(exp, buff, exp_info);

    if (info.conversion == 'a') {
        *--p = 'p';
    } else {
        *--p = 'P';
    }

    q = (unsigned char*)&x;

    for (hex_precision = info.precision; hex_precision >= 1; hex_precision--) {
        mantissa_byte = q[hex_precision / 2 + 1];

        if (hex_precision % 2) {
            working_byte = mantissa_byte & 0xF;
        } else {
            working_byte = mantissa_byte >> 4;
        }

        if (working_byte < 10) {
            working_byte += '0';
        } else if (info.conversion == 'a') {
            working_byte += 'a' - 10;
        } else {
            working_byte += 'A' - 10;
        }

        *--p = working_byte;
    }

    if (info.precision || info.alternate) {
        *--p = '.';
    }

    *--p = '1';

    if (info.conversion == 'a') {
        *--p = 'x';
    } else {
        *--p = 'X';
    }

    *--p = '0';

    if (*(short*)&ld & 0x8000) {
        *--p = '-';
    } else if (info.sign == sign_always) {
        *--p = '+';
    } else if (info.sign == space_holder) {
        *--p = ' ';
    }

    return p;
}

static char* longlong2str(long long num, char* buff, PrintfFormat info)
{
    unsigned long long unsigned_num, base;
    char* p;
    int n, digits;
    int minus;

    unsigned_num = num;
    minus = 0;

    p = buff;
    *--p = 0;
    digits = 0;

    if (!num && !info.precision && !(info.alternate && info.conversion == 'o')) {
        return p;
    }

    switch (info.conversion) {
    case 'd':
    case 'i':
        base = 10;

        if (num < 0) {
            unsigned_num = -unsigned_num;
            minus = 1;
        }
        break;

    case 'o':
        base = 8;
        info.sign = only_minus;
        break;

    case 'u':
        base = 10;
        info.sign = only_minus;
        break;

    case 'x':
    case 'X':
        base = 16;
        info.sign = only_minus;
        break;
    }

    do {
        n = unsigned_num % base;
        unsigned_num /= base;

        if (n < 10) {
            n += '0';
        } else {
            n -= 10;

            if (info.conversion == 'x') {
                n += 'a';
            } else {
                n += 'A';
            }
        }

        *--p = n;
        ++digits;
    } while (unsigned_num != 0);

    if (base == 8 && info.alternate && *p != '0') {
        *--p = '0';
        ++digits;
    }

    if (info.pad == zero_fill) {
        info.precision = info.field_width;

        if (minus || info.sign != only_minus) {
            --info.precision;
        }

        if (base == 16 && info.alternate) {
            info.precision -= 2;
        }
    }

    if (buff - p + info.precision > CONVERSION_BUFF_SIZE) {
        return 0;
    }

    while (digits < info.precision) {
        *--p = '0';
        ++digits;
    }

    if (base == 16 && info.alternate) {
        *--p = info.conversion;
        *--p = '0';
    }

    if (minus) {
        *--p = '-';
    } else if (info.sign == sign_always) {
        *--p = '+';
    } else if (info.sign == space_holder) {
        *--p = ' ';
    }

    return p;
}

static char* long2str(long num, char* buff, PrintfFormat info)
{
    unsigned long unsigned_num, base;
    char* p;
    int n, digits;
    int minus;

    unsigned_num = num;
    minus = 0;

    p = buff;
    *--p = 0;
    digits = 0;

    if (!num && !info.precision && !(info.alternate && info.conversion == 'o')) {
        return p;
    }

    switch (info.conversion) {
    case 'd':
    case 'i':
        base = 10;

        if (num < 0) {
            unsigned_num = -unsigned_num;
            minus = 1;
        }
        break;

    case 'o':
        base = 8;
        info.sign = only_minus;
        break;

    case 'u':
        base = 10;
        info.sign = only_minus;
        break;

    case 'x':
    case 'X':
        base = 16;
        info.sign = only_minus;
        break;
    }

    do {
        n = unsigned_num % base;
        unsigned_num /= base;

        if (n < 10) {
            n += '0';
        } else {
            n -= 10;

            if (info.conversion == 'x') {
                n += 'a';
            } else {
                n += 'A';
            }
        }

        *--p = n;
        ++digits;
    } while (unsigned_num != 0);

    if (base == 8 && info.alternate && *p != '0') {
        *--p = '0';
        ++digits;
    }

    if (info.pad == zero_fill) {
        info.precision = info.field_width;

        if (minus || info.sign != only_minus) {
            --info.precision;
        }

        if (base == 16 && info.alternate) {
            info.precision -= 2;
        }
    }

    if (buff - p + info.precision > CONVERSION_BUFF_SIZE) {
        return 0;
    }

    while (digits < info.precision) {
        *--p = '0';
        ++digits;
    }

    if (base == 16 && info.alternate) {
        *--p = info.conversion;
        *--p = '0';
    }

    if (minus) {
        *--p = '-';
    } else if (info.sign == sign_always) {
        *--p = '+';
    } else if (info.sign == space_holder) {
        *--p = ' ';
    }

    return p;
}

static const char* parse_format(const char* format, va_list arg, PrintfFormat* out)
{
    PrintfFormat f;
    const char* s = format;
    int c;
    int flag_found;

    f.pad = right_justification;
    f.sign = only_minus;
    f.precision_given = 0;
    f.alternate = 0;
    f.arg_type = normal_argument;
    f.field_width = 0;
    f.precision = 0;

    if ((c = *++s) == '%') {
        f.conversion = c;
        *out = f;
        return s + 1;
    }

    for (;;) {
        flag_found = 1;

        switch (c) {
        case '-':
            f.pad = left_justification;
            break;

        case '+':
            f.sign = sign_always;
            break;

        case ' ':
            if (f.sign != sign_always) {
                f.sign = space_holder;
            }
            break;

        case '#':
            f.alternate = 1;
            break;

        case '0':
            if (f.pad != left_justification) {
                f.pad = zero_fill;
            }
            break;

        default:
            flag_found = 0;
        }

        if (flag_found) {
            c = *++s;
        } else {
            break;
        }
    }

    if (c == '*') {
        if ((f.field_width = va_arg(arg, int)) < 0) {
            f.pad = left_justification;
            f.field_width = -f.field_width;
        }

        c = *++s;
    } else {
        while (isdigit(c)) {
            f.field_width = (f.field_width * 10) + (c - '0');
            c = *++s;
        }
    }

    if (f.field_width > CONVERSION_BUFF_SIZE) {
        f.conversion = BAD_CONVERSION;
        *out = f;
        return s + 1;
    }

    if (c == '.') {
        f.precision_given = 1;

        if ((c = *++s) == '*') {
            if ((f.precision = va_arg(arg, int)) < 0) {
                f.precision_given = 0;
            }

            c = *++s;
        } else {
            while (isdigit(c)) {
                f.precision = (f.precision * 10) + (c - '0');
                c = *++s;
            }
        }
    }

    flag_found = 1;

    switch (c) {
    case 'h':
        f.arg_type = short_argument;

        if (s[1] == 'h') {
            f.arg_type = char_argument;
            c = *++s;
        }
        break;

    case 'l':
        f.arg_type = long_argument;

        if (s[1] == 'l') {
            f.arg_type = long_long_argument;
            c = *++s;
        }
        break;

    case 'L':
        f.arg_type = long_double_argument;
        break;

    default:
        flag_found = 0;
    }

    if (flag_found) {
        c = *++s;
    }

    f.conversion = c;

    switch (c) {
    case 'd':
    case 'i':
    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (f.arg_type == long_double_argument) {
            f.conversion = BAD_CONVERSION;
            break;
        }

        if (!f.precision_given) {
            f.precision = 1;
        } else if (f.pad == zero_fill) {
            f.pad = right_justification;
        }
        break;

    case 'f':
    case 'F':
        if (f.arg_type == short_argument || f.arg_type == long_long_argument) {
            f.conversion = BAD_CONVERSION;
            break;
        }

        if (!f.precision_given) {
            f.precision = 6;
        }
        break;

    case 'a':
    case 'A':
        if (!f.precision_given) {
            f.precision = 13;
        }

        if (f.arg_type == short_argument || f.arg_type == long_long_argument ||
            f.arg_type == char_argument) {
            f.conversion = BAD_CONVERSION;
        }
        break;

    case 'g':
    case 'G':
        if (!f.precision) {
            f.precision = 1;
        }

    case 'e':
    case 'E':
        if (f.arg_type == short_argument || f.arg_type == long_long_argument ||
            f.arg_type == char_argument) {
            f.conversion = BAD_CONVERSION;
            break;
        }

        if (!f.precision_given) {
            f.precision = 6;
        }
        break;

    case 'p':
        f.conversion = 'x';
        f.alternate = 1;
        f.arg_type = long_argument;
        f.precision = 8;
        break;

    case 'c':
        if (f.arg_type == long_argument) {
            f.arg_type = wchar_argument;
        } else if (f.precision_given || f.arg_type != normal_argument) {
            f.conversion = BAD_CONVERSION;
        }
        break;

    case 's':
        if (f.arg_type == long_argument) {
            f.arg_type = wchar_argument;
        } else if (f.arg_type != normal_argument) {
            f.conversion = BAD_CONVERSION;
        }
        break;

    case 'n':
        if (f.arg_type == long_double_argument) {
            f.conversion = BAD_CONVERSION;
        }
        break;

    default:
        f.conversion = BAD_CONVERSION;
        break;
    }

    *out = f;
    return s + 1;
}
