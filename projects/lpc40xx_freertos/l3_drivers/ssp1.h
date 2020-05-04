#pragma once

#include <stdint.h>
#include <stdlib.h>

/**
 * This configures what DMA channels the SSP2 driver utilizes
 * for ssp1__dma_write_block() and ssp1__dma_read_block()
 */
#define SSP1__DMA_TX_CHANNEL 0
#define SSP1__DMA_RX_CHANNEL 1

/// Initialize the bus with the given maximum clock rate in Khz
void ssp__init(uint32_t max_clock_mhz);

/// After initialization, this allows you to change the bus clock speed
void ssp__set_max_clock(uint32_t max_clock_khz);

/**
 * Exchange a single byte over the SPI bus
 * @returns the byte received while sending the byte_to_transmit
 */
uint8_t ssp__exchange_byte(uint8_t byte_to_transmit);

/**
 * @{
 * @name    Exchanges larger blocks over the SPI bus
 * These are designed to be one-way transmission for the SPI for now
 */
void ssp__dma_write_block(const unsigned char *output_block,
                          size_t number_of_bytes);
void ssp__dma_read_block(unsigned char *input_block, size_t number_of_bytes);
/** @} */
