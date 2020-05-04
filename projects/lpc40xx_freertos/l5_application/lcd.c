#include "lcd.h"
#include "delay.h"
#include "gpio.h"
#include <stdlib.h>

gpio_s LCD_EN, LCD_RS, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6,
    LCD_D7;

void init_lcd() {
  // Init pins to 0:
  LCD_EN = gpio__construct_as_input(GPIO__PORT_2, 0);
  LCD_RS = gpio__construct_as_input(GPIO__PORT_1, 20);

  LCD_D0 = gpio__construct_as_output(GPIO__PORT_2, 2);
  LCD_D1 = gpio__construct_as_output(GPIO__PORT_2, 5);
  LCD_D2 = gpio__construct_as_output(GPIO__PORT_2, 7);
  LCD_D3 = gpio__construct_as_output(GPIO__PORT_0, 25);
  LCD_D4 = gpio__construct_as_output(GPIO__PORT_1, 30);
  LCD_D5 = gpio__construct_as_output(GPIO__PORT_1, 23);
  LCD_D6 = gpio__construct_as_output(GPIO__PORT_1, 29);
  LCD_D7 = gpio__construct_as_output(GPIO__PORT_2, 1);

  write_8_bit_mode(0b00110000, 0);
  write_8_bit_mode(0b00110000, 0);
  write_8_bit_mode(0b00110000, 0);

  write_8_bit_mode(0b00000001, 0); // clear
  write_8_bit_mode(0b00001100, 0); // on
  write_8_bit_mode(0b0100110, 0);  // mode
  write_8_bit_mode(0b00000010, 0); // home
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
  delay__ms(delay);
  gpio__set(LCD_EN);
  delay__ms(delay);
  gpio__reset(LCD_EN);
  delay__ms(delay);
}

void print_msg(char *write) {
  while (*write != '\0') // Print characters until end of line
  {
    write_8_bit_mode((uint8_t)atoi(write), 1);
    write++;
  }
}