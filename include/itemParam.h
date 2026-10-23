#ifndef ITEM_PARAM_H
#define ITEM_PARAM_H

#include <types.h>

/* Item parameters in packed form, as the itemParamOrig* tables store them:
 * four packed halfwords. */
struct ITEM_PARAM_ORIG {
    u16 mValue[4];
};

/* Item parameters after itemParamConvertOrigFormat has expanded the packed
 * form. The itemParamGet* accessors index the result as sixteen bytes. */
struct ITEM_PARAM {
    u8 mValue[16];
};

extern ITEM_PARAM_ORIG itemParamOrigField;
extern ITEM_PARAM_ORIG itemParamOrigBattle;

#ifdef __cplusplus
extern "C" {
#endif

void itemParamConvertOrigFormat(ITEM_PARAM *dst, const ITEM_PARAM_ORIG *src);
s32 itemParamGetRecoverType(ITEM_PARAM *param);
void *itemParamGetPtr(u8 functionalID);
u8 itemParamGetPPSelectFlag(void *param);

#ifdef __cplusplus
}
#endif

#endif
