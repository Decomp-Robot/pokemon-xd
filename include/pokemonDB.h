#ifndef POKEMONDB_H
#define POKEMONDB_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u32 pokemonDB_GetName(u16);
bool pokemonDB_GetTeachWaza(u16, u16);
#ifdef __cplusplus
}
#endif

#endif
