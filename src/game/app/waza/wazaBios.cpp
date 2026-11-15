#include <waza.h>

extern "C" {

u16 wazaDataBiosGetWazaFuncDataId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mWazaFuncDataId;
}

u8 wazaDataBiosGetRiskFlag(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mRiskFlag;
}

u8 wazaDataBiosGetTypeId(struct WazaData *data, s32 index)
{
    if (data == 0) {
        return 0;
    }
    u16 typeIndex = (u16)index;
    if (typeIndex >= WAZA_DATA_TYPE_ID_COUNT) {
        return 0;
    }
    return data->mTypeId[typeIndex];
}

void wazaDataBiosSetHidenFlag(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mHidenFlag = value;
}

u8 wazaDataBiosGetHidenFlag(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mHidenFlag;
}

void wazaDataBiosSetDoc(struct WazaData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mDoc = value;
}

void wazaDataBiosSetWazawzxdataId(struct WazaData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mWazawzxdataId = value;
}

void wazaDataBiosSetPressure(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mPressure = value;
}

void wazaDataBiosSetBouon(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mBouon = value;
}

void wazaDataBiosSetNegoto(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mNegoto = value;
}

void wazaDataBiosSetNekonote(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mNekonote = value;
}

void wazaDataBiosSetMonomane(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mMonomane = value;
}

void wazaDataBiosSetYubiwohuru(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mYubiwohuru = value;
}

void wazaDataBiosSetOujanosirusi(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mOujanosirusi = value;
}

void wazaDataBiosSetOumugaesi(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mOumugaesi = value;
}

void wazaDataBiosSetYokodori(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mYokodori = value;
}

void wazaDataBiosSetMajikku(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mMajikku = value;
}

void wazaDataBiosSetMamoru(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mMamoru = value;
}

void wazaDataBiosSetDageki(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mDageki = value;
}

void wazaDataBiosSetAddFightKoukaAvg(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mAddFightKoukaAvg = value;
}

void wazaDataBiosSetFightAttackMsgId(struct WazaData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mFightAttackMsgId = value;
}

void wazaDataBiosSetFightAttackTunagiMsgId(struct WazaData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mFightAttackTunagiMsgId = value;
}

void wazaDataBiosSetSeqId(struct WazaData *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mSeqId = value;
}

void wazaDataBiosSetFightKoukaDataId(struct WazaData *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mFightKoukaDataId = value;
}

void wazaDataBiosSetIryoku(struct WazaData *data, s16 value)
{
    if (data == 0) {
        return;
    }
    data->mIryoku = value;
}

void wazaDataBiosSetAvg(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mAvg = value;
}

void wazaDataBiosSetRangeId(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mRangeId = value;
}

void wazaDataBiosSetPri(struct WazaData *data, s8 value)
{
    if (data == 0) {
        return;
    }
    data->mPri = value;
}

void wazaDataBiosSetZokuseiDataId(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mZokuseiDataId = value;
}

void wazaDataBiosSetPp(struct WazaData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mPp = value;
}

void wazaDataBiosSetName(struct WazaData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mNameID = value;
}

u32 wazaDataBiosGetDoc(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDoc;
}

u32 wazaDataBiosGetWazawzxdataId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mWazawzxdataId;
}

u8 wazaDataBiosGetPressure(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPressure;
}

u8 wazaDataBiosGetBouon(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBouon;
}

u8 wazaDataBiosGetNegoto(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNegoto;
}

u8 wazaDataBiosGetNekonote(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNekonote;
}

u8 wazaDataBiosGetMonomane(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mMonomane;
}

u8 wazaDataBiosGetYubiwohuru(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mYubiwohuru;
}

u8 wazaDataBiosGetOujanosirusi(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mOujanosirusi;
}

u8 wazaDataBiosGetOumugaesi(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mOumugaesi;
}

u8 wazaDataBiosGetYokodori(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mYokodori;
}

u8 wazaDataBiosGetMajikku(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mMajikku;
}

u8 wazaDataBiosGetMamoru(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mMamoru;
}

u8 wazaDataBiosGetDageki(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDageki;
}

u8 wazaDataBiosGetAddFightKoukaAvg(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mAddFightKoukaAvg;
}

u32 wazaDataBiosGetFightAttackMsgId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightAttackMsgId;
}

u32 wazaDataBiosGetFightAttackTunagiMsgId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightAttackTunagiMsgId;
}

u16 wazaDataBiosGetSeqId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSeqId;
}

u16 wazaDataBiosGetFightKoukaDataId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightKoukaDataId;
}

s16 wazaDataBiosGetIryoku(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mIryoku;
}

u8 wazaDataBiosGetAvg(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mAvg;
}

u8 wazaDataBiosGetRangeId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mRangeId;
}

s8 wazaDataBiosGetPri(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPri;
}

u8 wazaDataBiosGetZokuseiDataId(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mZokuseiDataId;
}

u8 wazaDataBiosGetPp(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPp;
}

u32 wazaDataBiosGetName(struct WazaData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNameID;
}

}
