#ifndef GSSND_PLAYBACK_H
#define GSSND_PLAYBACK_H

#ifdef __cplusplus
extern "C" {
#endif

void GSsndSetMasterVolume(unsigned char volume, unsigned short time, unsigned char music, unsigned char effects);
unsigned long GSsndPlayNow(long soundID);
void GSsndStopNow(unsigned long soundID);
unsigned long GSsndPlay(long soundID, long time, long mode);
unsigned long GSsndPlayVolume(long soundID, long time, unsigned char volume, long mode);
unsigned long GSsndStop(long soundID, long time, long mode);
unsigned long GSsndClose(long soundID);
unsigned long GSsndContinue(unsigned long soundID, long time, long mode);
unsigned long GSsndPause(unsigned long soundID, long time, long mode);
void GSsndPopVolumeALL(long time, unsigned long music, unsigned long effects);
void GSsndPushVolumeALL(long volume, long time, unsigned long music, unsigned long effects);
unsigned long GSsndIsPlayingStream(long soundID);
long GSsndGetStatus(long soundID);

#ifdef __cplusplus
}
#endif

#endif
