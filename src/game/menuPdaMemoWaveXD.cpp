#include <types.h>
#include <memo.h>
#include <GSinput.h>
#include <GSthread.h>
#include <dolphin/pad.h>
#include <PokemonData.h>
#include <GSsndPlayback.h>
#include <GSsnd.h>
#include <GSspline.h>
#include <GSgfx.h>
#include <GSgfxC.h>
#include <GSprim.h>
#include "menuPdaMemoListXD.h"
#include "menuPdaMemoWaveXD.h"

enum { NAKIGOE_FADE_DELAY = 30 };

void _drawLine(GSgfxLayerID layerID, GSvec *start, GSvec *end, f32 wavePointCount);

pdaMemoNakigoe *_nakigoe;

void menuPdaMemoWaveHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    if (_nakigoe != NULL) {
        _nakigoe->NakigoeDispFunc(window, sprite);
    }
}

void menuPdaMemoWaveInit(void) {
    if (_nakigoe != NULL) {
        _nakigoe->Init();
    }
}

void menuPdaMemoWave(long index) {
    if (_nakigoe == NULL) {
        _nakigoe = new pdaMemoNakigoe;
    }
    _nakigoe->Init();
    _nakigoe->SetNakigoe(index);
    pdaMemoNakigoe *wave = _nakigoe;
    while (true) {
        if (!wave->NakigoeCheck()) {
            break;
        }
        const u32 changed = GSinputButtonsChanged(GS_INPUT_PORT_1);
        const u32 pressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
        if ((pressed & changed & PAD_BUTTON_A) != 0) {
            wave->SetNakigoe(index);
        } else {
            const u32 bChanged = GSinputButtonsChanged(GS_INPUT_PORT_1);
            const u32 bPressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
            if ((bPressed & bChanged & PAD_BUTTON_B) != 0) {
                break;
            }
        }
        GSthreadSwitch();
    }
}

void pdaMemoNakigoe::SetNakigoe(long index) {
    s32 voice = GetVocie(index);
    u16 soundID = (u16)voice;

    SetSE(soundID);
    DispRequest(GetWaveDispDataAddress(index), soundID);
}

void pdaMemoNakigoe::SetSE(unsigned long soundID) {
    GSsndPlay(soundID, 0, 0);
}

s32 pdaMemoNakigoe::GetVocie(long index) {
    if (memoDataGetCount(NULL) != 0) {
        PokemonSpeciesStats *stats = menuPdaMemoListGetPokemonDataPointer(index);
        if (stats != NULL) {
            return pokemonDataBiosGetVoice(stats);
        }
        return POKEMON_VOICE_ID_INVALID;
    }
    return POKEMON_VOICE_ID_INVALID;
}

void pdaMemoNakigoe::DispInit() {
    for (s32 i = 0; i < NAKIGOE_HISTORY_COUNT; i++) {
        DispRequest(NULL, 0);
    }
}

void pdaMemoNakigoe::DispRequest(s8 *data, unsigned long soundID) {
    m_workIndex++;
    if (m_workIndex >= NAKIGOE_HISTORY_COUNT) m_workIndex = 0;

    m_work[m_workIndex].m_waveData = data;
    m_work[m_workIndex].m_soundID = soundID;
    m_work[m_workIndex].m_startPoint = 0.0f;
    m_work[m_workIndex].m_endPoint = 0.0f;
    m_work[m_workIndex].m_scrollCount = 0;
    m_work[m_workIndex].m_drawCount = 1;
    m_work[m_workIndex].m_opacity = 255.0f;
    m_work[m_workIndex].m_pointStep = 0.5f;

    for (s32 i = m_workIndex - 1; i >= 0; i--) {
        m_work[i].m_scrollCount += m_work[i].m_pointCount;
        m_work[i].m_pointStep += 0.2f;
    }
    for (s32 i = NAKIGOE_HISTORY_COUNT - 1; i > m_workIndex; i--) {
        m_work[i].m_scrollCount += m_work[i].m_pointCount;
        m_work[i].m_pointStep += 0.2f;
    }
}

void pdaMemoNakigoe::NakigoeDispFunc(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    for (s32 i = 0; i < NAKIGOE_HISTORY_COUNT; i++) {
        DispFuncSub(&m_work[i], window->mLayerID);
    }
    for (s32 i = m_workIndex - 1; i >= 0; i--) {
        m_work[i].m_opacity -= 10.0f;
        if (m_work[i].m_opacity < 0.0f) m_work[i].m_opacity = 0.0f;
    }
    for (s32 i = NAKIGOE_HISTORY_COUNT - 1; i > m_workIndex; i--) {
        m_work[i].m_opacity -= 10.0f;
        if (m_work[i].m_opacity < 0.0f) m_work[i].m_opacity = 0.0f;
    }
}

void pdaMemoNakigoe::DispFuncSub(NakigoeDispWork *work, GSgfxLayerID layerID) {
    GSspline *spline;
    s8 *data;
    s32 dataCount;
    u8 controlPointCount;
    s32 i;
    s32 j;
    f32 wavePointCount;
    f32 time;
    f32 timeStep;
    GSvec controlPoint;
    GSvec vertex;
    GSvec nextVertex;
    data = work->m_waveData;
    if (data == NULL) return;

    dataCount = (u8)data[0] + 2;
    controlPointCount = dataCount;
    work->m_pointCount = controlPointCount;
    if (work->m_drawCount < controlPointCount) {
        work->m_drawCount++;
    } else if (work->m_scrollCount < controlPointCount) {
        work->m_scrollCount++;
    } else if (work->m_scrollCount < controlPointCount + NAKIGOE_FADE_DELAY) {
        work->m_scrollCount++;
        if (work->m_endPoint < controlPointCount) work->m_endPoint += work->m_pointStep;
    } else if (work->m_startPoint < controlPointCount) {
        work->m_startPoint += work->m_pointStep;
        if (work->m_endPoint < controlPointCount) work->m_endPoint += work->m_pointStep;
    } else if (work->m_endPoint < controlPointCount) {
        work->m_endPoint += work->m_pointStep;
    }

    spline = GSsplineCreate(GSSPLINE_LAGRANGE, GSSPLINE_VECTOR, dataCount);
    if (spline == NULL) return;

    controlPoint.x = 0.0f;
    controlPoint.y = 0.0f;
    controlPoint.z = work->m_opacity;
    timeStep = 1.0f / (u8)dataCount;
    wavePointCount = work->m_pointCount;
    GSsplineAddControlVectorValue(spline, &controlPoint, 0.0f);
    controlPoint.x += 556.0f / wavePointCount;
    time = timeStep;
    for (i = 0; i < controlPointCount - 2; i++) {
        controlPoint.y = data[i + 1];
        controlPoint.y = -controlPoint.y;
        GSsplineAddControlVectorValue(spline, &controlPoint, time);
        time += timeStep;
        controlPoint.x += 556.0f / wavePointCount;
    }
    controlPoint.y = 0.0f;
    controlPoint.x = 556.0f;
    GSsplineAddControlVectorValue(spline, &controlPoint, 1.0f);

    for (j = 0; j < (s32)work->m_startPoint; j++) {
        vertex = ((GSvec *)spline->values)[j];
        vertex.z = 0.0f;
        ((GSvec *)spline->values)[j] = vertex;
    }
    for (i = (s32)work->m_startPoint; i < (s32)work->m_endPoint; i++) {
        vertex = ((GSvec *)spline->values)[i];
        vertex.z = work->m_opacity / ((s32)work->m_endPoint - (s32)work->m_startPoint) * (i - (s32)work->m_startPoint);
        ((GSvec *)spline->values)[i] = vertex;
    }
    for (i = 0; i < work->m_drawCount - 1; i++) {
        vertex = ((GSvec *)spline->values)[i];
        nextVertex = ((GSvec *)spline->values)[i + 1];
        _drawLine(layerID, &vertex, &nextVertex, work->m_pointCount);
    }
    GSsplineFree(spline);
}

bool pdaMemoNakigoe::NakigoeCheck() {
    s32 i;
    bool playing = false;
    for (i = 0; i < NAKIGOE_HISTORY_COUNT; i++) {
        if (m_work[i].m_waveData != NULL) {
            switch (GSsndGetStatus(m_work[i].m_soundID)) {
            case GSSND_CLOSED:
            case 1:
            case GSSND_PAUSED:
                break;
            case GSSND_PLAYING:
                playing = true;
                break;
            }
        }
    }
    return playing;
}

void _drawLine(GSgfxLayerID layerID, GSvec *start, GSvec *end, f32 wavePointCount) {
    const u32 waveColor = (150 << 24) | (230 << 16) | (240 << 8);
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    layer->setRenderStateAndSelect(GSgfxVFBuildTemp(GX_POS_XY, GX_S16, 0, 0, 1, GX_RGBA8, 0, 0, 0));
    layer->mPrimitiveType = GSGFX_PRIM_LINES;
    GSprimLayerLineWidth(layer, 24);
    layer->beginSurface(2);
    s16 x = start->x;
    s16 y = 0.5f * start->y + 105.0f;
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y;
    GSgfxFifo.u32 = waveColor | (u8)start->z;
    x = end->x;
    y = 0.5f * end->y + 105.0f;
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y;
    GSgfxFifo.u32 = waveColor | (u8)end->z;
    layer->endSurface();
    GSprimLayerLineWidth(layer, 6);
}
