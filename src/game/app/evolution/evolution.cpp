#include <types.h>
#include <dolphin/pad.h>

#include <camera.h>
#include <GScamera.h>
#include <GSvec.h>
#include <sound.h>
#include <evolution.h>
#include <GSinput.h>
#include <GSmodel.h>
#include <GSres.h>
#include <GSsnd.h>
#include <GSsndPlayback.h>
#include <GSthread.h>
#include <fade.h>
#include <floor.h>
#include <floorManager.h>
#include <menuPokemonStatus.h>
#include <menuSub.h>
#include <menuIds.h>
#include <msgctrl.h>
#include <winMsg.h>
#include <pokemonBios.h>
#include <PokemonData.h>
#include <sequence.h>
#include <time.h>
#include <wazaSequence.h>
#include <wazaSequenceSys.h>
#include <Pokemon.h>
#include <pokemonEvolution.h>

/*
 * The camera of the evolution demo: the demo stores the camera the player left
 * behind before it takes the camera over for the evolution scene, and hands it
 * back to the game once the scene is over.
 */

// The camera the demo took over from the game.
typedef struct CAM_STORE {
    /* 0x00 */ GSvec position;  // eye position of the camera
    /* 0x0C */ GSvec rotation;  // rotation of the camera
    /* 0x18 */ GSvec targetPos; // point the camera looks at
    /* 0x24 */ GSvec targetOfs; // offset of the target from the camera
    /* 0x30 */ f32 fov;          // field of view of the camera
} CAM_STORE;

// Type the camera is put back to when the demo hands it back.
#define EVOLUTION_CAMERA_TYPE 2

// Hand the camera back to the game, which runs the scene the player came from.
void restoreCamera(CAM_STORE *store) {
    cameraSetType(EVOLUTION_CAMERA_TYPE);
    cameraSetPosition(&store->position);
    cameraSetRotation(&store->rotation);
    cameraSetTargetPos(&store->targetPos);
    cameraSetTargetOfs(&store->targetOfs);
    cameraSetFov(store->fov);
}

// Take the camera of the game over for the demo.
void storeCamera(CAM_STORE *store) {
    GSvec position;
    GSvec rotation;
    GSvec targetPos;
    GSvec targetOfs;
    f32 fov;
    f32 aspect;
    f32 near;
    f32 far;

    cameraGetPosition(&position);
    cameraGetRotation(&rotation);
    cameraGetTargetPos(&targetPos);
    cameraGetTargetOfs(&targetOfs);
    GScameraGetPerspective(GScameraGetActiveCamera(), &fov, &aspect, &near, &far);

    store->position.x = position.x;
    store->position.y = position.y;
    store->position.z = position.z;
    store->rotation.x = rotation.x;
    store->rotation.y = rotation.y;
    store->rotation.z = rotation.z;
    store->targetPos.x = targetPos.x;
    store->targetPos.y = targetPos.y;
    store->targetPos.z = targetPos.z;
    store->targetOfs.x = targetOfs.x;
    store->targetOfs.y = targetOfs.y;
    store->targetOfs.z = targetOfs.z;
    store->fov = fov;
}

class ModelSequence;


enum STEP {
    STEP_EVOLUTION = 0,
    STEP_EVOLUTION_COMPLETE = 1,
    STEP_CANCEL_EARLY = 2,
    STEP_CANCEL_MIDDLE = 3,
    STEP_CANCEL_LATE = 4,
    STEP_COUNT = 5
};

struct WORK {
    ModelSequence *sequenceBefore;
    ModelSequence *sequenceAfter;
    STEP step;
    u32 bgmID;
    u32 envID;
    u32 bgmVolume;
    u32 envVolume;
    GSsndVolumeStack bgmStack;
    GSsndVolumeStack envStack;
};

enum {
    EVOLUTION_SOUND_FADE_TIME = 50
};

void restoreSound(WORK *work) {
    if (work->bgmID != 0) {
        soundOpenPlay(work->bgmID, EVOLUTION_SOUND_FADE_TIME, work->bgmVolume);
        GSsndSetPushVolumeID(work->bgmID, &work->bgmStack);
    }
    if (work->envID != 0) {
        soundOpenPlayEnv(work->envID, EVOLUTION_SOUND_FADE_TIME, work->envVolume);
        GSsndSetPushVolumeID(work->envID, &work->envStack);
    }
}

void stopSound(WORK *work) {
    s32 bgmID;
    s32 envID;
    u32 bgmVolume;
    u32 envVolume;

    bgmID = soundGetBGMID();
    if (bgmID != 0) {
        GSsndGetPushVolumeID(bgmID, &work->bgmStack);
        bgmVolume = soundGetVolume(bgmID);
        soundOpenPlay(SOUND_SILENT_BGM, EVOLUTION_SOUND_FADE_TIME, SOUND_VOLUME_MAX);
    } else {
        bgmVolume = 0;
    }

    envID = soundGetEnvID();
    if (envID != 0) {
        GSsndGetPushVolumeID(envID, &work->envStack);
        envVolume = soundGetVolume(envID);
        soundStopEnv(EVOLUTION_SOUND_FADE_TIME);
    } else {
        envVolume = 0;
    }

    work->bgmID = bgmID;
    work->envID = envID;
    work->bgmVolume = bgmVolume;
    work->envVolume = envVolume;
}


// Category of the move sequences that the evolution demo plays.
static const int EVOLUTION_WAZA_CATEGORY = 4;
// Species lookup failure returned by getPkxID.
static const u16 INVALID_PKX_ID = 0xFFFF;

// Which of the two Pokemon models a scene step uses.
enum EVOLUTION_MODEL {
    EVOLUTION_MODEL_BEFORE = 0,
    EVOLUTION_MODEL_AFTER = 1
};

void toVisible(WORK *work, int model, int visible);
s32 cbWazaForget(Pokemon *pokemon, u16 wazaID, s32 context);

struct StepSequence {
    u16 sequence;
    EVOLUTION_MODEL after;
};

// Move sequences loaded for each step of the evolution scene.
extern const StepSequence StepSeqTbl[STEP_COUNT] = {
    { 74, EVOLUTION_MODEL_BEFORE },  // STEP_EVOLUTION
    { 94, EVOLUTION_MODEL_AFTER },   // STEP_EVOLUTION_COMPLETE
    { 91, EVOLUTION_MODEL_BEFORE },  // STEP_CANCEL_EARLY
    { 92, EVOLUTION_MODEL_BEFORE },  // STEP_CANCEL_MIDDLE
    { 93, EVOLUTION_MODEL_BEFORE }   // STEP_CANCEL_LATE
};

// Cancelling in either of the first two timing windows plays that window's
// move sequence; a cancellation after both windows have passed plays the
// late sequence.
static const u32 EVOLUTION_CANCEL_EARLY_FRAMES = 480;
static const u32 EVOLUTION_CANCEL_MIDDLE_FRAMES = 540;
static const u32 EVOLUTION_CANCEL_FALLBACK_FRAMES = 6000;

struct CancelStepData {
    u32 elapsedFrames;
    STEP step;
};

extern const CancelStepData CancelStep[3] = {
    { EVOLUTION_CANCEL_EARLY_FRAMES, STEP_CANCEL_EARLY },    // first cancel window
    { EVOLUTION_CANCEL_MIDDLE_FRAMES, STEP_CANCEL_MIDDLE },  // second cancel window
    { EVOLUTION_CANCEL_FALLBACK_FRAMES, STEP_CANCEL_LATE }  // fallback row
};

struct ParameterData {
    Pokemon *before;
    Pokemon *after;
    int enableCancel;
    s32 wazaCount;
    u16 *wazaIDs;
    u8 *learnedSlots;
    EVOLUTION_RESULT result;
};

static ParameterData Parameter;

enum FADE_CTRL {
    FADE_CTRL_NONE = 0,
    FADE_CTRL_IN_BLACK = 1 << 1,
    FADE_CTRL_OUT_BLACK = 1 << 2,
    FADE_CTRL_IN_WHITE = 1 << 3,
    FADE_CTRL_OUT_WHITE = 1 << 4
};


static PokemonData *getPokemonData(Pokemon *pokemon) {
    if (pokemon == 0) return 0;
    if (!pokemonCheckValid(pokemon)) return 0;
    u16 species = pokemonBiosGetPokemonDataId(pokemon);
    return pokemonDataBiosGetPtr(species);
}

static u32 playPokemonVoice(Pokemon *pokemon) {
    PokemonData *data = getPokemonData(pokemon);
    if (data == 0) return 0;
    u16 voice = pokemonDataBiosGetVoice(data);
    GSsndPlayNow(voice);
    return voice;
}


// Three-way result of a question window: Yes, No, or any other dialog result.
enum EVOLUTION_DIALOG_ANSWER {
    EVOLUTION_DIALOG_ANSWER_YES = 0,
    EVOLUTION_DIALOG_ANSWER_NO = 1,
    EVOLUTION_DIALOG_ANSWER_OTHER = 2
};

static EVOLUTION_DIALOG_ANSWER dispMsgYesNo(u32 messageID, EVOLUTION_WINDOW_TYPE windowType);
static s32 getYesNo(EVOLUTION_WINDOW_TYPE windowType);
static void dispMsg(u32 messageID, EVOLUTION_WINDOW_TYPE windowType);
void closeMsgWin(EVOLUTION_WINDOW_TYPE windowType);
void openMsgWin(u32 messageID, EVOLUTION_WINDOW_TYPE windowType);

static EVOLUTION_DIALOG_ANSWER dispMsgYesNo(u32 messageID, EVOLUTION_WINDOW_TYPE windowType) {
    s32 answer;
    openMsgWin(messageID, windowType);
    answer = getYesNo(windowType);
    closeMsgWin(windowType);
    switch (answer) {
        case EVOLUTION_DIALOG_ANSWER_YES: return EVOLUTION_DIALOG_ANSWER_YES;
        case EVOLUTION_DIALOG_ANSWER_NO: return EVOLUTION_DIALOG_ANSWER_NO;
        default: return EVOLUTION_DIALOG_ANSWER_OTHER;
    }
}

static s32 getYesNo(EVOLUTION_WINDOW_TYPE windowType) {
    if (windowType == EVOLUTION_WINDOW_FIELD) {
        return menuSubOpenYesNoField();
    }
    return menuSubOpenYesNo(MENU_SUB_YES_NO_DIALOG, -1, -1, 0);
}

static void dispMsg(u32 messageID, EVOLUTION_WINDOW_TYPE windowType) {
    openMsgWin(messageID, windowType);
    closeMsgWin(windowType);
}

void closeMsgWin(EVOLUTION_WINDOW_TYPE windowType) {
    if (windowType == EVOLUTION_WINDOW_FIELD) {
        winMsgCloseField(true);
    } else {
        winMsgClose(true);
    }
}

void openMsgWin(u32 messageID, EVOLUTION_WINDOW_TYPE windowType) {
    if (windowType == EVOLUTION_WINDOW_FIELD) {
        winMsgOpenField(messageID, true, false);
    } else {
        winMsgOpen(WINMSG_WINDOW_MENU, messageID, true, false);
    }
}


// Nominal frame rate used to convert the wait duration into frame ticks.
static const f32 EVOLUTION_FRAME_RATE = 60.0f;

static void waitTime(f32 seconds) {
    f32 limit;
    f32 elapsed = 0.0f;
    limit = EVOLUTION_FRAME_RATE * seconds;
    while (elapsed < limit) {
        GSthreadSwitch();
        elapsed += timeGetLastFrameTime();
    }
}

u16 getPkxID(Pokemon *pokemon) {
    u16 species = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0);
    if (species == 0) return INVALID_PKX_ID;
    u16 pkxID = pokemonGetStatus(0, species, POKEMON_QUERY_PKX_DATA_ID, 0);
    if (pkxID == 0) return INVALID_PKX_ID;
    return pkxID;
}

ModelSequence *loadSequence(Pokemon *pokemon, int after) {
    int i;
    ModelSequence *sequence;
    u16 pkxID = getPkxID(pokemon);
    if (pkxID == INVALID_PKX_ID) return 0;

    u32 rnd = pokemonBiosGetRnd(pokemon);
    bool shiny = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SHINY, 0) != 0;
    sequence = Sequence_LoadPokemon(pkxID, rnd, shiny);
    if (sequence == 0) return 0;

    for (i = 0; i < STEP_COUNT; ++i) {
        if (after == StepSeqTbl[i].after) {
            if (Sequence_LoadWaza(sequence, StepSeqTbl[i].sequence, EVOLUTION_WAZA_CATEGORY, 0) == 0) break;
        }
    }
    if (i < STEP_COUNT) return 0;
    return sequence;
}


static bool loadSequenceAfter(WORK *work, Pokemon *pokemon) {
    ModelSequence *sequence = loadSequence(pokemon, EVOLUTION_MODEL_AFTER);
    if (sequence == 0) return false;
    work->sequenceAfter = sequence;
    return true;
}

static bool loadSequenceBefore(WORK *work, Pokemon *pokemon) {
    ModelSequence *sequence = loadSequence(pokemon, EVOLUTION_MODEL_BEFORE);
    if (sequence == 0) return false;
    work->sequenceBefore = sequence;
    return true;
}

void toVisible(WORK *work, int model, int visible) {
    ModelSequence *sequence = model != EVOLUTION_MODEL_BEFORE ? work->sequenceAfter : work->sequenceBefore;
    Sequence_SetVisibility(sequence, visible != 0);
}

// Resource ID looked up across resource groups for the evolution GSmodel.
static const u32 EVOLUTION_MODEL_RES_ID = 0xFB31000;

static int loadAll(WORK *work, Pokemon *before, Pokemon *after) {
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(EVOLUTION_MODEL_RES_ID);
    if (model != 0) {
        GSmodelSetForceUseFractionalFramesFlag(model, true);
        GSmodelSetSkipFractionalFramesFlag(model, true);
    }
    work->sequenceBefore = 0;
    work->sequenceAfter = 0;
    if (loadSequenceBefore(work, before) && loadSequenceAfter(work, after)) return 1;
    return 0;
}

enum EVOLUTION_LEARN_MESSAGE_ID {
    EVOLUTION_LEARN_MESSAGE_ID_FORGET_PROMPT = 50026,
    EVOLUTION_LEARN_MESSAGE_ID_GIVE_UP_PROMPT = 50027,
    EVOLUTION_LEARN_MESSAGE_ID_GAVE_UP = 50028,
    EVOLUTION_LEARN_MESSAGE_ID_FORGOT_MOVE = 50029,
    EVOLUTION_LEARN_MESSAGE_ID_LEARNED_MOVE = 50030
};

static const int SE_FORGET_WAZA = 33;
static const int ME_LEARN_WAZA = 967;

// Number of move records a Pokemon carries, bound of the learn/forget scan.
static const int EVOLUTION_WAZA_CAPACITY = 4;


extern "C" s32 evolutionWazaLearn(Pokemon *pokemon, u16 wazaID, u8 *outIndex,
                                  EVOLUTION_WINDOW_TYPE windowType,
                                  s32 (*forgetWazaSelect)(Pokemon *, u16, s32), s32 context)
{
    s32 slot;
    for (slot = 0; slot < EVOLUTION_WAZA_CAPACITY; ++slot) {
        if (pokemonBiosGetPokemonWazaDataId(pokemon, slot) == 0) break;
    }
    if (slot >= EVOLUTION_WAZA_CAPACITY) {
        u16 *nickname = pokemon_GetNickname(pokemon);
        msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)nickname);
        msgctrlSetValue(MSGCTRL_VALUE_WAZA_ID, wazaID);
        for (;;) {
            if (dispMsgYesNo(EVOLUTION_LEARN_MESSAGE_ID_FORGET_PROMPT, windowType) == EVOLUTION_DIALOG_ANSWER_YES) {
                slot = forgetWazaSelect != 0 ? (s8)forgetWazaSelect(pokemon, wazaID, context) : 0;
                if (slot >= 0) break;
            }
            msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)nickname);
            msgctrlSetValue(MSGCTRL_VALUE_WAZA_ID, wazaID);
            if (dispMsgYesNo(EVOLUTION_LEARN_MESSAGE_ID_GIVE_UP_PROMPT, windowType) == EVOLUTION_DIALOG_ANSWER_YES) {
                dispMsg(EVOLUTION_LEARN_MESSAGE_ID_GAVE_UP, windowType);
                return 0;
            }
        }
        msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)pokemon_GetNickname(pokemon));
        msgctrlSetValue(MSGCTRL_VALUE_SOUND_ID, SE_FORGET_WAZA);
        msgctrlSetValue(MSGCTRL_VALUE_WAZA_ID, pokemonBiosGetPokemonWazaDataId(pokemon, slot));
        openMsgWin(EVOLUTION_LEARN_MESSAGE_ID_FORGOT_MOVE, windowType);
    }
    soundOpenPlayME(ME_LEARN_WAZA, 0, SOUND_VOLUME_MAX);
    msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)pokemon_GetNickname(pokemon));
    msgctrlSetValue(MSGCTRL_VALUE_WAZA_ID, wazaID);
    dispMsg(EVOLUTION_LEARN_MESSAGE_ID_LEARNED_MOVE, windowType);
    *outIndex = slot;
    return 1;
}

s32 cbWazaForget(Pokemon *pokemon, u16 wazaID, s32 context) {
    fadeSet(FADE_OUT_BLACK, 0.5f);
    fadeCheck(true);
    CAM_STORE store;
    storeCamera(&store);
    s32 result = menuPokemonStatusOpenWazaLearn(pokemon, wazaID);
    if (result >= EVOLUTION_WAZA_CAPACITY) result = -1;
    WORK *work = (WORK *)context;
    loadSequenceAfter(work, pokemon);
    GSmodelSetAnimType(Sequence_GetGSmodel((u32)work->sequenceAfter), GS_MODEL_ANIM_LOOP);
    toVisible(work, EVOLUTION_MODEL_AFTER, true);
    restoreCamera(&store);
    fadeSet(FADE_IN_BLACK, 0.5f);
    fadeCheck(true);
    return (s8)result;
}


ModelSequence *getStepSeq(WORK *work, STEP step);

s32 doWazaSequence(WORK *work, STEP step, int enableCancel, FADE_CTRL fadeCtrl) {
    s32 elapsed = 0;
    int cancelled = 0;
    CAM_STORE store;
    if (step < 0 || step >= STEP_COUNT) return 0;
    ModelSequence *sequence = StepSeqTbl[step].after != EVOLUTION_MODEL_BEFORE ? work->sequenceAfter : work->sequenceBefore;
    WazaSequence *waza = Sequence_GetWaza(sequence, StepSeqTbl[step].sequence, EVOLUTION_WAZA_CATEGORY);
    WazaSequence_Start(waza, true);
    while (!cancelled) {
        if (fadeCtrl & (FADE_CTRL_IN_BLACK | FADE_CTRL_IN_WHITE)) {
            u16 fadeType = FADE_IN_WHITE;
            if (fadeCtrl & FADE_CTRL_IN_BLACK) fadeType = FADE_IN_BLACK;
            fadeSet(fadeType, 0.5f);
            fadeCtrl = (FADE_CTRL)(fadeCtrl & ~(FADE_CTRL_IN_BLACK | FADE_CTRL_IN_WHITE));
        }
        if (!WazaSequence_IsActive(waza)) break;
        GSthreadSwitch();
        storeCamera(&store);
        elapsed += timeGetLastFrameTime();
        if (enableCancel != 0) {
            u32 changed = GSinputButtonsChanged(GS_INPUT_PORT_1);
            u32 pressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
            if ((pressed & changed & PAD_BUTTON_B) != 0) cancelled = 1;
        }
    }
    restoreCamera(&store);
    if (fadeCtrl & (FADE_CTRL_OUT_BLACK | FADE_CTRL_OUT_WHITE)) {
        u16 fadeType = FADE_OUT_WHITE;
        if (fadeCtrl & FADE_CTRL_OUT_BLACK) fadeType = FADE_OUT_BLACK;
        fadeSet(fadeType, 0.5f);
        fadeCheck(true);
    }
    WazaSequence_Free(waza);
    if (cancelled) return elapsed;
    return -1;
}


void wazaSeqEnd(WORK *work) {
    STEP step = work->step;
    ModelSequence *sequence = getStepSeq(work, step);
    WazaSequence *waza = Sequence_GetWaza(sequence, StepSeqTbl[step].sequence, EVOLUTION_WAZA_CATEGORY);
    WazaSequence_Free(waza);
}

int wazaSeqUpdate(WORK *work) {
    STEP step = work->step;
    ModelSequence *sequence = getStepSeq(work, step);
    WazaSequence *waza = Sequence_GetWaza(sequence, StepSeqTbl[step].sequence, EVOLUTION_WAZA_CATEGORY);
    return WazaSequence_IsActive(waza) ? 1 : 0;
}

ModelSequence *getStepSeq(WORK *work, STEP step) {
    if (StepSeqTbl[step].after != EVOLUTION_MODEL_BEFORE) return work->sequenceAfter;
    return work->sequenceBefore;
}

int wazaSeqStart(WORK *work, STEP step) {
    if (step < 0 || step >= STEP_COUNT) return 0;
    ModelSequence *sequence = getStepSeq(work, step);
    WazaSequence *waza = Sequence_GetWaza(sequence, StepSeqTbl[step].sequence, EVOLUTION_WAZA_CATEGORY);
    WazaSequence_Start(waza, true);
    work->step = step;
    return 1;
}


enum EVOLUTION_MESSAGE_ID {
    EVOLUTION_MESSAGE_ID_START = 50201,
    EVOLUTION_MESSAGE_ID_COMPLETE = 50202,
    EVOLUTION_MESSAGE_ID_CANCELLED = 50203
};

enum EVOLUTION_SOUND_ID {
    EVOLUTION_SOUND_ID_COMPLETE = 975,
    EVOLUTION_SOUND_ID_PRELUDE = 976,
    EVOLUTION_SOUND_ID_LOOP = 977
};

// Number of frame ticks of the cinematic, and its cancellation timing gates.
static const u32 EVOLUTION_DEMO_FRAMES = 570;
static const u32 EVOLUTION_CANCEL_MIN_FRAMES = 120;
static const int EVOLUTION_PRELUDE_GAP_FRAMES = 30;

enum EVOLUTION_SOUND_PHASE {
    EVOLUTION_SOUND_VOICE = 0,
    EVOLUTION_SOUND_PRELUDE = 1,
    EVOLUTION_SOUND_LOOP = 2
};


s32 evolutionDemo(WORK *work, int enableCancel, Pokemon *before, Pokemon *after) {
    toVisible(work, EVOLUTION_MODEL_BEFORE, true);
    u16 *nickname = pokemon_GetNickname(before);
    msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)nickname);
    openMsgWin(EVOLUTION_MESSAGE_ID_START, EVOLUTION_WINDOW_FIELD);
    int firstFrame;
    s32 voiceID;
    u32 elapsed;
    int cancelled;
    EVOLUTION_SOUND_PHASE soundPhase;
    int phaseTime;
    voiceID = playPokemonVoice(before);
    wazaSeqStart(work, STEP_EVOLUTION);
    firstFrame = 1;
    cancelled = 0;
    soundPhase = EVOLUTION_SOUND_VOICE;
    elapsed = 0;
    while (elapsed < EVOLUTION_DEMO_FRAMES) {
        if (enableCancel && elapsed >= EVOLUTION_CANCEL_MIN_FRAMES && soundPhase >= EVOLUTION_SOUND_LOOP) {
            u32 changed = GSinputButtonsChanged(GS_INPUT_PORT_1);
            u32 pressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
            if ((pressed & changed & PAD_BUTTON_B) != 0) {
                cancelled = 1;
                break;
            }
        }
        if (!wazaSeqUpdate(work)) break;
        switch (soundPhase) {
            case EVOLUTION_SOUND_VOICE:
                if (GSsndGetStatus(voiceID) != GSSND_PLAYING) {
                    soundOpenPlay(EVOLUTION_SOUND_ID_PRELUDE, 0, SOUND_VOLUME_MAX);
                    phaseTime = 0;
                    soundPhase = EVOLUTION_SOUND_PRELUDE;
                }
                break;
            case EVOLUTION_SOUND_PRELUDE:
                if (GSsndGetStatus(EVOLUTION_SOUND_ID_PRELUDE) != GSSND_PLAYING) {
                    phaseTime += timeGetLastFrameTime();
                    if (phaseTime >= EVOLUTION_PRELUDE_GAP_FRAMES) {
                        soundOpenPlay(EVOLUTION_SOUND_ID_LOOP, 0, SOUND_VOLUME_MAX);
                        soundPhase = EVOLUTION_SOUND_LOOP;
                    }
                }
                break;
        }
        if (firstFrame) {
            fadeSet(FADE_IN_BLACK, 0.5f);
            firstFrame = 0;
        }
        GSthreadSwitch();
        elapsed += timeGetLastFrameTime();
    }
    fadeSet(FADE_OUT_WHITE, 0.5f);
    while (fadeCheck(false)) {
        wazaSeqUpdate(work);
        GSthreadSwitch();
    }
    wazaSeqEnd(work);
    waitTime(1.0f);
    if (cancelled) {
        closeMsgWin(EVOLUTION_WINDOW_FIELD);
        soundStop(EVOLUTION_SOUND_ID_LOOP, EVOLUTION_SOUND_FADE_TIME);
        int index = 0;
        if (CancelStep[0].elapsedFrames < elapsed) {
            index = 1;
            if (CancelStep[1].elapsedFrames < elapsed) index = 2;
        }
        doWazaSequence(work, CancelStep[index].step, false, FADE_CTRL_IN_WHITE);
        voiceID = playPokemonVoice(before);
        while (GSsndGetStatus(voiceID) == GSSND_PLAYING) GSthreadSwitch();
        msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)pokemon_GetNickname(before));
        dispMsg(EVOLUTION_MESSAGE_ID_CANCELLED, EVOLUTION_WINDOW_FIELD);
        fadeSet(FADE_OUT_BLACK, 0.5f);
        fadeCheck(true);
        return 0;
    }
    toVisible(work, EVOLUTION_MODEL_BEFORE, false);
    toVisible(work, EVOLUTION_MODEL_AFTER, true);
    doWazaSequence(work, STEP_EVOLUTION_COMPLETE, false, FADE_CTRL_IN_WHITE);
    closeMsgWin(EVOLUTION_WINDOW_FIELD);
    soundStop(EVOLUTION_SOUND_ID_LOOP, EVOLUTION_SOUND_FADE_TIME);
    voiceID = playPokemonVoice(after);
    while (GSsndGetStatus(voiceID) == GSSND_PLAYING) GSthreadSwitch();
    msgctrlSetValue(MSGCTRL_POKEMON_NAME, (unsigned long)pokemon_GetNickname(before));
    u16 species = pokemonBiosGetPokemonDataId(after);
    msgctrlSetValue(MSGCTRL_VALUE_POKEMON_ID, species);
    msgctrlSetValue(MSGCTRL_VALUE_SOUND_ID, EVOLUTION_SOUND_ID_COMPLETE);
    dispMsg(EVOLUTION_MESSAGE_ID_COMPLETE, EVOLUTION_WINDOW_FIELD);
    waitTime(0.5f);
    return 1;
}

static const int EVOLUTION_FLOOR_ID = 902;

s32 evolution__Main(WORK *work, int enableCancel, Pokemon *before, Pokemon *after,
                    u16 *wazaIDs, s32 wazaCount, u8 *learnedSlots) {
    stopSound(work);
    s32 demoResult = evolutionDemo(work, enableCancel, before, after);
    restoreSound(work);
    if (demoResult == 0) return 0;
    Pokemon copy;
    pokemonBiosCopy(&copy, after);
    for (s32 i = 0; i < wazaCount; ++i, ++wazaIDs, ++learnedSlots) {
        u8 slot;
        u16 wazaID = *wazaIDs;
        if (evolutionWazaLearn(&copy, wazaID, &slot, EVOLUTION_WINDOW_FIELD, cbWazaForget, (s32)work)) {
            pokemonWazaCreate(&copy, slot, wazaID);
        } else {
            slot = EVOLUTION_WAZA_NOT_LEARNED;
        }
        *learnedSlots = slot;
    }
    fadeSet(FADE_OUT_BLACK, 0.5f);
    fadeCheck(true);
    return 1;
}

extern "C" EVOLUTION_RESULT evolutionStart(Pokemon *before, Pokemon *after, int enableCancel,
                                          u16 *wazaIDs, s32 wazaCount, u8 *learnedSlots) {
    wazaSequenceSysEnableLayerChange();
    WORK work;
    if (!loadAll(&work, before, after)) return EVOLUTION_RESULT_ERROR;
    return evolution__Main(&work, enableCancel, before, after, wazaIDs, wazaCount, learnedSlots) == 0
               ? EVOLUTION_RESULT_CANCEL : EVOLUTION_RESULT_COMPLETE;
}

extern "C" void evolution(void) {
    Parameter.result = evolutionStart(Parameter.before, Parameter.after, Parameter.enableCancel,
                                      Parameter.wazaIDs, Parameter.wazaCount, Parameter.learnedSlots);
    GSfloorPop();
    floorSetFadeScript(0, 0);
}

extern "C" void evolutionExit(void) {}

extern "C" void evolutionMain(void) {
    evolution();
}

extern "C" void evolutionInit(void) {}

extern "C" EVOLUTION_RESULT evolutionOpen(Pokemon *before, Pokemon *after, int enableCancel,
                                         u16 *wazaIDs, s32 wazaCount, u8 *learnedSlots) {
    Parameter.before = before;
    Parameter.after = after;
    Parameter.enableCancel = enableCancel;
    Parameter.wazaCount = wazaCount;
    Parameter.wazaIDs = wazaIDs;
    Parameter.learnedSlots = learnedSlots;
    GSfloorPush(EVOLUTION_FLOOR_ID);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    return Parameter.result;
}
