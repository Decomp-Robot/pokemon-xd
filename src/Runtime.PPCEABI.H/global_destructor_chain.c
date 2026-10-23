/* Runtime.PPCEABI.H/global_destructor_chain.c -- Metrowerks PowerPC EABI
 * runtime support for C++ static-object destruction.
 *
 * __global_destructor_chain is the head of the chain of registered static
 * destructors (sbss 0x804EA6C8). Each node holds the next link, the
 * destructor to call, and its object (see the 0x0/0x4/0x8 loads below).
 * __destroy_global_chain pops and runs the chain at exit (called from
 * exit); the head is advanced before each call so a destructor that
 * registers further entries cannot corrupt the walk.
 *
 * This binary exposes no __register_global_object, so this module hosts
 * only the chain head and the destroy walk; registration, if the original
 * library provided it here, left no trace in this link. */

typedef struct DestructorChain {
	struct DestructorChain *next;
	void (*destructor)(void *, int);
	void *object;
} DestructorChain;

DestructorChain *__global_destructor_chain;

void __destroy_global_chain(void)
{
	DestructorChain *chain;

	while ((chain = __global_destructor_chain) != 0) {
		__global_destructor_chain = chain->next;
		(*chain->destructor)(chain->object, -1);
	}
}
