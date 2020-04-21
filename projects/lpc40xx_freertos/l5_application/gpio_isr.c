// @file gpio_isr.c
#include "gpio_isr.h"
#include "lpc40xx.h"
#include "uart_printf.h"
#include <stdint.h>
#include <stdio.h>
// Note: You may want another separate array for falling vs. rising edge
// callbacks
static function_pointer_t gpio0_callbacks[32];

void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type,
                             function_pointer_t callback) {
  // 1) Store the callback based on the pin at gpio0_callbacks
  // 2) Configure GPIO 0 pin for rising or falling edge
  gpio0_callbacks[pin] = callback;
  if (interrupt_type == GPIO_INTR__FALLING_EDGE) {
    // LPC_GPIOINT->IO0IntEnF |= ~(1 << pin);

    LPC_GPIOINT->IO0IntEnF |= (1 << pin);
  } else if (interrupt_type == GPIO_INTR__RISING_EDGE) {
    // LPC_GPIOINT->IO0IntEnR |= ~(1 << pin);

    LPC_GPIOINT->IO0IntEnR |= (1 << pin);
  }
  fprintf(stderr, "attach_interrupt: %d", pin);
}

int logic_that_you_will_write() {
  int i = 0;
  int pin = 0;
  for (i = 0; i < 32; i++) {
    if (LPC_GPIOINT->IO0IntStatF & (1 << i)) {
      pin = i;
    } else if (LPC_GPIOINT->IO0IntStatR & (1 << i)) {
      pin = i;
    }
  }
  return pin;
}

// We wrote some of the implementation for you
void gpio0__interrupt_dispatcher(void) {
  // Check which pin generated the interrupt
  // uart_printf__polled(UART__0, "interrupt port 0\n");
  // fprintf("")
  const int pin_that_generated_interrupt = logic_that_you_will_write();
  gpioN__set_as_input(0, pin_that_generated_interrupt);
  uart_printf__polled(UART__0, "%d", pin_that_generated_interrupt);

  function_pointer_t attached_user_handler =
      gpio0_callbacks[pin_that_generated_interrupt];
  // uart_printf__polled(UART__0, "callback port 0\n");

  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();
  // uart_printf__polled(UART__0, "callback done port 0\n");
  LPC_GPIOINT->IO0IntClr |= (1 << pin_that_generated_interrupt);
}