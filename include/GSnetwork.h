#ifndef GS_NETWORK_H
#define GS_NETWORK_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
void GSnetworkSetIP(u8 *address, u8 *netmask, u8 *gateway);
void GSnetworkInit(void);
#ifdef __cplusplus
}
#endif
#endif
