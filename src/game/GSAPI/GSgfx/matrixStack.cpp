#include <GSgfx.h>
#include <GSmem.h>

void GSgfxMatrixStack::pop(void)
{
    --mCurrentEntry;
}

void GSgfxMatrixStack::pushMult(GSmtx *matrix)
{
    ++mCurrentEntry;
    PSMTXConcat((mCurrentEntry - 1)->mMatrix.m, matrix->m,
                mCurrentEntry->mMatrix.m);
    mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
}

void GSgfxMatrixStack::push(GSmtx *matrix)
{
    ++mCurrentEntry;
    if (matrix == 0) {
        PSMTXCopy((mCurrentEntry - 1)->mMatrix.m, mCurrentEntry->mMatrix.m);
    } else {
        PSMTXCopy(matrix->m, mCurrentEntry->mMatrix.m);
    }
    mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
}

GSgfxMatrixStack::~GSgfxMatrixStack()
{
    GSmemFree(mEntries);
    mEntries = 0;
}

GSgfxMatrixStack::GSgfxMatrixStack(u32 capacity)
{
    mEntries = (GSgfxMatrixStackEntry *)GSmemAlloc(capacity * sizeof(GSgfxMatrixStackEntry));
    mCurrentEntry = mEntries;
    mLastEntry = mEntries + (capacity - 1);
    identity();
}
