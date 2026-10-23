#include <dolphin/ai.h>
#include <dolphin/dtk.h>
#include <dolphin/dvd.h>
#include <dolphin/os.h>

static DTKTrack* __DTKCurrentTrack;
static DTKTrack* __DTKPlayListHead;
static DTKTrack* __DTKPlayListTail;
static volatile u32 __DTKState;
static volatile u32 __DTKTempState;
static volatile u32 __DTKRepeatMode;
static volatile u32 __DTKPosition;
static volatile u32 __DTKInterruptFrequency;
static volatile u8 __DTKVolumeL;
static volatile u8 __DTKVolumeR;
static volatile u32 __DTKShutdownFlag;
static volatile u32 __DTKTrackEnded;
static DTKFlushCallback __DTKFlushCallback;
static int __busy_for_ais_address;

static DVDCommandBlock __block_for_run_callback;
static DVDCommandBlock __block_for_prep_callback;
static DVDCommandBlock __block_for_stream_status;
static DVDCommandBlock __block_for_ais_isr;
static DVDCommandBlock __block_for_flushtracks;
static DVDCommandBlock __block_for_repeatmode;
static DVDCommandBlock __block_for_set_state;
static DVDCommandBlock __block_for_next_track;
static DVDCommandBlock __block_for_prev_track;

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKStartAi(void) {
    AISetStreamVolLeft(__DTKVolumeL);
    AISetStreamVolRight(__DTKVolumeR);
    AIResetStreamSampleCount();
    AISetStreamTrigger(__DTKInterruptFrequency);
    AISetStreamPlayState(1);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKStopAi(void) {
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AISetStreamPlayState(0);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCheckUserCallback(DTKTrack* track, u32 event) {
    ASSERTLINE(84, track);
    if (track && track->callback && (track->eventMask & event)) {
        track->callback(track->eventMask & event);
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKForward(void) {
    BOOL old = OSDisableInterrupts();
    if (__DTKCurrentTrack && __DTKCurrentTrack->next) {
        __DTKCurrentTrack = __DTKCurrentTrack->next;
    }
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKBackward(void) {
    BOOL old = OSDisableInterrupts();
    if (__DTKCurrentTrack && __DTKCurrentTrack->prev) {
        __DTKCurrentTrack = __DTKCurrentTrack->prev;
    }
    OSRestoreInterrupts(old);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCallbackForStreamStatus(s32 result, DVDCommandBlock* block) {
    if ((result & 0xFF) == 0) {
        __DTKTrackEnded = 1;
        __DTKPosition = 0;
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCallbackForRun(s32 result, DVDFileInfo* fileInfo) {
    __DTKStartAi();
    DVDStopStreamAtEndAsync(&__block_for_run_callback, 0);
    __DTKState = DTK_STATE_RUN;
    __DTKCheckUserCallback(__DTKCurrentTrack, 1);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCallbackForPreparePaused(s32 result, DVDFileInfo* fileInfo) {
    __DTKStopAi();
    DVDStopStreamAtEndAsync(&__block_for_prep_callback, 0);
    __DTKState = DTK_STATE_PAUSE;
    __DTKCheckUserCallback(__DTKCurrentTrack, 32);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKPrepareCurrentTrack(void) {
    DVDPrepareStreamAsync(&__DTKCurrentTrack->dvdFileInfo, 0, 0, __DTKCallbackForRun);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKPrepareCurrentTrackPaused(void) {
    DVDPrepareStreamAsync(&__DTKCurrentTrack->dvdFileInfo, 0, 0, __DTKCallbackForPreparePaused);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCallbackForPlaylist(s32 result, DVDCommandBlock* block) {
    __DTKPosition = result;
    __busy_for_ais_address = 0;

    if (__DTKTrackEnded) {
        __DTKTrackEnded = 0;
        __DTKCheckUserCallback(__DTKCurrentTrack, 16);
        __DTKState = DTK_STATE_BUSY;

        switch (__DTKRepeatMode) {
        case DTK_MODE_NOREPEAT:
            if (__DTKCurrentTrack) {
                if (__DTKCurrentTrack->next) {
                    __DTKCurrentTrack = __DTKCurrentTrack->next;
                    __DTKStopAi();
                    __DTKPrepareCurrentTrack();
                } else {
                    __DTKCurrentTrack = __DTKPlayListHead;
                    __DTKStopAi();
                    __DTKState = DTK_STATE_STOP;
                }
            }
            break;
        case DTK_MODE_ALLREPEAT:
            if (__DTKCurrentTrack) {
                if (__DTKCurrentTrack->next) {
                    __DTKCurrentTrack = __DTKCurrentTrack->next;
                    __DTKStopAi();
                    __DTKPrepareCurrentTrack();
                } else {
                    __DTKCurrentTrack = __DTKPlayListHead;
                    __DTKStopAi();
                    __DTKPrepareCurrentTrack();
                }
            }
            break;
        case DTK_MODE_REPEAT1:
            if (__DTKCurrentTrack) {
                __DTKStopAi();
                __DTKPrepareCurrentTrack();
            }
            break;
        }
    } else {
        DVDGetStreamErrorStatusAsync(&__block_for_stream_status, __DTKCallbackForStreamStatus);
    }
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCallbackForFlush(s32 result, DVDCommandBlock* block) {
    DTKTrack* track;

    AISetStreamPlayState(0);
    track = __DTKPlayListHead;
    while (track) {
        DVDClose(&track->dvdFileInfo);
        track = track->next;
    }

    __DTKPlayListHead = NULL;
    __DTKPlayListTail = NULL;
    __DTKCurrentTrack = NULL;
    __DTKState = DTK_STATE_STOP;

    if (__DTKFlushCallback) {
        __DTKFlushCallback();
        __DTKFlushCallback = NULL;
    }

    __DTKState = DTK_STATE_STOP;
    __DTKShutdownFlag = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
static void __DTKCallbackForStop(s32 result, DVDCommandBlock* block) {
    __DTKCheckUserCallback(__DTKCurrentTrack, 2);
    __DTKState = DTK_STATE_STOP;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void DTKShutdown(void) {
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    AIRegisterStreamCallback(NULL);
    AIResetStreamSampleCount();
    AISetStreamPlayState(0);

    __DTKShutdownFlag = 1;
    DTKFlushTracks(NULL);
    __DTKState = DTK_STATE_STOP;

    while (__DTKShutdownFlag) {}
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
int DTKFlushTracks(DTKFlushCallback callback) {
    u32 temp;

    if (__DTKState == DTK_STATE_BUSY) {
        return 0;
    }

    temp = __DTKState;
    __DTKState = DTK_STATE_BUSY;
    __DTKFlushCallback = callback;
    if (temp == DTK_STATE_RUN) {
        DVDCancelStreamAsync(&__block_for_flushtracks, __DTKCallbackForFlush);
    } else {
        __DTKCallbackForFlush(0, 0);
    }
    return 1;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
int DTKSetState(u32 state) {
    if (__DTKState == state) {
        return 1;
    }

    if (__DTKState == DTK_STATE_BUSY) {
        return 0;
    }

    switch (state) {
    case DTK_STATE_STOP:
        if (__DTKCurrentTrack) {
            __DTKState = DTK_STATE_BUSY;
            AISetStreamVolLeft(0);
            AISetStreamVolRight(0);
            AISetStreamPlayState(0);
            DVDCancelStreamAsync(&__block_for_set_state, __DTKCallbackForStop);
        }
        break;
    case DTK_STATE_RUN:
        if (__DTKState == DTK_STATE_PAUSE) {
            __DTKStartAi();
            __DTKState = DTK_STATE_RUN;
            if (__DTKCurrentTrack) {
                __DTKCheckUserCallback(__DTKCurrentTrack, 1);
            }
        } else if (__DTKCurrentTrack) {
            __DTKState = DTK_STATE_BUSY;
            __DTKPrepareCurrentTrack();
        } else {
            __DTKState = DTK_STATE_RUN;
        }
        __DTKTrackEnded = 0;
        break;
    case DTK_STATE_PREPARE:
        if (__DTKState == DTK_STATE_STOP) {
            if (__DTKCurrentTrack) {
                __DTKState = DTK_STATE_BUSY;
                __DTKPrepareCurrentTrackPaused();
            }
            __DTKTrackEnded = 0;
        }
        break;
    case DTK_STATE_PAUSE:
        AISetStreamPlayState(0);
        if (__DTKState == DTK_STATE_RUN) {
            __DTKState = DTK_STATE_PAUSE;
        }
        __DTKCheckUserCallback(__DTKCurrentTrack, 4);
        break;
    }

    return 1;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
DTKTrack* DTKGetCurrentTrack(void) {
    return __DTKCurrentTrack;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
