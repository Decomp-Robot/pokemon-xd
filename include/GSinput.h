#ifndef GS_INPUT_H
#define GS_INPUT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u32 GSinputButtonsChanged(u32 port);
u32 GSinputButtonsPressed(u32 port);
#ifdef __cplusplus
}
#endif

#endif
