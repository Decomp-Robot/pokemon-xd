#include <floorFieldCameraEditor.h>
#include <floorFieldCamera.h>
#include <floorDataBios.h>
#include <floorManager.h>
#include <relglobal.h>
#include <GSAPI/GSgapp/GSgapp.h>
#include <GSmem.h>
#include <GSinput.h>
#include <GScamera.h>
#include <app/camera/camera.h>
#include <dbgMenu.h>
#include <menu.h>
#include <libc/memory.h>
#include <libc/math.h>
#include <dolphin/pad.h>
#include <dolphin/gx/GXEnum.h>
#include <app/camera/CameraType.h>

struct FieldCameraEditorData {
    FloorFieldCameraRecord *selectedRecord;
    FloorFieldCameraList *cameraList;
    u32 recordCount;
    FloorFieldCameraList *listAllocation;
    FloorFieldCameraRecord *recordAllocation;
    u32 floorID;
    FloorFieldCameraList **originalList;
    u32 app;
    u8 testing;
    u8 mode;
};

FieldCameraEditorData gFieldCameraEditorData;

static const u32 EDITOR_APP_MAIN = 1;
static const u32 EDITOR_PRIORITY = 127;
static const s32 CIRCLE_SEGMENTS = 16;

u8 floorFieldCameraEditorGetMode(void)
{
    return gFieldCameraEditorData.mode;
}

void floorReleaseFieldCameraEditor(void)
{
    floorFieldCameraEditorStopTest();
    if (gFieldCameraEditorData.app) {
        GSgappTerminate(gFieldCameraEditorData.app);
    }
    GSmemFree(gFieldCameraEditorData.recordAllocation);
    GSmemFree(gFieldCameraEditorData.listAllocation);
    memset(&gFieldCameraEditorData, 0, sizeof(gFieldCameraEditorData));
}

void floorInitFieldCameraEditor(u32 floorID)
{
    u32 size;
    GSfloor_dd_ *floor = floorDataBiosGetPtr(floorID);
    u32 count;
    floorReleaseFieldCameraEditor();
    if (floor) {
        gFieldCameraEditorData.app = GSgappCreate(EDITOR_APP_MAIN, EDITOR_PRIORITY, 0, floorUpdateFieldCameraEditor);
        if (gFieldCameraEditorData.app) {
            FloorFieldCameraList *list = (FloorFieldCameraList *)GSmemAlloc(sizeof(FloorFieldCameraList));
            if (list) {
                gFieldCameraEditorData.cameraList = list;
                gFieldCameraEditorData.listAllocation = list;
                FloorFieldCameraList *original = floorDataBiosGetFieldCameraListPtr(floor);
                gFieldCameraEditorData.floorID = floorID;
                gFieldCameraEditorData.originalList = floor->fieldCameraList;
                gFieldCameraEditorData.mode = floorFieldCameraGetMode();
                gFieldCameraEditorData.cameraList->count = &gFieldCameraEditorData.recordCount;
                if (!original) return;
                count = *original->count;
                if (!count) return;
                size = count * sizeof(FloorFieldCameraRecord);
                FloorFieldCameraRecord *records = (FloorFieldCameraRecord *)GSmemAlloc(size);
                if (records) {
                    gFieldCameraEditorData.recordCount = count;
                    gFieldCameraEditorData.recordAllocation = records;
                    gFieldCameraEditorData.cameraList->records = records;
                    memcpy(gFieldCameraEditorData.cameraList->records, original->records, size);
                    return;
                }
            }
        }
    }
    floorReleaseFieldCameraEditor();
}

void floorUpdateFieldCameraEditor(unsigned long appID, unsigned long userData)
{
    s32 i;
    FloorFieldCameraRecord *record = gFieldCameraEditorData.selectedRecord;
    GSfloor_dd_ *floor = floorDataBiosGetPtr(GSfloorGetCurrent());
    GSfloor_dd_ *editedFloor = floorDataBiosGetPtr(gFieldCameraEditorData.floorID);
    if (!floor || floor != editedFloor) {
        floorReleaseFieldCameraEditor();
        return;
    }
    if (gFieldCameraEditorData.testing) return;
    if (record) {
        GSvec target;
        GSvec offset;
        target.clear();
        if (cameraGetType() != CAMERA_PAD_ROTATE) return;
        GScameraLoadActiveCameraMatrix(GSGFX_LAYER_INVALID);
        GScamera *camera = GScameraGetActiveCamera();
        if (!camera) return;
        GSvecCopy(&target, &camera->mInterest);
        cameraGetTargetOfs(&offset);
        if (floorFieldCameraEditorGetMode() == FIELD_CAMERA_MODE_AREA && !dbgMenuIsOpen() &&
            !menuIsCheck(MENU_ID_FIELD_CAMERA_EDITOR)) {
            u32 changed = GSinputButtonsChanged(GS_INPUT_PORT_1);
            u32 pressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
            u32 triggered = pressed & changed;
            if (triggered & PAD_TRIGGER_Z) {
                record->rectangular ^= 1;
            }
            f32 adjustmentStep = 1.0f;
            if (record->rectangular) {
                adjustmentStep = 0.1f;
                if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_L) {
                    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_BUTTON_A) {
                        record->angle += adjustmentStep;
                    } else if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_BUTTON_B) {
                        record->angle -= adjustmentStep;
                    }
                } else if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_R) {
                    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_BUTTON_A) {
                        record->aspect += adjustmentStep;
                        if (record->aspect > 20.0f) record->aspect = 20.0f;
                    } else if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_BUTTON_B) {
                        record->aspect -= adjustmentStep;
                        if (record->aspect < 1.0f) record->aspect = 1.0f;
                    }
                }
            } else {
                if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_BUTTON_A) {
                    record->radius += adjustmentStep;
                    if (record->radius > 500.0f) record->radius = 500.0f;
                } else if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_BUTTON_B) {
                    record->radius -= adjustmentStep;
                    if (record->radius < 5.0f) record->radius = 5.0f;
                }
            }
            GSgfxLayer *layer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
            if (layer) {
                GScameraLoadActiveCameraMatrix(GSGFX_LAYER_DEFAULT);
                layer->pixelMode(true, true, false);
                layer->zCompare(false, GSGFX_Z_COMPARE_ALWAYS);
                layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
                layer->cullMode(GSGFX_CULL_NONE);
                layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_ONE, true);
                layer->setRenderStateAndSelect(GSgfxVFBuildTemp(
                    GX_POS_XYZ, GX_F32, GX_FALSE, GX_NRM_XYZ,
                    1, GX_RGBA8, 0, GX_TEX_S, GX_U8));
                if (record->rectangular) {
                    layer->primitiveType(GSGFX_QUADS);
                    f32 cosine = std::cosf(record->angle);
                    f32 sine = std::sinf(record->angle);
                    f32 radius = record->radius;
                    f32 width = record->aspect * radius;
                    layer->beginSurface(4);
                    layer->vertex3f(target.x + radius * cosine + width * sine, 0.1f + (target.y - offset.y), target.z - radius * sine + width * cosine);
                    layer->color4u8(128, 128, 128, 128);
                    layer->vertex3f(target.x - radius * cosine + width * sine, 0.1f + (target.y - offset.y), target.z + radius * sine + width * cosine);
                    layer->color4u8(128, 128, 128, 128);
                    layer->vertex3f(target.x - radius * cosine - width * sine, 0.1f + (target.y - offset.y), target.z + radius * sine - width * cosine);
                    layer->color4u8(128, 128, 128, 128);
                    layer->vertex3f(target.x + radius * cosine - width * sine, 0.1f + (target.y - offset.y), target.z - radius * sine - width * cosine);
                    layer->color4u8(128, 128, 128, 128);
                    layer->endSurface();
                } else {
                    layer->primitiveType(GSGFX_PRIM_TRIANGLE_FAN);
                    layer->beginSurface(CIRCLE_SEGMENTS + 2);
                    layer->vertex3f(target.x, 0.1f + (target.y - offset.y), target.z);
                    layer->color4u8(128, 128, 128, 128);
                    const f32 segmentAngle = 2.0f * 3.1415927f / CIRCLE_SEGMENTS;
                    for (i = 0; i < CIRCLE_SEGMENTS + 1; i++) {
                        f32 angle = segmentAngle * i;
                        layer->vertex3f(record->radius * std::cosf(angle) + target.x,
                                        0.1f + (target.y - offset.y),
                                        record->radius * std::sinf(angle) + target.z);
                        layer->color4u8(128, 128, 128, 128);
                    }
                    layer->endSurface();
                }
            }
        }
        record->x = target.x;
        record->y = target.y - offset.y;
        record->z = target.z;
        record->distance = cameraGetDistance();
        record->height = cameraGetHeight();
        record->rotationY = cameraGetRotY();
        record->fov = cameraGetFov();
    }
}

void floorFieldCameraEditorStopTest(void)
{
    gFieldCameraEditorData.testing = 0;
    if (gFieldCameraEditorData.floorID) {
        GSfloor_dd_ *floor = floorDataBiosGetPtr(gFieldCameraEditorData.floorID);
        if (floor) {
            floor->fieldCameraList = gFieldCameraEditorData.originalList;
            floorInitFieldCamera(floor);
        }
    }
}

u32 floorFieldCameraEditorGetFloorID(void)
{
    return gFieldCameraEditorData.floorID;
}
