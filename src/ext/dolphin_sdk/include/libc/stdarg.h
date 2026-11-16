#ifndef _STDARG_H_
#define _STDARG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MWERKS__
typedef struct __va_list_struct {
  char gpr;
  char fpr;
  char reserved[2];
  char* input_arg_area;
  char* reg_save_area;
} __va_list[1];
typedef __va_list va_list;

#ifndef __MWERKS__
extern void __builtin_va_info(va_list*);
#endif

/* Argument classes reported by the compiler's _var_arg_typeof intrinsic for
 * the type named in va_arg.  The class selects which register bank is
 * consumed, whether the register slot is paired, and whether the saved slot
 * holds the argument itself or its address. */
typedef enum {
  ARG_CLASS_INDIRECT = 0,       /* slot holds the argument's address */
  ARG_CLASS_GENERAL = 1,        /* one general-purpose register slot */
  ARG_CLASS_DOUBLE_WORD = 2,    /* even-aligned pair of general registers */
  ARG_CLASS_FLOATING_POINT = 3  /* one floating-point register slot */
} ArgumentClass;

void* __va_arg(va_list v_list, ArgumentClass type);

#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_arg(ap, t) (*((t*)__va_arg(ap, (ArgumentClass)_var_arg_typeof(t))))
#define va_end(ap) (void)0

#else
typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, l) __builtin_va_arg(v, l)
#endif

#ifdef __cplusplus
}
#endif

#endif
