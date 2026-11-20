#ifndef MSGCTRL_H
#define MSGCTRL_H

#include <types.h>

// Substitution values consumed by message formats.
enum { MSG_VAR_CHARACTER_NAME = 0x59, MSG_VAR_KANE = 80 };

enum {
    MSGCTRL_NUM_ITEM = 0x2D,
    MSGCTRL_NUM_SWAP_ITEM = 0x2E,
    MSGCTRL_NUM_INPUT = 0x2F,
    MSGCTRL_POKEMON_NAME = 0x32,
    MSGCTRL_NUM_PANEL = 0x34,
    MSGCTRL_TEXT = 0x37,
    MSGCTRL_NUM_VALUE = 0x5F,
};

enum MSGCTRL_VALUE_SELECTOR {
    MSGCTRL_VALUE_ITEM_ID = 0x2D,
    MSGCTRL_VALUE_ITEM_QUANTITY = 0x2F,
    // Message whose text is inserted in place of this value.
    MSGCTRL_VALUE_MESSAGE_ID = 49,
    MSGCTRL_VALUE_NUMBER = 0x34,
    MSGCTRL_VALUE_ENCOUNTERED_POKEMON_COUNT = 0x35,
    MSGCTRL_VALUE_STRING = 0x37,
    MSGCTRL_VALUE_WAZA_ID = 57,
    MSGCTRL_VALUE_PLAY_TIME = 0x4C,
    MSGCTRL_VALUE_MONEY = 75,
    // Slot holding a string pointer that msgctrlString substitutes into
    // messages; the script supplies message text through this slot.
    MSGCTRL_VALUE_MESSAGE_STRING = 77,
    MSGCTRL_VALUE_NATURE_MESSAGE = 0x55,
    MSGCTRL_VALUE_NATURE_SUFFIX_MESSAGE = 0x56,
    MSGCTRL_VALUE_LEVEL = 0x5F,
    // Species ID that the Pokemon-name value renders.
    MSGCTRL_VALUE_POKEMON_ID = 78,
    // Sound ID read by msgctrlSndPlay when it handles the sound command.
    MSGCTRL_VALUE_SOUND_ID = 93,
};

// Message-value slots used by the save report templates.
enum {
    MSG_VALUE_REPORT_PLAYTIME = 0x4C,
    MSG_VALUE_REPORT_SNAGGED_POKEMON_COUNT = 0x34,
    MSG_VALUE_REPORT_MET_POKEMON_COUNT = 0x35
};

struct MSG_TASK_WORK;

#ifdef __cplusplus
extern "C" {
#endif

s32 msgctrlColor(struct MSG_TASK_WORK *task);
s32 msgctrlFont(struct MSG_TASK_WORK *task);
s32 msgctrlRubyEnd(struct MSG_TASK_WORK *task);
s32 msgctrlRubyTop(struct MSG_TASK_WORK *task);
s32 msgctrlRubyStart(struct MSG_TASK_WORK *task);
s32 msgctrlKeyWait(struct MSG_TASK_WORK *task);
s32 msgctrlCheckWait(struct MSG_TASK_WORK *task);
s32 msgctrlCheckEnd(struct MSG_TASK_WORK *task);
s32 msgctrlKeyEnd(struct MSG_TASK_WORK *task);
void msgctrlSetValue(s32 selector, unsigned long value);

#ifdef __cplusplus
}
#endif

#endif
