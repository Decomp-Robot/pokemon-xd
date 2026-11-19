#include <GSmsg.h>
#include <string.h>

u32 _msgGetLength(const u16 *text);
u32 _msgGetSize(const u16 *text);
u32 SJIStoGSchar(u16 *destination, char *source);

u32 GScharMakeFromSJIS(u16 *destination, char *source)
{
    return SJIStoGSchar(destination, source);
}

u16 *GScharCat(u16 *destination, const u16 *source)
{
    u32 length = _msgGetLength(destination);
    memcpy(&destination[length], source, _msgGetSize(source));
    return destination;
}

u16 *GScharLenCpy(u16 *destination, const u16 *source, s32 capacity)
{
    s32 count;
    s32 i;
    if (capacity <= 0) return destination;
    count = (_msgGetSize(source) + 1) / 2;
    if (count >= capacity) count = capacity - 1;
    memcpy(destination, source, count * 2);
    i = count;
    while (i < capacity) destination[i++] = 0;
    return destination;
}

u16 *GScharCpy(u16 *destination, const u16 *source)
{
    if (destination == NULL) return NULL;
    if (source == NULL) *destination = 0;
    else memcpy(destination, source, _msgGetSize(source));
    return destination;
}

s32 GScharCmp(const u16 *a, const u16 *b)
{
    u32 lengthA = _msgGetLength(a);
    u32 lengthB = _msgGetLength(b);
    u32 i;
    if (lengthA == lengthB) {
        for (i = 0; i < lengthB; i++) {
            if (a[i] != b[i]) {
                if (a[i] > b[i]) return 1;
                return -1;
            }
        }
        return 0;
    } else if (lengthA > lengthB) {
        for (i = 0; i < lengthB; i++) {
            if (a[i] != b[i]) {
                if (a[i] > b[i]) return 1;
                return -1;
            }
        }
        return 1;
    } else {
        for (i = 0; i < lengthA; i++) {
            if (a[i] != b[i]) {
                if (a[i] > b[i]) return 1;
                return -1;
            }
        }
        return -1;
    }
}

void GSmsgAdjustAlign(MSG_TASK_WORK *task)
{
    if (task->width == 0) {
        return;
    }

    s16 messageWidth = GSmsgGetMessageSize(task->messageId) >> 16;
    switch (task->align) {
    case MSG_ALIGN_LEFT:
        task->cursorX = task->originX;
        break;
    case MSG_ALIGN_CENTER:
        task->cursorX = task->originX + (task->width - messageWidth) / 2;
        break;
    case MSG_ALIGN_RIGHT:
        task->cursorX = task->originX + task->width - messageWidth;
        break;
    }
}
