#ifndef MSGCTRL_H
#define MSGCTRL_H

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
