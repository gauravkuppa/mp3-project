#include <stdbool.h>
#include <stddef.h>

#include "ssp0.h"

#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

void ssp0__initialize(uint32_t max_clock_khz) {

  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__SSP0);

  LPC_SSP0->CR0 = 7;
  LPC_SSP0->CR1 |= (1 << 1);

  ssp0__set_max_clock(max_clock_khz);
}

void ssp0__set_max_clock(uint32_t max_clock_khz) {
  uint32_t cvpdsr = 2; // 96MHz
  uint32_t pclk_khz = clock__get_peripheral_clock_hz() / 1000UL;

  while (max_clock_khz < pclk_khz / cvpdsr && cvpdsr < 255) {
    cvpdsr += 2;
  }

  LPC_SSP0->CPSR = cvpdsr;
}

uint8_t ssp0__exchange_byte(uint8_t byte_to_transmit) {

  LPC_SSP0->DR = byte_to_transmit;

  while (LPC_SSP0->SR & (1 << 4)) {
    ;
  }

  return (uint8_t)(LPC_SSP0->DR & 0xFF);
}