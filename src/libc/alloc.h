#ifndef MSL_ALLOC_H
#define MSL_ALLOC_H

typedef struct AllocState AllocState;

extern const int fix_pool_sizes[6];
extern AllocState defaultAllocatorState;
extern unsigned char allocatorInitialized;
void free(void *ptr);
void __pool_free(AllocState *state, void *ptr);

#endif /* MSL_ALLOC_H */
