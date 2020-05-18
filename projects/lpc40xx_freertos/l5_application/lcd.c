#include "lcd.h"
#include "delay.h"
#include "gpio.h"
#include <stdlib.h>
#include <string.h>

// #define EN (1 << 0)
// #define RW (1 << 2)
// #define RS (1 << 5)
// #define DB4 (1 << 1)
// #define DB5 (1 << 4)
// #define DB6 (1 << 6)
// #define DB7 (1 << 8)

gpio_s LCD_EN, LCD_RS, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6,
    LCD_D7;

void lcd_init() {
  // Init pins to 0:
  LCD_EN = gpio__construct_as_output(GPIO__PORT_2, 0);
  LCD_RS = gpio__construct_as_output(GPIO__PORT_2, 5);

  LCD_D0 = gpio__construct_as_output(GPIO__PORT_0, 16);
  LCD_D1 = gpio__construct_as_output(GPIO__PORT_2, 7);
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

void lcd_write_char(char c) { write_8_bit_mode((uint8_t)c, 1); }

void lcd_write_string(char *s) {
  int len = strlen(s);
  for (int i = 0; i < len; i++) {
    lcd_write_char(s[i]);
  }
}

void lcd_clear_display() { write_8_bit_mode(0x01, 0); }

void lcd_set_cursor(int row, int col) {
  uint8_t position = (col + (row * 40)) + 0x80;
  write_8_bit_mode(position, 0);
}

void lcd_up(song_memory_t *arr, int index) {
  /*
     _____________________________________________
    |                                            |
    |         song_i.mp3                         |
    |                                            |
    |         song_i+1.mp                        |
    |                                            |
    |____________________________________________|

    to
     _____________________________________________
    |                                            |
    |         song_i+1.mp3                       |
    |                                            |
    |         song_i+2.mp                        |
    |                                            |
    |____________________________________________|


  */

  // check edge conditions !!!!
  index++;
  printf("From LCD up: %s\n", arr[0]);
  printf("From LCD up: %s\n", arr[1]);

  lcd_clear_display();
  lcd_set_cursor(0, 0);
  lcd_write_string(arr[index]);
  lcd_set_cursor(1, 0);
  lcd_write_string(arr[index + 1]);
}

void lcd_build_menu(songname_t *arr) {
  for (int i = 0; i < 4; i++) {
    printf("in lcd_task, print options: %s\n", (&arr[i]));
  }

  int index = 0;
  printf("From LCD up: %s\n", arr);
  printf("From LCD up: %s\n", (arr + 1));
  lcd_clear_display();
  lcd_set_cursor(0, 0);
  lcd_write_string(arr[index]);
  lcd_set_cursor(1, 0);
  lcd_write_string(arr[index + 1]);
}
void lcd_down(song_memory_t *arr, int index) {
  /*
     _____________________________________________
    |                                            |
    |         song_i.mp3                         |
    |                                            |
    |         song_i+1.mp3                        |
    |                                            |
    |____________________________________________|

    to
     _____________________________________________
    |                                            |
    |         song_i-1.mp3                       |
    |                                            |
    |         song_i.mp3                         |
    |                                            |
    |____________________________________________|


  */

  // check edge conditions !!!!!!

  index--;
  printf("From LCD up: %s\n", arr[0]);
  printf("From LCD up: %s\n", arr[1]);

  lcd_clear_display();
  lcd_set_cursor(0, 0);
  lcd_write_string(arr[index]);
  lcd_set_cursor(1, 0);
  lcd_write_string(arr[index + 1]);
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

  pulse_clock(10);
}

void pulse_clock(uint8_t delay) {
  vTaskDelay(delay);
  gpio__set(LCD_EN);
  vTaskDelay(delay);
  gpio__reset(LCD_EN);
  vTaskDelay(delay);
}
