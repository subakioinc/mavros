#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dalpu/aes.h>
#include <dalpu/dalpu4.h>

int32_t dalpu_aes_aria_init(dalpu_aes_ctx *ctx, const uint8_t *key, uint32_t key_len, const uint8_t *iv, dalpu_cipher_id_t cipher_id, dalpu_cipher_mode_t cipher_mode, dalpu_operation_t cipher_encdec) {
    rg_alg_t alg;
    rg_key_t key_type;
    rg_mode_t mode;
    rg_encdec_t encdec;
    int32_t ret = -1;

    if (cipher_id == DALPU_CIPHER_ID_AES)
        alg = RG_ALG_AES;
    else if (cipher_id == DALPU_CIPHER_ID_ARIA)
        alg = RG_ALG_ARIA;
    else
        return -1;

    if (key_len == 16)
        key_type = RG_KEY_128;
    else if (key_len == 32)
        key_type = RG_KEY_256;
    else
        return -1;

    mode = cipher_mode;

    encdec = cipher_encdec;

    ctx->encdec = encdec;

    ctx->tls_key_idx = 0;

    ret = dalpu_state_standby();
    if (ret < 0) {
        goto err;
    }

    ret = dalpu_tls_init(alg, mode, encdec, key_type, 0);
    if (ret < 0) {
        goto err;
    }

    ret = dalpu_tls_set_mode(-1, ctx->tls_key_idx);
    if (ret < 0) {
        goto err;
    }

    ret = dalpu_tls_set_key_iv(0, key, key_len, iv, 16);

err:
    return ret;
}

int32_t dalpu_aes_aria_cipher(dalpu_aes_ctx *ctx, uint8_t *out, const uint8_t *in, uint32_t len) {
    int32_t i;
    int32_t block_len = 16;
    int32_t ret = -1;

    if (len < block_len)
        return -1;

    if ((len % block_len) != 0)
        return -1;

    if (ctx->encdec == DALPU_ENCRYPT) {
        for(i = 0, len -= block_len; i <= len; i += block_len) {
            ret = dalpu_tls_aes_aria_encrypt(in + i, out + i, block_len);
            if (ret < 0) {
                goto err;
            }
        }
    }
    else {
        for(i = 0, len -= block_len; i <= len; i += block_len) {
            ret = dalpu_tls_aes_aria_decrypt(in + i, out + i, block_len);
            if (ret < 0) {
                goto err;
            }
        }
    }

err:
    return ret;
}
