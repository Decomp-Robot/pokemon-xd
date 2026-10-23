#include <types.h>
#include <hsd/cobj.h>
#include <dolphin/os/OSFastCast.h>
#include <hsd/aobj.h>
#include <GScamera.h>
#include <GSmem.h>
#include <GSgfxMatrixStack.h>


extern "C" u8 GSgfxVideoGetVsyncRate(void);

extern _GScamera *_cameraStructList;
extern u32 _cameraStructCount;
extern _GScamera *_newActiveCamera;
extern f32 _frameCount;
extern "C" const f32 __float_epsilon[];

void cameraGetFrameCount(_HSD_AObj *aobj);
void _cameraLoadCameraMatrix(_GScamera *camera, GSgfxLayerID layer);
void _cameraLoadClipping(_GScamera *camera, GSgfxLayer *layer);



static inline void cameraGetOrtho(_GScamera *camera, f32 *top, f32 *bottom,
                                  f32 *left, f32 *right)
{
    if (top == 0) return;
    if (bottom == 0) return;
    if (left == 0) return;
    if (right == 0) return;
    HSD_CObjGetOrtho(camera->mCObj, top, bottom, left, right);
}

static inline void cameraGetProjectionPerspective(_GScamera *camera, f32 *fov,
                                                   f32 *aspect, f32 *nearPlane,
                                                   f32 *farPlane)
{
    f32 cameraFov, cameraAspect;
    HSD_CObjGetPerspective(camera->mCObj, &cameraFov, &cameraAspect);
    if (fov != 0) *fov = cameraFov;
    if (aspect != 0) *aspect = cameraAspect;
    if (nearPlane != 0) *nearPlane = HSD_CObjGetNear(camera->mCObj);
    if (farPlane != 0) *farPlane = HSD_CObjGetFar(camera->mCObj);
}

static inline void cameraSetAnimationFrame(_GScamera *camera, f32 frame)
{
    if (camera->mHasAnimation) {
        camera->mAnimFrame = frame;
        HSD_ForeachAnim(camera->mCObj, 2, 0xFFFFu,
                        (void (*)())HSD_AObjSetRate, 1, (0.0));
        HSD_CObjReqAnim(camera->mCObj, camera->mAnimFrame);
        HSD_CObjAnim(camera->mCObj);
        HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
        HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
        HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
        HSD_ForeachAnim(camera->mCObj, 2, 0xFFFFu,
                        (void (*)())HSD_AObjSetRate, 1, camera->mAnimRate);
    }
}

static inline void cameraSetAnimRate(_GScamera *camera, f32 rate)
{
    if (camera->mHasAnimation) {
        if (GSgfxVideoGetVsyncRate() == 50) {
            rate *= (1.2f);
        }
        camera->mAnimRate = rate;
        HSD_ForeachAnim(camera->mCObj, 2, 0xFFFFu,
                        (void (*)())HSD_AObjSetRate, 1, camera->mAnimRate);
    }
}

static inline void cameraSetAnimIndex(_GScamera *camera, unsigned int index)
{
    if (camera->mHasAnimation) {
        HSD_CObjRemoveAnim(camera->mCObj);
        if (index <= camera->mAnimCount) {
            camera->mAnimIndex = index;
            HSD_CObjAddAnim(camera->mCObj,
                            camera->mDesc->mAnimations[camera->mAnimIndex]);
            _frameCount = (0.0f);
            HSD_ForeachAnim(camera->mCObj, 2, 0xFFFFu,
                            (void (*)())cameraGetFrameCount, 0);
            camera->mAnimFrameCount = _frameCount;
            cameraSetAnimationFrame(camera, (0.0f));
        }
    }
}

static inline void cameraGetPosition(_GScamera *camera, GSvec *position)
{
    if (position == 0) return;
    if (camera->mAnimating) {
        HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
    }
    position->x = camera->mPosition.x;
    position->y = camera->mPosition.y;
    position->z = camera->mPosition.z;
}

static inline void cameraLoadMatrix(_GScamera *camera, GSgfxLayerID layer)
{
    if (camera == 0) {
        if (_activeCamera != 0) {
            _cameraLoadCameraMatrix(_activeCamera, layer);
        }
    } else {
        _cameraLoadCameraMatrix(camera, layer);
    }
}


void _cameraPostUpdate(f32 deltaTime)
{
    for (u32 i = 0; i < _cameraStructCount; ++i) {
        _GScamera *camera = &_cameraStructList[i];
        if (camera->mValid == 1 && camera->mAnimating == 1) {
            f32 lastFrame;
            f32 step;
            s32 type;
            step = camera->mAnimRate * deltaTime;
            lastFrame = camera->mAnimFrameCount - (1.0f);
            type = camera->mAnimType;
            if (camera->mAnimDirection == -1) {
                camera->mAnimFrame -= step;
            } else if (camera->mAnimDirection == 1) {
                camera->mAnimFrame += step;
            }
            switch (type) {
            case GSCAMERA_ANIM_ONCE:
                if (camera->mAnimFrame >= lastFrame - (0.1f)) {
                    camera->mAnimEnded = 1;
                    camera->mAnimDirection = 0;
                    camera->mAnimFrame = lastFrame - (0.1f);
                }
                break;
            case GSCAMERA_ANIM_LOOP:
                if (camera->mAnimFrame >= lastFrame) {
                    camera->mAnimFrame -= lastFrame;
                }
                break;
            case GSCAMERA_ANIM_PINGPONG:
                if (camera->mAnimFrame >= lastFrame) {
                    camera->mAnimDirection = -1;
                } else if (camera->mAnimFrame <= (0.0f)) {
                    camera->mAnimDirection = 1;
                }
                break;
            }
            camera->mMatrixDirty = 1;
        }
    }
    if (_newActiveCamera != 0) {
        if (_newActiveCamera->mValid) {
            _activeCamera = _newActiveCamera;
        }
        _newActiveCamera = 0;
    }
}

void _cameraPreUpdate(f32 deltaTime)
{
    #pragma unused(deltaTime)
    for (u32 i = 0; i < _cameraStructCount; ++i) {
        _GScamera *camera = &_cameraStructList[i];
        if (camera->mValid != 1) continue;

        camera->mPreviousPosition.x = camera->mPosition.x;
        camera->mPreviousPosition.y = camera->mPosition.y;
        camera->mPreviousPosition.z = camera->mPosition.z;
        if (camera->mAnimating == 1) {
            HSD_CObjReqAnim(camera->mCObj, camera->mAnimFrame);
            HSD_CObjAnim(camera->mCObj);
            HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
            HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
            HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
            camera->mMatrixDirty = 1;
        }
        if (camera->mAnimating) {
            PSMTXCopy(HSD_CObjGetViewingMtxPtr(camera->mCObj),
                      camera->mViewMatrix);
            HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
            HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
            HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
        } else if (camera->mLookAt == 1) {
            Vec distance;
            PSVECSubtract((Vec *)&camera->mPosition,
                          (Vec *)&camera->mInterest, &distance);
            if ((distance.x > (0.0f) ? distance.x : -distance.x) < __float_epsilon[0] &&
                (distance.y > (0.0f) ? distance.y : -distance.y) < __float_epsilon[0] &&
                (distance.z > (0.0f) ? distance.z : -distance.z) < __float_epsilon[0]) {
                camera->mInterest.x += (0.01);
            }
            C_MTXLookAt(camera->mViewMatrix, (Point3d *)&camera->mPosition,
                        (Vec *)&camera->mUp, (Point3d *)&camera->mInterest);
        } else {
            Mtx rotateY, rotateZ;
            PSMTXRotRad(camera->mViewMatrix, 'X', camera->mRotation.x);
            PSMTXRotRad(rotateY, 'Y', camera->mRotation.y);
            PSMTXConcat(rotateY, camera->mViewMatrix, camera->mViewMatrix);
            PSMTXRotRad(rotateZ, 'Z', camera->mRotation.z);
            PSMTXConcat(rotateZ, camera->mViewMatrix, camera->mViewMatrix);
            PSMTXTransApply(camera->mViewMatrix, camera->mViewMatrix,
                            camera->mPosition.x, camera->mPosition.y,
                            camera->mPosition.z);
            PSMTXInverse(camera->mViewMatrix, camera->mViewMatrix);
        }
        camera->mMatrixDirty = 0;
    }
}

extern "C" void GScameraPopState(_GScamera *camera, GScameraState *state)
{
    camera->mPosition.x = state->mPosition.x;
    camera->mPosition.y = state->mPosition.y;
    camera->mPosition.z = state->mPosition.z;
    camera->mPreviousPosition.x = state->mPreviousPosition.x;
    camera->mPreviousPosition.y = state->mPreviousPosition.y;
    camera->mPreviousPosition.z = state->mPreviousPosition.z;
    camera->mRotation.x = state->mRotation.x;
    camera->mRotation.y = state->mRotation.y;
    camera->mRotation.z = state->mRotation.z;
    PSMTXCopy(state->mViewMatrix, camera->mViewMatrix);
    PSMTXCopy(state->mInverseViewMatrix, camera->mInverseViewMatrix);
    camera->mUp.x = state->mUp.x;
    camera->mUp.y = state->mUp.y;
    camera->mUp.z = state->mUp.z;
    camera->mInterest.x = state->mInterest.x;
    camera->mInterest.y = state->mInterest.y;
    camera->mInterest.z = state->mInterest.z;
    camera->mAnimEnded = state->mAnimEnded;
    if (!state->mAnimEnded && state->mAnimating) {
        cameraSetAnimIndex(camera, state->mAnimIndex);
        camera->mAnimType = state->mAnimType;
        cameraSetAnimRate(camera, state->mAnimRate);
        cameraSetAnimationFrame(camera, state->mAnimFrame);
        if (camera->mHasAnimation) {
            camera->mAnimating = 1;
            camera->mAnimEnded = 0;
            camera->mAnimDirection = 1;
            cameraSetAnimationFrame(camera, camera->mAnimFrame);
        }
    }
}

extern "C" void GScameraPushState(_GScamera *camera, GScameraState *state)
{
    state->mAnimating = camera->mAnimating;
    state->mPosition.x = camera->mPosition.x;
    state->mPosition.y = camera->mPosition.y;
    state->mPosition.z = camera->mPosition.z;
    state->mPreviousPosition.x = camera->mPreviousPosition.x;
    state->mPreviousPosition.y = camera->mPreviousPosition.y;
    state->mPreviousPosition.z = camera->mPreviousPosition.z;
    state->mRotation.x = camera->mRotation.x;
    state->mRotation.y = camera->mRotation.y;
    state->mRotation.z = camera->mRotation.z;
    PSMTXCopy(camera->mViewMatrix, state->mViewMatrix);
    PSMTXCopy(camera->mInverseViewMatrix, state->mInverseViewMatrix);
    state->mUp.x = camera->mUp.x;
    state->mUp.y = camera->mUp.y;
    state->mUp.z = camera->mUp.z;
    state->mInterest.x = camera->mInterest.x;
    state->mInterest.y = camera->mInterest.y;
    state->mInterest.z = camera->mInterest.z;
    state->mAnimType = camera->mAnimType;
    state->mAnimIndex = camera->mAnimIndex;
    state->mAnimRate = camera->mAnimRate;
    state->mAnimFrame = camera->mAnimFrame;
    state->mAnimEnded = camera->mAnimEnded;
}

extern "C" f32 GScameraGetAnimFrame(_GScamera *camera)
{
    return camera->mAnimFrame;
}

extern "C" u8 GScameraHasAnimationEnded(_GScamera *camera)
{
    return camera->mAnimEnded;
}

extern "C" void GScameraStopAnimation(_GScamera *camera)
{
    camera->mAnimating = 0;
    camera->mLookAt = 1;
    HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
    HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
    HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
}

extern "C" void GScameraStartAnimation(_GScamera *camera)
{
    if (camera->mHasAnimation) {
        camera->mAnimating = 1;
        camera->mAnimEnded = 0;
        camera->mAnimDirection = 1;
        cameraSetAnimationFrame(camera, camera->mAnimFrame);
    }
}

extern "C" void GScameraSetAnimType(_GScamera *camera, int type)
{
    camera->mAnimType = type;
}

extern "C" void GScameraSetAnimFrame(_GScamera *camera, f32 frame)
{
    cameraSetAnimationFrame(camera, frame);
}

extern "C" void GScameraSetAnimRate(_GScamera *camera, f32 rate)
{
    if (camera->mHasAnimation) {
        if (GSgfxVideoGetVsyncRate() == 50) {
            rate *= (1.2f);
        }
        camera->mAnimRate = rate;
        HSD_ForeachAnim(camera->mCObj, 2, 0xFFFFu,
                        (void (*)())HSD_AObjSetRate, 1, camera->mAnimRate);
    }
}

extern "C" void GScameraSetAnimIndex(_GScamera *camera, unsigned int index)
{
    cameraSetAnimIndex(camera, index);
}

extern "C" u8 GScameraIsAnimating(_GScamera *camera)
{
    return camera->mAnimating;
}

extern "C" void GScameraGetDistanceVector(_GScamera *camera, GSvec *distance)
{
    if (distance != 0) {
        PSVECSubtract((Vec *)&camera->mPosition, (Vec *)&camera->mInterest,
                      (Vec *)distance);
    }
}

extern "C" f32 (*GScameraGetProjMatrixPtr(_GScamera *camera))[4]
{
    static Mtx44 projMtx;
    f32 fov, aspect, nearPlane, farPlane;
    f32 left, top, right, bottom;
    if (camera->mCObj->projectionType == HSD_COBJ_PERSPECTIVE) {
        cameraGetProjectionPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
        GSmtx44Perspective(projMtx, fov, aspect, nearPlane, farPlane);
    } else {
        cameraGetOrtho(camera, &top, &bottom, &left, &right);
        GSmtx44Ortho(projMtx, top, bottom, left, right, (0.1f), (2000.0f));
    }
    return projMtx;
}

static inline void cameraUpdateViewMatrix(_GScamera *camera)
{
    if (camera->mMatrixDirty) {
        if (camera->mAnimating) {
            PSMTXCopy(HSD_CObjGetViewingMtxPtr(camera->mCObj),
                      camera->mViewMatrix);
            HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
            HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
            HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
        } else if (camera->mLookAt == 1) {
            Vec direction;
            PSVECSubtract((Vec *)&camera->mPosition,
                          (Vec *)&camera->mInterest, &direction);
            if ((direction.x > (0.0f) ? direction.x : -direction.x) <
                    __float_epsilon[0] &&
                (direction.y > (0.0f) ? direction.y : -direction.y) <
                    __float_epsilon[0] &&
                (direction.z > (0.0f) ? direction.z : -direction.z) <
                    __float_epsilon[0]) {
                camera->mInterest.x += (0.01);
            }
            C_MTXLookAt(camera->mViewMatrix,
                        (Point3d *)&camera->mPosition,
                        (Vec *)&camera->mUp,
                        (Point3d *)&camera->mInterest);
        } else {
            Mtx rotateZ, rotateY;
            PSMTXRotRad(camera->mViewMatrix, 'X', camera->mRotation.x);
            PSMTXRotRad(rotateY, 'Y', camera->mRotation.y);
            PSMTXConcat(rotateY, camera->mViewMatrix, camera->mViewMatrix);
            PSMTXRotRad(rotateZ, 'Z', camera->mRotation.z);
            PSMTXConcat(rotateZ, camera->mViewMatrix, camera->mViewMatrix);
            PSMTXTransApply(camera->mViewMatrix, camera->mViewMatrix,
                            camera->mPosition.x, camera->mPosition.y,
                            camera->mPosition.z);
            PSMTXInverse(camera->mViewMatrix, camera->mViewMatrix);
        }
        camera->mMatrixDirty = 0;
    }
}

extern "C" f32 (*GScameraGetInvMatrixPtr(_GScamera *camera))[4]
{
    cameraUpdateViewMatrix(camera);
    PSMTXInverse(camera->mViewMatrix, camera->mInverseViewMatrix);
    return camera->mInverseViewMatrix;
}

extern "C" f32 (*GScameraGetMatrixPtr(_GScamera *camera))[4]
{
    cameraUpdateViewMatrix(camera);
    return camera->mViewMatrix;
}

extern "C" void GScameraGetLookAt(_GScamera *camera, GSvec *up, GSvec *interest)
{
    if (up != 0) {
        up->x = camera->mUp.x;
        up->y = camera->mUp.y;
        up->z = camera->mUp.z;
    }
    if (interest != 0) {
        interest->x = camera->mInterest.x;
        interest->y = camera->mInterest.y;
        interest->z = camera->mInterest.z;
    }
}

extern "C" void GScameraLookAt(_GScamera *camera, GSvec *up, GSvec *interest)
{
    if (up != 0) {
        camera->mUp.x = up->x;
        camera->mUp.y = up->y;
        camera->mUp.z = up->z;
    }
    if (interest != 0) {
        camera->mInterest.x = interest->x;
        camera->mInterest.y = interest->y;
        camera->mInterest.z = interest->z;
    }
    camera->mMatrixDirty = 1;
    camera->mLookAt = 1;
}

extern "C" void GScameraGetFrontVector(_GScamera *camera, GSvec *front)
{
    if (camera == 0 || front == 0) return;

    if (camera->mLookAt) {
        GSvec position;
        cameraGetPosition(camera, &position);
        PSVECSubtract((Vec *)&camera->mInterest, (Vec *)&position,
                      (Vec *)front);
    } else {
        Mtx rotateX, rotateY, rotateZ;
        f32 zero = (0.0f);
        front->x = zero;
        front->y = zero;
        front->z = (-1.0f);
        PSMTXRotRad(rotateX, 'X', camera->mRotation.x);
        PSMTXRotRad(rotateY, 'Y', camera->mRotation.y);
        PSMTXConcat(rotateY, rotateX, rotateX);
        PSMTXRotRad(rotateZ, 'Z', camera->mRotation.z);
        PSMTXConcat(rotateZ, rotateX, rotateX);
        PSMTXMultVec(rotateX, (Vec *)front, (Vec *)front);
    }
    PSVECNormalize((Vec *)front, (Vec *)front);
}

/* Scale used when the centre of the clip rectangle is derived. */
static const f32 sClipCentreScale[1] = { 0.5f };

extern "C" void GScameraGetRotation(_GScamera *camera, GSvec *rotation)
{
    if (rotation != 0) {
        rotation->x = camera->mRotation.x;
        rotation->y = camera->mRotation.y;
        rotation->z = camera->mRotation.z;
    }
}

extern "C" void GScameraGetPosition(_GScamera *camera, GSvec *position)
{
    cameraGetPosition(camera, position);
}

extern "C" void GScameraGetPerspective(_GScamera *camera, f32 *fov, f32 *aspect,
                                       f32 *nearPlane, f32 *farPlane)
{
    f32 cameraFov, cameraAspect;
    HSD_CObjGetPerspective(camera->mCObj, &cameraFov, &cameraAspect);
    if (fov != 0) *fov = cameraFov;
    if (aspect != 0) *aspect = cameraAspect;
    if (nearPlane != 0) *nearPlane = HSD_CObjGetNear(camera->mCObj);
    if (farPlane != 0) *farPlane = HSD_CObjGetFar(camera->mCObj);
}

extern "C" void GScameraSetRotation(_GScamera *camera, GSvec *rotation)
{
    camera->mRotation.x = rotation->x;
    camera->mRotation.y = rotation->y;
    camera->mRotation.z = rotation->z;
    camera->mMatrixDirty = 1;
    camera->mLookAt = 0;
}

extern "C" void GScameraSetPosition(_GScamera *camera, GSvec *position)
{
    HSD_CObjSetEyePosition(camera->mCObj, (Vec *)position);
    camera->mPosition.x = position->x;
    camera->mPosition.y = position->y;
    camera->mPosition.z = position->z;
    camera->mMatrixDirty = 1;
}

extern "C" void GScameraSetPerspective(_GScamera *camera, f32 fov, f32 aspect,
                                       f32 nearPlane, f32 farPlane)
{
    HSD_CObjSetProjectionType(camera->mCObj, HSD_COBJ_PERSPECTIVE);
    HSD_CObjSetPerspective(camera->mCObj, fov, aspect);
    HSD_CObjSetNear(camera->mCObj, nearPlane);
    HSD_CObjSetFar(camera->mCObj, farPlane);
    camera->mMatrixDirty = 1;
}

extern "C" void GScameraSetScissor(_GScamera *camera, u8 enabled,
                                   unsigned int left, unsigned int top,
                                   unsigned int width, unsigned int height)
{
    if (camera == 0 || !camera->mValid) return;

    camera->mCObj->scissorLeft = left;
    camera->mCObj->scissorTop = top;
    camera->mCObj->scissorRight = left + width;
    camera->mCObj->scissorBottom = top + height;
    if (enabled) camera->mFlags |= GSCAMERA_CUSTOM_SCISSOR;
    else camera->mFlags &= ~GSCAMERA_CUSTOM_SCISSOR;
}

extern "C" void GScameraSetViewport(_GScamera *camera, u8 enabled, f32 left,
                                    f32 top, f32 right, f32 bottom)
{
    if (camera == 0 || !camera->mValid) return;

    camera->mCObj->viewportLeft = left;
    camera->mCObj->viewportTop = top;
    camera->mCObj->viewportRight = left + 2.0f * (right - left);
    camera->mCObj->viewportBottom = top + 2.0f * (bottom - top);
    if (enabled) camera->mFlags |= GSCAMERA_CUSTOM_VIEWPORT;
    else camera->mFlags &= ~GSCAMERA_CUSTOM_VIEWPORT;
}

extern "C" void GScameraLoadProjection(_GScamera *camera, GSgfxLayer *layer)
{
    if (camera == 0) return;
    _HSD_CObj *cobj = camera->mCObj;
    if (cobj == 0) return;

    GSgfxLayerID layerID = layer->mLayerID;
    Mtx44 projection;
    cameraLoadMatrix(camera, layerID);
    HSD_CObjMakeProjectionMtx(cobj, projection);
    HSD_CObjSetupViewingMtx(cobj);
}

extern "C" void GScameraLoadCameraMatrix(_GScamera *camera, GSgfxLayerID layer)
{
    cameraLoadMatrix(camera, layer);
}

extern "C" void GScameraLoadActiveCameraMatrix(GSgfxLayerID layer)
{
    if (_activeCamera != 0) {
        _cameraLoadCameraMatrix(_activeCamera, layer);
    }
}

void _cameraLoadCameraMatrix(_GScamera *camera, GSgfxLayerID layerID)
{
    GSgfxLayer *layer = layerID == (GSgfxLayerID)-1 ? 0 : GSgfxLayerFindByID(layerID);

    f32 fovOrLeft, aspectOrTop, nearOrRight, farOrBottom;

    if (camera->mAnimating) {
        HSD_CObjClearFlags(camera->mCObj, HSD_COBJ_USER_MATRIX);
        PSMTXCopy(HSD_CObjGetViewingMtxPtr(camera->mCObj), camera->mViewMatrix);
        HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
        HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
        HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
        if (layer) {
            cameraGetProjectionPerspective(camera, &fovOrLeft, &aspectOrTop, &nearOrRight, &farOrBottom);
            f32 farDepth = farOrBottom;
            f32 nearDepth = nearOrRight;
            Mtx44 projection;
            GSmtx44Perspective(projection, fovOrLeft, aspectOrTop, nearDepth, farDepth);
            layer->perspectiveProjection(projection);
            layer->projectionDepthRange(nearDepth, farDepth);
            layer->mMatrixStack->identity();
            layer->mMatrixStack->mult((GSmtx *)camera->mViewMatrix);
        }
    } else if (camera->mCObj->projectionType == HSD_COBJ_PERSPECTIVE) {
        cameraUpdateViewMatrix(camera);
        _HSD_CObj *cobj = camera->mCObj;
        HSD_CObjSetFlags(cobj, HSD_COBJ_USER_MATRIX | HSD_COBJ_INV_VIEW_MATRIX_DIRTY);
        PSMTXCopy(camera->mViewMatrix, cobj->viewMtx);
        if (layer) {
            cameraGetProjectionPerspective(camera, &fovOrLeft, &aspectOrTop, &nearOrRight, &farOrBottom);
            f32 farDepth = farOrBottom;
            f32 nearDepth = nearOrRight;
            Mtx44 projection;
            GSmtx44Perspective(projection, fovOrLeft, aspectOrTop, nearDepth, farDepth);
            layer->perspectiveProjection(projection);
            layer->projectionDepthRange(nearDepth, farDepth);
            layer->mMatrixStack->identity();
            layer->mMatrixStack->mult((GSmtx *)camera->mViewMatrix);
        }
    } else if (layer) {
        {
            Mtx44 projection;
            cameraGetOrtho(camera, &aspectOrTop, &farOrBottom, &fovOrLeft, &nearOrRight);
            GSmtx44Ortho(projection, aspectOrTop, farOrBottom, fovOrLeft, nearOrRight, (0.0f), -30000.0f);
            layer->orthographicProjection(projection);
        }
        layer->mMatrixStack->identity();
        layer->projectionDepthRange((0.0f), 30000.0f);
        layer->mMatrixStack->identity();
    }
    if (layer) {
        _cameraLoadClipping(camera, layer);
    }
}


extern "C" void GSgfxVideoGetEFBSize(u16 *width, u16 *height);
extern "C" _HSD_CObj *HSD_CObjLoadDesc(_HSD_CObjDesc *desc);
void GSgfxRenderModuleRegister(u8 priority, GSgfxRenderModule *module, void *argument);

static inline void cameraLayerViewport(GSgfxLayer *layer, f32 left, f32 top,
                                      f32 centreX, f32 centreY)
{
    layer->mProjectionState.mViewportLeft = left;
    layer->mProjectionState.mViewportTop = top;
    layer->mProjectionState.mViewportCentreX = centreX;
    layer->mProjectionState.mViewportCentreY = centreY;
    layer->mPassState[0].mGroupBFlags |= 0x10;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    layer->mPassState[1].mGroupBFlags |= 0x10;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
}

static inline void cameraLayerScissorDirty(GSgfxLayer *layer)
{
    layer->mPassState[0].mGroupBFlags |= 1;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    layer->mPassState[1].mGroupBFlags |= 1;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
}

static inline void cameraLayerScissor(GSgfxLayer *layer, u16 left, u16 top,
                                     u16 width, u16 height)
{
    layer->mProjectionState.mScissorLeft = left;
    layer->mProjectionState.mScissorTop = top;
    layer->mProjectionState.mScissorWidth = width;
    layer->mProjectionState.mScissorHeight = height;
    cameraLayerScissorDirty(layer);
}

static inline void cameraGetScissor(_HSD_CObj *cobj, u16 *left, u16 *right,
                                   u16 *top, u16 *bottom)
{
    *right = cobj->scissorRight;
    *top = cobj->scissorTop;
    *left = cobj->scissorLeft;
    *bottom = cobj->scissorBottom;
}

void _cameraLoadClipping(_GScamera *camera, GSgfxLayer *layer)
{
    if (camera->mFlags & GSCAMERA_CLIPPING_ENABLED) {
        if (camera->mFlags & GSCAMERA_CUSTOM_VIEWPORT) {
            _HSD_CObj *cobj = camera->mCObj;
            f32 top = cobj->viewportTop;
            f32 left = cobj->viewportLeft;
            cameraLayerViewport(layer, left, top,
                left + sClipCentreScale[0] * (cobj->viewportRight - left),
                top + sClipCentreScale[0] * (cobj->viewportBottom - top));
        } else {
            u16 height, width;
            GSgfxVideoGetEFBSize(&width, &height);
            u16 halfWidth = width >> 1;
            f32 centreX = __OSu16tof32(&halfWidth);
            u16 halfHeight = height >> 1;
            f32 centreY = __OSu16tof32(&halfHeight);
            cameraLayerViewport(layer, 0.0f, 0.0f, centreX, centreY);
        }
        if (camera->mFlags & GSCAMERA_CUSTOM_SCISSOR) {
            u16 right, top, left, bottom;
            cameraGetScissor(camera->mCObj, &left, &right, &top, &bottom);
            cameraLayerScissor(layer, left, top, right - left, bottom - top);
        } else {
            layer->mProjectionState.mScissorLeft = 0;
            layer->mProjectionState.mScissorTop = 0;
            GSgfxVideoGetEFBSize(&layer->mProjectionState.mScissorWidth,
                                &layer->mProjectionState.mScissorHeight);
            cameraLayerScissorDirty(layer);
        }
    }
}

extern "C" _GScamera *GScameraGetActiveCamera(void)
{
    return _activeCamera;
}

extern "C" void GScameraSetActiveCamera(_GScamera *camera)
{
    if (!camera) return;
    _GScamera *active = _activeCamera;
    if (active == 0) _activeCamera = camera;
    if (active != 0) _newActiveCamera = camera;
    if (camera->mAnimating) {
        PSMTXCopy(HSD_CObjGetViewingMtxPtr(camera->mCObj), camera->mViewMatrix);
        HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
        HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
        HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
        } else if (camera->mLookAt == 1) {
            Vec distance;
            PSVECSubtract((Vec *)&camera->mPosition,
                          (Vec *)&camera->mInterest, &distance);
            if ((distance.x > (0.0f) ? distance.x : -distance.x) < __float_epsilon[0] &&
                (distance.y > (0.0f) ? distance.y : -distance.y) < __float_epsilon[0] &&
                (distance.z > (0.0f) ? distance.z : -distance.z) < __float_epsilon[0]) {
                camera->mInterest.x += (0.01);
            }
            C_MTXLookAt(camera->mViewMatrix, (Point3d *)&camera->mPosition,
                        (Vec *)&camera->mUp, (Point3d *)&camera->mInterest);
        } else {
            Mtx rotateY, rotateZ;
            PSMTXRotRad(camera->mViewMatrix, 'X', camera->mRotation.x);
            PSMTXRotRad(rotateY, 'Y', camera->mRotation.y);
            PSMTXConcat(rotateY, camera->mViewMatrix, camera->mViewMatrix);
            PSMTXRotRad(rotateZ, 'Z', camera->mRotation.z);
            PSMTXConcat(rotateZ, camera->mViewMatrix, camera->mViewMatrix);
            PSMTXTransApply(camera->mViewMatrix, camera->mViewMatrix,
                            camera->mPosition.x, camera->mPosition.y,
                            camera->mPosition.z);
            PSMTXInverse(camera->mViewMatrix, camera->mViewMatrix);
        }
        camera->mMatrixDirty = 0;
}

static inline int cameraObjectUnref(_HSD_CObj *object)
{
    int noRef = object->referenceCount == (u16)-1;
    if (noRef) return noRef;
    return object->referenceCount-- == 0;
}
static inline void cameraObjectRemove(_HSD_CObj *object)
{
    if (object && cameraObjectUnref(object)) {
        if (object) {
            object->classInfo->release(object);
            object->classInfo->destroy(object);
        }
    }
}
extern "C" void GScameraFree(_GScamera *camera)
{
    if (camera == _activeCamera) _activeCamera = 0;
    if (camera == _newActiveCamera) _newActiveCamera = 0;
    cameraObjectRemove(camera->mCObj);
    camera->mValid = 0;
}

extern "C" u8 GScameraIsValid(_GScamera *camera)
{
    if (camera == 0) return 0;
    if (camera->mValid != 1) return 0;
    if (camera->mCObj == 0) return 0;
    _GScamera *entry = _cameraStructList;
    for (u32 i = 0; i < _cameraStructCount; ++i, ++entry) {
        if (entry == camera) return 1;
    }
    return 0;
}

static inline _GScamera *cameraFindFree(void)
{
    _GScamera *camera = _cameraStructList;
    for (u32 i = 0; i < _cameraStructCount; ++i, ++camera) {
        if (camera->mValid == 0) return camera;
    }
    return 0;
}

extern "C" _GScamera *GScameraLoad(GScameraDesc *desc)
{
    _GScamera *camera = cameraFindFree();
    if (camera == 0) return 0;
    memset(camera, 0, sizeof(*camera));
    camera->mDesc = desc;
    camera->mCObj = HSD_CObjLoadDesc(camera->mDesc->mCamera);
    if (camera->mCObj == 0) return 0;
    camera->mValid = 1;
    camera->mAnimating = 0;
    if (camera->mDesc->mAnimations) {
        camera->mHasAnimation = 1;
        camera->mAnimRate = 1.0f;
        camera->mAnimType = GSCAMERA_ANIM_LOOP;
        camera->mAnimEnded = 0;
        camera->mAnimCount = 0;
        while (camera->mDesc->mAnimations[camera->mAnimCount]) ++camera->mAnimCount;
        cameraSetAnimIndex(camera, 0);
    } else {
        camera->mHasAnimation = 0;
    }
    u16 width, height;
    GSgfxVideoGetEFBSize(&width, &height);
    camera->mFlags = GSCAMERA_CLIPPING_ENABLED;
    _HSD_CObjDesc *cdesc = &camera->mCameraDesc;
    if (cdesc->viewportLeft != 0 || cdesc->viewportTop != 0 ||
        cdesc->viewportRight != width || cdesc->viewportBottom != height)
        camera->mFlags |= GSCAMERA_CUSTOM_VIEWPORT;
    if (cdesc->scissorLeft != 0 || cdesc->scissorTop != 0 ||
        cdesc->scissorRight != width || cdesc->scissorBottom != height)
        camera->mFlags |= GSCAMERA_CUSTOM_SCISSOR;
    cdesc->scissorLeft = 0;
    cdesc->scissorTop = 0;
    cdesc->scissorRight = width;
    cdesc->scissorBottom = height;
    camera->mValid = 1;
    camera->mLookAt = 0;
    HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
    HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
    HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
    return camera;
}

extern "C" _GScamera *GScameraCreate(void)
{
    _GScamera *camera = cameraFindFree();
    if (camera == 0) return 0;
    memset(camera, 0, sizeof(*camera));
    u16 width, height;
    GSgfxVideoGetEFBSize(&width, &height);
    camera->mEyeDesc.className = 0;
    camera->mEyeDesc.position.x = 0.0f;
    camera->mEyeDesc.position.y = 0.0f;
    camera->mEyeDesc.position.z = 0.0f;
    camera->mEyeDesc.constraints = 0;
    camera->mInterestDesc.className = 0;
    camera->mInterestDesc.position.x = 0.0f;
    camera->mInterestDesc.position.y = 0.0f;
    camera->mInterestDesc.position.z = 1.0f;
    camera->mInterestDesc.constraints = 0;
    _HSD_CObjDesc *desc = &camera->mCameraDesc;
    desc->className = 0;
    desc->flags = 0;
    desc->projectionType = HSD_COBJ_PERSPECTIVE;
    desc->viewportLeft = 0;
    desc->viewportTop = 0;
    desc->viewportRight = width;
    desc->viewportBottom = height;
    desc->scissorLeft = 0;
    desc->scissorTop = 0;
    desc->scissorRight = width;
    desc->scissorBottom = height;
    desc->eye = &camera->mEyeDesc;
    desc->interest = &camera->mInterestDesc;
    desc->roll = 0.0f;
    desc->up = 0;
    desc->nearPlane = 0.1f;
    desc->farPlane = 2000.0f;
    desc->fov = 60.0f;
    desc->aspect = 1.333333f;
    camera->mUp.x = 0.0f;
    camera->mUp.y = 1.0f;
    camera->mUp.z = 0.0f;
    camera->mFlags = GSCAMERA_CLIPPING_ENABLED | GSCAMERA_CUSTOM_VIEWPORT;
    camera->mCObj = HSD_CObjLoadDesc(desc);
    if (camera->mCObj == 0) return 0;
    HSD_CObjGetEyePosition(camera->mCObj, (Vec *)&camera->mPosition);
    HSD_CObjGetUpVector(camera->mCObj, (Vec *)&camera->mUp);
    HSD_CObjGetInterest(camera->mCObj, (Vec *)&camera->mInterest);
    camera->mValid = 1;
    camera->mLookAt = 0;
    camera->mAnimating = 0;
    camera->mHasAnimation = 0;
    camera->mDesc = 0;
    return camera;
}

extern "C" void GScameraInit(u32 count)
{
    GSgfxRenderModule module = {0, _cameraPreUpdate, 0, 0, _cameraPostUpdate, 0,
                               GSGFX_RENDER_MODULE_SIGNATURE};
    _cameraStructCount = count;
    _cameraStructList = (_GScamera *)GSmemAlloc(count * sizeof(_GScamera));
    if (_cameraStructList) {
        for (u32 i = 0; i < _cameraStructCount; ++i) _cameraStructList[i].mValid = 0;
        GSgfxRenderModuleRegister(0, &module, 0);
    }
}

void cameraGetFrameCount(_HSD_AObj *aobj)
{
    if (!aobj) __assert("aobj.h", 0xAC, "aobj");
    _frameCount = 1.0f + aobj->mEndFrame;
}
