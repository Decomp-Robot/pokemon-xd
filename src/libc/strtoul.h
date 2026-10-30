#ifndef MSL_STRTOUL_H
#define MSL_STRTOUL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Convert a character stream to an unsigned long. The stream is read through
 * readCharacter, and the consumed length, the detected sign and unsigned
 * overflow are reported back separately. */
unsigned long __strtoul(int base, int maxWidth,
                        int (*readCharacter)(void *context, int character, int operation),
                        void *context, int *charsScanned, int *negative,
                        int *overflow);
int atoi(const char *text);

#ifdef __cplusplus
}
#endif

#endif /* MSL_STRTOUL_H */
