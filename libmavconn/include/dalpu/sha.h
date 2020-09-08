#ifndef _DALPU_SHA_H
#define _DALPU_SHA_H	1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t dalpu_sha256(const uint8_t *msg, uint32_t msg_len, uint8_t *hash);

#ifdef __cplusplus
}
#endif

#endif /* _DALPU_SHA_H */
