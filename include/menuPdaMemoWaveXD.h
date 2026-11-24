#ifndef MENU_PDA_MEMO_WAVE_XD_H
#define MENU_PDA_MEMO_WAVE_XD_H

#include <types.h>
#include <winSprite.h>
#include <GSgfxTypes.h>

enum { NAKIGOE_HISTORY_COUNT = 8 };

// One pending waveform request. The cry samples are drawn through a window
// that fills in, then scrolls once the whole cry has become visible.
struct NakigoeDispWork {
    s8 *m_waveData;
    f32 m_pointCount;
    u32 m_soundID;
    f32 m_startPoint;
    s32 m_scrollCount;
    f32 m_endPoint;
    s32 m_drawCount;
    f32 m_opacity;
    f32 m_pointStep;
};

// Voice playback for the Pokemon whose memo is open, and the waveform request
// that shows the sound being played.
class pdaMemoNakigoe {
public:
    void SetNakigoe(long index);
    void SetSE(unsigned long soundID);
    s32 GetVocie(long index);
    s8 *GetWaveDispDataAddress(long index);
    void DispRequest(s8 *data, unsigned long soundID);
    void DispInit();
    void NakigoeDispFunc(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
    void DispFuncSub(NakigoeDispWork *work, GSgfxLayerID layerID);
    bool NakigoeCheck();

    pdaMemoNakigoe() { Init(); }

    // Clear the pending requests and start a fresh display cycle.
    void Init() {
        for (int i = 0; i < NAKIGOE_HISTORY_COUNT; i++) {
            m_work[i].m_waveData = NULL;
        }
        m_workIndex = 0;
        DispInit();
    }

    NakigoeDispWork m_work[NAKIGOE_HISTORY_COUNT];
    u32 m_workIndex;
};

extern pdaMemoNakigoe *_nakigoe;

extern "C" {
void menuPdaMemoWave(long index);
void menuPdaMemoWaveInit(void);
void menuPdaMemoWaveHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
}

#endif
