#ifndef GS_FILTER_H
#define GS_FILTER_H

#include <GSgfx.h>

struct GSfilter;
extern "C" {
GSfilter *GSfilterCreate(GScolor *color);
void GSfilterFree(GSfilter *filter);
void GSfilterSetColor(GSfilter *filter, GScolor *color);
}

#endif
