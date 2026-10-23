#ifndef GS_SCRIPT_H
#define GS_SCRIPT_H

#include <types.h>
#include <GSgfx.h>
#include <GSres.h>

class NullSequence;
struct WazaSequence;

struct ScriptResultContext;

namespace GSscript {
void tigaErrMessage(const char*);
class tigaArrayFixed;
class tigaCellMemory;
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
    long getCnvInteger();
    float getCnvSingle();
    GSvec* getCnvVector();
    char* getCnvString(char* buffer);
};
class TigaThread {
public:
    u8 unknownState[0x14c];
    int lineNumber;
};
class Tiga {
public:
    Tiga();
    u8 mUnknown[0x18];
    TigaObjManager mObjectManager;
    u8 mUnknownState[0x38];
    tigaCellMemory* mCellMemory;
    ScriptResultContext* mResultContext;
    u8 mUnknownStateAfterContext[0x1c];
    int mLiveWaza;
    u8 mUnknownStateAfterCounter[0x1c];
    char* mStringBuffer;
    u8 mUnknownStateAfterStringBuffer[0xc];
    GSresID* mPeople;
    int mArrayCount;
    tigaArrayFixed** mArrays;
    u8 mUnknownStateAfterArrays[0x10];
    TigaThread* mCurrentThread;

    void setRpVec(const GSvec*);
    int runParamsWait(int, tigaVariant*, int, tigaVariant*);
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
    u8 executionState[0x10];
    GSscript::tigaVariant result;
    u8 variableState[0x120];
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
    u8 mUnknown[8];
    tigaVariant mValues[1];

    tigaVariant* get(int);
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
