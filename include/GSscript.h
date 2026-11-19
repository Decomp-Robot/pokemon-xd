#ifndef GS_SCRIPT_H
#define GS_SCRIPT_H

#include <types.h>
#include <GSgfx.h>
#include <GSres.h>
#include <GSBinStream.h>
#include <GSAPI/GSscript/chank.h>

class NullSequence;
class WazaSequence;

struct ScriptResultContext;
struct GSthread;
struct TPCCode {
    u8 opcode;
    u8 operand;
    u16 immediate;
};

namespace GSscript {
void tigaErrMessage(const char*);
class tigaArrayFixed;
class tigaCellMemory;
class Tiga;
class TigaObjManager {
public:
    GSresID* getResource(int);
    int getResourceIdx(GSresID*);
    GSresID* newResource();
    u8 mVectorReferences[128];
    u8 mResourceReferences[64];
    GSvec mVectors[128];
    GSresID mResources[64];
};
}


namespace GSscript {
enum VariantKind {
    VariantEmpty = 0,
    VariantInteger = 1,
    VariantSingle = 2,
    VariantString = 3,
    VariantVector = 4,
    VariantArray = 7,
    VariantFunctionPointer = 8,
    VariantResource = 35,
    VariantPeople = 0x23,
    VariantPokemon = 0x25,
    VariantWaza = 0x3a
};
struct tigaWaza {
    NullSequence* sequence;
    WazaSequence* wazaSequence;
};
class tigaArrayFixed;
struct tigaVariant {
    s16 kind;
    s16 reserved;
    union {
        long value;
        float single;
        char* string;
        GSvec* vector;
        tigaWaza* waza;
        void* object;
        GSresID* people;
        GSresID* resource;
        tigaArrayFixed* array;
    };
    long getInteger()
    {
        if (kind == VariantInteger)
            return value;
        if (kind == VariantSingle)
            return (long)single;
        return getCnvInteger();
    }
    float getSingle()
    {
        if (kind == VariantInteger)
            return (float)value;
        if (kind == VariantSingle)
            return single;
        return getCnvSingle();
    }
    void setInteger(long integer)
    {
        kind = VariantInteger;
        value = integer;
    }
    GSvec* getVector()
    {
        if (kind == VariantVector)
            return vector;
        return getCnvVector();
    }
    GSresID* getPeople()
    {
        if (kind == VariantPeople)
            return people;
        tigaErrMessage("people not access");
        return 0;
    }
    tigaArrayFixed* getArray()
    {
        if (kind == VariantArray)
            return array;
        tigaErrMessage("pokemon not access");
        return 0;
    }
    void setPeople(GSresID* people)
    {
        kind = VariantPeople;
        this->people = people;
    }
    void setSingle(float single)
    {
        kind = VariantSingle;
        this->single = single;
    }
    long getCnvInteger();
    float getCnvSingle();
    GSvec* getCnvVector();
    char* getCnvString(char* buffer);

    void restoreStateData(GSBinReadStream* stream);
    void makeStateData(GSBinWriteStream* stream);
};
class TigaThread {
public:
    u32 mState;
    u8 mUnknownStateAfterState[0x148];
    int lineNumber;
    u8 mUnknownStateAfterLineNumber[0x18];
    u8 mWaitFlagKind;
    u8 mWaitFlagIndex;
    u8 mUnknownWaitFlag[2];
    long mWaitFlagValue;
    Tiga* mTiga;
    long mHandle;
    u32 mFloorGroupId;
    GSthread* mThread;

    void initTiga();
    bool restoreStateData(GSBinReadStream* stream);
    void makeStateData(GSBinWriteStream* stream);
    void setSaveFlag(bool flag);
};
class Tiga {
public:
    enum { LocalFlagCount = 16 };
    Tiga();
    CChankBlock mChank;
    u32 mChankSize;
    u32 mChankVersion;
    TigaObjManager mObjectManager;
    u32 mStateDataId;
    u32 mRunThreadCount;
    f32 mTimer[4];
    TigaThread* mThreadSlots[8];
    tigaCellMemory* mCellMemory;
    ScriptResultContext* mResultContext;
    u8 mLocalFlags[LocalFlagCount];
    int mNextThreadHandle;
    bool mSaveFlag;
    u32 mUnknownStateAfterSaveFlag;
    int mLiveWaza;
    u8 mUnknownStateAfterCounter[0xC];
    TPCCode* mCodeSection;
    int mVariantCount;
    tigaVariant* mVariants;
    u8 mUnknownStateAfterVariants[0x4];
    char* mStringBuffer;
    u8 mUnknownStateAfterStringBuffer[0xc];
    GSresID* mPeople;
    int mArrayCount;
    tigaArrayFixed** mArrays;
    u8 mUnknownStateAfterArrays[0x10];
    TigaThread* mCurrentThread;

    unsigned long getStateSize();
    bool restoreStateData(void* data, unsigned long size);
    void makeStateData(GSBinWriteStream* stream);

    bool isMapping();
    int getNameToFuncId(const char* name);
    GSresID* searchResID(unsigned long groupID, unsigned long resourceID);
    long runParams(int functionID, tigaVariant* parameters, int count, bool newThread);
    long getResultValue(long threadHandle);
    bool isRunningThread(long threadHandle);
    void blockThread(long threadHandle);
    void unblockThread(long threadHandle);
    void setRpVec(const GSvec*);
    bool runParamsWait(int, tigaVariant*, int, tigaVariant*);
    int cmpArray(int, tigaVariant*, tigaVariant*);
    int cmpVector(int, tigaVariant*, tigaVariant*);
    bool cmpThread(int, tigaVariant*, tigaVariant*);
    bool makeThread(TPCCode*, int&, long&);
    void deleteThread(long);
    virtual int execExtendMethod(int, int, tigaVariant*, tigaVariant*);
    virtual int execExtendGlobalFunction(int, tigaVariant*, tigaVariant*);
    virtual int getExtendVariable(tigaVariant*, int, int);
    virtual int setExtendVariable(tigaVariant*, int, int);
    virtual void mapping();
    virtual void unmapping();
    virtual ~Tiga();
    static Tiga* m_cpCurrentObject;
};
}


struct ScriptResultContext {
    enum { StringBufferSize = 256 };
    u8 executionState[0x10];
    GSscript::tigaVariant result;
    u8 variableState[0x20];
    char stringBuffer[StringBufferSize];
    GSvec vectorResult;
};

namespace GSscript {
inline void Tiga::setRpVec(const GSvec* vector)
{
    mResultContext->vectorResult = *vector;
    mResultContext->result.vector = &mResultContext->vectorResult;
    mResultContext->result.kind = VariantVector;
}

class tigaArrayFixed {
public:
    int mLength;
    unsigned int mCursor;
    bool mOmitStateData;
    u8 mUnknown[7];
    tigaVariant mValues[1];

    tigaVariant* get(int);
    void set(int, const tigaVariant*);
    void push_back(tigaVariant*);
    void resize(int);
    tigaVariant* fe_get();
};
class tigaCellMemory {
public:
    int mStringCount;
    u32 mUnknown;

    tigaCellMemory();
    virtual ~tigaCellMemory();
    void freeVariant(tigaVariant*);
    void dupVariant(tigaVariant*);
};
}


#endif
