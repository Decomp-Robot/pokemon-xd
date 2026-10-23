/*
 * The library only builds one heap: the first time a free reaches a
 * program that never seeded one, the memory is carved out of the OS arena.
 */

/* Handle of the default heap, or -1 until the library creates one. */
extern int __OSCurrHeap;

/* ------------------------------------------------------------------ */
/* Interfaces the library expects from the OS and from its own helpers */
/* ------------------------------------------------------------------ */

extern void OSReport(const char *msg, ...);

/*
 * The library reports these two when it has to build a default heap on
 * demand, and remembers its handle so it only builds one.
 */
const char defaultHeapWarning[] = "GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n";
const char defaultHeapArenaWarning[] = "Metrowerks CW runtime library initializing default heap\n";

/* The arena the default heap is built from is aligned to thirty-two bytes. */
#define ALIGN32(x) ((void *)(((unsigned long)(x) + 0x1F) & ~31UL))
#define FLOOR32(x) ((void *)(((unsigned long)(x)) & ~31UL))

extern void *OSGetArenaLo(void);
extern void *OSGetArenaHi(void);
extern void OSSetArenaLo(void *addr);
extern void *OSInitAlloc(void *lo, void *hi, int max_heaps);
extern int OSCreateHeap(void *start, void *end);
extern int OSSetCurrentHeap(int heap);
extern void OSFreeToHeap(int heap, void *ptr);

static inline void InitDefaultHeap(void) {
    void *lo;
    void *hi;

    OSReport(defaultHeapWarning);
    OSReport(defaultHeapArenaWarning);
    lo = OSGetArenaLo();
    hi = OSGetArenaHi();
    lo = OSInitAlloc(lo, hi, 1);
    OSSetArenaLo(lo);
    lo = (void *)ALIGN32(lo);
    hi = (void *)FLOOR32(hi);
    OSSetCurrentHeap(OSCreateHeap(lo, hi));
    OSSetArenaLo(hi);
}

void __sys_free(void *ptr) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    OSFreeToHeap(__OSCurrHeap, ptr);
}
