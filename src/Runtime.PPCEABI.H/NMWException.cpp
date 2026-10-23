#include "new.h"

enum { kArrayHeaderSize = 16 };

typedef void (*ConstructorDestructor)(void *, int);

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
    result = (char *)block;
    if (result) {
        ((unsigned long *)result)[0] = size;
        ((unsigned long *)result)[1] = count;
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
            unsigned long count;
            unsigned long size;
            char *element;
            size = ((unsigned long *)((char *)array - kArrayHeaderSize))[0];
            count = ((unsigned long *)((char *)array - kArrayHeaderSize))[1];
            element = (char *)array + size * count;
            for (i = 0; i < count; ++i) {
                element -= size;
                destructor(element, -1);
            }
        }
        operator delete[]((char *)array - kArrayHeaderSize);
    }
}
