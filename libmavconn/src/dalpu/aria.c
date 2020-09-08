#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dalpu/aria.h>

int32_t dalpu_aria_init(dalpu_aria_ctx *ctx, const uint8_t *key, uint32_t key_len, const uint8_t *iv, dalpu_cipher_mode_t mode, dalpu_operation_t encdec) {
    return dalpu_aes_aria_init(ctx, key, key_len, iv, DALPU_CIPHER_ID_ARIA, mode, encdec);
}

int32_t dalpu_aria_cipher(dalpu_aria_ctx *ctx, uint8_t *out, const uint8_t *in, uint32_t len) {
    return dalpu_aes_aria_cipher(ctx, out, in, len);
}
