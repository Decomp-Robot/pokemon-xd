#ifndef WAZA_SEQUENCE_SYS_H
#define WAZA_SEQUENCE_SYS_H

struct _GSmodel;

extern "C" {
_GSmodel* Sequence_GetGSmodel(unsigned long sequence);
int Sequence_GetModelSize(unsigned long sequence);
}

#endif
