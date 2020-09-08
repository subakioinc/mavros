#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <dalpu/ecc.h>
#include "dalpu/cm3.h"
#include "dalpu/spi.h"
#include "dalpu/register.h"

#if 0
int32_t dalpu_ecc_mode_set(void) {
    int32_t channel = 0;
    uint8_t inst = 0x31;
    uint8_t tx_data[128] = { 0x00, };
    uint8_t rx_data[128] = { 0x00, };

    tx_data[0] = 0x05;
    return dalpu_spi_command(channel, inst, RG_ST0_OPMODE, tx_data, rx_data, 1);
}
#endif

int32_t dalpu_ecc_p256_pub_key_gen(const uint8_t *pri_key, ecc_point *pub_key) {
    int32_t channel = 1;
    uint8_t tx_buf[256];
    uint8_t rx_buf[256];
    int32_t ret = -1;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SIZE_ECDH_256;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDH_PrivateKey;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pri_key, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Create_ECHD_PublicKey;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Get_ECDH_PublicKey_X;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(pub_key->x, rx_buf, 32);

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Get_ECDH_PublicKey_Y;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(pub_key->y, rx_buf, 32);

err:
    return ret;
}

int32_t dalpu_ecdsa_p256_sign(const uint8_t *pri_key, const uint8_t *rand_k, const uint8_t *hash, uint8_t *sig_r, uint8_t *sig_s) {
    int32_t channel = 1;
    uint8_t buf[256] = { 0x00, };
    int32_t ret = -1;

    buf[0] = SPI1_WRITE_DATA;
    buf[1] = 0;
    buf[2] = SIZE_ECDSA_256;
    buf[3] = 0;
    buf[4] = 0;
    ret = dalpu_spi_send_data(channel, buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    buf[0] = SPI1_WRITE_DATA;
    buf[1] = 0;
    buf[2] = Set_ECDSA_PrivateKey;
    buf[3] = 0;
    buf[4] = 32;
    memcpy(&buf[5], pri_key, 32);
    ret = dalpu_spi_send_data(channel, buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    buf[0] = SPI1_WRITE_DATA;
    buf[1] = 0;
    buf[2] = Set_ECDSA_K_RND;
    buf[3] = 0;
    buf[4] = 32;
    memcpy(&buf[5], rand_k, 32);
    ret = dalpu_spi_send_data(channel, buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    buf[0] = SPI1_WRITE_DATA;
    buf[1] = 0;
    buf[2] = Set_ECDSA_h;
    buf[3] = 0;
    buf[4] = 32;
    memcpy(&buf[5], hash, 32);
    ret = dalpu_spi_send_data(channel, buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    buf[0] = SPI1_WRITE_DATA;
    buf[1] = 0;
    buf[2] = Create_ECDSA_Sign;
    buf[3] = 0;
    buf[4] = 0;
    ret = dalpu_spi_send_data(channel, buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    buf[0] = SPI1_READ_DATA;
    buf[1] = 0;
    buf[2] = Get_ECDSA_r;
    buf[3] = 0;
    buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, buf, 5, sig_r, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    buf[0] = SPI1_READ_DATA;
    buf[1] = 0;
    buf[2] = Get_ECDSA_s;
    buf[3] = 0;
    buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, buf, 5, sig_s, 32, 20*1000);

err:
    return ret;
}

int32_t dalpu_ecdsa_p256_verify(const ecc_point *pub_key, const uint8_t *hash, const uint8_t *sig_r, const uint8_t *sig_s) {
    int32_t channel = 1;
    uint8_t tx_buf[256];
    uint8_t rx_buf[256];
    int32_t ret = -1;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SIZE_ECDSA_256;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDSA_r;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], sig_r, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDSA_s;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], sig_s, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDSA_Public_Key_Xq;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pub_key->x, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDSA_Public_Key_Yq;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pub_key->y, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDSA_h;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], hash, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Do_ECDSA_Verify;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Get_ECDSA_Result;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 0);
    if (ret < 0) {
        goto err;
    }

    ret = rx_buf[0];

err:
    return ret;
}

int32_t dalpu_ecdh_p256_shared_key_gen(const uint8_t *pri_key, const ecc_point *point, uint8_t *shared_key) {
    int32_t channel = 1;
    uint8_t tx_buf[256];
    uint8_t rx_buf[256];
    int32_t ret = -1;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SIZE_ECDH_256;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDH_PrivateKey;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pri_key, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDH_PublicKey_X;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], point->x, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_ECDH_PublicKey_Y;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], point->y, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Create_ECHD_KEY;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Get_ECDH_KEY_X;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(shared_key, rx_buf, 32);

err:
    return ret;
}

int32_t dalpu_ec_elgamal_p256_calc_point(const uint8_t *pt, ecc_point *pt_point) {
    int32_t channel = 1;
    uint8_t tx_buf[256];
    uint8_t rx_buf[256];
    int32_t ret = -1;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SIZE_EC_ELGAMEL_256;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_PmSrc;
    tx_buf[3] = 0;
    tx_buf[4] = 16;
    memcpy(&tx_buf[5], pt, 16);
    ret = dalpu_spi_send_data(channel, tx_buf, 21, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_PmSrcX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 40*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(pt_point->x, rx_buf, 32);

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_PmSrcY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(pt_point->y, rx_buf, 32);

err:
    return ret;
}

int32_t dalpu_ec_elgamal_p256_encrypt(const ecc_point *pub_key, ecc_point *c_point, ecc_point *d_point) {
    int i = 0;
    int32_t channel = 1;
    unsigned char tx_buf[256];
    unsigned char rx_buf[256];
    unsigned char rand_k[32];
    int32_t ret = -1;

    for (i = 0; i < 32; i++) rand_k[i] = rand() & 0xFF;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SIZE_EC_ELGAMEL_256;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_k;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], rand_k, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_YX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pub_key->x, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_YY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pub_key->y, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_C;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 60*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_C2;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 60*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_D;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_CX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(c_point->x, rx_buf, 32);

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_CY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(c_point->y, rx_buf, 32);

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_DX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(d_point->x, rx_buf, 32);

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_DY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(d_point->y, rx_buf, 32);

err:
    return ret;
}

int32_t dalpu_ec_elgamal_p256_decrypt(const uint8_t *pri_key, const ecc_point *c_point, const ecc_point *d_point, ecc_point *pt_point) {
    int32_t channel = 1;
    unsigned char tx_buf[256];
    unsigned char rx_buf[256];
    int32_t ret = -1;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SIZE_EC_ELGAMEL_256;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_x;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], pri_key, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_CX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], c_point->x, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 60*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_CY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], c_point->y, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 40*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_DX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], d_point->x, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = SET_EC_ELGAMEL_DY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    memcpy(&tx_buf[5], d_point->y, 32);
    ret = dalpu_spi_send_data(channel, tx_buf, 37, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_Xprime;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 60*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_Cprime;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 60*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = CALC_EC_ELGAMEL_Pm;
    tx_buf[3] = 0;
    tx_buf[4] = 0;
    ret = dalpu_spi_send_data(channel, tx_buf, 5, 60*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_PmX;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(pt_point->x, rx_buf, 32);

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = GET_EC_ELGAMEL_PmY;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(pt_point->y, rx_buf, 32);

err:
    return ret;
}
