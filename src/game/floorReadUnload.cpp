// The release callbacks the reader registers with the resource system. The
// resource system calls one when it drops a resource it loaded, so that the
// system the resource was handed to can let go of it first.

#include <types.h>
#include <GScamera.h>
#include <GSlight.h>
#include <GSmsg.h>
#include <GSparticle.h>
#include <ModelSequence.h>
#include <flashsystem.h>
#include <floorManager.h>
#include <wazaSequence.h>

// The lens flare system keeps its flares under the handle they were loaded
// with.
extern "C" void GSlensFlareFree(u32 handle);

// Gives the waza sequence of a stage back to the sequence system, which
// releases the sequence it built around the data.
bool _unloadWZX(void *data, u32 group, u32 res)
{
    WazaSequence::Unload((u8 *)data);
    return true;
}

// Gives the Pokemon model sequence of a stage back to the sequence system.
bool _unloadPKX(void *data, u32 group, u32 res)
{
    if (data != 0) {
        ModelSequence::Unload((u8 *)data);
    }
    return true;
}

// Gives the flash movie a station plays back to the flash system.
bool _unloadGSW(void *data, u32 group, u32 res)
{
    FlashSystemRelease(res);
    return true;
}

// Gives the lens flare of a stage back to the lens flare system.
bool _unloadFlare(void *data, u32 group, u32 res)
{
    GSlensFlareFree((u32)data);
    return true;
}

// Gives the particle bank of a stage back to the particle system.
bool _unloadParticles(void *data, u32 group, u32 res)
{
    GSparticleFree((_GSparticleBank *)data);
    return true;
}

// Gives the camera of a stage back to the camera system.
bool _unloadCamera(void *data, u32 group, u32 res)
{
    GScameraFree((_GScamera *)data);
    return true;
}

// Gives the lights of a stage back to the light system.
bool _unloadLight(void *data, u32 group, u32 res)
{
    GSlightFree((GSlight *)data);
    return true;
}

// Gives the floor script of a stage back to the floor manager. A script that
// pushes the next floor hands its own script over at the push, so there is
// nothing left to take back here.
bool _unloadScript(void *data, u32 group, u32 res)
{
    if (floorIsPush()) {
        return false;
    }
    floorUnmappingScript(data);
    return true;
}

// Keep the current font available while another floor is pushed.
bool _unloadFont(void *data, u32 group, u32 res)
{
    if (floorIsPush()) {
        return false;
    }
    GSmsgFontClose(data);
    return true;
}

// Keep the current messages available while another floor is pushed.
bool _unloadMsg(void *data, u32 group, u32 res)
{
    if (floorIsPush()) {
        return false;
    }
    GSmsgClose(data);
    return true;
}
