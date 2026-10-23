#include <GSquat.h>

/* The unrotated quaternion the GS math module hands out. It is filled in by the
 * static initializer of this translation unit instead of being stored in
 * read-only data, so the .ctors entry runs before main(). */
extern "C" {
GSquat zeroQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
}
