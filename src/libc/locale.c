/* Default numeric locale record. */

#include "locale.h"

/* ".", an empty string, and the "C" locale name. */
static char const locale_strings[8] = { '.', '\0', '\0', 'C', '\0', '\0', '\0', '\0' };

MSLNumericLocale __lconv = {
    (char *) locale_strings,
    {
        (char *) (locale_strings + 2), (char *) (locale_strings + 2),
        (char *) (locale_strings + 2), (char *) (locale_strings + 2),
        (char *) (locale_strings + 2), (char *) (locale_strings + 2),
        (char *) (locale_strings + 2), (char *) (locale_strings + 2),
    },
    { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00 },
    (char *) (locale_strings + 2),
    { 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00 },
};
