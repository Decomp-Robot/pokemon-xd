#ifndef BOOL_H
#define BOOL_H

#if defined(__MWERKS__)
#if !__option(bool)
typedef unsigned char bool;
#endif
#elif !defined(__cplusplus)
typedef unsigned char bool;
#endif

#endif
