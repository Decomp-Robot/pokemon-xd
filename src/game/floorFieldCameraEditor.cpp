#pragma peephole off
#include <GSgfxLayer.h>
#include <floor.h>
#include <floorRoom.h>
#include <floorFieldCamera.h>
#include <GScamera.h>
#include <camera.h>
#include <GSmem.h>
#include <GSinput.h>
#include <GStrig.h>
#include <menuIds.h>
#include <string.h>

extern "C" {
    u8 dbgMenuIsOpen(void);
    u8 menuIsOpen(s32 menuID);
    void GSgfxRenderDrawEnd(GSgfxLayer *layer);
}

struct FloorCameraEditorState {
    FloorCameraRegion *selectedRegion;
    FloorCameraPositionList *positionList;
    u32 count;
    void *listAllocation;
    void *regionAllocation;
    u32 roomID;
    FloorCameraPositionList *savedCameraPositions;
    GSgappID appID;
    u8 testing;
    u8 mode;
};

FloorCameraEditorState floorCameraEditor __attribute__((aligned(8)));

static const f32 EditorRotationStep = 0.1f;
static const f32 EditorAspectStep = 0.1f;
static const f32 EditorMinimumAspect = 1.0f;
static const f32 EditorMaximumAspect = 20.0f;
static const f32 EditorRadiusStep = 1.0f;
static const f32 EditorMinimumRadius = 5.0f;
static const f32 EditorMaximumRadius = 500.0f;
static const f32 EditorSurfaceLift = 0.1f;
static const f32 EditorCircleAngleStep = 0.3926991f;

enum {
    EditorInputPort = 1,
    EditorAppType = GS_GAPP_FRAME_UPDATE,
    EditorAppPriority = 127,
    EditorOverlayLayer = 2,
    ActiveCameraDefaultLayer = -1,
    EditorOverlayColor = 128,
    EditorCirclePerimeterPoints = 17,
    EditorCircleVertexCount = 18,
    EditorRectangleVertexCount = 4,
    EditorPositionComponents = 1,
    EditorPositionType = 4,
    EditorColorCount = 1,
    EditorColorType = 5,
    EditorDisabledAttribute = 0,
    EditorToggleShape = 0x10,
    EditorRotateShape = 0x40,
    EditorResizeAspect = 0x20,
    EditorIncrease = 0x100,
    EditorDecrease = 0x200,
    FieldCameraType = 5,
    FieldCameraEditorMode = 1,
    FieldCameraMenu = MENU_ID_FIELD_CAMERA_EDITOR
};
extern "C" u8 floorFieldCameraEditorGetMode()
{
    return floorCameraEditor.mode;
}
extern "C" void floorReleaseFieldCameraEditor()
{
    floorFieldCameraEditorStopTest();
    if (floorCameraEditor.appID != 0) GSgappTerminate(floorCameraEditor.appID);
    GSmemFree(floorCameraEditor.regionAllocation);
    GSmemFree(floorCameraEditor.listAllocation);
    memset(&floorCameraEditor, 0, sizeof(floorCameraEditor));
}
extern "C" void floorInitFieldCameraEditor(u32 roomID)
{
    {
        void *allocation;
        FloorCameraPositionList *original;
        u32 *countPtr;
        u32 size;
        GSfloor_dd_ *floor;
        u32 count;
        floor = (GSfloor_dd_ *)roomIDGetMapData(roomID);
        floorReleaseFieldCameraEditor();
        if (floor == 0) goto failed;
        floorCameraEditor.appID = GSgappCreate(EditorAppType, EditorAppPriority, 0, floorUpdateFieldCameraEditor);
        if (floorCameraEditor.appID == 0) goto failed;
        allocation = GSmemAllocNoClr(sizeof(FloorCameraPositionList));
        if (allocation == 0) goto failed;
        floorCameraEditor.positionList = (FloorCameraPositionList *)allocation;
        floorCameraEditor.listAllocation = allocation;
        original = floorDataBiosGetPosListPtr(floor);
        floorCameraEditor.roomID = roomID;
        floorCameraEditor.savedCameraPositions = floor->cameraPositions;
        floorCameraEditor.mode = floorFieldCameraGetMode();
        countPtr = &floorCameraEditor.count;
        floorCameraEditor.positionList->count = countPtr;
        if (original) {
            count = *original->count;
            if (count) {
                size = count * sizeof(FloorCameraRegion);
                void *allocation = GSmemAllocNoClr(size);
                if (allocation == 0) goto failed;
                *countPtr = count;
                floorCameraEditor.regionAllocation = allocation;
                floorCameraEditor.positionList->regions = (FloorCameraRegion *)allocation;
                memcpy(floorCameraEditor.positionList->regions, original->regions, size);
            }
        }
        return;
    }
failed:
    floorReleaseFieldCameraEditor();
}

extern "C" void floorUpdateFieldCameraEditor(GSgappID appID, void *userData)
{
    FloorCameraRegion *region = floorCameraEditor.selectedRegion;
    GSfloor_dd_ *current = (GSfloor_dd_ *)roomIDGetMapData(floorGetCurrentRoomID());
    GSfloor_dd_ *edited = (GSfloor_dd_ *)roomIDGetMapData(floorCameraEditor.roomID);
    if (current == 0 || current != edited) {
        floorReleaseFieldCameraEditor();
        return;
    }
    if (!floorCameraEditor.testing && region) {
        s32 point;
        GSvec target;
        GSvec offset;
        target.clear();
        if (cameraGetType() != FieldCameraType) return;
        GScameraLoadActiveCameraMatrix((GSgfxLayerID)ActiveCameraDefaultLayer);
        _GScamera *camera = GScameraGetActiveCamera();
        if (camera == 0) return;
        GSvecCopy(&target, &camera->mTarget);
        cameraGetTargetOfs(&offset);
        if (floorFieldCameraEditorGetMode() == FieldCameraEditorMode && !dbgMenuIsOpen() && !menuIsOpen(FieldCameraMenu)) {
            u32 changed = GSinputButtonsChanged(EditorInputPort);
            u32 pressed = GSinputButtonsPressed(EditorInputPort);
            if (pressed & changed & EditorToggleShape) region->rectangular ^= 1;
            f32 sizeStep = EditorRadiusStep;
            if (region->rectangular) {
                sizeStep = EditorAspectStep;
                if (GSinputButtonsPressed(EditorInputPort) & EditorRotateShape) {
                    if (GSinputButtonsPressed(EditorInputPort) & EditorIncrease) region->rotation += EditorRotationStep;
                    else if (GSinputButtonsPressed(EditorInputPort) & EditorDecrease) region->rotation -= EditorRotationStep;
                } else if (GSinputButtonsPressed(EditorInputPort) & EditorResizeAspect) {
                    if (GSinputButtonsPressed(EditorInputPort) & EditorIncrease) {
                        region->aspect += sizeStep;
                        if (region->aspect > EditorMaximumAspect) region->aspect = EditorMaximumAspect;
                    } else if (GSinputButtonsPressed(EditorInputPort) & EditorDecrease) {
                        region->aspect -= sizeStep;
                        if (region->aspect < EditorMinimumAspect) region->aspect = EditorMinimumAspect;
                    }
                }
            } else {
                if (GSinputButtonsPressed(EditorInputPort) & EditorIncrease) {
                    region->radius += sizeStep;
                    if (region->radius > EditorMaximumRadius) region->radius = EditorMaximumRadius;
                } else if (GSinputButtonsPressed(EditorInputPort) & EditorDecrease) {
                    region->radius -= sizeStep;
                    if (region->radius < EditorMinimumRadius) region->radius = EditorMinimumRadius;
                }
            }
            GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)EditorOverlayLayer);
            if (layer) {
                GScameraLoadActiveCameraMatrix((GSgfxLayerID)EditorOverlayLayer);
                layer->pixelMode(true, true, false);
                layer->zCompare(false, GS_GFX_Z_COMPARE_ALWAYS);
                layer->alphaCompare(false, GS_GFX_A_COMPARE_ALWAYS, 0, GS_GFX_ALPHA_AND, GS_GFX_A_COMPARE_ALWAYS, 0);
                layer->cullMode(GS_GFX_CULL_NONE);
                layer->blendMode(GS_GFX_BLEND_ALPHA, GS_GFX_FACTOR_SOURCE_ALPHA, GS_GFX_FACTOR_ONE, true);
                layer->setRenderStateAndSelect(GSgfxVFBuildTemp(EditorPositionComponents, EditorPositionType, 0, EditorDisabledAttribute, EditorColorCount, EditorColorType, 0, EditorDisabledAttribute, EditorDisabledAttribute));
                if (region->rectangular) {
                    layer->primitiveType(GS_GFX_PRIM_QUADS);
                    f32 cosine = GScosRadians(region->rotation);
                    f32 halfWidth;
                    f32 sine = GSsinRadians(region->rotation);
                    halfWidth = region->radius;
                    f32 halfDepth = region->aspect * halfWidth;
                    layer->beginSurface(EditorRectangleVertexCount);
                    layer->vertex3f(target.x + halfWidth * cosine + halfDepth * sine, EditorSurfaceLift + (target.y - offset.y), target.z - halfWidth * sine + halfDepth * cosine);
                    layer->color4u8(EditorOverlayColor, EditorOverlayColor, EditorOverlayColor, EditorOverlayColor);
                    layer->vertex3f(target.x - halfWidth * cosine + halfDepth * sine, EditorSurfaceLift + (target.y - offset.y), target.z + halfWidth * sine + halfDepth * cosine);
                    layer->color4u8(EditorOverlayColor, EditorOverlayColor, EditorOverlayColor, EditorOverlayColor);
                    layer->vertex3f(target.x - halfWidth * cosine - halfDepth * sine, EditorSurfaceLift + (target.y - offset.y), target.z + halfWidth * sine - halfDepth * cosine);
                    layer->color4u8(EditorOverlayColor, EditorOverlayColor, EditorOverlayColor, EditorOverlayColor);
                    layer->vertex3f(target.x + halfWidth * cosine - halfDepth * sine, EditorSurfaceLift + (target.y - offset.y), target.z - halfWidth * sine - halfDepth * cosine);
                    layer->color4u8(EditorOverlayColor, EditorOverlayColor, EditorOverlayColor, EditorOverlayColor);
                    GSgfxRenderDrawEnd(layer);
                } else {
                    layer->primitiveType(GS_GFX_PRIM_TRIANGLE_FAN);
                    layer->beginSurface(EditorCircleVertexCount);
                    layer->vertex3f(target.x, EditorSurfaceLift + (target.y - offset.y), target.z);
                    layer->color4u8(EditorOverlayColor, EditorOverlayColor, EditorOverlayColor, EditorOverlayColor);
                    for (point = 0; point < EditorCirclePerimeterPoints; ++point) {
                        f32 angle = EditorCircleAngleStep * (f32)point;
                        layer->vertex3f(region->radius * GScosRadians(angle) + target.x, EditorSurfaceLift + (target.y - offset.y), region->radius * GSsinRadians(angle) + target.z);
                        layer->color4u8(EditorOverlayColor, EditorOverlayColor, EditorOverlayColor, EditorOverlayColor);
                    }
                    GSgfxRenderDrawEnd(layer);
                }
            }
        }
        region->x = target.x;
        region->y = target.y - offset.y;
        region->z = target.z;
        region->cameraDistance = cameraGetDistance();
        region->cameraHeight = cameraGetHeight();
        region->cameraRotation = cameraGetRotY();
        region->cameraFov = cameraGetFov();
    }
}
extern "C" void floorFieldCameraEditorStopTest()
{
    floorCameraEditor.testing = 0;
    if (floorCameraEditor.roomID != 0) {
        GSfloor_dd_ *floor = (GSfloor_dd_ *)roomIDGetMapData(floorCameraEditor.roomID);
        if (floor != 0) {
            floor->cameraPositions = floorCameraEditor.savedCameraPositions;
            floorInitFieldCamera(floor);
        }
    }
}
extern "C" u32 floorFieldCameraEditorGetFloorID()
{
    return floorCameraEditor.roomID;
}

#include <GSgfxInline.h>
#pragma peephole reset
#include <dolphin/gx/GXVert.h>

#include <GSvecCopyInline.h>
