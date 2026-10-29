#include <app/recovery/recoveryEvent.h>
#include <GSsnd.h>
#include <sound.h>
#include <types.h>

// Pauses the BGM the recovery event interrupts and returns the BGM id that
// recoveryEventPC resumes when the event is over.
s32 _recoveryPauseBGM(unsigned long time)
{
    return soundPauseBGMEX((s32)time, 0);
}

// Plays the sound effect of the recovery event.
void _recoveryEventSetSE(unsigned long soundID)
{
    GSsndPlay((long)soundID, 0, 0);
}

// Opens the music stream the recovery event plays in its scene.
void _recoveryEventSetME(unsigned long soundID, unsigned long time, unsigned char volume)
{
    soundOpenPlayME((s32)soundID, (s32)time, volume);
}
