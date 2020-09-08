#ifndef _DALPU4_H
#define _DALPU4_H   1

#include "register.h"

int32_t dalpu_state_ready(void);

int32_t dalpu_state_standby(void);

int32_t dalpu_state_standby_mode(void);

int32_t dalpu_tls_init(rg_alg_t alg, rg_mode_t mode_op, rg_encdec_t encdec, rg_key_t key_type, int32_t two_frame);

int32_t dalpu_tls_set_mode(int32_t idx_r, int32_t idx_w);

int32_t dalpu_tls_set_key_iv(int32_t key_read, const uint8_t *key, uint32_t key_len, const uint8_t *iv, uint32_t iv_len);

int32_t dalpu_tls_aes_aria_encrypt(const uint8_t *in, uint8_t *out, uint32_t len);

int32_t dalpu_tls_aes_aria_decrypt(const uint8_t *in, uint8_t *out, uint32_t len);

#endif //_DALPU4_H
