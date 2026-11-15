#ifndef APP_SCRIPT_CALL_H
#define APP_SCRIPT_CALL_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
bool appScriptCommonCall(int scriptID);
#else
unsigned char appScriptCommonCall(int scriptID);
#endif
#ifdef __cplusplus
}
#endif

#endif
