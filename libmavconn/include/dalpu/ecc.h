#ifndef _DALPU_ECC_H
#define _DALPU_ECC_H	1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t x[32];
    uint8_t y[32];
} ecc_point;

int32_t dalpu_ecc_p256_pub_key_gen(const uint8_t *pri_key, ecc_point *pub_key);

int32_t dalpu_ecdsa_p256_sign(const uint8_t *pri_key, const uint8_t *rand_k, const uint8_t *hash, uint8_t *sig_r, uint8_t *sig_s);

int32_t dalpu_ecdsa_p256_verify(const ecc_point *pub_key, const uint8_t *hash, const uint8_t *sig_r, const uint8_t *sig_s);

int32_t dalpu_ecdh_p256_shared_key_gen(const uint8_t* pri_key, const ecc_point *point, uint8_t *shared_key);

int32_t dalpu_ec_elgamal_p256_calc_point(const uint8_t *pt, ecc_point *pt_point);

int32_t dalpu_ec_elgamal_p256_encrypt(const ecc_point *pub_key, ecc_point *c_point, ecc_point *d_point);

int32_t dalpu_ec_elgamal_p256_decrypt(const uint8_t *pri_key, const ecc_point *c_point, const ecc_point *d_point, ecc_point *pt_point);

#ifdef __cplusplus
}
#endif

#endif /* _DALPU_ECC_H */
