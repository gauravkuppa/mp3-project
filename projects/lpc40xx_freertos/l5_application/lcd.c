#if 1
#include "lcd.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include <stdlib.h>

void setReg(uint8_t addr, uint8_t dta) {

  i2c__write_single(I2C__2, RGB_ADDRESS, addr, dta);
}

void display(uint8_t _displaycontrol) {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void clear() {
  command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
  delay__ms(2000);           // this command takes a long time!
}

void command(uint8_t value) {
  // unsigned char dta[2] = {0x80, value};
  i2c__write_single(I2C__2, LCD_ADDRESS, 0x80, value); //?????????
  // i2c_send_byteS(dta, 2);
}

void setRGB(unsigned char r, unsigned char g, unsigned char b) {
  setReg(REG_RED, r);
  setReg(REG_GREEN, g);
  setReg(REG_BLUE, b);
}
void init_lcd() {

  LPC_IOCON->P0_10 &= ~(3 << 3);
  LPC_IOCON->P0_11 &= ~(3 << 3);

  i2c__initialize(I2C__2, 97000, 96000000);

  uint8_t _display;
  _display |= LCD_2LINE;

  delay__ms(50000);

  command(LCD_FUNCTIONSET | _display);
  delay__ms(4500); // wait more than 4.1ms

  // second try
  command(LCD_FUNCTIONSET | _display);
  delay__ms(150);

  // third go
  command(LCD_FUNCTIONSET | _display);

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _display);

  uint8_t _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display(_displaycontrol);

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  uint8_t _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

  // backlight init
  setReg(REG_MODE1, 0);
  // set LEDs controllable by both PWM and GRPPWM registers
  setReg(REG_OUTPUT, 0xFF);
  // set MODE2 values
  // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
  setReg(REG_MODE2, 0x20);

  setRGB(255, 255, 255);
  delay__ms(2000);
  setRGB(0, 0, 0);
}

#endif
