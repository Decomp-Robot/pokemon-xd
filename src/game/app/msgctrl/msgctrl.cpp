#include <types.h>
#include <msgctrl.h>
#include <GSmsg.h>
#include <menuIds.h>
#include <menu.h>
#include <peopleTalk.h>
#include <GSsndPlayback.h>
#include <pokecolo.h>

enum MSGCTRL_RUBY_STATE { MSGCTRL_RUBY_OFF, MSGCTRL_RUBY_BASE, MSGCTRL_RUBY_TOP };
enum { MSGCTRL_FLAG_AUTO_ADVANCE = 2 };

enum MSGCTRL_COLOR_MASK {
    MSGCTRL_COLOR_MASK_ALPHA = 0xFF,
    MSGCTRL_COLOR_MASK_RGB = 0xFFFFFF00,
};

extern "C" {

s32 msgctrlColor(MSG_TASK_WORK *task) {
    if (task->renderPass != 0) {
        u32 alpha = task->color & MSGCTRL_COLOR_MASK_ALPHA;
        task->color = *(u32 *)task->stream;
        task->color &= MSGCTRL_COLOR_MASK_RGB;
        task->color |= alpha;
        GSmsgSetColor(task);
    }
    task->stream += 4;
    return 0;
}

s32 msgctrlFont(MSG_TASK_WORK *task) {
    if (task->renderPass == 0) {
        task->font = *task->stream;
        GSmsgApplyFontMetrics(task);
    } else {
        task->font = *task->stream;
        GSmsgApplyFontMetrics(task);
    }
    task->stream++;
    return 0;
}

s32 msgctrlRubyEnd(MSG_TASK_WORK *task) {
    task->ruby = MSGCTRL_RUBY_OFF;
    return 0;
}

s32 msgctrlRubyTop(MSG_TASK_WORK *task) {
    task->ruby = MSGCTRL_RUBY_TOP;
    return 0;
}

s32 msgctrlRubyStart(MSG_TASK_WORK *task) {
    if (task->renderPass != 0)
        GSmsgCalcRubyLayout(task);
    task->ruby = MSGCTRL_RUBY_BASE;
    return 0;
}

s32 msgctrlKeyWait(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->renderPass == 0) {
        if (task->keyWait != 0) {
            task->keyWait = 0;
            task->savedStream = task->stream;
            if (*task->stream != 0)
                peopleSetTalkKeyWaitCallback();
            if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) == 0)
                GSsndPlayNow(MENU_SE_CONFIRM);
        } else {
            task->stream -= 3;
            task->keyEndWait = 1;
        }
    } else {
        task->cursorX = task->originX;
        task->cursorY = task->originY;
    }
    return 1;
}

s32 msgctrlCheckWait(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->renderPass == 0) {
        if (task->keyWait != 0) {
            task->keyWait = 0;
            task->savedStream = task->stream;
            if (*task->stream != 0)
                peopleSetTalkKeyWaitCallback();
            if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) == 0)
                GSsndPlayNow(MENU_SE_CONFIRM);
        } else {
            task->stream -= 3;
            task->checkEndWait = 1;
        }
    } else {
        task->cursorX = task->originX;
        task->cursorY = task->originY;
    }
    return 1;
}

s32 msgctrlCheckEnd(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->keyWait != 0) {
        task->keyWait = 0;
    } else {
        task->stream -= 3;
        task->checkEndWait = 1;
    }
    return 1;
}

s32 msgctrlKeyEnd(MSG_TASK_WORK *task) {
    if ((task->flags & MSGCTRL_FLAG_AUTO_ADVANCE) != 0)
        task->keyWait = 1;
    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR) != 0)
        task->keyWait = 0;
    if (task->keyWait != 0)
        task->keyWait = 0;
    else
        task->stream -= 3;
    return 1;
}

s32 msgctrlCR(MSG_TASK_WORK *task) {
    task->cursorX = task->originX;
    task->cursorY += task->scaleY * (task->charHeight + task->lineSpace);
    return 0;
}

u16 half_digit_code[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

u16 full_digit_code[16] = {
    0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17,
    0xFF18, 0xFF19, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26
};

enum MSGCTRL_NUMBER_MODE {
    MSGCTRL_NUMBER_SIGNED = 0,
    MSGCTRL_NUMBER_UNSIGNED = 1,
    MSGCTRL_NUMBER_ZERO_PAD = 2,
    MSGCTRL_NUMBER_HEX = 3,
    MSGCTRL_NUMBER_MONEY = 4,
    MSGCTRL_NUMBER_FULL = 5,
    MSGCTRL_NUMBER_HALF_SIGNED = 6,
    MSGCTRL_NUMBER_HALF_UNSIGNED = 7,
    MSGCTRL_NUMBER_HALF_ZERO_PAD = 8,
    MSGCTRL_NUMBER_HALF_HEX = 9,
    MSGCTRL_NUMBER_HALF_MONEY = 10
};

u16 *msgctrlNumStr(u16 *dst, s32 pos, u32 value, MSGCTRL_NUMBER_MODE mode) {
    u8 negative = 0;
    s32 base = 10;
    s32 digitCount = 0;
    s32 minDigits = 1;
    u16 *digits;
    dst[--pos] = 0;
    switch (mode) {
    case MSGCTRL_NUMBER_SIGNED: case MSGCTRL_NUMBER_FULL: case MSGCTRL_NUMBER_HALF_SIGNED:
        if ((s32 &)value < 0) { value = 0u - value; negative = 1; }
        break;
    case MSGCTRL_NUMBER_ZERO_PAD: case MSGCTRL_NUMBER_HALF_ZERO_PAD:
        minDigits = 10; break;
    case MSGCTRL_NUMBER_HEX: case MSGCTRL_NUMBER_HALF_HEX:
        base = 16; minDigits = 8; break;
    case MSGCTRL_NUMBER_HALF_MONEY: break;
    }
    switch (mode) {
    case MSGCTRL_NUMBER_SIGNED: case MSGCTRL_NUMBER_UNSIGNED: case MSGCTRL_NUMBER_ZERO_PAD:
    case MSGCTRL_NUMBER_HEX: case MSGCTRL_NUMBER_MONEY:
        if (pokecoloGetCountryCode() == POKECOLO_COUNTRY_CODE_JAPAN)
            digits = full_digit_code;
        else
            digits = half_digit_code;
        break;
    case MSGCTRL_NUMBER_FULL: digits = full_digit_code; break;
    default: digits = half_digit_code; break;
    }
    while (value != 0) {
        if (mode == MSGCTRL_NUMBER_MONEY || mode == MSGCTRL_NUMBER_HALF_MONEY) {
            if (digitCount != 0 && digitCount % 3 == 0) {
                PokeColoLanguage language = (PokeColoLanguage)pokecoloGetLanguage();
                switch (language) {
                case POKECOLO_LANGUAGE_GERMAN:
                case POKECOLO_LANGUAGE_SPANISH:
                    dst[--pos] = '.';
                    break;
                case POKECOLO_LANGUAGE_FRENCH:
                case POKECOLO_LANGUAGE_ITALIAN:
                    break;
                case POKECOLO_LANGUAGE_ENGLISH:
                default:
                    dst[--pos] = ',';
                    break;
                }
            }
        }
        u32 digit = value % base;
        value = value / base;
        dst[--pos] = digits[digit];
        digitCount++;
    }
    for (; digitCount < minDigits; digitCount++)
        dst[--pos] = digits[0];
    if (negative) dst[--pos] = '-';
    return dst + pos;
}

}
