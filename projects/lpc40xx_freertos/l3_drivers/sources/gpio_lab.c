#include "gpio_lab.h"
#include "lpc40xx.h"

/// Should alter the hardware registers to set the pin as input
void gpioN__set_as_input(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 0) {
    LPC_GPIO0->DIR &= ~(1 << pin_num);
  } else if (port_num == 1) {
    LPC_GPIO1->DIR &= ~(1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->DIR &= ~(1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->DIR &= ~(1 << pin_num);
  } else if (port_num == 4) {
    LPC_GPIO4->DIR &= ~(1 << pin_num);
  } else if (port_num == 5) {
    LPC_GPIO5->DIR &= ~(1 << pin_num);
  }
}

/// Should alter the hardware registers to set the pin as output
void gpioN__set_as_output(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 0) {
    LPC_GPIO0->DIR |= (1 << pin_num);
  } else if (port_num == 1) {
    LPC_GPIO1->DIR |= (1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->DIR |= (1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->DIR |= (1 << pin_num);
  } else if (port_num == 4) {
    LPC_GPIO4->DIR |= (1 << pin_num);
  } else if (port_num == 5) {
    LPC_GPIO5->DIR |= (1 << pin_num);
  }
}

/// Should alter the hardware registers to set the pin as high
void gpioN__set_high(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 0) {
    LPC_GPIO0->PIN |= (1 << pin_num);
  } else if (port_num == 1) {
    LPC_GPIO1->PIN |= (1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->PIN |= (1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->PIN |= (1 << pin_num);
  } else if (port_num == 4) {
    LPC_GPIO4->PIN |= (1 << pin_num);
  } else if (port_num == 5) {
    LPC_GPIO5->PIN |= (1 << pin_num);
  }
}

/// Should alter the hardware registers to set the pin as low
void gpioN__set_low(uint8_t port_num, uint8_t pin_num) {
  if (port_num == 0) {
    LPC_GPIO0->PIN &= ~(1 << pin_num);
  } else if (port_num == 1) {
    LPC_GPIO1->PIN &= ~(1 << pin_num);
  } else if (port_num == 2) {
    LPC_GPIO2->PIN &= ~(1 << pin_num);
  } else if (port_num == 3) {
    LPC_GPIO3->PIN &= ~(1 << pin_num);
  } else if (port_num == 4) {
    LPC_GPIO4->PIN &= ~(1 << pin_num);
  } else if (port_num == 5) {
    LPC_GPIO5->PIN &= ~(1 << pin_num);
  }
}

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpioN__set(uint8_t port_num, uint8_t pin_num, bool high) {
  if (high) {
    gpioN__set_high(port_num, pin_num);
  } else {
    gpioN__set_low(port_num, pin_num);
  }
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpioN__get_level(uint8_t port_num, uint8_t pin_num) {
  bool result = false;
  LPC_GPIO_TypeDef *gpio;
  if (port_num == 0) {
    gpio = LPC_GPIO0;
  } else if (port_num == 1) {
    gpio = LPC_GPIO1;
  } else if (port_num == 2) {
    gpio = LPC_GPIO2;
  } else if (port_num == 3) {
    gpio = LPC_GPIO3;
  } else if (port_num == 4) {
    gpio = LPC_GPIO4;
  } else if (port_num == 5) {
    gpio = LPC_GPIO5;
  }

  if (gpio->PIN & (1 << pin_num)) {
    result = true;
  }

  return result;
}