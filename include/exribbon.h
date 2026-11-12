#ifndef EXRIBBON_H
#define EXRIBBON_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 exribbonGetNo(s32 ribbon);
void exribbonSetNo(s32 ribbon, u8 no);

#ifdef __cplusplus
}
#endif

#endif
