#include <stdio.h>

void init_lcd();

void write_8_bit_mode(uint8_t command, uint8_t rs_value);
void pulse_clock(uint8_t delay);

void print_msg(char *write);