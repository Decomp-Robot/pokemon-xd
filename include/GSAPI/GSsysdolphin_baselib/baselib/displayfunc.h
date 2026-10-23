#ifndef BASELIB_DISPLAYFUNC_H
#define BASELIB_DISPLAYFUNC_H


#ifdef __cplusplus
extern "C" {
#endif

struct _HSD_JObj;
void HSD_JObjSetSPtclCallback(void (*callback)(int, int, int, struct _HSD_JObj *));

#ifdef __cplusplus
}
#endif

#endif
