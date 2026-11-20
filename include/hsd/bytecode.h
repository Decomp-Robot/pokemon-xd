#ifndef HSD_BYTECODE_H
#define HSD_BYTECODE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

f32 HSD_ByteCodeEval(u8 *bytecode, f32 *args, s32 nb_args);

#ifdef __cplusplus
}
#endif

#endif
