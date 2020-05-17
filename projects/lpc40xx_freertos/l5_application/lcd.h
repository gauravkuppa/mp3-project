#if 1
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include <stdio.h>

// i2c commands
#define CLEARDISPLAY 0x01
#define RETURNHOME 0x02
#define ENTRYMODESET 0x04
#define DISPLAYCONTROL 0x08
#define CURSORSHIFT 0x10
#define FUNCTIONSET 0x20
#define SETCGRAMADDR 0x40
#define SETDDRAMADDR 0x80

// Flags for display entry mode
#define ENTRYRIGHT 0x00
#define ENTRYLEFT 0x02
#define ENTRYSHIFTINCREMENT 0x01
#define ENTRYSHIFTDECREMENT 0x00

// Flags for display on/off control
#define DISPLAYON 0x04
#define DISPLAYOFF 0x00
#define CURSORON 0x02
#define CURSOROFF 0x00
#define BLINKON 0x01
#define BLINKOFF 0x00

// Flags for display/cursor shift
#define DISPLAYMOVE 0x08
#define CURSORMOVE 0x00
#define MOVERIGHT 0x04
#define MOVELEFT 0x00

// Flags for function set
#define EIGHTBITMODE 0x10
#define FOURBITMODE 0x00
#define TWOLINE 0x08
#define ONELINE 0x00
#define FIVExTENDOTS 0x04
#define FIVExEIGHTDOTS 0x00

#define En 0x04 // Enable bit
#define Rw 0x02 // Read/Write bit
#define Rs 0x01 // Register select bit

// I2C addresses for LCD and RGB backlight
#define DISPLAY_COLOR_ADDRESS 0x62
#define DISPLAY_TEXT_ADDRESS 0x3e
#define DISPLAY_COLOR_ADDRESS_1 0x7C
#define DISPLAY_TEXT_ADDRESS_1 0xC4

void init();
void start();
void clear();
void home();
void set_cursor(uint8_t col, uint8_t row);
void display_off();
void display_on();
void cursor_off();
void cursor_on();

void blink_off();
void blink_on();
void backlight_off();
void backlight_on();
void backlight_color(uint8_t red, uint8_t green, uint8_t blue);
void print_string(char *characters);

void _write4bits(uint8_t val);
void _pulseEnable(uint8_t data);
void _sendCommand(uint8_t value);
void _writeData(uint8_t value);
void _sendData(uint8_t val, uint8_t mode);

// trial code
static void lcd__clock_pulse(void);

static void lcd__set_databits(uint8_t value);

void lcd__write_value(uint8_t value);

void lcd__write_character(char c);

void lcd__clear_display(void);

void lcd__write_name(const char *name);

void lcd__write_continue(const char *value);

void initialize_lcd_pins(void);

void initialize_lcd_screen(void);

void lcd__set_cursor_position(uint8_t row, uint8_t col);

void lcd__cursor_move_left(void);

void lcd__cursor_move_right(void);

void lcd__show_levels(uint8_t volume_level, uint8_t treble_level,
                      uint8_t bass_level);

void lcd__write_digit(uint8_t digit);

#endif