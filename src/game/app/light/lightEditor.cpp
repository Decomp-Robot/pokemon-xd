#include <app/light/lightEditor.h>
#include <GSlight.h>
#include <light.h>
#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GSinput.h>
#include <GSmsg.h>
#include <GSres.h>
#include <GSstream.h>
#include <GSprim.h>
#include <GScamera.h>
#include <GSmtx.h>
#include <GSAPI/GSvec/GSvec.h>
#include <people.h>
#include <peopleTalk.h>
#include <floorBios.h>
#include <app/floor/floorDataBios.h>
#include <app/camera/camera.h>
#include <app/camera/CameraType.h>
#include <dbgMenu.h>
#include <dolphin/pad.h>
#include <dolphin/gx/GXVert.h>
#include <libc/math.h>

#define ABS(x) ((x) < 0.0f ? -(x) : (x))

enum LightEditMode {
    LIGHT_EDIT_TYPE = 0,
    LIGHT_EDIT_COLOR = 1,
    LIGHT_EDIT_VECTOR = 2
};

enum LightEditComponent {
    LIGHT_EDIT_COMPONENT_X = 0,
    LIGHT_EDIT_COMPONENT_Y = 1,
    LIGHT_EDIT_COMPONENT_Z = 2
};

enum LightPositionMode {
    LIGHT_POSITION_CAMERA = 0,
    LIGHT_POSITION_PEOPLE = 1
};

enum LightEditCommand {
    LIGHT_EDIT_COMMAND = -1
};

enum LightEditPeopleFocus {
    LIGHT_EDIT_PEOPLE_FOCUS_GROUP = 0,
    LIGHT_EDIT_PEOPLE_FOCUS_ID = 100
};

#define LIGHT_EDIT_COLOR_NORMAL 0xFFFFFFFF
#define LIGHT_EDIT_COLOR_SELECT 0xFFFF80FF

static void editVectorComponent(GSvec *pos, s8 item)
{
    f32 step = 0.1f;
    f32 *value = NULL;
    switch (item) {
    case LIGHT_EDIT_COMPONENT_X:
        value = &pos->x;
        break;
    case LIGHT_EDIT_COMPONENT_Y:
        value = &pos->y;
        break;
    case LIGHT_EDIT_COMPONENT_Z:
        value = &pos->z;
        break;
    }
    if (value != NULL) {
        if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_L) {
            step = 1.0f;
        }
        if (GSinputButtonsRepeat(GS_INPUT_PORT_1) & PAD_BUTTON_X) {
            *value -= step;
        }
        if (GSinputButtonsRepeat(GS_INPUT_PORT_1) & PAD_BUTTON_Y) {
            *value += step;
        }
    }
}

inline void LightEditor::selectNextPeople()
{
    u32 trigger = GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1);
    s32 peopleMax = peopleBiosGetMaxNum();
    s32 index = mPeopleIndex;

    if (trigger & PAD_TRIGGER_R) {
        for (s32 i = 0; i < peopleMax; i++) {
            index++;
            if (index >= peopleMax) {
                index = 0;
            }
            if (peopleBiosGetState(peopleBiosGetWorkPtr(index)) == (PEOPLE_BIOS_STATE_ACTIVE | PEOPLE_BIOS_STATE_MODEL_VISIBLE)) {
                mPeopleIndex = index;
                break;
            }
        }
    }
}

static inline GSmtx makeRotAxis(GSvec *axis, f32 rad)
{
    GSmtx m;
    PSMTXRotAxisRad(m.sdk(), (Vec *)axis, rad);
    return m;
}

void LightEditor::editDirection(GSlight *light)
{
    f32 leftX;
    f32 leftY;
    f32 pitch;
    f32 yaw;
    f32 frameTime = GSgfxVideoGetLastRenderTime();
    f32 divisor = 1000.0f;
    GSvec pos;
    GSvec dir;
    GSvec axis;
    GSvec up;

    selectNextPeople();

    GSlightGetPosition(light, &pos);
    dir = pos;
    PSVECNormalize(&dir, &dir);

    leftX = frameTime * GSinputGetLeftStickXData(GS_INPUT_PORT_1, false);
    leftY = frameTime * GSinputGetLeftStickYData(GS_INPUT_PORT_1, false);
    GSinputGetRightStickXData(GS_INPUT_PORT_1, false);
    GSinputGetRightStickYData(GS_INPUT_PORT_1, false);
    if (!(GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_L)) {
        divisor *= 4.0f;
    }
    yaw = leftX / divisor;
    pitch = leftY / divisor;

    dir = pos;
    PSVECNormalize(&dir, &dir);
    up = GSvec(0.0f, 1.0f, 0.0f);
    if (dir.x < 0.00001f && dir.x > -0.00001f && dir.z < 0.00001f && dir.z > -0.00001f) {
        if (dir.y > 0.0f) {
            up = GSvec(1.0f, 0.0f, 0.0f);
        } else {
            up = GSvec(1.0f, 0.0f, 0.0f);
        }
    }
    PSVECCrossProduct(&dir, &up, &axis);

    Mtx mtx;
    GSmtx rot = makeRotAxis(&axis, -pitch);
    Mtx yawMtx;
    PSMTXCopy(rot.sdk(), mtx);
    PSMTXRotRad(yawMtx, 'Y', yaw);
    PSMTXConcat(yawMtx, mtx, mtx);
    PSMTXMultVec(mtx, &pos, &pos);
    GSlightSetPosition(light, &pos);
}

void LightEditor::editPosition(GSlight *light)
{
    f32 frameTime = GSgfxVideoGetLastRenderTime();
    f32 divisor = 64.0f;
    _GScamera *camera = GScameraGetActiveCamera();
    GSvec pos;
    GSvec move;
    GSvec moveX;
    GSvec moveY;
    GSvec moveZ;
    GSvec front;
    GSvec up;
    GSvec right;

    GScameraGetPosition(camera, &pos);
    GScameraGetFrontVector(camera, &front);
    up = GSvec(0.0f, 1.0f, 0.0f);
    PSVECCrossProduct(&front, &up, &right);
    PSVECCrossProduct(&front, &right, &up);

    moveX = GSvec(0.0f, 0.0f, 0.0f);
    moveY = GSvec(0.0f, 0.0f, 0.0f);
    moveZ = GSvec(0.0f, 0.0f, 0.0f);
    GSlightGetPosition(light, &pos);

    f32 leftX = frameTime * GSinputGetLeftStickXData(GS_INPUT_PORT_1, false);
    f32 leftY = frameTime * GSinputGetLeftStickYData(GS_INPUT_PORT_1, false);
    f32 rightX = frameTime * GSinputGetRightStickXData(GS_INPUT_PORT_1, false);
    f32 rightY = frameTime * GSinputGetRightStickYData(GS_INPUT_PORT_1, false);
    if (ABS(leftX) < ABS(rightX)) {
        leftX = rightX;
    }
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & PAD_TRIGGER_L) {
        divisor *= 0.25;
    }
    moveX = right;
    PSVECScale(&moveX, &moveX, leftX / divisor);
    moveY = up;
    PSVECScale(&moveY, &moveY, leftY / divisor);
    moveZ = front;
    PSVECScale(&moveZ, &moveZ, -rightY / divisor);
    PSVECAdd(&moveX, &moveY, &move);
    PSVECAdd(&move, &moveZ, &move);
    PSVECAdd(&pos, &move, &pos);

    if (mEditMode == LIGHT_EDIT_VECTOR) {
        editVectorComponent(&pos, mEditItem);
    }

    if (GSinputButtonsRepeat(GS_INPUT_PORT_1) & PAD_TRIGGER_Z) {
        mPositionMode = !mPositionMode;
        if (mPositionMode) {
            peopleGetPos(LIGHT_EDIT_PEOPLE_FOCUS_GROUP, LIGHT_EDIT_PEOPLE_FOCUS_ID, &pos);
            pos.y += 18.0f;
        } else {
            GSvec offset;
            _GScamera *cam = GScameraGetActiveCamera();
            GScameraGetPosition(cam, &pos);
            GScameraGetFrontVector(cam, &offset);
            PSVECScale(&offset, &offset, 50.0f);
            PSVECAdd(&pos, &offset, &pos);
        }
    }
    GSlightSetPosition(light, &pos);
}

void LightEditor::editType(GSlight *light)
{
    int type;
    GSlightType newType;

    if (mEditMode != LIGHT_EDIT_TYPE) {
        return;
    }
    if (GSresGetResourceAllGroup(floorDataBiosGetShadowLightID(floorDataBiosGetCurrentPtr())) == light) {
        return;
    }
    type = GSlightGetType(light);
    newType = GSLIGHT_TYPE_INVALID;
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1) & PAD_BUTTON_X) {
        if (type == GSLIGHT_TYPE_AMBIENT) {
            newType = GSLIGHT_TYPE_SPOT;
        } else if (type == GSLIGHT_TYPE_INFINITE) {
            newType = GSLIGHT_TYPE_AMBIENT;
        } else if (type == GSLIGHT_TYPE_POINT) {
            newType = GSLIGHT_TYPE_INFINITE;
        } else if (type == GSLIGHT_TYPE_SPOT) {
            newType = GSLIGHT_TYPE_POINT;
        }
    }
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1) & PAD_BUTTON_Y) {
        if (type == GSLIGHT_TYPE_AMBIENT) {
            newType = GSLIGHT_TYPE_INFINITE;
        } else if (type == GSLIGHT_TYPE_INFINITE) {
            newType = GSLIGHT_TYPE_POINT;
        } else if (type == GSLIGHT_TYPE_POINT) {
            newType = GSLIGHT_TYPE_SPOT;
        } else if (type == GSLIGHT_TYPE_SPOT) {
            newType = GSLIGHT_TYPE_AMBIENT;
        }
    }
    if (newType != GSLIGHT_TYPE_INVALID) {
        GSlightSetType(light, newType);
    }
}

void LightEditor::editCommand()
{
    GSstreamFileOutput file;
    GSvec position;
    GSvec target;
    GXColor color;
    bool isShadowLight;
    const char *typeName;
    bool hasPosition;
    int i;
    GSlight *light;

    if (!(GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1) & PAD_BUTTON_A)) {
        return;
    }
    if (!file.open("lightinfo.txt")) {
        return;
    }
    file.writePrintf("//type, color, pos/dir\n");
    for (i = 0; i < lightGetLightSize(); i++) {
        light = lightGetLight(i);
        isShadowLight = GSresGetResourceAllGroup(floorDataBiosGetShadowLightID(floorDataBiosGetCurrentPtr())) == light;
        hasPosition = false;
        switch (GSlightGetType(light)) {
        case GSLIGHT_TYPE_AMBIENT:
            typeName = "Amb";
            break;
        case GSLIGHT_TYPE_INFINITE:
            typeName = "Dir";
            hasPosition = true;
            break;
        case GSLIGHT_TYPE_POINT:
            typeName = "Pnt";
            hasPosition = true;
            break;
        case GSLIGHT_TYPE_SPOT:
            typeName = "Spt";
            hasPosition = true;
            break;
        default:
            typeName = "???";
            break;
        }
        GSlightGetPosition(light, &position);
        GSlightGetTarget(light, &target);
        GSlightGetColor(light, &color);
        file.writePrintf("%s: ", typeName);
        file.writePrintf("(%d,%d,%d) ", color.r, color.b, color.g);
        if (hasPosition) {
            file.writePrintf("(%.2f,%.2f,%.2f)", position.x, position.y, position.z);
        }
        if (isShadowLight) {
            file.writePrintf("\t// shadow light");
        }
        file.writePrintf("\n");
    }
}

void LightEditor::dispCommand(GSgfxLayerID layer, long y)
{
    if (layer != GSGFX_LAYER_INVALID) {
        GSprint(layer, 20, y, mLightIndex < 0 ? LIGHT_EDIT_COLOR_SELECT : LIGHT_EDIT_COLOR_NORMAL, "save");
    }
}

void LightEditor::dispLightModel(GSlight *light, bool selected)
{
    static f32 widthArray[4][2] = {
        { -1.0f, -1.0f },
        { 1.0f, -1.0f },
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
    };
    f32 size = 0.5f;
    GSvec pos;
    GSvec dir;
    GSvec target;
    GXColor color;
    GSgfxLayer *layer;
    int i;

    if (!GSlightGetActive(light)) {
        return;
    }
    if (selected) {
        size += ABS((f32)sin(mBlinkPhase) / 2.0f);
        mBlinkPhase += 0.1f;
        if (mBlinkPhase > 6.2831855f) {
            mBlinkPhase = 0.0f;
        }
    }
    if (GSlightGetType(light) == GSLIGHT_TYPE_AMBIENT) {
        return;
    }
    GSlightGetPosition(light, &pos);
    GSlightGetTarget(light, &target);
    GSlightGetColor(light, &color);

    if (GSlightGetType(light) == GSLIGHT_TYPE_INFINITE) {
        tagPeopleWork *work = peopleBiosGetWorkPtr(mPeopleIndex);
        if (work != NULL) {
            GSvec peoplePos = *peopleBiosGetPosPtr(work);
            GSvec offset;
            dir = pos;
            PSVECNormalize(&dir, &dir);
            offset = dir;
            PSVECScale(&offset, &offset, 20.0f);
            PSVECAdd(&offset, &peoplePos, &pos);
            pos.y += 5.0f;
        }
    }

    layer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
    if (layer == NULL) {
        return;
    }
    GScameraLoadActiveCameraMatrix(layer->mLayerID);
    layer->pixelMode(true, true, true);
    layer->zCompare(true, GSGFX_Z_COMPARE_LESS);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_POSITION_COLOR));
    layer->mPrimitiveType = GSGFX_QUADS;
    layer->beginSurface(24);
    for (i = 0; i < 4; i++) {
        GXPosition3f32(pos.x + size, size * widthArray[i][0] + pos.y, size * widthArray[i][1] + pos.z);
        GXColor4u8(color.r, color.g, color.b, 255);
    }
    for (i = 0; i < 4; i++) {
        GXPosition3f32(pos.x - size, size * widthArray[3 - i][0] + pos.y, size * widthArray[3 - i][1] + pos.z);
        GXColor4u8(color.r, color.g, color.b, 255);
    }
    for (i = 0; i < 4; i++) {
        GXPosition3f32(size * widthArray[i][0] + pos.x, pos.y + size, size * widthArray[i][1] + pos.z);
        GXColor4u8(color.r, color.g, color.b, 255);
    }
    for (i = 0; i < 4; i++) {
        GXPosition3f32(size * widthArray[3 - i][0] + pos.x, pos.y - size, size * widthArray[3 - i][1] + pos.z);
        GXColor4u8(color.r, color.g, color.b, 255);
    }
    for (i = 0; i < 4; i++) {
        GXPosition3f32(size * widthArray[i][0] + pos.x, size * widthArray[i][1] + pos.y, pos.z + size);
        GXColor4u8(color.r, color.g, color.b, 255);
    }
    for (i = 0; i < 4; i++) {
        GXPosition3f32(size * widthArray[3 - i][0] + pos.x, size * widthArray[3 - i][1] + pos.y, pos.z - size);
        GXColor4u8(color.r, color.g, color.b, 255);
    }
    layer->endSurface();

    if (selected) {
        layer->mPrimitiveType = GSGFX_PRIM_LINES;
        layer->lineWidth(12, GX_TO_ZERO);
        layer->beginSurface(6);
        GXPosition3f32(pos.x, pos.y - 100.0f, pos.z);
        GXColor4u8(255, 0, 0, 255);
        GXPosition3f32(pos.x, pos.y + 100.0f, pos.z);
        GXColor4u8(255, 0, 0, 255);
        GXPosition3f32(pos.x - 100.0f, pos.y, pos.z);
        GXColor4u8(0, 255, 0, 255);
        GXPosition3f32(pos.x + 100.0f, pos.y, pos.z);
        GXColor4u8(0, 255, 0, 255);
        GXPosition3f32(pos.x, pos.y, pos.z - 100.0f);
        GXColor4u8(0, 0, 255, 255);
        GXPosition3f32(pos.x, pos.y, pos.z + 100.0f);
        GXColor4u8(0, 0, 255, 255);
        layer->endSurface();
    }

    if (GSlightGetType(light) == GSLIGHT_TYPE_INFINITE) {
        PSVECScale(&dir, &dir, 10.0f);
        layer->mPrimitiveType = GSGFX_PRIM_LINE_STRIP;
        layer->lineWidth(12, GX_TO_ZERO);
        layer->beginSurface(2);
        GXPosition3f32(pos.x, pos.y, pos.z);
        GXColor4u8(color.r, color.g, color.b, 255);
        GXPosition3f32(pos.x - dir.x, pos.y - dir.y, pos.z - dir.z);
        GXColor4u8(color.r, color.g, color.b, 255);
        layer->endSurface();
    }
}

static inline bool isShadowLight(GSlight *light)
{
    return GSresGetResourceAllGroup(floorDataBiosGetShadowLightID(floorDataBiosGetCurrentPtr())) == light;
}
void LightEditor::dispInfo(GSlight *light, long x, long y, bool selected)
{
    u32 textColor;
    const char *typeName;
    GXColor color;
    u32 shadowColor;
    GSvec position;
    GSvec target;
    bool hasPosition = false;
    bool isShadow = isShadowLight(light);
    long layer = dbgMenuGetLayerID();
    u32 selectColor;

    if (layer == GSGFX_LAYER_INVALID) {
        return;
    }
    selectColor = selected ? LIGHT_EDIT_COLOR_SELECT : LIGHT_EDIT_COLOR_NORMAL;
    GSlightGetActive(light);
    GSlightGetPosition(light, &position);
    GSlightGetTarget(light, &target);
    GSlightGetColor(light, &color);
    GSlightCanAnimate(light);
    switch (GSlightGetType(light)) {
    case GSLIGHT_TYPE_AMBIENT:
        typeName = "Amb";
        break;
    case GSLIGHT_TYPE_INFINITE:
        typeName = "Dir";
        hasPosition = true;
        break;
    case GSLIGHT_TYPE_POINT:
        typeName = "Pnt";
        hasPosition = true;
        break;
    case GSLIGHT_TYPE_SPOT:
        typeName = "Spt";
        hasPosition = true;
        break;
    default:
        typeName = "???";
        break;
    }

    textColor = LIGHT_EDIT_COLOR_NORMAL;
    if (mEditMode == LIGHT_EDIT_TYPE) {
        textColor = selectColor;
    }
    GSprint((GSgfxLayerID)layer, x, y, textColor, "%s:", typeName);
    textColor = (mEditItem == LIGHT_EDIT_COMPONENT_X && mEditMode == LIGHT_EDIT_COLOR) ? selectColor : LIGHT_EDIT_COLOR_NORMAL;
    GSprint((GSgfxLayerID)layer, x + 52, y, textColor, "(%02x,", color.r);
    textColor = (mEditItem == LIGHT_EDIT_COMPONENT_Y && mEditMode == LIGHT_EDIT_COLOR) ? selectColor : LIGHT_EDIT_COLOR_NORMAL;
    GSprint((GSgfxLayerID)layer, x + 91, y, textColor, "%02x,", color.g);
    textColor = (mEditItem == LIGHT_EDIT_COMPONENT_Z && mEditMode == LIGHT_EDIT_COLOR) ? selectColor : LIGHT_EDIT_COLOR_NORMAL;
    shadowColor = textColor;
    GSprint((GSgfxLayerID)layer, x + 122, y, textColor, "%02x)", color.b);
    if (hasPosition) {
        textColor = (mEditItem == LIGHT_EDIT_COMPONENT_X && mEditMode == LIGHT_EDIT_VECTOR) ? selectColor : LIGHT_EDIT_COLOR_NORMAL;
        GSprint((GSgfxLayerID)layer, x + 182, y, textColor, "(%.2f,", position.x);
        textColor = (mEditItem == LIGHT_EDIT_COMPONENT_Y && mEditMode == LIGHT_EDIT_VECTOR) ? selectColor : LIGHT_EDIT_COLOR_NORMAL;
        GSprint((GSgfxLayerID)layer, x + 247, y, textColor, " %.2f,", position.y);
        textColor = (mEditItem == LIGHT_EDIT_COMPONENT_Z && mEditMode == LIGHT_EDIT_VECTOR) ? selectColor : LIGHT_EDIT_COLOR_NORMAL;
        shadowColor = textColor;
        GSprint((GSgfxLayerID)layer, x + 304, y, textColor, " %.2f)", position.z);
    }
    if (isShadow) {
        GSprint((GSgfxLayerID)layer, x + 382, y, shadowColor, "(shadow)");
    }
}

inline void LightEditor::editColor(GSlight *light)
{
    u8 *component = NULL;
    if (mEditMode == LIGHT_EDIT_COLOR) {
        GXColor color;
        GSlightGetColor(light, &color);
        switch (mEditItem) {
        case LIGHT_EDIT_COMPONENT_X:
            component = &color.r;
            break;
        case LIGHT_EDIT_COMPONENT_Y:
            component = &color.g;
            break;
        case LIGHT_EDIT_COMPONENT_Z:
            component = &color.b;
            break;
        }
        if (component != NULL) {
            if (GSinputButtonsRepeat(GS_INPUT_PORT_1) & PAD_BUTTON_X) {
                (*component)--;
            }
            if (GSinputButtonsRepeat(GS_INPUT_PORT_1) & PAD_BUTTON_Y) {
                (*component)++;
            }
            GSlightSetColor(light, color);
        }
    }
}

inline void LightEditor::disp()
{
    GSgfxLayerID layer = dbgMenuGetLayerID();
    u32 lightResource;
    long lightCount;
    floorGetLightInfo(&lightResource, &lightCount);
    lightCount = lightGetLightSize();
    GSgfxLayer *gfxLayer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
    if (gfxLayer != NULL) {
        GSprimSetup3DRender(gfxLayer);
    }
    for (int i = 0; i < lightCount; i++) {
        dispLightModel(lightGetLight(i), mLightIndex == i);
    }
    long y = 30;
    for (int i = 0; i < lightCount; i++) {
        dispInfo(lightGetLight(i), 20, y, mLightIndex == i);
        y += 13;
    }
    dispCommand(layer, y);
    layer = dbgMenuGetLayerID();
    if (layer != GSGFX_LAYER_INVALID) {
        if (cameraGetType() == CAMERA_PAD_MOVE) {
            GSprint(layer, 500, 32, LIGHT_EDIT_COLOR_NORMAL, "camera mode");
        } else {
            GSprint(layer, 500, 32, LIGHT_EDIT_COLOR_NORMAL, "light mode");
        }
    }
}

void LightEditor::editUpdate()
{
    GSgfxVideoGetLastRenderTime();
    u32 repeat = GSinputButtonsRepeat(GS_INPUT_PORT_1);

    if (repeat & PAD_BUTTON_START) {
        if (cameraGetType() == CAMERA_PAD_MOVE) {
            cameraSetType(CAMERA_FREE);
        } else {
            cameraSetType(CAMERA_PAD_MOVE);
        }
    }
    if (cameraGetType() != CAMERA_PAD_MOVE) {
        if (repeat & PAD_BUTTON_DOWN) {
            mLightIndex++;
        }
        if (mLightIndex >= lightGetLightSize()) {
            mLightIndex = LIGHT_EDIT_COMMAND;
        }
        if (repeat & PAD_BUTTON_UP) {
            mLightIndex--;
        }
        if (mLightIndex < LIGHT_EDIT_COMMAND) {
            mLightIndex = lightGetLightSize() - 1;
        }
        if (mLightIndex < 0) {
            editCommand();
        } else {
            GSlight *light = lightGetLight(mLightIndex);
            if (light == NULL) {
                return;
            }
            int type = GSlightGetType(light);
            if (repeat & PAD_BUTTON_RIGHT) {
                if (mEditMode == LIGHT_EDIT_TYPE) {
                    mEditMode = LIGHT_EDIT_COLOR;
                    mEditItem = LIGHT_EDIT_COMPONENT_X;
                } else if (mEditMode == LIGHT_EDIT_COLOR) {
                    mEditItem++;
                    if (mEditItem > LIGHT_EDIT_COMPONENT_Z) {
                        if (type == GSLIGHT_TYPE_AMBIENT) {
                            mEditMode = LIGHT_EDIT_TYPE;
                        } else {
                            mEditMode = LIGHT_EDIT_VECTOR;
                        }
                        mEditItem = LIGHT_EDIT_COMPONENT_X;
                    }
                } else if (mEditMode == LIGHT_EDIT_VECTOR) {
                    mEditItem++;
                    if (mEditItem > LIGHT_EDIT_COMPONENT_Z) {
                        mEditMode = LIGHT_EDIT_TYPE;
                        mEditItem = LIGHT_EDIT_COMPONENT_X;
                    }
                }
            }
            if (repeat & PAD_BUTTON_LEFT) {
                if (mEditMode == LIGHT_EDIT_TYPE) {
                    if (type == GSLIGHT_TYPE_AMBIENT) {
                        mEditMode = LIGHT_EDIT_COLOR;
                        mEditItem = LIGHT_EDIT_COMPONENT_Z;
                    } else {
                        mEditMode = LIGHT_EDIT_VECTOR;
                    }
                    mEditItem = LIGHT_EDIT_COMPONENT_Z;
                } else if (mEditMode == LIGHT_EDIT_COLOR) {
                    mEditItem--;
                    if (mEditItem < LIGHT_EDIT_COMPONENT_X) {
                        mEditMode = LIGHT_EDIT_TYPE;
                        mEditItem = LIGHT_EDIT_COMPONENT_X;
                    }
                } else if (mEditMode == LIGHT_EDIT_VECTOR) {
                    mEditItem--;
                    if (mEditItem < LIGHT_EDIT_COMPONENT_X) {
                        mEditMode = LIGHT_EDIT_COLOR;
                        mEditItem = LIGHT_EDIT_COMPONENT_Z;
                    }
                }
            }
            editType(light);
            editColor(light);
            if (type == GSLIGHT_TYPE_INFINITE) {
                editDirection(light);
            } else {
                editPosition(light);
            }
        }
    }

    disp();
}

void LightEditor::init()
{
    mPeopleIndex = 0;
    mLightIndex = 0;
    mEditMode = LIGHT_EDIT_TYPE;
    mBlinkPhase = 1.0f;
    mPositionMode = LIGHT_POSITION_CAMERA;
}

LightEditor::LightEditor()
{
    init();
}
