#ifndef APP_RECOVERY_EVENT_H
#define APP_RECOVERY_EVENT_H

void _recoveryEventSetSE(unsigned long soundID);

#ifdef __cplusplus
extern "C" {
#endif
void recoveryEventPC(unsigned long personGroup, unsigned long personID,
                     unsigned long machineID, long* ballParts, long* motions);
#ifdef __cplusplus
}
#endif

#endif
