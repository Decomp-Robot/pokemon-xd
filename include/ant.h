#ifndef ANT_H
#define ANT_H

struct GStexture;

GStexture *antGetGStexture(void *resource);
void antReadPostFunc(void *resource, unsigned long size);

#endif
