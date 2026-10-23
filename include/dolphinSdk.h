#ifndef DOLPHIN_SDK_H
#define DOLPHIN_SDK_H

extern "C" {
#include <libc/stdio.h>
#include <libc/stdarg.h>
#include <libc/string.h>
#include <libc/ctype.h>
#include <cmath.h>
}

// Keep the SDK's long-based integers separate from game integer aliases.
namespace Dolphin {
extern "C" {
#include <dolphin/types.h>
}
}

#endif
