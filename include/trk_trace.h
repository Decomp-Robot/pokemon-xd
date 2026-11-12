#ifndef TRK_TRACE_H
#define TRK_TRACE_H

#define TRK_TRACE_LEVEL_DETAIL 1
#define TRK_TRACE_LEVEL_DEBUG  4
#define TRK_TRACE_LEVEL_ERROR  8

void MWTRACE(int level, const char* fmt, ...);

#endif
