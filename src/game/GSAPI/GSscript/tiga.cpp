#include <GSscript.h>
#include <GSBinStreamImpl.h>
#include <GSmemory.h>
#include <floor.h>
#include <floorManager.h>
#include <GSthread.h>
#include <string.h>


namespace GSscript {

void _executeScriptThread(Tiga* tiga, long handle);

Tiga* Tiga::m_cpCurrentObject = 0;

namespace {

class CurrentObjectScope {
public:
    explicit CurrentObjectScope(Tiga* current)
        : m_pPrevious(Tiga::m_cpCurrentObject)
    {
        Tiga::m_cpCurrentObject = current;
    }

    ~CurrentObjectScope()
    {
        Tiga::m_cpCurrentObject = m_pPrevious;
    }

private:
    Tiga* m_pPrevious;
};

}

unsigned long Tiga::getStateSize()
{
    GSBinWriteSizeStream stream;
    makeStateData(&stream);
    return stream.getWriteSize();
}

bool Tiga::restoreStateData(void* data, unsigned long size)
{
    struct ThreadRestoreState {
        long handle;
        int threadCount;
        int savedThreadCount;
    } th;
    int i;
    int j;
    struct FloatBits { union { u32 word; float value; }; u32 pad; } conv;
    GSBinReadStream stream(data, size);
    CurrentObjectScope scope(this);
    if (size == 0) {
        return false;
    }

    if (stream.readU32() != 0) {
        return false;
    }

    mStateDataId = stream.readU32();

    u32 version = stream.readU32();
    if (version != mChankVersion) {
        if (version != 0 || mChankVersion != 0) {
            return false;
        }
    }

    u32 chankSize = stream.readU32();
    if (chankSize != mChankSize) {
        return false;
    }

    mRunThreadCount = stream.readU32();
    for (i = 0; i < 4; i++) {
        conv.word = stream.readU32();
        mTimer[i] = conv.value;
    }
    for (i = 0; i < LocalFlagCount; i++) {
        mLocalFlags[i] = stream.readU8();
    }

    mNextThreadHandle = stream.readU32();
    mSaveFlag = stream.readBool();
    mUnknownStateAfterSaveFlag = stream.readU32();

    mVariantCount = stream.readU32();
    for (i = 0; i < mVariantCount; i++) {
        mVariants[i].restoreStateData(&stream);
    }

    mArrayCount = stream.readU32();
    for (i = 0; i < mArrayCount; i++) {
        /* the saved array flag and length are not restored */
        stream.skip(1);
        if (mArrays[i]->mOmitStateData == 0) {
            stream.skip(4);
            for (j = 0; j < mArrays[i]->mLength; j++) {
                mArrays[i]->get(j)->restoreStateData(&stream);
            }
        }
    }

    th.threadCount = 8;
    th.savedThreadCount = stream.readU32();
    if (th.savedThreadCount < th.threadCount) {
        th.threadCount = th.savedThreadCount;
    }

    for (i = 0; i < th.threadCount; i++) {
        mThreadSlots[i]->initTiga();
        mThreadSlots[i]->restoreStateData(&stream);
        if (mThreadSlots[i]->mState != 0) {
            mThreadSlots[i]->mHandle = mNextThreadHandle++;
            mThreadSlots[i]->mFloorGroupId = floorGetCurrentFloorGroupID();
            if (mNextThreadHandle == 0) {
                mNextThreadHandle = 1;
            }
        }
    }

    for (i = 0; i < th.threadCount; i++) {
        if (mThreadSlots[i]->mState != 0) {
            th.handle = mThreadSlots[i]->mHandle;
            GSthread* thread = floorCreateThreadLocal(
                1, 8192, 1, (GSthreadEntry)_executeScriptThread);
            GSthreadSetName(thread, "GSscript thread");
            GSthreadSetArgs(thread, 2, this, th.handle);
            mThreadSlots[i]->mThread = thread;
        }
    }

    return true;
}

void Tiga::makeStateData(GSBinWriteStream* stream)
{
    int i;
    int j;
    stream->writeU32(0);
    stream->writeU32(mStateDataId);
    stream->writeU32(mChankVersion);
    stream->writeU32(mChankSize);
    stream->writeU32(mRunThreadCount);

    for (i = 0; i < 4; i++) {
        stream->writeFloat(mTimer[i]);
    }
    for (i = 0; i < LocalFlagCount; i++) {
        stream->writeU8(mLocalFlags[i]);
    }

    stream->writeU32(mNextThreadHandle);
    stream->writeBool(mSaveFlag);
    stream->writeU32(mUnknownStateAfterSaveFlag);

    stream->writeU32(mVariantCount);
    for (i = 0; i < mVariantCount; i++) {
        mVariants[i].makeStateData(stream);
    }

    stream->writeU32(mArrayCount);
    for (i = 0; i < mArrayCount; i++) {
        stream->writeBool(mArrays[i]->mOmitStateData);
        if (mArrays[i]->mOmitStateData == 0) {
            stream->writeU32(mArrays[i]->mLength);
            for (j = 0; j < mArrays[i]->mLength; j++) {
                mArrays[i]->get(j)->makeStateData(stream);
            }
        }
    }

    stream->writeU32(8);
    for (i = 0; i < 8; i++) {
        mThreadSlots[i]->makeStateData(stream);
    }
}

int Tiga::getNameToFuncId(const char* name)
{
    enum { FunctionIdTag = 0x01000000 };
    int id = 0;
    if (mCodeCount == 0) {
        return 0;
    }
    TPCFunc* func = mFuncTable;
    while (func != &mFuncTable[mFuncCount]) {
        if (strcmp(func->name, name) == 0) {
            return FunctionIdTag | id;
        }
        func++;
        id++;
    }
    return 0;
}

long Tiga::getResultValue(long threadHandle)
{
    if (mResultVariant != 0
        && (mResultVariant->kind == VariantInteger || mResultVariant->kind == VariantSingle)) {
        return mResultVariant->getInteger();
    }
    return 0;
}

TPCCode* Tiga::getIdToCodeAdress(int id)
{
    int index = id & 0xFFFF;
    if (index < 0 || index > mFuncCount) {
        return 0;
    }
    return mFuncTable[index].code;
}

GSresID* Tiga::searchResID(unsigned long groupID, unsigned long resourceID)
{
    GSresID* end = mPeople + mPeopleCount;
    GSresID* res = mPeople;
    while (res != end) {
        if (res->groupId == groupID && res->resourceId == resourceID) {
            return res;
        }
        res++;
    }
    return 0;
}

void Tiga::setPresetVar(tigaVariant* variant, int variable, int index)
{
    setExtendVariable(variant, variable, index);
}

void Tiga::getPresetVar(tigaVariant* variant, int variable, int index)
{
    enum { VariableThreadHandle = 5 };
    variant->kind = VariantEmpty;
    switch (variable) {
    case VariableThreadHandle:
        // The current thread reports its own handle.
        variant->kind = VariantInteger;
        variant->value = mCurrentThread != 0 ? mCurrentThread->mHandle : 0;
        break;
    default:
        getExtendVariable(variant, variable, index);
        break;
    }
}

}
