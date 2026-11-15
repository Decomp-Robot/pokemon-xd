#ifndef UNDERTAKER_H
#define UNDERTAKER_H

#ifdef __cplusplus
#include <heroMove.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
int underTakerGetMiraboPlace(void);
void underTakerMiraboDisAppear(void);
#ifdef __cplusplus
void underTakerCallBackFunc(long context, FootStepCounter counter, long steps);
}
#endif

#endif
