// #include <stdint.h>
// //#include <xc.h>

// #define _XTAL_FREQ 8000000 // System clock frequency
// /***    Send command:   RS=0, EN=1
//         Send data:      RS=1, EN=1 */

// /*** Define the pins used */
// #define LCD_EN PORTAbits.RA0
// #define LCD_RS PORTAbits.RA1
// #define LCD_D PORTB
// /*** Define commands */
// #define LCD_CLEAR 0x01
// #define LCD_ON 0x0C   // Display ON, cursor and blink OFF
// #define LCD_MODE 0x38 // 8-bit mode, 2 lines, 5x8dots characters
// #define LCD_HOME 0x02 // Moves cursor to line 0, column 0

// void LCDInit();
// void LCDClear();
// void LCDCommand(uint8_t cmd);
// void LCDWriteChar(unsigned char data);
// void LCDWriteString(unsigned char *str);
// void LCDMoveCursor(uint8_t line, uint8_t column);