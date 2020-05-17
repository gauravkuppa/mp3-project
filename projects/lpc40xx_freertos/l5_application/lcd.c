#include "lcd.h"
#include "delay.h"
#include "gpio.h"
#include <stdlib.h>

// #define EN (1 << 0)
// #define RW (1 << 2)
// #define RS (1 << 5)
// #define DB4 (1 << 1)
// #define DB5 (1 << 4)
// #define DB6 (1 << 6)
// #define DB7 (1 << 8)

gpio_s LCD_EN, LCD_RS, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6,
    LCD_D7;

void init_lcd() {
  // Init pins to 0:
  LCD_EN = gpio__construct_as_output(GPIO__PORT_2, 0);
  LCD_RS = gpio__construct_as_output(GPIO__PORT_2, 5);

  LCD_D0 = gpio__construct_as_output(GPIO__PORT_0, 16);
  LCD_D1 = gpio__construct_as_output(GPIO__PORT_0, 17);
  LCD_D2 = gpio__construct_as_output(GPIO__PORT_0, 22);
  LCD_D3 = gpio__construct_as_output(GPIO__PORT_0, 0);
  LCD_D4 = gpio__construct_as_output(GPIO__PORT_2, 1);
  LCD_D5 = gpio__construct_as_output(GPIO__PORT_2, 4);
  LCD_D6 = gpio__construct_as_output(GPIO__PORT_2, 6);
  LCD_D7 = gpio__construct_as_output(GPIO__PORT_2, 8);

  write_8_bit_mode(0b00110000, 0);
  write_8_bit_mode(0b00110000, 0);
  write_8_bit_mode(0b00110000, 0);

  write_8_bit_mode(0x38, 0); // function set 0b00111000
  // write_8_bit_mode(0b00000001, 0);
  write_8_bit_mode(0x06, 0); // entry mode set 0b00000110
  write_8_bit_mode(0x0E, 0); // display on/off 0b00001111

  write_8_bit_mode(0x01, 0); // clear 0b00000001
}

void write_8_bit_mode(uint8_t command, uint8_t rs_value) {
  // bool gpio__get(gpio_s gpio);
  // void gpio__set(gpio_s gpio);   ///< Sets the pin value as 'high' -> 3.3v
  // void gpio__reset(gpio_s gpio); ///< Sets the pin value as 'low' -> ground
  // void gpio__toggle(gpio_s gpio);

  if (rs_value) {
    gpio__set(LCD_RS);
  } else {
    gpio__reset(LCD_RS);
  }

  // time.sleep()

  if (command >> 0 & 1) {
    gpio__set(LCD_D0);
  } else {
    gpio__reset(LCD_D0);
  }

  if (command >> 1 & 1) {
    gpio__set(LCD_D1);
  } else {
    gpio__reset(LCD_D1);
  }

  if (command >> 2 & 1) {
    gpio__set(LCD_D2);
  } else {
    gpio__reset(LCD_D2);
  }

  if (command >> 3 & 1) {
    gpio__set(LCD_D3);
  } else {
    gpio__reset(LCD_D3);
  }

  if (command >> 4 & 1) {
    gpio__set(LCD_D4);
  } else {
    gpio__reset(LCD_D4);
  }

  if (command >> 5 & 1) {
    gpio__set(LCD_D5);
  } else {
    gpio__reset(LCD_D5);
  }

  if (command >> 6 & 1) {
    gpio__set(LCD_D6);
  } else {
    gpio__reset(LCD_D6);
  }

  if (command >> 7 & 1) {
    gpio__set(LCD_D7);
  } else {
    gpio__reset(LCD_D7);
  }

  pulse_clock(100);
}

void pulse_clock(uint8_t delay) {
  vTaskDelay(delay);
  gpio__set(LCD_EN);
  vTaskDelay(delay);
  gpio__reset(LCD_EN);
  vTaskDelay(delay);
}

void print_msg(char *write) {
  while (*write != '\0') // Print characters until end of line
  {
    write_8_bit_mode((uint8_t)atoi(write), 1);
    write++;
  }
}
