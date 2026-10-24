#ifndef _MUSYX_SYNTH
#define _MUSYX_SYNTH
#include "musyx/musyx.h"

#include "musyx/adsr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  MAC_STATE_RUNNABLE = 0,
  MAC_STATE_YIELDED,
  MAC_STATE_STOPPED,
} MAC_STATE;

typedef struct MSTEP {
  u32 para[2];
} MSTEP;

typedef struct CALLSTACK {
  MSTEP* addr;
  MSTEP* curAddr;
} CALLSTACK;

typedef struct SYNTH_VOICELIST {
  u8 prev;
  u8 next;
  u16 user;
} SYNTH_VOICELIST;
extern SYNTH_VOICELIST voicePrioSortVoices[64];
extern u8 voicePrioSortVoicesRoot[256];
extern SYNTH_VOICELIST voicePrioSortVoices[64];

typedef struct SYNTH_ROOTLIST {
  u16 next;
  u16 prev;
} SYNTH_ROOTLIST;

extern SYNTH_ROOTLIST voicePrioSortRootList[256];

typedef struct synthInfo {
  u32 mixFrq;
  u32 numSamples;
  SND_PLAYBACKINFO pbInfo;
  u8 voiceNum;
  u8 maxMusic;
  u8 maxSFX;
  u8 studioNum;
} SynthInfo;
extern SynthInfo synthInfo;

typedef struct VID_LIST {
  struct VID_LIST* next;
  struct VID_LIST* prev;
  u32 vid;
  u32 root;
} VID_LIST;

typedef struct SYNTH_QUEUE {
  struct SYNTH_QUEUE* next;
  struct SYNTH_QUEUE* prev;
  u8 voice;
  u8 jobTabIndex;
} SYNTH_QUEUE;

typedef enum {
  SYNTH_JOBTYPE_LOW = 0,
  SYNTH_JOBTYPE_ZERO = 1,
  SYNTH_JOBTYPE_EVENT = 2,
} SYNTH_JOBTYPE;

typedef struct {
  SYNTH_QUEUE* lowPrecision;
  SYNTH_QUEUE* event;
  SYNTH_QUEUE* zeroOffset;
} SYNTH_JOBTAB;

typedef struct SYNTH_LFO {
  u32 time;
  u32 period;
  s16 value;
  s16 lastValue;
} SYNTH_LFO;

typedef struct SYNTHMasterFader {
  f32 volume;
  f32 target;
  f32 start;
  f32 time;
  f32 deltaTime;
  f32 pauseVol;
  f32 pauseTarget;
  f32 pauseStart;
  f32 pauseTime;
  f32 pauseDeltaTime;
  u32 seqId;
  u8 seqMode;
  u8 type;
} SYNTHMasterFader;

typedef struct CTRL_SOURCE {
  u8 midiCtrl;
  u8 combine;
  s32 scale;
} CTRL_SOURCE;

typedef struct CTRL_DEST {
  CTRL_SOURCE source[4];
  u16 oldValue;
  u8 numSource;
} CTRL_DEST;

#pragma push
#pragma pack(4)
typedef struct SYNTH_VOICE {
  SYNTH_QUEUE lowPrecisionJob;
  SYNTH_QUEUE zeroOffsetJob;
  SYNTH_QUEUE eventJob;
  u64 lastLowCallTime;
  u64 lastZeroCallTime;
  MSTEP* addr;
  MSTEP* curAddr;
  struct SYNTH_VOICE* nextMacActive;
  struct SYNTH_VOICE* prevMacActive;
  struct SYNTH_VOICE* nextTimeQueueMacro;
  struct SYNTH_VOICE* prevTimeQueueMacro;
  MAC_STATE macState;
  MSTEP* trapEventAddr[3];
  MSTEP* trapEventCurAddr[3];
  u8 trapEventAny;
  CALLSTACK callStack[4];
  u8 callStackEntryNum;
  u8 callStackIndex;
  u64 macStartTime;
  u64 wait;
  u64 waitTime;
  u8 timeUsedByInput;
  u16 loop;
  s32 local_vars[16];
  u32 child;
  u32 parent;
  u32 id;
  VID_LIST* vidList;
  VID_LIST* vidMasterList;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 0) && MUSY_VERSION <= MUSY_VERSION_CHECK(2, 0, 0)
  u16 allocId;
#else
  u32 allocId;
#endif
  u16 macroId;
  u8 keyGroup;
  u32 lastVID;
  u8 prio;
  u16 ageSpeed;
  u32 age;
  u64 cFlags;
  u8 block;
  u8 fxFlag;
  u8 vGroup;
  u8 studio;
  u8 track;
  u8 midi;
  u8 midiSet;
  u8 section;
#if MUSY_VERSION <= MUSY_VERSION_CHECK(1, 5, 0)
  void* sAddr;
#endif
  u32 sInfo;
  u32 playFrq;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  u16 sampleId;
#endif
  u16 curNote;
  s8 curDetune;
  u8 orgNote;
  u8 lastNote;
  u8 portType;
  u16 portLastCtrlState;
  u32 portDuration;
  u32 portCurPitch;
  u32 portTime;
  u8 vibKeyRange;
  u8 vibCentRange;
  u32 vibPeriod;
  u32 vibCurTime;
  s32 vibCurOffset;
  s16 vibModAddScale;
  u32 volume;
  u32 orgVolume;
  float lastVolFaderScale;
  u32 lastPan;
  u32 lastSPan;
  float treCurScale;
  u16 treScale;
  u16 treModAddScale;
  u32 panning[2];
  s32 panDelta[2];
  u32 panTarget[2];
  u32 panTime[2];
  u8 revVolScale;
  u8 revVolOffset;
  u8 volTable;
  u8 itdMode;
  s32 envDelta;
  u32 envTarget;
  u32 envCurrent;
  u32 sweepOff[2];
  s32 sweepAdd[2];
  s32 sweepCnt[2];
  u8 sweepNum[2];
  SYNTH_LFO lfo[2];
  u8 lfoUsedByInput[2];
  u8 pbLowerKeyRange;
  u8 pbUpperKeyRange;
  u16 pbLast;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  u32 lpfLowerFrqBoundary;
  u32 lpfUpperFrqBoundary;
#endif
  ADSR_VARS pitchADSR;
  s16 pitchADSRRange;
  u16 curPitch;
  struct setup {
    u8 vol;
    u8 pan;
    u8 midi;
    u8 midiSet;
    u8 section;
    u8 track;
    u8 vGroup;
    u8 studio;
    u8 itdMode;
  } setup;
  u32 midiDirtyFlags;
  CTRL_DEST inpVolume;
  CTRL_DEST inpPanning;
  CTRL_DEST inpSurroundPanning;
  CTRL_DEST inpPitchBend;
  CTRL_DEST inpDoppler;
  CTRL_DEST inpModulation;
  CTRL_DEST inpPedal;
  CTRL_DEST inpPortamento;
  CTRL_DEST inpPreAuxA;
  CTRL_DEST inpReverb;
  CTRL_DEST inpPreAuxB;
  CTRL_DEST inpPostAuxB;
  CTRL_DEST inpTremolo;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  CTRL_DEST inpFilterSwitch;
  CTRL_DEST inpFilterParameter;
#endif
  u8 mesgNum;
  u8 mesgRead;
  u8 mesgWrite;
  s32 mesgQueue[4];
  u16 curOutputVolume;
} SYNTH_VOICE;

typedef struct synthITDInfo {
  u8 music;
  u8 sfx;
} synthITDInfo;

#pragma pop
typedef void (*SYNTH_MESSAGE_CALLBACK)(u32, s32);

extern SND_AUX_CALLBACK synthAuxACallback[8];
extern u8 synthAuxAMIDI[8];
extern u8 synthAuxAMIDISet[8];
extern void* synthAuxAUser[8];
extern SND_AUX_CALLBACK synthAuxBCallback[8];
extern u8 synthAuxBMIDI[8];
extern u8 synthAuxBMIDISet[8];
extern void* synthAuxBUser[8];

extern synthITDInfo synthITDDefault[8];
extern u32 synthFlags;
extern SYNTH_VOICE* synthVoice;
extern u8 sndActive;
extern u8 synthIdleWaitActive;
extern SynthInfo synthInfo;
extern s32 synthGlobalVariable[16];
extern u64 synthRealTime;
extern SYNTH_MESSAGE_CALLBACK synthMessageCallback;

u32 synthGetTicksPerSecond(SYNTH_VOICE* svoice);
void synthKillVoicesByMacroReferences(u16* ref);
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
void synthKillVoicesBySampleReferences(u16* ref);
#endif
void synthExit();
void synthInit(u32 mixFrq, u32 numVoices);
void synthSetBpm(u32 pbm, u8 set, u8 section);
void synthFXCloneMidiSetup(SYNTH_VOICE* dest, SYNTH_VOICE* src);
void synthSetMusicVolumeType(u8 vGroup, u8 type);
void synthInitAllocationAids();
void synthInitPortamento(SYNTH_VOICE* svoice);
void synthStartSynthJobHandling(SYNTH_VOICE* svoice);
void synthForceLowPrecisionUpdate(SYNTH_VOICE* svoice);
void synthKeyStateUpdate(SYNTH_VOICE* svoice);
void synthHandle(u32 deltaTime);
bool synthFXSetCtrl(SND_VOICEID vid, u8 ctrl, u8 value);
bool synthFXSetCtrl14(SND_VOICEID vid, u8 ctrl, u16 value);
bool synthSendKeyOff(SND_VOICEID vid);
SND_VOICEID synthFXStart(u16 fid,
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 2)
                         u8 key,
#endif
                         u8 vol, u8 pan, u8 studio, u32 itd);
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
int synthCheckFXRealloc(unsigned short fid);
#endif
void synthVolume(u8 volume, u16 time, u8 vGroup, u8 seqMode, u32 seqId);
u32 synthStartSound(u16 id, u8 prio, u8 max,
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
                    u32 sourceID,
#endif
                    u8 key, u8 vol, u8 panning, u8 midi, u8 midiSet, u8 section, u16 step,
                    u16 trackid, u8 vGroup, s16 prioOffset, u8 studio, u32 itd);
bool synthIsFadeOutActive(u8 vGroup);
void synthActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type);
void synthDeactivateStudio(u8 studio);
u8 synthFXGetMaxVoices(u16 fid);
void synthPauseVolume(u8 volume, u16 time, u8 vGroup);
void synthKillAllVoices(bool8 musiconly);
void synthKeyStateUpdate(SYNTH_VOICE* svoice);
bool synthAddStudioInput(u8 studio, SND_STUDIO_INPUT* in_desc);
bool synthRemoveStudioInput(u8 studio, SND_STUDIO_INPUT* in_desc);
u32 synthGetTicksPerSecond(SYNTH_VOICE* svoice);
u32 adsrSetup(ADSR_VARS* adsr);

#ifdef __cplusplus
}
#endif

#endif // _MUSYX_SYNTH
