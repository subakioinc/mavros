#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "spi.h"
#include "register.h"

int32_t dalpu_state_ready(void) {
    uint8_t tx_data[64];
    uint8_t rx_data[64];
    int32_t channel = 0;
    int32_t ret = -1;

    tx_data[0] = 1;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST1_SYMCIP_OPMODE, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 1;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST0_OPMODE, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 0;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ACCESS, tx_data, rx_data, 1);

err:
    return ret;
}

int32_t dalpu_state_standby(void) {
    uint8_t tx_data[64];
    uint8_t rx_data[64];
    int32_t channel = 0;
    int32_t ret = -1;

    tx_data[0] = 1;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    ret = dalpu_state_ready();

err:
    return ret;
}

int32_t dalpu_state_standby_mode(void) {
    uint8_t tx_data[64];
    uint8_t rx_data[64];
    int32_t channel = 0;
    int32_t ret = -1;

    tx_data[0] = 1;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ST2_SYMCIP_OPMODE, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    tx_data[0] = 0;
    ret = dalpu_spi_command(channel, ADDR_NOR_W, RG_ACCESS, tx_data, rx_data, 1);
    if (ret < 0) {
        goto err;
    }

    ret = dalpu_state_ready();

err:
    return ret;
}
