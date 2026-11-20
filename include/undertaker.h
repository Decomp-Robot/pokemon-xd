#ifndef UNDERTAKER_H
#define UNDERTAKER_H

#include <types.h>

#ifdef __cplusplus
#include <heroMove.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
int underTakerGetMiraboPlace(void);
void underTakerMiraboDisAppear(void);
void underTakerMenuResore(void);
u32 undertakerGetPlace(u32 *places);
void undertakerSetPlace(u32 place);
long undertakerGetStartFlagID(void);
#ifdef __cplusplus
void underTakerCallBackFunc(long context, FootStepCounter counter, long steps);
}
#endif

#endif
