#include <hsd/cobj.h>

void HSD_CObjGetOrtho(_HSD_CObj *camera, f32 *top, f32 *bottom, f32 *left,
                      f32 *right)
{
    if (camera == NULL || camera->projectionType != HSD_COBJ_ORTHO) {
        return;
    }
    if (top != NULL) {
        *top = camera->projection.ortho.top;
    }
    if (bottom != NULL) {
        *bottom = camera->projection.ortho.bottom;
    }
    if (left != NULL) {
        *left = camera->projection.ortho.left;
    }
    if (right != NULL) {
        *right = camera->projection.ortho.right;
    }
}

void HSD_CObjGetPerspective(_HSD_CObj *camera, f32 *fov, f32 *aspect)
{
    if (camera == NULL || camera->projectionType != HSD_COBJ_PERSPECTIVE) {
        return;
    }
    if (fov != NULL) {
        *fov = camera->projection.perspective.fov;
    }
    if (aspect != NULL) {
        *aspect = camera->projection.perspective.aspect;
    }
}

void HSD_CObjSetOrtho(_HSD_CObj *camera, f32 top, f32 bottom, f32 left,
                      f32 right)
{
    if (camera == NULL) {
        return;
    }
    camera->projectionType = HSD_COBJ_ORTHO;
    camera->projection.ortho.top = top;
    camera->projection.ortho.bottom = bottom;
    camera->projection.ortho.left = left;
    camera->projection.ortho.right = right;
}

void HSD_CObjSetFrustum(_HSD_CObj *camera, f32 top, f32 bottom, f32 left,
                        f32 right)
{
    if (camera == NULL) {
        return;
    }
    camera->projectionType = HSD_COBJ_FRUSTUM;
    camera->projection.frustum.top = top;
    camera->projection.frustum.bottom = bottom;
    camera->projection.frustum.left = left;
    camera->projection.frustum.right = right;
}

void HSD_CObjSetPerspective(_HSD_CObj *camera, f32 fov, f32 aspect)
{
    if (camera == NULL) {
        return;
    }
    camera->projectionType = HSD_COBJ_PERSPECTIVE;
    camera->projection.perspective.fov = fov;
    camera->projection.perspective.aspect = aspect;
}

void HSD_CObjSetProjectionType(_HSD_CObj *camera, u8 type)
{
    if (camera == NULL) {
        return;
    }
    camera->projectionType = type;
}

int HSD_CObjGetProjectionType(_HSD_CObj *camera)
{
    if (camera == NULL) {
        return HSD_COBJ_PERSPECTIVE;
    }
    return camera->projectionType;
}

void HSD_CObjSetViewportfx4(_HSD_CObj *camera, f32 left, f32 right, f32 top,
                            f32 bottom)
{
    if (camera == NULL) {
        return;
    }
    camera->viewportLeft = left;
    camera->viewportRight = right;
    camera->viewportTop = top;
    camera->viewportBottom = bottom;
}

void HSD_CObjSetScissorx4(_HSD_CObj *camera, u16 left, u16 right, u16 top,
                          u16 bottom)
{
    if (camera == NULL) {
        return;
    }
    camera->scissorLeft = left;
    camera->scissorRight = right;
    camera->scissorTop = top;
    camera->scissorBottom = bottom;
}

void HSD_CObjSetFar(_HSD_CObj *camera, f32 farPlane)
{
    if (camera == NULL) {
        return;
    }
    camera->farPlane = farPlane;
}

f32 HSD_CObjGetFar(_HSD_CObj *camera)
{
    if (camera == NULL) {
        return 0.0f;
    }
    return camera->farPlane;
}

void HSD_CObjSetNear(_HSD_CObj *camera, f32 nearPlane)
{
    if (camera == NULL) {
        return;
    }
    camera->nearPlane = nearPlane;
}

f32 HSD_CObjGetNear(_HSD_CObj *camera)
{
    if (camera == NULL) {
        return 0.0f;
    }
    return camera->nearPlane;
}

void HSD_CObjSetAspect(_HSD_CObj *camera, f32 aspect)
{
    if (camera == NULL || camera->projectionType != HSD_COBJ_PERSPECTIVE) {
        return;
    }
    camera->projection.perspective.aspect = aspect;
}

void HSD_CObjSetFov(_HSD_CObj *camera, f32 fov)
{
    if (camera == NULL || camera->projectionType != HSD_COBJ_PERSPECTIVE) {
        return;
    }
    camera->projection.perspective.fov = fov;
}
