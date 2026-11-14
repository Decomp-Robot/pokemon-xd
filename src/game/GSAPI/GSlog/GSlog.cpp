#include <GSlog.h>
#include <GSmem.h>
#include <dolphin/os.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

enum { LOG_HEX_DIGIT_MASK = 0xF };

char* _logBuffer;
u16* _lenBuffer;
u32 _logBufSize;
u32 _logLineCount;
u32 _logMaxLines;
u32 _logStampMode;
OSThread* _mainThread;

char* _inthexTable = "0123456789ABCDEF";
char _logOutputBuf[256];
char _logTimeStampBuf[20];

void logFloat2Str(float value, char* str, char* fractionText);
void logHex2Str(unsigned long value, char* str, bool upper);
void logInt2Str(int value, char* str);
int logStr2Int(char* str);
void logStrRev(char* str);

extern "C" const char* GSlogGetLine(u32 line) {
    if (_logBuffer == 0) return 0;
    if (line > _logLineCount) return 0;
    u16* lengths = _lenBuffer;
    u32 offset = 0;
    u32 i;
    for (i = 0; i < line; ++i) offset += lengths[i];
    return _logBuffer + offset;
}

extern "C" u32 GSlogGetLineCount(void) {
    return _logBuffer == 0 ? 0 : _logLineCount;
}

extern "C" void GSlogWritef(const char* format, ...) {}

extern "C" void GSlogWrite(const char* format, ...) {}

extern "C" void GSlogSetIP(u8* address, u16 port) {}

extern "C" int GSlogInit(u32 size, u32 stampMode) {
    _logStampMode = stampMode;
    _logBufSize = size;
    if (size == 0) {
        GSlogWrite("GSlog: Init OK, no buffer\n");
        return 1;
    }
    _logBuffer = (char*)GSmemAlloc(size);
    if (_logBuffer == 0) {
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }
    _logMaxLines = size >> 7;
    _lenBuffer = (u16*)GSmemAlloc(_logMaxLines * sizeof(*_lenBuffer));
    if (_lenBuffer == 0) {
        GSmemFree(_logBuffer);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }
    _mainThread = OSGetCurrentThread();
    GSlogWrite("GSlog: Init OK, log buffer size %d bytes\n", _logBufSize);
    return 1;
}

static void logMakeTimeStamp() {
    OSCalendarTime time;
    OSTicksToCalendarTime(OSGetTime(), &time);
    sprintf(_logTimeStampBuf, "%d/%02d %d:%02d:%02d] ",
            time.mon + 1, time.mday, time.hour, time.min, time.sec);
}

extern "C" void logVsnprintf_float(char* dst, u32 size, const char* fmt, va_list args) {
    static char fmtbuf[16];
    static char cvtbuf[16];
    static char cvtbuf2[16];
    const char* format = fmt;
    char* out = dst;
    char* spec;
    char* text;
    bool done = false;
    bool inFormat = false;
    bool left = false;
    bool zeroPad = false;
    bool converted = false;
    int count;

    while (!done) {
        if (!inFormat) {
            if (*format == '%') {
                if (format[1] == '%') {
                    ++format;
                    *out++ = '%';
                } else {
                    spec = fmtbuf;
                    inFormat = true;
                }
            } else {
                *out++ = *format;
            }
        } else {
            switch (*format) {
            case 'c':
                text = cvtbuf;
                cvtbuf[0] = va_arg(args, int);
                cvtbuf[1] = 0;
                converted = true;
                break;
            case 'd':
                logInt2Str(va_arg(args, int), cvtbuf);
                text = cvtbuf;
                converted = true;
                break;
            case 'f': {
                float value = va_arg(args, double);
                logFloat2Str(value, cvtbuf, cvtbuf2);
                text = cvtbuf;
                converted = true;
                break;
            }
            case 's':
                text = va_arg(args, char*);
                converted = true;
                break;
            case 'x':
            case 'X': {
                unsigned long value = va_arg(args, unsigned long);
                if (*format == 'X') logHex2Str(value, cvtbuf, true);
                else logHex2Str(value, cvtbuf, false);
                text = cvtbuf;
                converted = true;
                break;
            }
            default:
                *spec++ = *format;
                break;
            }
            if (converted == true) {
                *spec = 0;
                spec = fmtbuf;
                if (*spec == '-') { left = true; ++spec; }
                if (*spec == '0') { zeroPad = true; ++spec; }
                count = logStr2Int(spec);
                int length = strlen(text);
                if (count > length) {
                    count -= length;
                    if (!left) {
                        while (count-- != 0 && (u32)(out - dst) < size) {
                            if (zeroPad) *out++ = '0';
                            else *out++ = ' ';
                        }
                    }
                }
                while (*text && (u32)(out - dst) < size) *out++ = *text++;
                if (*format == 'f') {
                    text = cvtbuf2;
                    int length = strlen(text);
                    char* dot = strchr(spec, '.');
                    if (dot && (count = logStr2Int(dot + 1)) > 0 && length > count)
                        text[count] = 0;
                    *out++ = '.';
                    while (*text && (u32)(out - dst) < size) *out++ = *text++;
                }
                if (left == true) {
                    while (count-- != 0 && (u32)(out - dst) < size) *out++ = ' ';
                }
                left = false;
                zeroPad = false;
                inFormat = false;
                converted = false;
            }
        }
        if (*format == 0 || (u32)(out - dst) >= size) done = true;
        ++format;
    }
    *out = 0;
}

static void logVsnprintf(char* dst, u32 size, const char* fmt, va_list args) {
    static char fmtbuf[16];
    static char cvtbuf[16];
    static char cvtbuf2[16];
    const char* format = fmt;
    char* out = dst;
    char* spec;
    char* text;
    bool done = false;
    bool inFormat = false;
    bool left = false;
    bool zeroPad = false;
    bool converted = false;
    int count;

    while (!done) {
        if (!inFormat) {
            if (*format == '%') {
                if (format[1] == '%') {
                    ++format;
                    *out++ = '%';
                } else {
                    spec = fmtbuf;
                    inFormat = true;
                }
            } else {
                *out++ = *format;
            }
        } else {
            switch (*format) {
            case 'c':
                text = cvtbuf;
                cvtbuf[0] = va_arg(args, int);
                cvtbuf[1] = 0;
                converted = true;
                break;
            case 'd':
                logInt2Str(va_arg(args, int), cvtbuf);
                text = cvtbuf;
                converted = true;
                break;
            case 's':
                text = va_arg(args, char*);
                converted = true;
                break;
            case 'x':
            case 'X': {
                unsigned long value = va_arg(args, unsigned long);
                if (*format == 'X') logHex2Str(value, cvtbuf, true);
                else logHex2Str(value, cvtbuf, false);
                text = cvtbuf;
                converted = true;
                break;
            }
            default:
                *spec++ = *format;
                break;
            }
            if (converted == true) {
                *spec = 0;
                spec = fmtbuf;
                if (*spec == '-') { left = true; ++spec; }
                if (*spec == '0') { zeroPad = true; ++spec; }
                count = logStr2Int(spec);
                int length = strlen(text);
                if (count > length) {
                    count -= length;
                    if (!left) {
                        while (count-- != 0 && (u32)(out - dst) < size) {
                            if (zeroPad) *out++ = '0';
                            else *out++ = ' ';
                        }
                    }
                }
                while (*text && (u32)(out - dst) < size) *out++ = *text++;
                if (left == true) {
                    while (count-- != 0 && (u32)(out - dst) < size) *out++ = ' ';
                }
                left = false;
                zeroPad = false;
                inFormat = false;
                converted = false;
            }
        }
        if (*format == 0 || (u32)(out - dst) >= size) done = true;
        ++format;
    }
    *out = 0;
}

void logFloat2Str(float value, char* str, char* fractionText) {
    if (value < 0.0f) {
        *str++ = '-';
        value = -value;
    }
    int integer = (int)value;
    logInt2Str(integer, str);
    value -= (float)integer;

    int leadingZeroes;
    float place = 0.1f;
    for (leadingZeroes = 0; leadingZeroes < 10; ++leadingZeroes) {
        if (place <= value) break;
        place /= 10.0f;
    }
    char* p = fractionText;
    while (leadingZeroes--) *p++ = '0';
    value *= 1000000000.0f;
    logInt2Str((int)value, p);
    int length = strlen(fractionText);
    p = fractionText + length;
    for (; length < 10; ++length) *p++ = '0';
    *p = 0;
}

void logHex2Str(unsigned long value, char* str, bool upper) {
    char* p = str;
    do {
        *p = _inthexTable[value & LOG_HEX_DIGIT_MASK];
        if (!upper && *p >= 'A') *p += 'a' - 'A';
        ++p;
        value >>= 4;
    } while (value != 0);
    *p = 0;
    logStrRev(str);
}

void logInt2Str(int value, char* str) {
    char* p = str;
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }
    do {
        int digit = value - value / 10 * 10;
        *p++ = _inthexTable[digit];
        value /= 10;
    } while (value != 0);
    if (negative == true) *p++ = '-';
    *p = 0;
    logStrRev(str);
}

int logStr2Int(char* str) {
    int value = 0;
    while (*str) {
        if (*str < '0' || *str > '9') break;
        value *= 10;
        value += *str++ - '0';
    }
    return value;
}

void logStrRev(char* str) {
    char* end = str;
    end += strlen(str) - 1;
    while (str < end) {
        *str ^= *end;
        *end ^= *str;
        *str ^= *end;
        ++str;
        --end;
    }
}
