#include <types.h>

#include <camera.h>
#include <GScamera.h>
#include <GSvec.h>
#include <sound.h>

/*
 * The camera of the evolution demo: the demo stores the camera the player left
 * behind before it takes the camera over for the evolution scene, and hands it
 * back to the game once the scene is over.
 */

// The camera the demo took over from the game.
typedef struct CAM_STORE {
    /* 0x00 */ GSvec position;  // eye position of the camera
    /* 0x0C */ GSvec rotation;  // rotation of the camera
    /* 0x18 */ GSvec targetPos; // point the camera looks at
    /* 0x24 */ GSvec targetOfs; // offset of the target from the camera
    /* 0x30 */ f32 fov;          // field of view of the camera
} CAM_STORE;

// Type the camera is put back to when the demo hands it back.
#define EVOLUTION_CAMERA_TYPE 2

// Hand the camera back to the game, which runs the scene the player came from.
void restoreCamera(CAM_STORE *store) {
    cameraSetType(EVOLUTION_CAMERA_TYPE);
    cameraSetPosition(&store->position);
    cameraSetRotation(&store->rotation);
    cameraSetTargetPos(&store->targetPos);
    cameraSetTargetOfs(&store->targetOfs);
    cameraSetFov(store->fov);
}

// Take the camera of the game over for the demo.
void storeCamera(CAM_STORE *store) {
    GSvec position;
    GSvec rotation;
    GSvec targetPos;
    GSvec targetOfs;
    f32 fov;
    f32 aspect;
    f32 near;
    f32 far;

    cameraGetPosition(&position);
    cameraGetRotation(&rotation);
    cameraGetTargetPos(&targetPos);
    cameraGetTargetOfs(&targetOfs);
    GScameraGetPerspective(GScameraGetActiveCamera(), &fov, &aspect, &near, &far);

    store->position.x = position.x;
    store->position.y = position.y;
    store->position.z = position.z;
    store->rotation.x = rotation.x;
    store->rotation.y = rotation.y;
    store->rotation.z = rotation.z;
    store->targetPos.x = targetPos.x;
    store->targetPos.y = targetPos.y;
    store->targetPos.z = targetPos.z;
    store->targetOfs.x = targetOfs.x;
    store->targetOfs.y = targetOfs.y;
    store->targetOfs.z = targetOfs.z;
    store->fov = fov;
}

class ModelSequence;

enum STEP {
    STEP_EVOLUTION = 0,
    STEP_EVOLUTION_COMPLETE = 1,
    STEP_CANCEL_EARLY = 2,
    STEP_CANCEL_MIDDLE = 3,
    STEP_CANCEL_LATE = 4
};

struct WORK {
    ModelSequence *sequenceBefore;
    ModelSequence *sequenceAfter;
    STEP step;
    u32 bgmID;
    u32 envID;
    u32 bgmVolume;
    u32 envVolume;
    GSsndVolumeStack bgmStack;
    GSsndVolumeStack envStack;
};

enum {
    EVOLUTION_SOUND_FADE_TIME = 50
};

void restoreSound(WORK *work) {
    if (work->bgmID != 0) {
        soundOpenPlay(work->bgmID, EVOLUTION_SOUND_FADE_TIME, work->bgmVolume);
        GSsndSetPushVolumeID(work->bgmID, &work->bgmStack);
    }
    if (work->envID != 0) {
        soundOpenPlayEnv(work->envID, EVOLUTION_SOUND_FADE_TIME, work->envVolume);
        GSsndSetPushVolumeID(work->envID, &work->envStack);
    }
}

void stopSound(WORK *work) {
    s32 bgmID;
    s32 envID;
    u32 bgmVolume;
    u32 envVolume;

    bgmID = soundGetBGMID();
    if (bgmID != 0) {
        GSsndGetPushVolumeID(bgmID, &work->bgmStack);
        bgmVolume = soundGetVolume(bgmID);
        soundOpenPlay(SOUND_SILENT_BGM, EVOLUTION_SOUND_FADE_TIME, SOUND_VOLUME_MAX);
    } else {
        bgmVolume = 0;
    }

    envID = soundGetEnvID();
    if (envID != 0) {
        GSsndGetPushVolumeID(envID, &work->envStack);
        envVolume = soundGetVolume(envID);
        soundStopEnv(EVOLUTION_SOUND_FADE_TIME);
    } else {
        envVolume = 0;
    }

    work->bgmID = bgmID;
    work->envID = envID;
    work->bgmVolume = bgmVolume;
    work->envVolume = envVolume;
}
