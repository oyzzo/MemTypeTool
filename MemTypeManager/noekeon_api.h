#ifndef __NOEKEON_API_H__
#define __NOEKEON_API_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void NoekeonEncrypt(const uint32_t * const key, uint32_t * const data);
void NoekeonDecrypt(const uint32_t * const key, uint32_t * const data);

#ifdef __cplusplus
}
#endif

#endif                          /* NOEKEON_API_H */
