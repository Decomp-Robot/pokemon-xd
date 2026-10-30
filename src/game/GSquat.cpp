#include <GSquat.h>

/* The unrotated quaternion the GS math module hands out. Its components are
 * filled in by this file's static initializer before main() runs. */
extern "C" {
GSquat zeroQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
}
