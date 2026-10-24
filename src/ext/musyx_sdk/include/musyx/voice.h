#ifndef _MUSYX_VOICE_H_
#define _MUSYX_VOICE_H_

#include "musyx/synth.h"
#include "musyx/version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PBMIX {
  u16 vL;
  u16 vDeltaL;
  u16 vR;
  u16 vDeltaR;
  u16 vAuxAL;
  u16 vDeltaAuxAL;
  u16 vAuxAR;
  u16 vDeltaAuxAR;
  u16 vAuxBL;
  u16 vDeltaAuxBL;
  u16 vAuxBR;
  u16 vDeltaAuxBR;
  u16 vAuxBS;
  u16 vDeltaAuxBS;
  u16 vS;
  u16 vDeltaS;
  u16 vAuxAS;
  u16 vDeltaAuxAS;
} _PBMIX;

typedef struct _PBITD {
  u16 flag;
  u16 bufferHi;
  u16 bufferLo;
  u16 shiftL;
  u16 shiftR;
  u16 targetShiftL;
  u16 targetShiftR;
} _PBITD;

typedef struct _PBUPDATE {
  u16 updNum[5];
  u16 dataHi;
  u16 dataLo;
} _PBUPDATE;

typedef struct _PBDPOP {
  u16 aL;
  u16 aAuxAL;
  u16 aAuxBL;
  u16 aR;
  u16 aAuxAR;
  u16 aAuxBR;
  u16 aS;
  u16 aAuxAS;
  u16 aAuxBS;
} _PBDPOP;

typedef struct _PBVE {
  u16 currentVolume;
  u16 currentDelta;
} _PBVE;

typedef struct _PBFIR {
  u16 numCoefs;
  u16 coefsHi;
  u16 coefsLo;
} _PBFIR;

typedef struct _PBADDR {
  u16 loopFlag;
  u16 format;
  u16 loopAddressHi;
  u16 loopAddressLo;
  u16 endAddressHi;
  u16 endAddressLo;
  u16 currentAddressHi;
  u16 currentAddressLo;
} _PBADDR;

typedef struct _PBADPCM {
  u16 a[8][2];
  u16 gain;
  u16 pred_scale;
  u16 yn1;
  u16 yn2;
} _PBADPCM;

typedef struct _PBSRC {
  u16 ratioHi;
  u16 ratioLo;
  u16 currentAddressFrac;
  u16 last_samples[4];
} _PBSRC;

typedef struct _PBADPCMLOOP {
  u16 loop_pred_scale;
  u16 loop_yn1;
  u16 loop_yn2;
} _PBADPCMLOOP;

#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
typedef struct _PBLPF {
  unsigned short flag;
  unsigned short yn1;
  unsigned short a0;
  unsigned short b0;
} _PBLPF;
#endif

typedef struct _PB {
  u16 nextHi;
  u16 nextLo;
  u16 currHi;
  u16 currLo;
  u16 srcSelect;
  u16 coefSelect;
  u16 mixerCtrl;
  u16 state;
  u16 loopType;
  _PBMIX mix;
  _PBITD itd;
  _PBUPDATE update;
  _PBDPOP dpop;
  _PBVE ve;
  _PBFIR fir;
  _PBADDR addr;
  _PBADPCM adpcm;
  _PBSRC src;
  _PBADPCMLOOP adpcmLoop;
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
  _PBLPF lpf;
#endif
  u16 streamLoopCnt;
} _PB;

typedef struct VSampleInfo {
  void* loopBufferAddr;
  u32 loopBufferLength;
  u8 inLoopBuffer;
} VSampleInfo;

typedef struct VS_BUFFER {
  u8 state;
  u8 hwId;
  u8 smpType;
  u8 voice;
  u32 last;
  u32 finalGoodSamples;
  u32 finalLast;
  SND_VIRTUALSAMPLE_INFO info;
} VS_BUFFER;

typedef struct _VS {
  u8 numBuffers;
  u32 bufferLength;
  VS_BUFFER streamBuffer[64];
  u8 voices[64];
  u16 nextInstID;
  u32 (*callback)(u8,
                  const SND_VIRTUALSAMPLE_INFO*);
} VS;

extern VS vs;

typedef struct _SPB {
  u16 dpopLHi;
  u16 dpopLLo;
  u16 dpopLDelta;
  u16 dpopRHi;
  u16 dpopRLo;
  u16 dpopRDelta;
  u16 dpopSHi;
  u16 dpopSLo;
  u16 dpopSDelta;
  u16 dpopALHi;
  u16 dpopALLo;
  u16 dpopALDelta;
  u16 dpopARHi;
  u16 dpopARLo;
  u16 dpopARDelta;
  u16 dpopASHi;
  u16 dpopASLo;
  u16 dpopASDelta;
  u16 dpopBLHi;
  u16 dpopBLLo;
  u16 dpopBLDelta;
  u16 dpopBRHi;
  u16 dpopBRLo;
  u16 dpopBRDelta;
  u16 dpopBSHi;
  u16 dpopBSLo;
  u16 dpopBSDelta;
} _SPB;

extern u16 voicePrioSortRootListRoot;
extern u8 voiceMusicRunning;
extern u8 voiceFxRunning;
extern u8 voiceListInsert;
extern u8 voiceListRoot;

void vsInit(); /* extern */
u32 vsSampleStartNotify(
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 2)
    u32 voiceID
#else
    u8 voice
#endif
);
void vsSampleEndNotify(u32 pubID);
void vsSampleUpdates();

void voiceSetPriority(SYNTH_VOICE* svoice, u8 prio);
u32 voiceIsLastStarted(SYNTH_VOICE* svoice);
void voiceSetLastStarted(SYNTH_VOICE* svoice);
void voiceResetLastStarted(SYNTH_VOICE* svoice);
void voiceInitLastStarted();
s32 voiceKillSound(u32 voiceid);
void voiceKill(u32 vi);
void voiceSetPriority(SYNTH_VOICE* svoice, u8 prio);
u32 voiceIsLastStarted(SYNTH_VOICE* svoice);
void voiceSetLastStarted(SYNTH_VOICE* svoice);
void voiceResetLastStarted(SYNTH_VOICE* svoice);
void voiceInitLastStarted();
s32 voiceKillSound(u32 voiceid);
void voiceKill(u32 vi);
u32 voiceBlock(u8 prio);
u32 voiceAllocate(u8 priority, u8 maxVoices,
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 1)
                  u32 allocId,
#else
                  u16 allocId,
#endif
                  u8 fxFlag);
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 2)
int voiceAllocatePeek(u8 priority, u8 maxVoices, u32 allocId, u8 fxFlag, u32* currentAllocId);
#endif

void voiceFree(SYNTH_VOICE* svoice);
void voiceUnblock(u32 voice);
void voiceRemovePriority(SYNTH_VOICE* svoice);

void vidInit();
u32 vidGetInternalId(SND_VOICEID id);
#if MUSY_VERSION >= MUSY_VERSION_CHECK(2, 0, 2)
u32 vidGetPublicId(u32 voiceID);
#endif
void vidRemoveVoiceReferences(SYNTH_VOICE* svoice);
u32 vidMakeNew(SYNTH_VOICE* svoice, u32 isMaster);
u32 vidMakeRoot(SYNTH_VOICE* svoice);

#ifdef __cplusplus
}
#endif
#endif
