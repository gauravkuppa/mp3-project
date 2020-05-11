#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "event_groups.h"
#include "ff.h"
#include "gpio.h"
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
#include "ssp0.h"
#include "ssp2.h"
#include "task.h"
#include "uart_lab.h"
#include "uart_printf.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef char songname_t[16];
typedef uint8_t mp3_data_blocks[512];

QueueHandle_t Q_songname;
QueueHandle_t Q_songdata;

xSemaphoreHandle_t spi_sem;

static QueueHandle_t sensor_data_queue;
static EventGroupHandle_t xEventGroup;
static EventBits_t uxBits;

static volatile uint8_t slave_memory[256];

gpio_s sck, miso, mosi, dreq, mp3cs, sdcs, xdcs, rst;

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
// Song list populate
// typedef char song_memory_t[128];
// static song_memory_t list_of_songs[32];
// static size_t number_of_songs;

// static void song_list__handle_filename(const char *filename) {
//   // This will not work for cases like "file.mp3.zip"
//   if (NULL != strstr(filename, ".mp3")) {
//     // printf("Filename: %s\n", filename);

//     // Dangerous function: If filename is > 128 chars, then it will copy
//     extra
//     // bytes leading to memory corruption
//     strcpy(list_of_songs[number_of_songs],
//     // filename);

//     // Better: But strncpy() does not guarantee to copy null char if max
//     length
//     // encountered So we can manually subtract 1 to reserve as NULL char
//     strncpy(list_of_songs[number_of_songs], filename,
//             sizeof(song_memory_t) - 1);

//     // Best: Compensates for the null, so if 128 char filename, then it
//     copies
//     // 127 chars, AND the NULL char snprintf(list_of_songs[number_of_songs],
//     // sizeof(song_memory_t), "%.149s", filename);

//     ++number_of_songs;
//     // or
//     // number_of_songs++;
//   }
// }

// void song_list__populate(void) {
//   FRESULT res;
//   static FILINFO file_info;
//   const char *root_path = "/";

//   DIR dir;
//   res = f_opendir(&dir, root_path);

//   if (res == FR_OK) {
//     for (;;) {
//       res = f_readdir(&dir, &file_info); /* Read a directory item */
//       if (res != FR_OK || file_info.fname[0] == 0) {
//         break; /* Break on error or end of dir */
//       }

//       if (file_info.fattrib & AM_DIR) {
//         /* Skip nested directories, only focus on MP3 songs at the root */
//       } else { /* It is a file. */
//         song_list__handle_filename(file_info.fname);
//       }
//     }
//     f_closedir(&dir);
//   }
// }

// size_t song_list__get_item_count(void) { return number_of_songs; }

// const char *song_list__get_name_for_item(size_t item_number) {
//   const char *return_pointer = "";

//   if (item_number >= number_of_songs) {
//     return_pointer = "";
//   } else {
//     return_pointer = list_of_songs[item_number];
//   }

//   return return_pointer;
// }
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
      // printf("Received song to play: %s\n", name);

      const char *filename = &name;
      uint8_t counter = 0;
      FIL file;
      UINT bytes_written = 0;

      FRESULT result = f_open(&file, filename, (FA_READ));

      if (result == FR_OK) {

        while (!f_eof(&file)) {
          // printf("About to read\n");

          memset(bytes_512, 0, sizeof(bytes_512));

          if (FR_OK == f_read(&file, mp3_data_block, sizeof(mp3_data_block),
                              &bytes_written)) {
            // printf("read file %x\n", &mp3_data_block[0]);

            // printf("about to send\n");
            xQueueSend(Q_songdata, mp3_data_block, portMAX_DELAY);
            // printf("%x", mp3_data_block);mp3_data_block // testing print
            // vTaskDelay(300);

            if (xQueueReceive(Q_songname, &name[0], 0)) {
              break;
            }
          } else {
            printf("Error: Failed to read\n");
          }
        }
      } else {
        printf("Error: Failed to open file\n");
      }

      f_close(&file);
    } // if end
  }   // while loop end
}

// Player task receives song data over Q_songdata to send it to the MP3 decoder
void mp3_player_task(void *p) {

  mp3_data_blocks mp3_data_block;
  // char bytes_512[512];
  // uint8_t response = 0;

  while (1) {

    if (xQueueReceive(Q_songdata, &mp3_data_block[0], portMAX_DELAY)) {
      // printf("recieved song data: %d\n", sizeof(mp3_data_block));
      // xSemaphoreGive(spi_sem);
      gpio__reset(xdcs); // set xdcs low

      // fprintf(stderr, "size: %d \n", sizeof(mp3_data_block));

      for (int i = 0; i < sizeof(mp3_data_block) / 32; i++) {
        // fprintf(stderr, "i: %d \n", i);
        while (!gpio__get(dreq)) {
          printf("waiting for dreq\n");
        }
        for (int j = (i * 32); j < (i * 32) + 32; j++) {
          ssp0__exchange_byte(*(mp3_data_block + j));
        }
      }
      gpio__set(xdcs); // set xdcs high
      // xSemaphoreTake(spi_sem);
      // printf("out\n");
    }
  }
}
// void volume_task(void *p){





// }
// Used to initialize the decoder

void decoder_init(uint8_t address, uint16_t value) {

  uint8_t highVal = (value >> 8);
  uint8_t lowVal = value;

  fprintf(stderr, "in the init\n");
  fprintf(stderr, "high: %x, low:%x \n", highVal, lowVal);

  gpioN__set(0, 6, false); // select MP3 SCI

  ssp0__exchange_byte(0x02); // WRITE_CODE = 0x02
  ssp0__exchange_byte(address);
  ssp0__exchange_byte(highVal); // high byte
  ssp0__exchange_byte(lowVal);  // low byte
  // wait until DREQ becomes 1
  while (!gpioN__get_level(0, 8))
    ;
  gpioN__set(0, 6, true); // deselect MP3 SCI
}

void mp3write(uint8_t address, uint8_t highB, uint8_t lowB) {
  while (!gpio__get(dreq)) {
    printf("waiting for dreq in sci\n");
  }

  gpio__reset(mp3cs); // set mp3cs low (select)

  ssp0__exchange_byte(0x02);
  ssp0__exchange_byte(address);
  ssp0__exchange_byte(highB);
  ssp0__exchange_byte(lowB);

  while (!gpio__get(dreq)) {
    printf("waiting for dreq in sci\n");
  }

  gpio__set(mp3cs); // set mp3cs high (deselect)
}

void reset() {
  gpioN__set(2, 4, true);
  delay__ms(1);
  gpioN__set(2, 4, false);
}

unsigned int mp3read(unsigned char addressbyte) {
  while (!gpio__get(dreq))
    ;
  gpio__reset(mp3cs); // Select control

  // SCI consists of instruction byte, address byte, and 16-bit data word.
  ssp0__exchange_byte(0x03); // Read instrucction
  ssp0__exchange_byte(addressbyte);

  char response1 = ssp0__exchange_byte(0xFF); // Read the first byte
  while (!gpio__get(dreq))
    ;
  char response2 = ssp0__exchange_byte(0xFF); // Read the second byte
  while (!gpio__get(dreq))
    ;
  gpio__set(mp3cs);

  int resultvalue = response1 << 8;
  resultvalue |= response2;
  return resultvalue;
}

void lcd_task() {
  init();
  start();
  char *s = "hello";
  print_string(s);
  // backlight_color(255, 0, 0);
  // backlight_off();
  // backlight_on();

  while (1) {
  }
}

int main(void) {
  printf("in main\n");
  //printf("populating songs...\n");
  //song_list__populate();
  //for (size_t song_number = 0; song_number < song_list__get_item_count(); song_number++) {
  //  printf("Song %2d: %s\n", (1 + song_number), song_list__get_name_for_item(song_number));
  //}
  // vTaskDelay(1000);
  /**ssp2__initialize(24000);
  xTaskCreate(sj2_cli__init, "cli", (2048 / sizeof(void *)), NULL, 1, NULL);

  Q_songname = xQueueCreate(1, sizeof(songname_t));
  Q_songdata = xQueueCreate(1, 512);

  sck = gpio__construct_with_function(
      GPIO__PORT_0, 15, GPIO__FUNCTION_2); // SSP0 is function 010, SCK
  miso = gpio__construct_with_function(
      GPIO__PORT_0, 17, GPIO__FUNCTION_2); // SSP0 is function 010, MISO
  mosi = gpio__construct_with_function(
      GPIO__PORT_0, 18, GPIO__FUNCTION_2); // SSP0 is function 010, MOSI

  // GPIO output, reset
  rst = gpio__construct_with_function(GPIO__PORT_2, 4, 0);
  gpio__set_as_output(rst);

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
  sdcs = gpio__construct_with_function(GPIO__PORT_1, 31, 1);
  gpio__set_as_output(sdcs);

  // explicitly deactivate slaves and initiate reset
  gpio__set(mp3cs);
  gpio__set(xdcs);
  gpio__set(sdcs);
  gpio__reset(rst);
  delay__ms(2);

  // ssp__init(24);          // ssp1 not sure what this is used for
  ssp0__initialize(1000); // ssp0 driver sets in KHz. Set to 1Mhz (internal
                          // clock is 12 MHz - SCI reads at clock/7 - initial
                          // commands should not be faster than 1.7 MHz)

  ssp0__exchange_byte(0xFF);
  gpio__set(rst);
  delay__ms(50);

  mp3write(0x00, 0x88, 0x00); // output mode
  // mp3write(0x03, 0x60, 0x00); // output mode
  // mp3write(0x0B, 0x10, 0x10); // set volume
  // mp3write(0x02, 0x7A, 0x00); // set bass
  // decoder_init(0x2, 0x7A00); // bass
  // decoder_init(0x3, 0x6000); // clk

  gpio__set(sdcs);  // set sdcs high
  gpio__set(xdcs);  // set xdcs high
  gpio__set(mp3cs); // deselect mp3 functionality

  int MP3Mode = mp3read(0x00);
  int MP3Status = mp3read(0x01);
  int MP3Clock = mp3read(0x03);
  int version = (MP3Status >> 4) & 0x000F;

  fprintf(stderr, "clock: %x \n", MP3Clock);
  fprintf(stderr, "mode: %x \n", MP3Mode);

  mp3write(0x00, 0x48, 0x10);
  MP3Mode = mp3read(0x00);

  // mp3write(0x03, 0x88, 0x00); // clock
  mp3write(0x03, 0x60, 0x00);
  MP3Clock = mp3read(0x03);
  ssp0__set_max_clock(6000); // increase spi clock

  // reset();
  delay__ms(10);
  xTaskCreate(mp3_reader_task, "reader", (4096 / sizeof(void *)), NULL, 1,
              NULL);
  xTaskCreate(mp3_player_task, "player", (4096 / sizeof(void *)), NULL, 2,
              NULL);

  fprintf(stderr, "mode: %x \n", MP3Mode);
  fprintf(stderr, "status: %x \n", MP3Status);
  fprintf(stderr, "clock: %x \n", MP3Clock);
  fprintf(stderr, "version: %x \n", version);**/
  xTaskCreate(lcd_task, "lcd", (4096 / sizeof(void *)), NULL, 1, NULL);
  xTaskCreate(sj2_cli__init, "cli", (2048 / sizeof(void *)), NULL, 1, NULL);
  // uint8_t value = 0x28;
  // i2c__write_slave_data(I2C__2, 0xC4, 0x80, &value, 2);
  // init();
  // start();
  // backlight_on();
  // backlight_off();

  fprintf(stderr, "starting scheduler\n");

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
      printf("sent");
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
      printf("sent");
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    printf("ERROR: Failed to open: %s because %d\n", filename, result);
  }
}
