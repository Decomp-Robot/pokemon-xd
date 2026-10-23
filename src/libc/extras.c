/* Non-standard MSL string extras. */

#include "extras.h"
#include "ctype_inline.h"

int stricmp(const char *left, const char *right) {
    char leftChar, rightChar;

    do {
        leftChar = __tolower(*left++);
        rightChar = __tolower(*right++);

        if (leftChar < rightChar) {
            return -1;
        }
        if (leftChar > rightChar) {
            return 1;
        }
    } while (leftChar);

    return 0;
}
