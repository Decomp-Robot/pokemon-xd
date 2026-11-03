#ifndef FIGHT_KIND_H
#define FIGHT_KIND_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Reports whether the current fight belongs to the given fight kind.
u8 fightKind_CheckFightKind(u8 kind);

#ifdef __cplusplus
}
#endif

#endif
