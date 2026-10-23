#include <dolphin.h>
#include <dolphin/ax.h>

#include "__ax.h"

static AXPROFILE* __AXProfile;
static u32 __AXMaxProfiles;
static u32 __AXCurrentProfile;
static u32 __AXProfileInitialized;

// UNUSED: the game never calls this; dead-stripped at link.
AXPROFILE* __AXGetCurrentProfile(void) {
    AXPROFILE* profile;

    if (__AXProfileInitialized != 0) {
        profile = &__AXProfile[__AXCurrentProfile];
        __AXCurrentProfile += 1;
        __AXCurrentProfile %= __AXMaxProfiles;
        return profile;
    }

    return 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
void AXInitProfile(AXPROFILE* profile, u32 maxProfiles) {
    ASSERTLINE(60, profile);
    ASSERTLINE(61, maxProfiles);

    __AXProfile = profile;
    __AXMaxProfiles = maxProfiles;
    __AXCurrentProfile = 0;
    __AXProfileInitialized = 1;
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 AXGetProfile(void) {
    BOOL old;
    u32 n;

    old = OSDisableInterrupts();
    n = __AXCurrentProfile;
    if (n != 0) {
        n -= 1;
    }

    __AXCurrentProfile = 0;
    OSRestoreInterrupts(old);
    return n;
}
