#include <leaffx.h>
#include <GSbezier.h>
#include <GSgfxVideo.h>
#include <GSlog.h>
#include <GSmaterial.h>
#include <GSmem.h>
#include <GSres.h>
#include <app/floor/floorLoad.h>
#include <wazaSequenceSys.h>
#include <hsdRandom.h>
#include <math.h>

namespace {
const u32 leafResourceGroup = 20000;
const u32 maxLeaves = 32;

struct LeafParameters {
    f32 scale;
    f32 scaleRandom;
    f32 distance;
    f32 distanceRandom;
    f32 controlDistance;
    f32 controlDistanceRandom;
    f32 fall;
    f32 speed;
    f32 initialDistance;
    f32 initialDistanceRandom;
    u32 leafCount;
    f32 fadeTime;
    u32 reserved;
    u32 modelSize;
    void* modelArchive;
    u32 reservedTail;
};

inline void randomRotation(Quaternion* rotation)
{
    f32 sinLongitude;
    f32 sinLatitude;
    f32 longitude = 6.2831855f * HSD_Randf();
    f32 latitude = 3.1415927f * HSD_Randf();
    sinLatitude = sin(latitude);
    GSvec axis;
    sinLongitude = sin(longitude);
    f32 cosLatitude = cos(latitude);
    f32 cosLongitude = cos(longitude);
    axis.set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
    C_QUATRotAxisRad(rotation, (Vec*)&axis, 6.2831855f * HSD_Randf());
}

}

leafEffect::~leafEffect()
{
    Stop();
    if (mResourceID) {
        GSresFreeResource(leafResourceGroup, mResourceID);
    }
}

void leafEffect::_stop()
{
    if (mLeaves) {
        leafData* leaf = mLeaves;
        for (u32 leafIndex = 0; leafIndex < mLeafCount; leafIndex++, leaf++) {
            if (leaf->model) {
                GSmodelSetVisibility(leaf->model, false);
                GSmodelFree(leaf->model);
                leaf->model = 0;
            }
        }
        u32 count;
        if (mModel) {
            count = mModel->mMaterialCount;
        } else {
            count = 0;
        }
        GSmaterial* material;
        if (mModel) {
            material = mModel->mMaterials;
        } else {
            material = 0;
        }
        for (u32 materialIndex = 0; materialIndex < count; materialIndex++, material++) {
            GSmaterialSetAlpha(material, 1.0f);
        }
        GSmemFree(mLeaves);
    }
}

bool leafEffect::_update(f32 delta)
{
    u32 materialIndex;
    GSmaterial* material;
    if (!mModel) {
        return false;
    }
    mTime += delta;
    if (mTime >= mDuration) {
        return false;
    }
    f32 step = mSpeed * (u32)(delta * GSgfxVideoGetVsyncRate());
    if (mTime < mFadeTime) {
        step *= 2.0f - mTime / mFadeTime;
    } else {
        f32 alpha = 1.0f - (mTime - mFadeTime) / (mDuration - mFadeTime);
        u32 count;
        if (mModel) {
            count = mModel->mMaterialCount;
        } else {
            count = 0;
        }
        if (mModel) {
            material = mModel->mMaterials;
        } else {
            material = 0;
        }
        for (materialIndex = 0; materialIndex < count; materialIndex++, material++) {
            GSmaterialSetAlpha(material, alpha);
        }
    }
    u32 leafIndex;
    leafData* leaf = mLeaves;
    for (leafIndex = 0; leafIndex < mLeafCount; leafIndex++, leaf++) {
        if (leaf->model) {
            leaf->time += step;
            if (leaf->time >= 1.0f) {
                UpdateLeafData(leaf);
            }
            GSvec position;
            GSbezierCalculateVector(&position, leaf->points, leaf->time);
            if (position.y < 0.0f) {
                GSmodelSetVisibility(leaf->model, false);
                GSmodelFree(leaf->model);
                leaf->model = 0;
            } else {
                GSmodel* model = leaf->model;
                f32 scale = leaf->scale;
                Quaternion rotation;
                GSmtx matrix;
                C_QUATSlerp(&leaf->rotationStart, &leaf->rotationEnd, &rotation, leaf->time);
                PSMTXQuat(matrix.m, &rotation);
                PSMTXScaleApply(matrix.m, matrix.m, scale, scale, scale);
                PSMTXTransApply(matrix.m, matrix.m, position.x, position.y, position.z);
                GSmodelSetMatrix(model, &matrix);
            }
        }
    }
    return true;
}


bool leafEffect::_start()
{
    if (mLeafCount > maxLeaves) {
        mLeafCount = maxLeaves;
    }
    if (!mModel) {
        return false;
    }
    mLeaves = (leafData*)GSmemAllocClr(mLeafCount * sizeof(leafData));
    if (!mLeaves) {
        return false;
    }
    leafData* leaf = mLeaves;
    mTime = 0.0f;
    for (u32 i = 0; i < mLeafCount; i++, leaf++) {
        GenerateLeafData(leaf);
    }
    return true;
}

leafEffect::leafEffect(u8** data, f32 duration)
{
    mScale = 1.0f;
    mTime = 0.0f;
    mDuration = duration;
    mPosition.set(0.0f, 0.0f, 0.0f);
    LeafParameters* parameters = (LeafParameters*)*data;
    mLeaves = 0;
    mLeafScale = parameters->scale;
    mLeafScaleRandom = parameters->scaleRandom;
    mDistance = parameters->distance;
    mDistanceRandom = parameters->distanceRandom;
    mControlDistance = parameters->controlDistance;
    mControlDistanceRandom = parameters->controlDistanceRandom;
    mFall = parameters->fall;
    mSpeed = parameters->speed;
    mInitialDistance = parameters->initialDistance;
    mInitialDistanceRandom = parameters->initialDistanceRandom;
    mLeafCount = (u16)parameters->leafCount;
    mFadeTime = parameters->fadeTime;
    *data += sizeof(LeafParameters);
    *data = (u8*)(((u32)*data + 31) & ~31);
    mResourceID = 0;
    void* modelArchive = parameters->modelArchive;
    if (modelArchive) {
        mResourceID = wazaSequenceSysGetResID();
        loadModel(modelArchive, leafResourceGroup, 0, mResourceID);
        GSresIsUniqueID(leafResourceGroup, mResourceID, false);
        mModel = (GSmodel*)GSresGetResource(leafResourceGroup, mResourceID);
        if (mModel) {
            GSmodelSetVisibility(mModel, false);
        }
    }
    *data += (parameters->modelSize + 31) & ~31;
}

void leafEffect::UpdateLeafData(leafData* leaf)
{
    leaf->time -= 1.0f;
    leaf->rotationStart = leaf->rotationEnd;
    randomRotation(&leaf->rotationEnd);
    leaf->points[0] = leaf->points[1];
    leaf->points[2] = leaf->points[3];
    GSvec* point = &leaf->points[2];
    PSVECSubtract((Vec*)point, (Vec*)&leaf->points[1], (Vec*)point);
    PSVECNormalize((Vec*)point, (Vec*)point);
    f32 distance = mControlDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mControlDistance;
    distance *= leaf->scale;
    PSVECScale((Vec*)point, (Vec*)point, -distance);
    PSVECAdd((Vec*)point, (Vec*)&leaf->points[0], (Vec*)point);
    point = &leaf->points[1];
    {
        f32 sinLatitude;
        f32 sinLongitude;
        f32 latitude;
        f32 distance;
        f32 longitude;
        f32 cosLatitude;
        f32 cosLongitude;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 3.1415927f * HSD_Randf();
        sinLatitude = sin(latitude);
        distance = mDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mDistance;
        distance *= leaf->scale;
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        point->set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        PSVECScale((Vec*)point, (Vec*)point, distance);
        point->y += mFall;
        PSVECAdd((Vec*)point, (Vec*)&leaf->points[0], (Vec*)point);
    }
    point = &leaf->points[3];
    {
        f32 longitude;
        f32 sinLatitude;
        f32 sinLongitude;
        f32 latitude;
        f32 distance;
        f32 cosLatitude;
        f32 cosLongitude;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 3.1415927f * HSD_Randf();
        sinLatitude = sin(latitude);
        distance = mControlDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mControlDistance;
        distance *= leaf->scale;
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        point->set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        PSVECScale((Vec*)point, (Vec*)point, distance);
        PSVECAdd((Vec*)point, (Vec*)&leaf->points[1], (Vec*)point);
    }
}

bool leafEffect::GenerateLeafData(leafData* leaf)
{
    leaf->model = GSmodelClone(mModel);
    if (!leaf->model) {
        GSlogWrite("_leaffxGenerateLeafData: Could not create leaf clone model.\n");
        return false;
    }
    GSmodelSetVisibility(leaf->model, true);
    leaf->scale = mLeafScaleRandom * (2.0f * HSD_Randf() - 1.0f) + mLeafScale;
    leaf->scale *= mScale;
    leaf->time = 0.0f;
    {
        f32 latitude;
        f32 longitude;
        f32 sinLatitude;
        f32 sinLongitude;
        f32 cosLatitude;
        f32 cosLongitude;
        GSvec axis;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 3.1415927f * HSD_Randf();
        sinLatitude = sin(latitude);
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        axis.set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        C_QUATRotAxisRad(&leaf->rotationStart, (Vec*)&axis, 6.2831855f * HSD_Randf());
    }
    {
        f32 sinLatitude;
        f32 sinLongitude;
        f32 latitude;
        f32 longitude;
        f32 cosLatitude;
        f32 cosLongitude;
        GSvec axis;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 3.1415927f * HSD_Randf();
        sinLatitude = sin(latitude);
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        axis.set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        C_QUATRotAxisRad(&leaf->rotationEnd, (Vec*)&axis, 6.2831855f * HSD_Randf());
    }
    leaf->points[0] = mPosition;
    GSvec* point;
    point = &leaf->points[2];
    {
        f32 sinLatitude;
        f32 sinLongitude;
        f32 latitude;
        f32 distance;
        f32 longitude;
        f32 cosLatitude;
        f32 cosLongitude;
        distance = mControlDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mControlDistance;
        distance *= leaf->scale;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 3.1415927f * HSD_Randf();
        sinLatitude = sin(latitude);
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        point->set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        PSVECScale((Vec*)point, (Vec*)point, distance);
        PSVECAdd((Vec*)point, (Vec*)&mPosition, (Vec*)point);
    }
    point = &leaf->points[1];
    {
        f32 longitude;
        f32 sinLatitude;
        f32 sinLongitude;
        f32 latitude;
        f32 distance;
        f32 cosLatitude;
        f32 cosLongitude;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 1.5707964f * HSD_Randf();
        sinLatitude = sin(latitude);
        distance = mDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mDistance;
        distance *= mInitialDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mInitialDistance;
        distance *= leaf->scale;
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        point->set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        PSVECScale((Vec*)point, (Vec*)point, distance);
        PSVECAdd((Vec*)point, (Vec*)&mPosition, (Vec*)point);
    }
    point = &leaf->points[3];
    {
        f32 longitude;
        f32 sinLatitude;
        f32 sinLongitude;
        f32 latitude;
        f32 distance;
        f32 cosLatitude;
        f32 cosLongitude;
        longitude = 6.2831855f * HSD_Randf();
        latitude = 3.1415927f * HSD_Randf();
        sinLatitude = sin(latitude);
        distance = mControlDistanceRandom * (2.0f * HSD_Randf() - 1.0f) + mControlDistance;
        distance *= leaf->scale;
        sinLongitude = sin(longitude);
        cosLatitude = cos(latitude);
        cosLongitude = cos(longitude);
        point->set(sinLatitude * cosLongitude, cosLatitude, sinLatitude * sinLongitude);
        PSVECScale((Vec*)point, (Vec*)point, distance);
        PSVECAdd((Vec*)point, (Vec*)&leaf->points[1], (Vec*)point);
    }
    GSmtx matrix;
    PSMTXQuat(matrix.m, &leaf->rotationStart);
    PSMTXScaleApply(matrix.m, matrix.m, leaf->scale, leaf->scale, leaf->scale);
    PSMTXTransApply(matrix.m, matrix.m, mPosition.x, mPosition.y, mPosition.z);
    GSmodelSetMatrix(leaf->model, &matrix);
    return true;
}
