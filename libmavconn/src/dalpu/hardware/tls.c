#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "spi.h"
#include "register.h"

int32_t dalpu_tls_init(rg_alg_t alg, rg_mode_t mode_op, rg_encdec_t encdec, rg_key_t key_type, int32_t two_frame) {
    uint8_t tx_data[64];
    uint8_t rx_data[64];
    int32_t channel = 0;
    int32_t ret = -1;

    tx_data[0] = 0x01;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_SPI_LENGTH, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 0x01;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_BUF_CTRL, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 0x00; // KEY_0
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_EE_KEY_AES_CTRL, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = (mode_op << 4) | (two_frame << 3) | (key_type << 1) | alg;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_AES_CTRL, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 0x09;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST0_OPMODE, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    if(encdec == RG_ENC)
        tx_data[0] = 0x02;
    else
        tx_data[0] = 0x03;

    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST1_SYMCIP_OPMODE, tx_data, rx_data, 1);

err:
    return ret;
}

int32_t dalpu_tls_set_mode(int32_t idx_r, int32_t idx_w) {
    uint8_t tx_data[64];
    uint8_t rx_data[64];
    int32_t channel = 0;
    int32_t rw_flags = 0;
    int32_t ret = -1;

    if(idx_r < 0 && idx_w < 0) {
        goto err;
    }

    tx_data[0] = 0x3F;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_TLS_START_ADDR0, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 0xFF;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_TLS_START_ADDR1, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    if(idx_r > -1) {
        tx_data[0] = idx_r;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_TLS_xN_RD, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }

        rw_flags += 1;
    }

    if(idx_w > -1) {
        tx_data[0] = idx_w;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_TLS_xN_WR, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }

        rw_flags += 2;
    }

    tx_data[0] = rw_flags;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_TLS_MODE, tx_data, rx_data, 1);

err:
    return ret;
}

int32_t dalpu_tls_set_key_iv(int32_t key_read, const uint8_t *key, uint32_t key_len, const uint8_t *iv, uint32_t iv_len) {
    uint8_t tx_data[64];
    uint8_t rx_data[64];
    int32_t channel = 0;
    int32_t ret = -1;

    if(key_read == 0) { // Key WRITE
        if(iv != NULL) {
            tx_data[0] = 2;
            ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
            if (ret < 0) {
                goto err;
            }

            ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_EEBUF320, iv, rx_data, iv_len);
            if (ret < 0) {
                goto err;
            }

            tx_data[0] = 1;
            ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
            if (ret < 0) {
                goto err;
            }
        }

        tx_data[0] = 3;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }

        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_EEBUF320, key, rx_data, key_len);
        if (ret < 0) {
            goto err;
        }

        tx_data[0] = 1;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }
    }
    else { // Key READ
        tx_data[0] = 2;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }

        tx_data[0] = 1;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }

        tx_data[0] = 3;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }

        tx_data[0] = 1;
        ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
        if (ret < 0) {
            goto err;
        }
    }

    tx_data[0] = 4;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);

err:
    return ret;
}

int32_t dalpu_tls_aes_aria_encrypt(const uint8_t *in, uint8_t *out, uint32_t len) {
    return dalpu_spi_command(0, ADDR_DUPLEX_W, RG_EEBUF300, in, out, len);
}

int32_t dalpu_tls_aes_aria_decrypt(const uint8_t *in, uint8_t *out, uint32_t len) {
    return dalpu_spi_command(0, ADDR_DUPLEX_W, RG_EEBUF400, in, out, len);
}
