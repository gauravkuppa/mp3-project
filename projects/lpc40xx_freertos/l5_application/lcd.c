// #include "lcd.h"

// /***
//  * Initialize pins of the PIC associated to the LCD
//  * and the LCD itself.
//  */
// void LCDInit() {
//   // Init pins to 0:
//   LCD_EN = 0;
//   LCD_RS = 0;
//   LCD_D = 0;
//   // Set pins as output:
//   TRISAbits.TRISA0 = 0;
//   TRISAbits.TRISA1 = 0;
//   TRISB = 0;
//   // Init LCD:
//   LCDCommand(LCD_CLEAR);
//   LCDCommand(LCD_ON);
//   LCDCommand(LCD_MODE);
//   LCDCommand(LCD_HOME);
// }

// /***
//  * Clear the LCD and move cursor to origin
//  */
// void LCDClear() {
//   // Send clear command:
//   LCDCommand(LCD_CLEAR);
//   LCDCommand(LCD_HOME);
// }

// /***
//  * Send a command to the LCD display
//  */
// void LCDCommand(uint8_t cmd) {
//   LCD_D = cmd;
//   LCD_RS = 0; // Cmd mode
//   LCD_EN = 1;
//   __delay_ms(5);
//   LCD_EN = 0;
// }

// /***
//  * Send a character to display
//  */
// void LCDWriteChar(unsigned char data) {
//   LCD_D = data;
//   LCD_RS = 1; // data mode
//   LCD_EN = 1;
//   __delay_ms(5);
//   LCD_EN = 0;
// }

// /***
//  * Write a String on the display
//  */
// void LCDWriteString(unsigned char *str) {
//   while (*str != '\0') // Print characters until end of line
//   {
//     LCDWriteChar(*str);
//     str++;
//   }
// }

// /* First line addresses: from 0x00 to 0x27
//  Second line addresses: from 0x40 to 0x67 */
// void LCDMoveCursor(uint8_t line, uint8_t column) {
//   uint8_t AC = 0x80;
//   if (line > 1 || column > 15) {
//     return;
//   }
//   AC += line * 0x40;
//   AC += column;
//   LCDCommand(AC);
// }