#ifndef _MUSYX_SEQ
#define _MUSYX_SEQ

#include "musyx/musyx.h"
#include "musyx/voice.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct PAGE {
  u16 macro;
  u8 prio;
  u8 maxVoices;
  u8 index;
  u8 reserved;
} PAGE;

typedef struct ARR {
  u32 tTab;
  u32 pTab;
  u32 tmTab;
  u32 mTrack;
  u32 info;
  u32 loopPoint[16];
  u32 tsTab;
} ARR;

#define ARR_GET(arr, offset) ((void*)(offset + (uintptr_t)arr))
#define ARR_GET_TYPE(arr, offset, ty) ((ty)ARR_GET(arr, offset))

typedef struct TENTRY {
  u32 time;
  u8 prgChange;
  u8 velocity;
  u8 res[2];
  u16 pattern;
  s8 transpose;
  s8 velocityAdd;
} TENTRY;

typedef struct TRACK {
  struct TENTRY* base;
  struct TENTRY* addr;
} TRACK;

typedef struct NOTE {
  struct NOTE* next;
  struct NOTE* prev;
  u32 id;
  s32 endTime;
  u8 section;
  u8 timeIndex;
  u8 reserved[2];
} NOTE;

typedef struct NOTE_DATA {
  u16 time;
  u8 key;
  u8 velocity;
  u16 length;
} NOTE_DATA;

typedef struct PRG_STATE {
  u16 macId;
  u8 priority;
  u8 maxVoices;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  u8 program;
#endif
} PRG_STATE;

typedef struct SEQ_STREAM {
  u8* nextAddr;
  u16 value;
  s16 nextDelta;
  u32 nextTime;
} SEQ_STREAM;

typedef struct CPAT {
  u32 lTime;
  u32 baseTime;
  NOTE_DATA* addr;
  TENTRY* patternInfo;
  SEQ_STREAM pitchBend;
  SEQ_STREAM modulation;
  u8 midi;
} CPAT;

typedef struct SEQ_EVENT {
  struct SEQ_EVENT* next;
  struct SEQ_EVENT* prev;
  u32 time;
  union evInfo {
    TENTRY* trackAddr;
    struct {
      NOTE_DATA* addr;
      CPAT* base;
    } pattern;
  } info;
  u8 type;
  u8 trackId;
} SEQ_EVENT;

typedef struct MTRACK_DATA {
  volatile u32 time;
  u32 bpm;
} MTRACK_DATA;

typedef struct MTRACK {
  MTRACK_DATA* base;
  MTRACK_DATA* addr;
} MTRACK;

typedef struct TICKS {
  u32 low;
  s32 high;
} TICKS;

typedef struct SEQ_SECTION {
  MTRACK mTrack;
  u32 bpm;
  TICKS tickDelta[2];
  SEQ_EVENT* globalEventRoot;
  TICKS time[2];
  u8 timeIndex;
  u16 speed;
  u16 loopCnt;
  u8 loopDisable;
} SEQ_SECTION;

typedef struct SEQ_INSTANCE {
  struct SEQ_INSTANCE* next;
  struct SEQ_INSTANCE* prev;
  u8 state;
  u8 index;
  u16 groupID;
  u32 publicId;
  PAGE* normtab;
  u8 normTrans[128];
  PAGE* drumtab;
  u8 drumTrans[128];
  ARR* arrbase;
  u32 trackMute[2];
  TRACK track[64];
  u8 trackVolGroup[64];
  CPAT pattern[64];
  NOTE* noteUsed[2];
  NOTE* noteKeyOff;
  PRG_STATE prgState[16];
  u8 defVGroup;
  SND_CROSSFADE syncCrossInfo;
  u32* syncSeqIdPtr;
  u8 syncActive;
  u8 defStudio;
  u8 keyOffCheck;
  SEQ_EVENT event[64];
  u8* trackSectionTab;
  SEQ_SECTION section[16];
} SEQ_INSTANCE;

typedef struct SEQ_PATTERN {
  u32 headerLen;
  u32 pitchBend;
  u32 modulation;
  u32 noteData;
} SEQ_PATTERN;

#pragma push
#pragma pack(1)
typedef struct CHANNEL_DEFAULTS {
  u8 pbRange;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  u32 lpfLowerFrqBoundary;
  u32 lpfUpperFrqBoundary;
#endif
} CHANNEL_DEFAULTS;
#pragma pop

typedef struct MIDI_CHANNEL_SETUP {
  u8 program;
  u8 volume;
  u8 panning;
  u8 reverb;
  u8 chorus;
} MIDI_CHANNEL_SETUP;

typedef struct MIDISETUP {
  u16 songId;
  u16 reserved;
  MIDI_CHANNEL_SETUP channel[16];
} MIDISETUP;

extern u8 synthTrackVolume[64];
extern SEQ_INSTANCE seqInstance[8];
extern u16 seqMIDIPriority[8][16];

void seqInit(); /* extern */
u32 seqStartPlay(PAGE* norm, PAGE* drum, MIDISETUP* midiSetup, u32* song, SND_PLAYPARA* para,
                 u8 studio, u16 sgid);
u32 seqGetPrivateId(u32 seqId);
void seqSpeed(u32 seqId, u16 speed);
void seqVolume(u8 volume, u16 time, u32 seqId, u8 mode);
void seqStop(u32 seqId);
u16 seqGetMIDIPriority(u8 set, u8 channel);
void seqCrossFade(SND_CROSSFADE* ci, u32* new_seqId, bool8 irq_call);
u32 seqPlaySong(u16 sgid, u16 sid, void* arrfile, SND_PLAYPARA* para, u8 irq_call, u8 studio);
void seqPause(SND_SEQID seqId);
void seqContinue(SND_SEQID seqId);
void seqMute(SND_SEQID seqId, u32 mask1, u32 mask2);
void seqKillInstancesByGroupID(SND_GROUPID sgid);
void seqKillAllInstances();
void seqHandle(u32 deltaTime);

u8 inpTranslateExCtrl(u8 ctrl);
void inpSetGlobalMIDIDirtyFlag(u8 chan, u8 midiSet, s32 flag);
void inpAddCtrl(CTRL_DEST* dest, u8 ctrl, s32 scale, u8 comb, u32 isVar);
void inpSetMidiCtrl(u8 ctrl, u8 channel, u8 set, u8 value);
void inpSetMidiCtrl14(u8 ctrl, u8 channel, u8 set, u16 value);
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 2)
u32 seqGetInstanceForVoice(u32 vid);
#endif
void inpSetExCtrl(SYNTH_VOICE* svoice, u8 ctrl, s16 v);
CHANNEL_DEFAULTS* inpGetChannelDefaults(u8 midi, u8 midiSet);
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
unsigned short inpGetFilterSwitch(struct SYNTH_VOICE* svoice);
unsigned short inpGetFilterParameter(struct SYNTH_VOICE* svoice);
void inpSetLPFDefaultRange(u32 lowFrq, u32 highFrq);
#endif
extern CTRL_DEST inpAuxA[8][4];
extern CTRL_DEST inpAuxB[8][4];
void inpSetMidiLastNote(u8 midi, u8 midiSet, u8 key);
u8 inpGetMidiLastNote(u8 midi, u8 midiSet);
u16 inpGetExCtrl(SYNTH_VOICE* svoice, u8 ctrl);
u16 inpGetMidiCtrl(u8 ctrl, u8 channel, u8 set);
void inpSetMidiLastNote(u8 midi, u8 midiSet, u8 key);
u16 inpGetModulation(SYNTH_VOICE* svoice);
void inpResetMidiCtrl(u8 ch, u8 set, u32 coldReset);
void inpResetChannelDefaults(u8 midi, u8 midiSet);
u16 inpGetPitchBend(SYNTH_VOICE* svoice);
u16 inpGetDoppler(SYNTH_VOICE* svoice);
u16 inpGetTremolo(SYNTH_VOICE* svoice);
u16 inpGetPanning(SYNTH_VOICE* svoice);
u16 inpGetSurPanning(SYNTH_VOICE* svoice);
u16 inpGetVolume(SYNTH_VOICE* svoice);
u16 inpGetReverb(SYNTH_VOICE* svoice);
u16 inpGetPreAuxA(SYNTH_VOICE* svoice);
u16 inpGetPostAuxA(SYNTH_VOICE* svoice);
u16 inpGetPreAuxB(SYNTH_VOICE* svoice);
u16 inpGetPostAuxB(SYNTH_VOICE* svoice);
u16 inpGetPedal(SYNTH_VOICE* svoice);
u16 inpGetAuxA(u8 studio, u8 index, u8 midi, u8 midiSet);
u16 inpGetAuxB(u8 studio, u8 index, u8 midi, u8 midiSet);
void inpInit(SYNTH_VOICE* svoice);
void inpFXCopyCtrl(u8 ctrl, SYNTH_VOICE* dvoice, SYNTH_VOICE* svoice);

s16 varGet(SYNTH_VOICE* svoice, u32 ctrl, u8 index);
#ifdef __cplusplus
}
#endif

#endif // _MUSYX_SEQ
