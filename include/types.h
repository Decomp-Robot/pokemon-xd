#ifndef TYPES_H
#define TYPES_H

// Shorthand scalar types of the game. An SDK header already declares the same
// set (<dolphin/types.h>), so defer to it when one was included first.
#ifndef _DOLPHIN_TYPES_H_

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef unsigned long long u64;
typedef long long s64;
typedef float f32;
typedef double f64;

#endif

#endif
