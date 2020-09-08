#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <errno.h>

// SPI0 Channel-0 : Symmetric-Key Cipher
// SPI0 Channel-1 : Public-Key Cipher / Hash Function / Random Generator

#if defined(__linux__)
static uint32_t spi_fd[2] = { -1, -1 };

static uint32_t spi_speed[2];
#else
// Firmware Implementation

#endif

int32_t dalpu_spi_open(int32_t channel, int32_t speed, int32_t mode) {
#if defined(__linux__)
    int32_t fd;
    char spi_dev[32] = { 0x00, };
    const uint8_t spi_bpw = 8;
    int spi_lsb = 0;

    mode &= 3; // Mode is 0, 1, 2 or 3

    snprintf(spi_dev, sizeof(spi_dev), "/dev/spidev0.%d", channel);

    spi_speed[channel] = speed;

    if ((fd = open(spi_dev, O_RDWR)) < 0) {
        printf("SPI Device Open Failure : %s\n", strerror(errno));

        return -1;
    }

    spi_fd[channel] = fd;

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        printf("SPI Mode Change Failure : %s\n", strerror(errno));

        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0) {
        printf("SPI BPW Change Failure : %s\n", strerror(errno));

        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        printf("SPI Speed Change Failure : %s\n", strerror(errno));

        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_LSB_FIRST, &spi_lsb) < 0) {
        printf("SPI Bit-Order Change Failure : %s\n", strerror(errno));

        return -1;
    }

    return 0;
#else
// Firmware Implementation

#endif
}

int32_t dalpu_spi_close(int32_t channel) {
#if defined(__linux__)
    int32_t ret;

    ret = close(spi_fd[channel]);
    if (ret < 0) {
        printf("SPI Device Close Failure : %s\n", strerror(errno));
    }

    return ret;
#else
// Firmware Implementation

#endif
}

int32_t dalpu_spi_transfer(int32_t channel, uint8_t *data, uint32_t data_len, uint16_t delay) {
#if defined(__linux__)
    struct spi_ioc_transfer spi;

    channel &= 1;

    memset(&spi, 0, sizeof(spi));

    spi.tx_buf = (uint64_t)data;
    spi.rx_buf = (uint64_t)data;
    spi.len = data_len;
    spi.delay_usecs = delay;
    spi.speed_hz = spi_speed[channel];
    spi.bits_per_word = 8;

    return ioctl(spi_fd[channel], SPI_IOC_MESSAGE(1), &spi);
#else
// Firmware Implementation

#endif
}

int32_t dalpu_spi_command(int32_t channel, uint8_t inst, uint32_t addr, const uint8_t *tx_data, uint8_t *rx_data, uint32_t data_len) {
#if defined(__linux__)
    uint8_t buf[1024] = { 0x00, };
    int32_t ret = -1;

    buf[0] = inst;
    buf[1] = (addr >> 8) & 0xFF;
    buf[2] = addr & 0xFF;
    buf[3] = 0xFF;

    memset(rx_data, 64, 0);

    if (inst == 0x31 || inst == 0x30 || inst ==0x39) {
        // 1 instruction
        // 2 addr MSB
        // 3 addr LSB
        // 4 dummy
        // 5 ~ N byte payload
        // 6 dummy

        memcpy(buf + 4, tx_data, data_len);

        buf[data_len + 4] = 0xFF;

        if (inst == 0x39) { // send / recv
            ret = dalpu_spi_transfer(channel, buf, data_len + 5 + 20, 0);
            if (ret < 0) {
                goto err;
            }

            memcpy(rx_data, &buf[4 + 20], data_len);
        }
        else { // send
            ret = dalpu_spi_transfer(channel, buf, data_len + 5, 0);
        }
    }
    else { // recv
        // 1 instruction
        // 2 addr MSB
        // 3 addr LSB
        // 4 dummy

        ret = dalpu_spi_transfer(channel, buf, data_len + 4, 0);
        if (ret < 0) {
            goto err;
        }

        memcpy(rx_data, &buf[4], data_len);
    }

#if 0
    {
        int i = 0;

        printf("*****************************************************************\n") ;
        printf(" Instruction    : 0x%02x\n", inst);
        printf(" Address        : 0x%04x\n", addr);
        printf(" SPI_TX_DATA    : 0x");
        for (i = 0; i < data_len; i++)
            printf("%02x", tx_data[i]);
        printf("\n");
        printf(" SPI_RX_DATA    : 0x");
        for (i = 0; i < data_len; i++)
            printf("%02x", rx_data[i]);
        printf("\n");
        printf("*****************************************************************\n\n") ;
    }
#endif

err:
    return ret;
#else
// Firmware Implementation

#endif
}

int32_t dalpu_spi_send_data(int32_t channel, const uint8_t *tx_data, uint32_t tx_data_len, uint16_t delay) {
#if defined(__linux__)
    uint8_t buf[1024] = { 0x00, };
    int32_t ret = -1;

    memcpy(buf, tx_data, tx_data_len);

    return dalpu_spi_transfer(channel, buf, tx_data_len, delay);
#else
// Firmware Implementation

#endif
}

int32_t dalpu_spi_recv_data(int32_t channel, const uint8_t *tx_data, uint32_t tx_data_len,
        uint8_t *rx_data, uint32_t rx_data_len, uint16_t delay) {
#if defined(__linux__)
    uint8_t buf[1024] = { 0x00, };
    int32_t ret = -1;

    memcpy(buf, tx_data, tx_data_len);

    ret = dalpu_spi_transfer(channel, buf, tx_data_len, 50*1000);
    if (ret < 0) {
        goto err;
    }

    ret = dalpu_spi_transfer(channel, rx_data, rx_data_len, delay);

err:
    return ret;
#else
// Firmware Implementation

#endif
}
