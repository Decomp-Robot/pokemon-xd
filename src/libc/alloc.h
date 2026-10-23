#ifndef MSL_ALLOC_H
#define MSL_ALLOC_H

typedef struct AllocState AllocState;
void free(void *ptr);
void __pool_free(AllocState *state, void *ptr);

#endif /* MSL_ALLOC_H */
