#ifndef ADSR_H
#define ADSR_H

#include "musyx/musyx.h"
typedef struct ADSR_INFO {
  union ai_data {
    struct {
      s32 atime;
      s32 dtime;
      u16 slevel;
      u16 rtime;
      s32 ascale;
      s32 dscale;
    } dls;
    struct {
      u16 atime;
      u16 dtime;
      u16 slevel;
      u16 rtime;
    } linear;
  } data;
} ADSR_INFO;

typedef struct ADSR_VARS {
  u8 mode;
  u8 state;
  u32 cnt;
  s32 currentVolume;
  s32 currentIndex;
  s32 currentDelta;

  union data {
    struct _dls {
      u32 aTime;
      u32 dTime;
      u16 sLevel;
      u32 rTime;
      u16 cutOff;
      u8 aMode;
    } dls;

    struct _linear {
      u32 aTime;
      u32 dTime;
      u16 sLevel;
      u32 rTime;
    } linear;
  } data;
} ADSR_VARS;

#ifdef __cplusplus
extern "C" {
#endif
u32 adsrStartRelease(ADSR_VARS* adsr, u32 rtime);
u32 adsrHandle(ADSR_VARS* adsr, u16* adsr_start, u16* adsr_delta);
u32 adsrHandleLowPrecision(ADSR_VARS* adsr, u16* adsr_start, u16* adsr_delta);
bool adsrRelease(ADSR_VARS* adsr);
u32 adsrConvertTimeCents(s32 tc);
#ifdef __cplusplus
}
#endif

#endif // ADSR_H
