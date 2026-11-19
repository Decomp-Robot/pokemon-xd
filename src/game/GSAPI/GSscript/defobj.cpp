#include <GSscript.h>
#include <GSAPI/GSvec/GSvec.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

namespace GSscript {

enum ArrayOperation {
    ArrayOpWithoutReceiver = 1,
    ArrayOpFormat = 3,
    ArrayOpGet = 16,
    ArrayOpSet = 17,
    ArrayOpLength = 18,
    ArrayOpResize = 19,
    ArrayOpAppend = 20,
    ArrayOpResetCursor = 22,
    ArrayOpNext = 23,
    ArrayOpAtEnd = 24,
    ArrayOpAppendMany = 25,
};

static inline void setStringResult(Tiga* tiga, const char* text)
{
    strncpy(tiga->mResultContext->stringBuffer, text, sizeof(tiga->mResultContext->stringBuffer) - 1);
    tiga->mResultContext->result.string = tiga->mResultContext->stringBuffer;
    tiga->mResultContext->result.kind = VariantString;
}

static inline void setArrayElementResult(Tiga* tiga, tigaVariant* result, const tigaVariant* elem)
{
    if (elem->kind == VariantString) {
        setStringResult(tiga, elem->string);
    } else if (elem->kind == VariantVector) {
        tiga->setRpVec(elem->vector);
    } else {
        *result = *elem;
    }
}

int Tiga::cmpArray(int op, tigaVariant* args, tigaVariant* result)
{
    tigaArrayFixed* array = 0;
    if (op != ArrayOpWithoutReceiver) {
        array = args[0].array;
        if (args[0].kind != VariantArray || array == 0)
            return 0;
    }
    switch (op) {
    case 0:
    case ArrayOpWithoutReceiver:
    case 2:
        break;
    case ArrayOpFormat: {
        char buffer[ScriptResultContext::StringBufferSize];
        sprintf(buffer, "tarray: size=%d", array->mLength);
        setStringResult(this, buffer);
        break;
    }
    case ArrayOpGet: {
        int length = array->mLength;
        long index = args[1].getInteger();
        if (index >= length) {
            result->setInteger(0);
        } else {
            tigaVariant* elem = array->get(index);
            setArrayElementResult(this, result, elem);
        }
        break;
    }
    case ArrayOpSet:
        array->set(args[1].getInteger(), &args[2]);
        break;
    case ArrayOpLength:
        result->setInteger(args[0].array->mLength);
        break;
    case ArrayOpResize:
        array->resize(args[1].getInteger());
        break;
    case ArrayOpAppendMany: {
        long count = args[1].getInteger();
        tigaVariant* element = args;
        for (long i = 0; i < count; element++, i++)
            array->push_back(&element[2]);
        break;
    }
    case ArrayOpAppend:
        array->push_back(&args[1]);
        break;
    case ArrayOpResetCursor:
        array->mCursor = 0;
        break;
    case ArrayOpNext: {
        tigaVariant* elem = array->fe_get();
        setArrayElementResult(this, result, elem);
        break;
    }
    case ArrayOpAtEnd:
        result->kind = VariantInteger;
        if (array->mLength <= (int)array->mCursor)
            result->value = 1;
        else
            result->value = 0;
        break;
    case 26:
        break;
    }
    return 0;
}

static inline void assignVector(tigaVariant* dest, GSvec* source)
{
    if (dest->kind == VariantInteger || dest->kind == VariantSingle) {
        dest->kind = VariantEmpty;
    } else {
        Tiga::m_cpCurrentObject->mCellMemory->freeVariant(dest);
        dest->kind = VariantEmpty;
    }
    dest->kind = VariantVector;
    dest->reserved = 0;
    dest->vector = source;
    if (dest->kind != VariantInteger && dest->kind != VariantSingle)
        Tiga::m_cpCurrentObject->mCellMemory->dupVariant(dest);
}

static inline void setVectorComponents(GSvec* vector, tigaVariant* components)
{
    vector->x = components[0].getSingle();
    vector->y = components[1].getSingle();
    vector->z = components[2].getSingle();
}

enum VectorOperation {
    VectorOpWithoutReceiver = 1,
    VectorOpFormat = 3,
    VectorOpZero = 16,
    VectorOpNormalize = 17,
    VectorOpSetComponents = 18,
    VectorOpSetComponentsAlias = 19,
    VectorOpFill = 20,
    VectorOpAbs = 21,
    VectorOpNegate = 22,
    VectorOpIsNearZero = 23,
    VectorOpCross = 24,
    VectorOpDot = 25,
    VectorOpMagnitude = 26,
    VectorOpSquareMagnitude = 27,
    VectorOpAngle = 28,
};

int Tiga::cmpVector(int op, tigaVariant* args, tigaVariant* result)
{
    GSvec zeroVector(0.0f, 0.0f, 0.0f);
    GSvec* v = 0;
    if (op != VectorOpWithoutReceiver) {
        if (args->kind != VariantVector) {
            assignVector(args, &zeroVector);
        }
        v = args->vector;
    }
    switch (op) {
    case VectorOpFormat: {
        char buffer[ScriptResultContext::StringBufferSize];
        sprintf(buffer, "<%.3f,%.3f,%.3f>", v->x, v->y, v->z);
        setStringResult(this, buffer);
        break;
    }
    case VectorOpZero:
        v->x = v->y = v->z = 0.0f;
        break;
    case VectorOpNormalize:
        PSVECNormalize(v, v);
        break;
    case VectorOpSetComponents:
        setVectorComponents(v, &args[1]);
        break;
    case VectorOpSetComponentsAlias:
        setVectorComponents(v, &args[1]);
        break;
    case VectorOpFill:
        v->x = v->y = v->z = args[1].getSingle();
        break;
    case VectorOpAbs:
        v->x = (v->x >= 0.0f) ? v->x : -v->x;
        v->y = (v->y >= 0.0f) ? v->y : -v->y;
        v->z = (v->z >= 0.0f) ? v->z : -v->z;
        break;
    case VectorOpNegate:
        v->x = -v->x;
        v->y = -v->y;
        v->z = -v->z;
        break;
    case VectorOpIsNearZero: {
        const f32 zeroTolerance = 0.00001f;
        bool nearZero;
        if (v->x < zeroTolerance && v->x > -zeroTolerance
            && v->y < zeroTolerance && v->y > -zeroTolerance
            && v->z < zeroTolerance && v->z > -zeroTolerance)
            nearZero = true;
        else
            nearZero = false;
        if (nearZero)
            result->setInteger(1);
        else
            result->setInteger(0);
        break;
    }
    case VectorOpCross:
        if (args[1].kind != VariantVector)
            break;
        PSVECCrossProduct(v, args[1].getVector(), &zeroVector);
        setRpVec(&zeroVector);
        break;
    case VectorOpDot:
        if (args[1].kind == VariantVector)
            result->setSingle(PSVECDotProduct(v, args[1].getVector()));
        else
            result->setSingle(0.0f);
        break;
    case VectorOpMagnitude:
        result->setSingle(PSVECMag(v));
        break;
    case VectorOpSquareMagnitude:
        result->setSingle(PSVECSquareMag(v));
        break;
    case VectorOpAngle: {
        if (args[1].kind == VariantVector) {
            GSvec a;
            GSvec b;
            PSVECNormalize(v, &a);
            PSVECNormalize(args[1].getVector(), &b);
            f32 angle = MTXRadToDeg((f32)acos(PSVECDotProduct(&a, &b)));
            while (angle > 360.0f)
                angle -= 360.0f;
            while (angle < 0.0f)
                angle += 360.0f;
            result->setSingle(angle);
        } else {
            result->setSingle(0.0f);
        }
        break;
    }
    }
    return 0;
}

enum ThreadOperation {
    ThreadOpCreate = 16,
    ThreadOpSetZeroResult = 17,
    ThreadOpCount = 18,
    ThreadOpDelete = 19,
    ThreadOpSetLocalFlag = 20,
    ThreadOpGetLocalFlag = 21,
    ThreadOpWaitUntilFlagBelow = 22,
    ThreadOpWaitUntilFlagEqual = 23,
};

enum FlagWaitKind {
    FlagWaitUntilBelow = 1,
    FlagWaitUntilEqual = 2,
};

static inline bool isLocalFlagIndex(long index)
{
    if (index < Tiga::LocalFlagCount && index >= 0)
        return true;
    else
        return false;
}

static inline void waitForLocalFlag(Tiga* tiga, long index, long value, FlagWaitKind kind)
{
    TigaThread* thread = tiga->mCurrentThread;
    thread->mWaitFlagKind = kind;
    thread->mWaitFlagIndex = index;
    thread->mWaitFlagValue = value;
}

bool Tiga::cmpThread(int op, tigaVariant* args, tigaVariant* result)
{
    bool ret = false;
    int threadIndex = 0;
    long threadHandle;
    switch (op) {
    case ThreadOpCreate: {
        long codeIndex = args[1].getInteger();
        TPCCode* code = &mCodeSection[codeIndex];
        if (makeThread(code, threadIndex, threadHandle))
            result->setInteger(threadHandle);
        break;
    }
    case ThreadOpSetZeroResult:
        result->setInteger(0);
        break;
    case ThreadOpCount:
        result->setInteger(mRunThreadCount);
        break;
    case ThreadOpDelete:
        deleteThread(args[1].getInteger());
        break;
    case ThreadOpSetLocalFlag: {
        u8 value = args[2].getInteger();
        long index = args[1].getInteger();
        if (isLocalFlagIndex(index))
            mLocalFlags[index] = value;
        break;
    }
    case ThreadOpGetLocalFlag: {
        long index = args[1].getInteger();
        bool inRange = isLocalFlagIndex(index);
        u8 value = inRange ? mLocalFlags[index] : 0;
        result->setInteger(value);
        break;
    }
    case ThreadOpWaitUntilFlagBelow: {
        long index = args[1].getInteger();
        if (isLocalFlagIndex(index)) {
            long value = args[2].getInteger();
            long flagIndex = args[1].getInteger();
            waitForLocalFlag(this, flagIndex, value, FlagWaitUntilBelow);
            ret = true;
        }
        break;
    }
    case ThreadOpWaitUntilFlagEqual: {
        long index = args[1].getInteger();
        if (isLocalFlagIndex(index)) {
            long value = args[2].getInteger();
            long flagIndex = args[1].getInteger();
            waitForLocalFlag(this, flagIndex, value, FlagWaitUntilEqual);
            ret = true;
        }
        break;
    }
    case 24:
        break;
    }
    return ret;
}

}
