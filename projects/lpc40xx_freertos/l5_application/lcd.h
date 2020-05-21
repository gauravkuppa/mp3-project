#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include <stdio.h>

typedef char song_memory_t[128];
typedef char songname_t[16];

void lcd_init();
void lcd_write_char(char c);
void lcd_write_string(char *s);
void lcd_clear_display();
void lcd_set_cursor(int row, int col);
void lcd_build_menu(song_memory_t *arr);
void lcd_move_menu(song_memory_t *arr, int length, int current_cursor_index,
                   int current_song_index);
void lcd_now_playing(song_memory_t *arr, int current_song_index);
void write_8_bit_mode(uint8_t command, uint8_t rs_value);
void pulse_clock(uint8_t delay);
