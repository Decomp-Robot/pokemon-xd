#ifndef GS_LOG_H
#define GS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

void GSlogWrite(const char* format, ...);
void GSlogWritef(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif
