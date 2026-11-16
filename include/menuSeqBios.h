#ifndef MENU_SEQ_BIOS_H
#define MENU_SEQ_BIOS_H

#include <types.h>

// One command of a menu window sequence. The sequence player walks the records
// by index, so each record is a command header followed by its two operands.
typedef struct MenuSeqData {
    u8 mFlags;         // Bit flags tested when the command is played.
    u8 mCommand;       // Command code, 0-11.
    s16 mCount;        // Command-dependent count: frames to wait or
                       // interpolate over, or the repeat count of a command
                       // that loops back to the index in mOperands[0].
    s32 mOperands[2];  // Command operands, read according to the command code.
} MenuSeqData;

#ifdef __cplusplus
extern "C" {
#endif

MenuSeqData *menuSeqBiosGetPtr(u32 sequenceID);

#ifdef __cplusplus
}
#endif

#endif
