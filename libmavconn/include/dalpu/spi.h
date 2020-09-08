#ifndef _DALPU_SPI_H
#define _DALPU_SPI_H    1

int32_t dalpu_spi_open(int32_t channel, int32_t speed, int32_t mode);

int32_t dalpu_spi_close(int32_t channel);

int32_t dalpu_spi_transfer(int32_t channel, uint8_t *data, uint32_t data_len, uint16_t delay);

int32_t dalpu_spi_command(int32_t channel, uint8_t inst, uint32_t addr, const uint8_t *tx_data, uint8_t *rx_data, uint32_t data_len);

int32_t dalpu_spi_send_data(int32_t channel, const uint8_t *tx_data, uint32_t tx_data_len, uint16_t delay);

int32_t dalpu_spi_recv_data(int32_t channel, const uint8_t *tx_data, uint32_t tx_data_len, uint8_t *rx_data, uint32_t rx_data_len, uint16_t delay);

#endif //_DALPU_SPI_H
