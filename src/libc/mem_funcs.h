#ifndef __MEM_FUNCS_H__
#define __MEM_FUNCS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Internal large-copy paths: length is a byte count of at least 32.
 * memmove selects an overlap-safe traversal direction. Aligned variants
 * require matching word alignment; unaligned variants require differing
 * source and destination word alignment. */
void __copy_longs_rev_unaligned(void *destination, const void *source, unsigned long length);
void __copy_longs_unaligned(void *destination, const void *source, unsigned long length);
void __copy_longs_rev_aligned(void *destination, const void *source, unsigned long length);
void __copy_longs_aligned(void *destination, const void *source, unsigned long length);

#ifdef __cplusplus
}
#endif

#endif
