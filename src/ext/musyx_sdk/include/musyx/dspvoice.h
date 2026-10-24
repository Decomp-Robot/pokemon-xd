#ifndef DSPVOICE_H
#define DSPVOICE_H

#include "musyx/musyx.h"

#include "musyx/synthdata.h"
#include "musyx/voice.h"

typedef struct FILTERInfo {
  u8 on;
  u16 coefA;
  u16 coefB;
} FILTERInfo;

typedef struct DSPvoice {
  _PB* pb;
  void* patchData;
  void* itdBuffer;
  struct DSPvoice* next;
  struct DSPvoice* prev;
  struct DSPvoice* nextAlien;
  u32 mesgCallBackUserValue;
  u32 prio;
  u32 currentAddr;
  u32 changed[5];
  u32 pitch[5];
  u16 volL;
  u16 volR;
  u16 volS;
  u16 volLa;
  u16 volRa;
  u16 volSa;
  u16 volLb;
  u16 volRb;
  u16 volSb;
  u16 lastVolL;
  u16 lastVolR;
  u16 lastVolS;
  u16 lastVolLa;
  u16 lastVolRa;
  u16 lastVolSa;
  u16 lastVolLb;
  u16 lastVolRb;
  u16 lastVolSb;
  u16 smp_id;
  SAMPLE_INFO smp_info;
  VSampleInfo vSampleInfo;
  u8 streamLoopPS;
  ADSR_VARS adsr;
  u16 srcTypeSelect;
  u16 srcCoefSelect;
  u16 itdShiftL;
  u16 itdShiftR;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  FILTERInfo filter;
#endif
  u8 singleOffset;
  struct {
    u32 posHi;
    u32 posLo;
    u32 pitch;
  } playInfo;

  struct {
    u8 pitch;
    u8 vol;
    u8 volA;
    u8 volB;
  } lastUpdate;

  u32 virtualSampleID;
  u8 state;
  u8 postBreak;
  u8 startupBreak;
  u8 studio;
  u32 flags;
} DSPvoice;

typedef struct DSPhostDPop {
  s32 l;
  s32 r;
  s32 s;
  s32 lA;
  s32 rA;
  s32 sA;
  s32 lB;
  s32 rB;
  s32 sB;
} DSPhostDPop;

typedef struct DSPinput {
  u8 studio;
  u16 vol;
  u16 volA;
  u16 volB;
  SND_STUDIO_INPUT* desc;
} DSPinput;

typedef struct DSPstudioinfo {
  _SPB* spb;
  DSPhostDPop hostDPopSum;
  s32* main[2];
  s32* auxA[3];
  s32* auxB[3];
  DSPvoice* voiceRoot;
  DSPvoice* alienVoiceRoot;
  u8 state;
  u8 isMaster;
  u8 numInputs;
  SND_STUDIO_TYPE type;
  DSPinput in[7];
  SND_AUX_CALLBACK auxAHandler;
  SND_AUX_CALLBACK auxBHandler;
  void* auxAUser;
  void* auxBUser;
} DSPstudioinfo;

extern DSPstudioinfo dspStudio[8];
extern DSPvoice* dspVoice;
extern bool dspCompressorOn;
#endif
