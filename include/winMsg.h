#ifndef WIN_MSG_H
#define WIN_MSG_H
#ifdef __cplusplus
extern "C" {
void winMsgOpen(signed char mode, unsigned int messageID, bool wait, bool instantText);
void winMsgOpenWithSE(signed char mode, unsigned int messageID, bool wait, bool instantText, unsigned char soundID);
void winMsgClose(bool wait);
void winMsgCloseFight(bool wait);
void winMsgCloseField(bool wait);
#else
void winMsgOpen(signed char mode, unsigned int messageID, unsigned char wait, unsigned char instantText);
void winMsgOpenWithSE(signed char mode, unsigned int messageID, unsigned char wait, unsigned char instantText, unsigned char soundID);
void winMsgClose(unsigned char wait);
void winMsgCloseFight(unsigned char wait);
void winMsgCloseField(unsigned char wait);
#endif
#ifdef __cplusplus
}
#endif
#endif
