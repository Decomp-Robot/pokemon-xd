#ifndef APP_D2PRESENT_D2PRESENT_H
#define APP_D2PRESENT_D2PRESENT_H

typedef enum D2PRESENT_MODE {
    D2PRESENT_MODE_0 = 0,
    D2PRESENT_MODE_1 = 1,
    D2PRESENT_MODE_2 = 2
} D2PRESENT_MODE;

#ifdef __cplusplus
extern "C" {
#endif

void d2presentOpen(D2PRESENT_MODE mode);

#ifdef __cplusplus
}
#endif

#endif
