#include "qsort.h"

#define SWAP_BYTES(firstElement, secondElement, elementSize) \
    do { \
        char *first = (firstElement) - 1; \
        char *second = (secondElement) - 1; \
        size_t width = (elementSize) + 1; \
        int temp; \
        while (--width) { \
            temp = *++second; \
            *second = *++first; \
            *first = temp; \
        } \
    } while (0)

void qsort(void *base, size_t count, size_t elementSize,
           int (*compare)(const void *, const void *)) {
    size_t parent;
    size_t heapSize;
    size_t child;
    char *parentPtr;
    char *lastPtr;
    char *tempPtr;
    char *childPtr;
    char *rightPtr;

    if (count < 2) {
        return;
    }

    parent = (count >> 1) + 1;
    heapSize = count;
    parentPtr = (char *)base + elementSize * (parent - 1);
    lastPtr = (char *)base + elementSize * (count - 1);

    for (;;) {
        if (parent > 1) {
            parentPtr -= elementSize;
            parent--;
        } else {
            SWAP_BYTES(lastPtr, parentPtr, elementSize);
            if (--heapSize == 1) {
                return;
            }
            lastPtr -= elementSize;
        }

        child = parent;
        childPtr = (char *)base + elementSize * (child - 1);
        while (child * 2 <= heapSize) {
            child *= 2;
            tempPtr = childPtr;
            childPtr = (char *)base + elementSize * (child - 1);
            if (child < heapSize) {
                rightPtr = childPtr + elementSize;
                if (compare(childPtr, rightPtr) < 0) {
                    childPtr = rightPtr;
                    child++;
                }
            }
            if (compare(tempPtr, childPtr) >= 0) {
                break;
            }
            SWAP_BYTES(childPtr, tempPtr, elementSize);
        }
    }
}
