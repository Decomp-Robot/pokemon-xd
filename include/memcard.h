#ifndef MEMCARD_H
#define MEMCARD_H
#ifdef __cplusplus
extern "C" {
#endif
void *memcardMemAllocForSavedata(void);
void *memcardMemFreeForSavedata(void *buffer);
#ifdef __cplusplus
}
#endif
#endif
