#ifndef BASELIB_JOBJ_H
#define BASELIB_JOBJ_H


#ifdef __cplusplus
extern "C" {
#endif

struct _HSD_JObj;
void HSD_JObjSetDPtclCallback(void (*callback)(int, int, int, struct _HSD_JObj *));
void HSD_JObjSetPtclTargetCallback(void (*callback)(struct _HSD_JObj *jobj, int id));

#ifdef __cplusplus
}
#endif

#endif
