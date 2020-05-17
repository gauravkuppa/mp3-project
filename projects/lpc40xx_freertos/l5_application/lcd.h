#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include <stdio.h>

void lcd_init();
void lcd_write_char(char c);
void lcd_write_string(char *s);
void lcd_clear_display();
void lcd_set_cursor(int row, int col);
void led_up(int index);
void led_down(int index);
void write_8_bit_mode(uint8_t command, uint8_t rs_value);
void pulse_clock(uint8_t delay);
