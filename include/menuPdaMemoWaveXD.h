#ifndef MENU_PDA_MEMO_WAVE_XD_H
#define MENU_PDA_MEMO_WAVE_XD_H

#include <types.h>

// Voice playback for the Pokemon whose memo is open, and the waveform request
// that shows the sound being played.
class pdaMemoNakigoe {
public:
    void SetNakigoe(long index);
    void SetSE(unsigned long soundID);
    s32 GetVocie(long index);
    s8 *GetWaveDispDataAddress(long index);
    void DispRequest(s8 *data, unsigned long soundID);
};

#endif
