#ifndef GSAPI_GSVTR_GSVTR_H
#define GSAPI_GSVTR_GSVTR_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void GSvtrInit(void);
void GSvtrEnable(void);
void GSvtrDisable(void);
s32 GSvtrRegisterGSgapp(s32 hdl);
void GSvtrLoadTexture(void);
void GSvtrPlay(void);
void GSvtrPause(void);
void GSvtrSlowPlay(void);
void GSvtrSingleStep(void);
void GSvtrSetInfoDisp(u8 disp, u8 frame);
u8 GSvtrGetInfoDisp(void);
s32 GSvtrGetState(void);
u8 GSvtrIsActive(void);

#ifdef __cplusplus
}
#endif

#endif
