#include <GSmodelInternal.h>

enum {
    MODEL_BANK_CAPACITY = 8,
    MODEL_BANK_LINKED = 0x1,
    MODEL_BANK_EMPTY = 0x2,
    MODEL_BANK_FULL = 0x4
};

struct GSmodelBank {
    u32 mFlags;
    GSmodelBank *mNext;
    _GSmodel mModels[MODEL_BANK_CAPACITY];
};

GSmodelBank *_modelBankList;
_GSmodel *_modelTopPtr;

static inline GSmodelBank *modelBankCreate(void)
{
    GSmodelBank *bank = (GSmodelBank *)GSmemAllocClr(sizeof(GSmodelBank));
    if (bank == 0)
        return 0;

    bank->mFlags = MODEL_BANK_EMPTY;
    bank->mNext = 0;
    for (s32 i = 0; i < MODEL_BANK_CAPACITY; i++) {
        bank->mModels[i].mFlags = 0;
        bank->mModels[i].mRenderToken = 0;
        bank->mModels[i].mNext = 0;
    }
    return bank;
}

static inline u8 modelBankIsLinked(GSmodelBank *bank)
{
    for (GSmodelBank *entry = _modelBankList; entry; entry = entry->mNext) {
        if (entry == bank)
            return 1;
    }
    return 0;
}

static inline void modelBankLink(GSmodelBank *bank)
{
    if (modelBankIsLinked(bank) == 1)
        return;
    bank->mNext = _modelBankList;
    _modelBankList = bank;
    bank->mFlags |= MODEL_BANK_LINKED;
}

static inline void modelBankUnlink(GSmodelBank *bank)
{
    if (modelBankIsLinked(bank)) {
        GSmodelBank *entry = _modelBankList;
        if (entry == bank) {
            bank->mFlags &= ~MODEL_BANK_LINKED;
            _modelBankList = bank->mNext;
        } else {
            for (; entry; entry = entry->mNext) {
                if (entry->mNext == bank) {
                    entry->mNext = bank->mNext;
                    bank->mFlags &= ~MODEL_BANK_LINKED;
                    break;
                }
            }
        }
    }
}

static inline GSmodelBank *modelBankFindModel(_GSmodel *model)
{
    for (GSmodelBank *bank = _modelBankList; bank; bank = bank->mNext) {
        if (!(bank->mFlags & MODEL_BANK_EMPTY)) {
            for (s32 i = 0; i < MODEL_BANK_CAPACITY; i++) {
                if (&bank->mModels[i] == model)
                    return bank;
            }
        }
    }
    return 0;
}

static inline _GSmodel *modelBankFindFreeModel(GSmodelBank *bank)
{
    for (s32 i = 0; i < MODEL_BANK_CAPACITY; i++) {
        _GSmodel *model = &bank->mModels[i];
        if (!(model->mFlags & MODEL_FLAG_RENDER_ENABLED))
            return model;
    }
    return 0;
}

static inline void modelBankUpdateFlags(GSmodelBank *bank)
{
    s32 used = 0;
    s32 free = 0;
    bank->mFlags &= ~(MODEL_BANK_EMPTY | MODEL_BANK_FULL);
    for (s32 i = 0; i < MODEL_BANK_CAPACITY; i++) {
        if (bank->mModels[i].mFlags & MODEL_FLAG_RENDER_ENABLED)
            used++;
        if (!(bank->mModels[i].mFlags & MODEL_FLAG_RENDER_ENABLED))
            free++;
    }
    if (used == MODEL_BANK_CAPACITY)
        bank->mFlags |= MODEL_BANK_FULL;
    else if (free == MODEL_BANK_CAPACITY)
        bank->mFlags |= MODEL_BANK_EMPTY;
}

void modelBankReleaseModel(_GSmodel *model)
{
    GSmodelBank *bank = modelBankFindModel(model);
    if (_modelTopPtr == model) {
        _modelTopPtr = model->mNext;
    } else {
        _GSmodel *entry = _modelTopPtr;
        for (; entry && entry->mNext != model; entry = entry->mNext) {}
        entry->mNext = model->mNext;
    }
    model->mFlags = 0;
    model->mNext = 0;
    modelBankUpdateFlags(bank);

    for (bank = _modelBankList; bank; bank = bank->mNext) {
        if ((bank->mFlags & MODEL_BANK_EMPTY) && bank->mNext) {
            modelBankUnlink(bank);
            GSmemFree(bank);
            break;
        }
    }
}

_GSmodel *modelBankGetFreeModel(void)
{
    GSmodelBank *bank = _modelBankList;
    for (; bank && (bank->mFlags & MODEL_BANK_FULL); bank = bank->mNext) {}
    if (!bank) {
        bank = modelBankCreate();
        if (!bank)
            return 0;
        modelBankLink(bank);
    }
    _GSmodel *model = modelBankFindFreeModel(bank);
    memset(model, 0, sizeof(_GSmodel));
    model->mFlags = MODEL_FLAG_RENDER_ENABLED;
    modelBankUpdateFlags(bank);
    model->mNext = 0;
    if (!_modelTopPtr) {
        _modelTopPtr = model;
    } else {
        _GSmodel *entry = _modelTopPtr;
        for (; entry->mNext; entry = entry->mNext) {}
        entry->mNext = model;
    }
    return model;
}

void modelBankInit(void)
{
    modelBankLink(modelBankCreate());
}
