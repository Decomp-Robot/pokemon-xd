#ifndef MSGCTRL_H
#define MSGCTRL_H

enum { MSG_VAR_CHARACTER_NAME = 0x59 };

enum {
    MSGCTRL_NUM_ITEM = 0x2D,
    MSGCTRL_NUM_SWAP_ITEM = 0x2E,
    MSGCTRL_NUM_INPUT = 0x2F,
    MSGCTRL_POKEMON_NAME = 0x32,
    MSGCTRL_NUM_PANEL = 0x34,
    MSGCTRL_TEXT = 0x37,
    MSGCTRL_NUM_VALUE = 0x5F,
};

// Message-value slots used by the save report templates.
enum {
    MSG_VALUE_REPORT_PLAYTIME = 0x4C,
    MSG_VALUE_REPORT_SNAGGED_POKEMON_COUNT = 0x34,
    MSG_VALUE_REPORT_MET_POKEMON_COUNT = 0x35
};

#ifdef __cplusplus
extern "C" {
#endif

void msgctrlSetValue(long slot, long value);

#ifdef __cplusplus
}
#endif

#endif
