#include <GSgfx.h>
#include <GScamera.h>
#include <dolphin/mtx.h>
#include <dolphin/gx/GXTransform.h>


enum {
    GSGFX_PROJECT_RESULT_UNAVAILABLE = 0,
    GSGFX_PROJECT_RESULT_REJECTED = 1,
    GSGFX_PROJECT_RESULT_SUCCESS = 2,
    GSGFX_PROJECT_RESULT_MIXED = 3,
};

enum {
    GSGFX_MATRIX_VIEW = 2,
    GSGFX_MATRIX_VIEW_INVERSE = 3,
    GSGFX_MATRIX_PROJECTION = 4,
};

f32 g_projProjectParams[7];
f32 g_projViewPort[6] = { 0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f };

f32 (*g_projCameraMatrix)[4];
f32 (*g_projInvCameraMatrix)[4];
f32 (*g_projProjectMatrix)[4];

static s32 projectPoint(GSvec *src, GSvec *dst)
{
    f32 z, y, x;
    x = g_projInvCameraMatrix[0][2];
    y = g_projInvCameraMatrix[1][2];
    z = g_projInvCameraMatrix[2][2];
    GSvec axis(x, y, z);
    x = g_projInvCameraMatrix[0][3];
    y = g_projInvCameraMatrix[1][3];
    z = g_projInvCameraMatrix[2][3];
    GSvec delta(x, y, z);
    f32 depth;

    PSVECSubtract((const Vec *)src, (const Vec *)&delta, (Vec *)&delta);
    depth = PSVECDotProduct((const Vec *)&axis, (const Vec *)&delta);
    if (depth <= -g_projProjectMatrix[2][3] && depth >= g_projProjectMatrix[2][3])
        return GSGFX_PROJECT_RESULT_REJECTED;
    GXProject(src->x, src->y, src->z, g_projCameraMatrix, g_projProjectParams,
              g_projViewPort, &dst->x, &dst->y, &dst->z);
    if (depth > 0.0f) {
        dst->x = -dst->x;
        dst->y = -dst->y;
        dst->z = -1.0f;
    }
    return GSGFX_PROJECT_RESULT_SUCCESS;
}

extern "C" s32 GSgfxProjectFastList(GSvec *src, GSvec *dst, u32 count)
{
    s32 status;
    GSvec *output = dst;
    GSvec *input = src;
    u32 remaining;
    u32 i;

    if (g_projCameraMatrix == 0 || g_projProjectMatrix == 0) {
        status = GSGFX_PROJECT_RESULT_UNAVAILABLE;
    } else {
        status = GSGFX_PROJECT_RESULT_SUCCESS;
        remaining = count;
        for (i = 0; i < count; i++, input++, output++) {
            if (projectPoint(input, output) == GSGFX_PROJECT_RESULT_REJECTED) {
                status = GSGFX_PROJECT_RESULT_MIXED;
                remaining--;
                output->z = 1.0f;
                output->y = 1.0f;
                output->x = 1.0f;
            }
        }
        if (remaining == 0)
            status = GSGFX_PROJECT_RESULT_REJECTED;
    }
    return status;
}

extern "C" s32 GSgfxProjectFast(GSvec *src, GSvec *dst)
{
    if (g_projCameraMatrix == 0 || g_projProjectMatrix == 0)
        return GSGFX_PROJECT_RESULT_UNAVAILABLE;
    return projectPoint(src, dst);
}

extern "C" void GSgfxProjectSetupCameraParameters(GScamera *camera)
{
    f32 (*projMtx)[4];
    f32 (*invMtx)[4];
    f32 (*camMtx)[4];

    if (camera == 0) {
        projMtx = GSgfxMatrixGetPtr(GSGFX_MATRIX_PROJECTION);
        invMtx = GSgfxMatrixGetPtr(GSGFX_MATRIX_VIEW_INVERSE);
        camMtx = GSgfxMatrixGetPtr(GSGFX_MATRIX_VIEW);
        g_projCameraMatrix = camMtx;
        g_projInvCameraMatrix = invMtx;
        g_projProjectMatrix = projMtx;
        if (camMtx != 0 && projMtx != 0) {
            g_projProjectParams[0] = 0.0f;
            g_projProjectParams[1] = projMtx[0][0];
            g_projProjectParams[2] = projMtx[0][2];
            g_projProjectParams[3] = projMtx[1][1];
            g_projProjectParams[4] = projMtx[1][2];
            g_projProjectParams[5] = projMtx[2][2];
            g_projProjectParams[6] = projMtx[2][3];
            g_projViewPort[0] = 0.0f;
            g_projViewPort[1] = 0.0f;
            g_projViewPort[2] = 640.0f;
            g_projViewPort[3] = 480.0f;
            g_projViewPort[4] = 0.0f;
            g_projViewPort[5] = 1.0f;
        }
    } else {
        projMtx = GScameraGetProjMatrixPtr(camera);
        invMtx = GScameraGetInvMatrixPtr(camera);
        camMtx = GScameraGetMatrixPtr(camera);
        g_projCameraMatrix = camMtx;
        g_projInvCameraMatrix = invMtx;
        g_projProjectMatrix = projMtx;
        if (camMtx != 0 && projMtx != 0) {
            g_projProjectParams[0] = 0.0f;
            g_projProjectParams[1] = projMtx[0][0];
            g_projProjectParams[2] = projMtx[0][2];
            g_projProjectParams[3] = projMtx[1][1];
            g_projProjectParams[4] = projMtx[1][2];
            g_projProjectParams[5] = projMtx[2][2];
            g_projProjectParams[6] = projMtx[2][3];
            g_projViewPort[0] = 0.0f;
            g_projViewPort[1] = 0.0f;
            g_projViewPort[2] = 640.0f;
            g_projViewPort[3] = 480.0f;
            g_projViewPort[4] = 0.0f;
            g_projViewPort[5] = 1.0f;
        }
    }
}

extern "C" void GSgfxProjectSetupParameters(void)
{
    f32 (*projMtx)[4];
    f32 (*invMtx)[4];
    f32 (*camMtx)[4];
    projMtx = GSgfxMatrixGetPtr(GSGFX_MATRIX_PROJECTION);
    invMtx = GSgfxMatrixGetPtr(GSGFX_MATRIX_VIEW_INVERSE);
    camMtx = GSgfxMatrixGetPtr(GSGFX_MATRIX_VIEW);
    g_projCameraMatrix = camMtx;
    g_projInvCameraMatrix = invMtx;
    g_projProjectMatrix = projMtx;
    if (camMtx != 0 && projMtx != 0) {
        g_projProjectParams[0] = 0.0f;
        g_projProjectParams[1] = projMtx[0][0];
        g_projProjectParams[2] = projMtx[0][2];
        g_projProjectParams[3] = projMtx[1][1];
        g_projProjectParams[4] = projMtx[1][2];
        g_projProjectParams[5] = projMtx[2][2];
        g_projProjectParams[6] = projMtx[2][3];
        g_projViewPort[0] = 0.0f;
        g_projViewPort[1] = 0.0f;
        g_projViewPort[2] = 640.0f;
        g_projViewPort[3] = 480.0f;
        g_projViewPort[4] = 0.0f;
        g_projViewPort[5] = 1.0f;
    }
}
