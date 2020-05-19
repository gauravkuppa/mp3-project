// @file gpio_isr.c
#include "gpio_isr.h"
#include "lpc40xx.h"
#include "uart_printf.h"

// Note: You may want another separate array for falling vs. rising edge
// callbacks
static function_pointer_t gpio0_callbacks[32];

void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type,
                             function_pointer_t callback) {
  // 1) Store the callback based on the pin at gpio0_callbacks
  // 2) Configure GPIO 0 pin for rising or falling edge

  gpio0_callbacks[pin] = callback;

  if (interrupt_type == GPIO_INTR__FALLING_EDGE) {
    LPC_GPIOINT->IO0IntEnF |= (1 << pin);
  } else if (interrupt_type == GPIO_INTR__RISING_EDGE) {
    LPC_GPIOINT->IO0IntEnR |= (1 << pin);
  }
}

// We wrote some of the implementation for you
void gpio0__interrupt_dispatcher(void) {
  // Check which pin generated the interrupt
  const int pin_that_generated_interrupt = get_pin();
  function_pointer_t attached_user_handler =
      gpio0_callbacks[pin_that_generated_interrupt];

  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();
  clear_pin_interrupt(pin_that_generated_interrupt);
}

int get_pin(void) {
  // uint32_t the_pin = (LPC_GPIOINT->IO0IntStatR | LPC_GPIOINT->IO0IntStatF);
  int pos = 0;
  int i;

  for (i = 0; i < 32; i++) {
    if (LPC_GPIOINT->IO0IntStatR & (1 << i)) {
      pos = i;
    } else if (LPC_GPIOINT->IO0IntStatF & (1 << i)) {
      pos = i;
    }
  }
  uart_printf__polled(UART__0, "pin is %d\n", pos);
  return pos;
}

void clear_pin_interrupt(int pin_to_clear) {
  LPC_GPIOINT->IO0IntClr |= (1 << pin_to_clear);
  uart_printf__polled(UART__0, "cleared %d\n", pin_to_clear);
}