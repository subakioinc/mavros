#ifndef _DALPU_AES_H
#define _DALPU_AES_H    1

#include <stddef.h>
#include <stdint.h>
#include <dalpu/cipher.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char *cipher_mode_str(dalpu_cipher_mode_t);

typedef struct {
    dalpu_operation_t encdec;

    int32_t tls_key_idx;

} dalpu_aes_ctx;

int32_t dalpu_aes_init(dalpu_aes_ctx *ctx, const uint8_t *key, uint32_t key_len, const uint8_t *iv, dalpu_cipher_mode_t mode, dalpu_operation_t encdec);

int32_t dalpu_aes_cipher(dalpu_aes_ctx *ctx, uint8_t *out, const uint8_t *in, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _DALPU_AES_H */
