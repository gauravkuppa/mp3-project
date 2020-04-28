#include "FreeRTOS.h"
#include "acceleration.h"
#include "adc.h"
#include "board_io.h"
#include "common_macros.h"
#include "event_groups.h"
#include "ff.h"
#include "gpio_isr.h"
#include "gpio_lab.h"
#include "i2c.h"
#include "i2c_slave_functions.h"
#include "i2c_slave_init.h"
#include "lcd.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "pwm1.h"
#include "queue.h"
#include "semphr.h"
#include "sj2_cli.h"
#include "ssp1.h"
#include "ssp2.h"
#include "task.h"
#include "uart_lab.h"
#include "uart_printf.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <xc.h>

#define _XTAL_FREQ 8000000 // System clock frequency

typedef char songname_t[16];
typedef uint8_t mp3_data_blocks[512];

QueueHandle_t Q_songname;
QueueHandle_t Q_songdata;

static QueueHandle_t sensor_data_queue;
static EventGroupHandle_t xEventGroup;
static EventBits_t uxBits;

static volatile uint8_t slave_memory[256];

gpio_s sck, miso, mosi, dreq, mp3cs, sdcs, xdcs;

// // CONFIG1
// #pragma config FOSC = HS   // Oscillator Selection bits (HS oscillator)
// #pragma config WDTE = OFF  // Watchdog Timer Enable bit (WDT disabled)
// #pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
// #pragma config MCLRE = \
//     ON // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is
//        // digital I/O, MCLR internally tied to VDD)
// #pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
// #pragma config LVP = OFF  // Low-Voltage Programming Enable bit (RB3/PGM pin
// has
//                           // PGM function, Low-Voltage Programming enabled)
// #pragma config CPD = \
//     OFF // Data EE Memory Code Protection bit (Code protection off)
// #pragma config WRT = \
//     OFF // Flash Program Memory Write Enable bits (Write protection off)
// #pragma config CCPMX = RB0 // CCP1 Pin Selection bit (CCP1 function on RB0)
// #pragma config CP = \
//     OFF // Flash Program Memory Code Protection bit (Code protection off)
// // CONFIG2
// #pragma config FCMEN = OFF // Fail-Safe Clock Monitor Enable bit (Fail-Safe
//                            // Clock Monitor disabled)
// #pragma config IESO = OFF // Internal External Switchover bit (Internal
// External
//                           // Switchover mode disabled)

// void test_lcd(void) {
//   OSCCONbits.SCS = 0b00; // Oscillator mode defined by FOSC<2:0>
//   ANSEL = 0x00;          // Set all I/O to digital I/O

//   PORTB = 0x00; // Preset port B to 0
//   TRISB = 0x00; // Set port B as output

//   LCDInit();

//   while (1) {
//     LCDWriteString("Hello           ");
//     __delay_ms(1000);
//     LCDMoveCursor(1, 0);
//     LCDWriteString("World!          ");
//     __delay_ms(1000);
//     LCDClear();
//     __delay_ms(1000);
//   }
//   return;
// }

bool i2c_slave_callback__read_memory(uint8_t memory_index, uint8_t *memory) {
  // TODO: Read the data from slave_memory[memory_index] to *memory pointer
  // TODO: return true if all is well (memory index is within bounds)
  bool status = true;

  // LPC_I2C2->ADRO0 |= memory_index;
  *memory = slave_memory[memory_index];

  if (LPC_I2C2->STAT == 0x78 || LPC_I2C2->STAT == 0xB0) {
  }

  return status;
}

bool i2c_slave_callback__write_memory(uint8_t memory_index,
                                      uint8_t memory_value) {
  // TODO: Write the memory_value at slave_memory[memory_index]
  // TODO: return true if memory_index is within bounds
  bool status = true;

  slave_memory[memory_index] = memory_value;

  if (LPC_I2C2->STAT == 0x78 || LPC_I2C2->STAT == 0xB0) {
    status = false;
  }

  return status;
}

void uart_read_task(void *p) {
  while (1) {
    // TODO: Use uart_lab__polled_get() function and printf the received value
    fprintf(stderr, "in read task\n");

    char *val;
    bool get = uart_lab__polled_get(UART_2, &val);

    // fprintf(stderr, "bool get: %d\n", get);
    fprintf(stderr, "get char %x\n", val);
    vTaskDelay(500);
  }
}

spi_send_to_mp3_decoder(mp3_data_blocks data_to_decoder) {}

// Reader tasks receives song-name over Q_songname to start reading it(sending
// it into the queue for player to recieve)
void mp3_reader_task(void *p) {
  printf("in\n");

  mp3_data_blocks mp3_data_block;
  songname_t name;
  char bytes_512[512];

  while (1) {
    printf("in while\n");

    if (xQueueReceive(Q_songname, &name[0], portMAX_DELAY)) {
      printf("Received song to play: %s\n", name);

      const char *filename = &name;
      uint8_t counter = 0;
      FIL file;
      UINT bytes_written = 0;

      FRESULT result = f_open(&file, filename, (FA_READ));

      if (result == FR_OK) {

        while (!f_eof(&file)) {
          printf("About to read\n");

          memset(bytes_512, 0, sizeof(bytes_512));

          if (FR_OK == f_read(&file, mp3_data_block, sizeof(mp3_data_block),
                              &bytes_written)) {
            printf("read file %x\n", &mp3_data_block[0]);

            printf("about to send");
            xQueueSend(Q_songdata, mp3_data_block[0], portMAX_DELAY);
            // printf("%x", mp3_data_block); // testing print
            vTaskDelay(300);

            if (xQueueReceive(Q_songname, &name[0], 0)) {
              break;
            }
          } else {
            printf("Error: Failed to read");
          }
        }
      } else {
        printf("Error: Failed to open file");
      }

      f_close(&file);
    } // if end
  }   // while loop end
}

// Player task receives song data over Q_songdata to send it to the MP3 decoder
void mp3_player_task(void *p) {

  mp3_data_blocks mp3_data_block;
  // char bytes_512[512];

  while (1) {

    if (xQueueReceive(Q_songdata, &mp3_data_block[0], portMAX_DELAY)) {
      printf("recieved song data: %d\n", sizeof(mp3_data_block));
      bool dreq = false;
      dreq = gpioN__get_level(0, 8); // if dreq is high it will be true`
                                     // printf("dreq: %d\n", dreq);

      if (dreq == true) {
        // printf("in mp3 if\n");
        gpioN__set(0, 26, false); // set xdcs low
        ssp__exchange_byte(0x2);  // op code
        // ssp__exchange_byte();     // address
        for (int i = 0; i < sizeof(mp3_data_block); i++) {

          if (0) {
            vTaskDelay(1);
          }
          // printf("output: %s ", c);
          // if (i > 2 && i < 33) {

          // printf("%x", *(mp3_data_block + i));
          // }
          // spi_send_to_mp3_decoder(bytes_512[i]);

          // for (int j = 0; j < 512; j++) {
          printf("data: %x", mp3_data_block[i]);
          ssp__exchange_byte(mp3_data_block[i]);
          //}

          // ssp2__dma_write_block(bytes_512[i], 512);
          // spi_send_to_mp3_decoder(mp3_data_block[i]);
        }
        gpioN__set(0, 26, true); // set xdcs high
      }

      printf("out\n");
    }
  }
}
// Used to initialize the decoder

void decoder_init(uint8_t address, uint16_t value) {
  gpioN__set(0, 6, false); // select MP3 SCI

  ssp__exchange_byte(0x02); // WRITE_CODE = 0x02
  ssp__exchange_byte(address);
  ssp__exchange_byte(value >> 8); // high byte
  ssp__exchange_byte(value);      // low byte

  gpioN__set(0, 6, true); // deselect MP3 SCI
  // wait until DREQ becomes 1
  while (!gpioN__get_level(0, 8))
    ;
}

int main(void) {

  xTaskCreate(sj2_cli__init, "cli", (2048) / sizeof(void *), NULL, 1, NULL);

  Q_songname = xQueueCreate(1, sizeof(songname_t));
  Q_songdata = xQueueCreate(1, 512);

  sck = gpio__construct_with_function(
      GPIO__PORT_0, 15, GPIO__FUNCTION_2); // SSP0 is function 010, SCK
  miso = gpio__construct_with_function(
      GPIO__PORT_0, 17, GPIO__FUNCTION_2); // SSP0 is function 010, MISO
  mosi = gpio__construct_with_function(
      GPIO__PORT_0, 18, GPIO__FUNCTION_2); // SSP0 is function 010, MOSI

  // GPIO output, mp3cs
  mp3cs = gpio__construct_with_function(GPIO__PORT_0, 6, 0);
  gpio__set_as_output(mp3cs);

  // GPIO input, dreq
  dreq = gpio__construct_with_function(GPIO__PORT_0, 8, 0);
  gpio__set_as_input(dreq);

  // GPIO output, xdcs
  xdcs = gpio__construct_with_function(GPIO__PORT_0, 26, 0);
  gpio__set_as_output(xdcs);

  // GPIO output, sdcs
  sdcs = gpio__construct_with_function(GPIO__PORT_1, 31, 0);
  gpio__set_as_output(sdcs);

  ssp__init(24);

  decoder_init(0x0, 0x0810); // mode
  decoder_init(0x2, 0x7A00); // bass
  decoder_init(0x3, 0xA000); // clk

  gpioN__set(0, 26, true); // set xdcs high
  gpioN__set(0, 6, false); // select mp3 functionality

  xTaskCreate(mp3_reader_task, "reader", (4000) / sizeof(void *), NULL, 1,
              NULL);
  xTaskCreate(mp3_player_task, "player", (4000) / sizeof(void *), NULL, 2,
              NULL);

  vTaskStartScheduler();

  return -1;
}

void write_file_using_fatfs_pi(bool inTime, char *input) {
  const char *filename = "logging.txt";
  FIL file; // File handle
  UINT bytes_written = 0;
  FILINFO fno;

  FRESULT result = f_open(&file, filename, (FA_OPEN_APPEND | FA_WRITE));

  if (FR_OK == result) {

    char string[128];

    if (inTime == true) {
      printf("in and input: %s\n", input);

      char time_data[32];

      sprintf(time_data, "Timestamp: %u/%02u/%02u, %02u:%02u\n",
              (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
              fno.ftime >> 11, fno.ftime >> 5 & 63);
      printf("%s\n", time_data);
      sprintf(string, "[%s] \t %s\n", time_data, input);
      printf("true\n");
      // sprintf(string, "%s", input);

    } else {
      fprintf(stderr, "false\n");
      sprintf(string, "%s", input);
    }
    printf("out");
    if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
      // printf("sent");
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    printf("ERROR: Failed to open: %s because %d\n", filename, result);
  }
}

void write_file_using_fatfs_pi_1(char *sensor_value) {
  const char *filename = "logging.txt";
  FIL file; // File handle
  UINT bytes_written = 0;

  FRESULT result = f_open(&file, filename, (FA_OPEN_APPEND | FA_WRITE));
  fprintf(stderr, "in write: %s\n", sensor_value);

  if (FR_OK == result) {

    char string[64];
    sprintf(string, "%li, %s\n", xTaskGetTickCount(), sensor_value);

    if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
      // printf("sent");
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    printf("ERROR: Failed to open: %s because %d\n", filename, result);
  }
}
