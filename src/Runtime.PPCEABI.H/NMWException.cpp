#include "new.h"

/* new[] blocks carry the element size and count in front of the elements so
 * that delete[] can rebuild the element stride; the elements start sixteen
 * bytes into the block. */
enum { kArrayHeaderSize = 16 };

struct ArrayHeader {
    unsigned long element_size;
    unsigned long element_count;
};

typedef void (*ConstructorDestructor)(void *element, int construct);

/* Guards a partially constructed array: if construction is abandoned, the
 * elements completed so far are destroyed newest first. */
class __partial_array_destructor {
public:
    __partial_array_destructor(void *buffer, unsigned long size, unsigned long count,
                               ConstructorDestructor destructor)
        : array(buffer), size(size), element_count(count), destructor(destructor),
          constructed_count(count) {}

    ~__partial_array_destructor() {
        if (constructed_count < element_count && destructor) {
            char *element = (char *)array + size * constructed_count;
            while (constructed_count > 0) {
                element -= size;
                destructor(element, -1);
                --constructed_count;
            }
        }
    }

    void *array;
    unsigned long size;
    unsigned long element_count;
    ConstructorDestructor destructor;
    unsigned long constructed_count;
};

extern "C" void *__construct_new_array(void *block, ConstructorDestructor constructor,
                                       ConstructorDestructor destructor, unsigned long size,
                                       unsigned long count) {
    char *element;
    char *result;
    ArrayHeader *header;
    result = (char *)block;
    if (result) {
        header = (ArrayHeader *)result;
        header->element_size = size;
        header->element_count = count;
        result = (char *)result + kArrayHeaderSize;
        if (constructor) {
            __partial_array_destructor guard(result, size, count, destructor);
            element = (char *)result;
            for (guard.constructed_count = 0; guard.constructed_count < count;
                 ++guard.constructed_count, element += size) {
                constructor(element, 1);
            }
        }
    }
    return result;
}

extern "C" void __construct_array(void *array, ConstructorDestructor constructor,
                                  ConstructorDestructor destructor, unsigned long size,
                                  unsigned long count) {
    __partial_array_destructor guard(array, size, count, destructor);
    char *element = (char *)array;
    for (guard.constructed_count = 0; guard.constructed_count < count;
         ++guard.constructed_count, element += size) {
        constructor(element, 1);
    }
}

extern "C" void __destroy_new_array(void *array, ConstructorDestructor destructor) {
    if (array) {
        if (destructor) {
            unsigned long i;
            ArrayHeader *header;
            unsigned long count;
            unsigned long size;
            char *element;
            header = (ArrayHeader *)((char *)array - kArrayHeaderSize);
            size = header->element_size;
            count = header->element_count;
            element = (char *)array + size * count;
            for (i = 0; i < count; ++i) {
                element -= size;
                destructor(element, -1);
            }
        }
        operator delete[]((char *)array - kArrayHeaderSize);
    }
}
