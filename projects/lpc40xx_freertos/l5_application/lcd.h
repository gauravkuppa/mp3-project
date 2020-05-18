#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include <stdio.h>

typedef char song_memory_t[128];

void lcd_init();
void lcd_write_char(char c);
void lcd_write_string(char *s);
void lcd_clear_display();
void lcd_set_cursor(int row, int col);
void lcd_build_menu(song_memory_t *arr);
void lcd_up(song_memory_t *arr, int index);
void lcd_down(song_memory_t *arr, int index);
void write_8_bit_mode(uint8_t command, uint8_t rs_value);
void pulse_clock(uint8_t delay);
