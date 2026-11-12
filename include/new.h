#ifndef GS_NEW_H
#define GS_NEW_H

void *operator new(unsigned long size);
inline void *operator new(unsigned long, void *memory) { return memory; }
void operator delete(void *memory);

#endif
