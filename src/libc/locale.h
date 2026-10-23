#ifndef MSL_LOCALE_H
#define MSL_LOCALE_H

typedef struct MSLNumericLocale {
    char *decimal_point;
    char *otherStrings[8];
    unsigned char attributes[8];
    char *additionalString;
    unsigned char additionalAttributes[8];
} MSLNumericLocale;

#ifdef __cplusplus
extern "C" {
#endif

extern MSLNumericLocale __lconv;

#ifdef __cplusplus
}
#endif

#endif /* MSL_LOCALE_H */
