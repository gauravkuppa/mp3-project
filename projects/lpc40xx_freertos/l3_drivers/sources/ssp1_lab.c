#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "ssp1.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

void ssp__init(uint32_t max_clock_mhz) {
  // Refer to LPC User manual and setup the register bits correctly
  // a) Power on Peripheral
  // b) Setup control registers CR0 and CR1
  // c) Setup prescalar register to be <= max_clock_mhz
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__SSP2);
  // set CPOL, CPHA?
  LPC_SSP2->CR0 |= (7 << 0);
  LPC_SSP2->CR1 |= (1 << 1); // SSP enable
  // fprintf(stderr, max_clock_mhz);
  uint32_t cvpdsr = 2; // 96MHz
  uint32_t pclk_mhz = clock__get_peripheral_clock_hz() / 1000000;

  // APB clock divicer to support max ADC clock
  while (max_clock_mhz < pclk_mhz / cvpdsr && cvpdsr < 255) {
    cvpdsr += 2;
  }

  LPC_SSP2->CPSR &= ~(0xFF);
  LPC_SSP2->CPSR |= cvpdsr;
}
// fprintf(stderr, "%d", cvpdsr);

uint8_t ssp__exchange_byte(uint8_t byte_to_transmit) {
  // Configure the Data register(DR) to send and receive data by checking the
  // status register
  LPC_SSP2->DR = byte_to_transmit;
  while (LPC_SSP2->SR & (1 << 4)) {
    ;
  }
  return (uint8_t)LPC_SSP2->DR;
}