#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <dalpu/sha.h>
#include "dalpu/cm3.h"
#include "dalpu/spi.h"
#include "dalpu/register.h"

int32_t dalpu_sha256(const uint8_t *msg, uint32_t msg_len, uint8_t *hash) {
    int32_t channel = 1;
    unsigned char tx_buf[256];
    unsigned char rx_buf[256];
    int32_t ret = -1;

    tx_buf[0] = SPI1_WRITE_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Set_SHA_Data;
    tx_buf[3] = 0;
    tx_buf[4] = msg_len;
    memcpy(&tx_buf[5], msg, msg_len);
    ret = dalpu_spi_send_data(channel, tx_buf, msg_len + 5, 20*1000);
    if (ret < 0) {
        goto err;
    }

    tx_buf[0] = SPI1_READ_DATA;
    tx_buf[1] = 0;
    tx_buf[2] = Get_SHA_Data;
    tx_buf[3] = 0;
    tx_buf[4] = 32;
    ret = dalpu_spi_recv_data(channel, tx_buf, 5, rx_buf, 32, 20*1000);
    if (ret < 0) {
        goto err;
    }

    memcpy(hash, rx_buf, 32);

err:
    return ret;
}
