#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "event_groups.h"
#include "ff.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "gpio_lab.h"
//#include "i2c.h"
//#include "i2c_slave_functions.h"
//#include "i2c_slave_init.h"
#include "lcd.h"
//#include "lcd_lab.c"
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
TaskHandle_t xReader;
QueueHandle_t Q_songname;
QueueHandle_t Q_songdata;
SemaphoreHandle_t play_sem;
SemaphoreHandle_t spi_sem;
SemaphoreHandle_t lcd_sem_up;
SemaphoreHandle_t lcd_sem_down;
SemaphoreHandle_t lcd_sem_play;
SemaphoreHandle_t lcd_sem_back;
SemaphoreHandle_t lcd_sem_select;
//SemaphoreHandle_t skip_sem;
//SemaphoreHandle_t previous_sem;
//

static QueueHandle_t sensor_data_queue;
static EventGroupHandle_t xEventGroup;
static EventBits_t uxBits;
static volatile uint8_t slave_memory[256];
//clock_t start, end, dif;

gpio_s sck, miso, mosi, dreq, mp3cs, sdcs, xdcs, rst, gpio30, gpio29, gpio01,
    gpio07, gpio09, gpio10, gpio25;

// typedef struct {
//   uint8_t ports;
//   uint8_t pins;
// } port_pin_s;

// void pin29_isr(void) {
//   // uart_printf__polled(UART__0, "P0.29");
//   // delay__ms(100);
//   lcd_move_menu(&list_of_songs, 3, index--, 0);
// }
// void pin30_isr(void) {
//   // uart_printf__polled(UART__0, "P0.30");
//   // delay__ms(100);
//   lcd_move_menu(&list_of_songs, 3, index++, 0);
// }

// void task2main(void) {
//   //    port_pin_s gpio30 = {0,
//   //                               30}; // struct used to set input from
//   //                               gpio_lab.c
//   //   static port_pin_s gpio29 = {0, 29};
//   // struct used to set as input from gpio_lab.c

//   // void gpio__set_as_input(gpio_s gpio);
//   gpio30 = gpio__construct_with_function(GPIO__PORT_0, 30, 0);
//   gpio__set_as_input(gpio30); // set pin30 to input
//   // // gpio__set_as_output(LED2);     // sets led2 to output
//   gpio29 = gpio__construct_with_function(GPIO__PORT_0, 29, 0);
//   gpio__set_as_input(gpio29); // set pin29 to input

//   // gpio__set_as_output(LED3);     // set led3 to output
//   LPC_IOCON->P0_30 &= ~(3 << 3); // reset resistor for p30
//   LPC_IOCON->P0_30 |= (1 << 3);  // set resistor for p30
//   LPC_IOCON->P0_29 &= ~(3 << 3); // reset resistor for p29
//   LPC_IOCON->P0_29 |= (1 << 3);  // set p29
//   gpio0__attach_interrupt(30, GPIO_INTR__RISING_EDGE, pin30_isr);
//   gpio0__attach_interrupt(29, GPIO_INTR__FALLING_EDGE, pin29_isr);
//   lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO,
//                                    gpio0__interrupt_dispatcher);
//   // while (1) {
//   //   uart_printf__polled(UART__0, "In Main: Idle Looping\n");
//   //   delay__ms(1000);
//   // }
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
// Song list populate
typedef char song_memory_t[128];
static song_memory_t list_of_songs[32];
static size_t number_of_songs;

static void song_list__handle_filename(const char *filename) {
  // This will not work for cases like "file.mp3.zip"
  if (NULL != strstr(filename, ".mp3")) {
    // printf("Filename: %s\n", filename);

    // Dangerous function: If filenalist_of_songsme is > 128 chars, then it will
    // copy extra bytes leading to memory corruption
    // strcpy(list_of_songs[number_of_songs], filename);

    // Better: But strncpy() does not guarantee to copy null char if max length
    // encountered So we can manually subtract 1 to reserve as NULL char
    strncpy(list_of_songs[number_of_songs], filename,
            sizeof(song_memory_t) - 1);

    // Best: Compensates for the null, so if 128 char filename, then it copies
    // 127 chars, AND the NULL char snprintf(list_of_songs[number_of_songs],
    // sizeof(song_memory_t), "%.149s", filename);

    ++number_of_songs;
    // or
    // number_of_songs++;
  }
}

void song_list__populate(void) {
  FRESULT res;
  static FILINFO file_info;
  const char *root_path = "/";

  DIR dir;
  res = f_opendir(&dir, root_path);

  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &file_info); /* Read a directory item */
      if (res != FR_OK || file_info.fname[0] == 0) {
        break; /* Break on error or end of dir */
      }

      if (file_info.fattrib & AM_DIR) {
        /* Skip nested directories, only focus on MP3 songs at the root */
      } else { /* It is a file. */
        song_list__handle_filename(file_info.fname);
      }
    }
    f_closedir(&dir);
  }
}

size_t song_list__get_item_count(void) { return number_of_songs; }

const char *song_list__get_name_for_item(size_t item_number) {
  const char *return_pointer = "";

  if (item_number >= number_of_songs) {
    return_pointer = "";
  } else {
    return_pointer = list_of_songs[item_number];
  }

  return return_pointer;
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
            // xSemaphoreGive(play_sem);
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
      // xSemaphoreTake(spi_sem);
      gpio__reset(xdcs); // set xdcs low
      // fprintf(stderr, "size: %d \n", sizeof(mp3_data_block));
      for (int i = 0; i < sizeof(mp3_data_block) / 32; i++) {
        // fprintf(stderr, "i: %d \n", i);
        while (!gpio__get(dreq)) {
          // printf("waiting for dreq\n");
        }
        for (int j = (i * 32); j < (i * 32) + 32; j++) {
          ssp0__exchange_byte(*(mp3_data_block + j));
        }
      }
      gpio__set(xdcs); // set xdcs high
      // xSemaphoreGive(spi_sem);
      // xSemaphoreGive(player_sem);
      // printf("out\n");
    }
  }
}
// void volume_task(void *p){

// }

//////////////// Need to define mins and maxes of volume, bass, and treble,
//////////////// link lcd and play functions
#if 1
int selectedsong = -1;
int cursor = 0;
bool playing = false;
bool on_start = true;
bool in_main = true;
bool in_song = false;
bool adjust_volume = false;
bool adjust_treble = false;
bool adjust_bass = false;
const uint8_t volumeMAX = 0x00;
const uint8_t volumeMIN = 0xFF;
const int8_t trebleMAX = 0b10000110;
const int8_t trebleMIN = 0b10010110;
const uint8_t bassMAX = 0xF6;
const uint8_t bassMIN = 0x06;
uint8_t volume = 0x20;
uint16_t bassreg = 0x0000;
uint8_t bass = 0x06;
int8_t treble = 0x00000110;
int isStart = 0;
bool paused = false;

static song_memory_t options[4] = {"songs", "volume", "treble", "bass"};

//void skip(void){
//    if (selectedsong != 8)
//        selectedsong = selectedsong + 1;
//    else if (selectedsong == -1){
//        on_start = false;
//        selectedsong = 0;
//    }
//    else
//        selectedsong = 0;
//    if(paused){
//        xSemaphoreGive(play_sem);
//    }
//    xQueueSend(Q_songname, list_of_songs[selectedsong], portMAX_DELAY);
//    playing = true;
//
//}
//void previous(void){
//    if (selectedsong != 0)
//        selectedsong = selectedsong - 1;
//    else if (selectedsong == -1){
//        on_start = 0;
//        selectedsong = 0;
//    }
//    else
//        selectedsong = 8;
//    if(paused){
//        xSemaphoreGive(play_sem);
//    }
//    xQueueSend(Q_songname, list_of_songs[selectedsong], portMAX_DELAY);
//    playing = true;
//}
void buttonup(void) {
  if (in_main) {
    if (adjust_volume || adjust_treble || adjust_bass) {
      if (adjust_volume) {
        if (volume == volumeMAX) {
          ;
        } else {
          volume = volume - 0x10;
          mp3write(0x0B, volume, volume);
        }
      }
      if (adjust_treble) {
        if (treble == trebleMAX) {
          ;
        } else {
          treble = treble + 0b10000;
          // fprintf(stderr, "treble write: %x\n", treble);
          mp3write(0x02, treble, bass);
        }
      }
      if (adjust_bass) {
        if (bass == bassMAX) {
          ;
        } else {
          bass = bass + 0x10;
          // fprintf(stderr, "bass write: %x\n", bass);
          mp3write(0x02, treble, bass);
        }
      }
    } else {
      if (cursor == 3) {
        ;
        // adjust_lcd_screen();
      } else {
        fprintf(stderr, "lcd_move_up options\n");
        lcd_move_menu(&options, 4, ++cursor, -1);
      }
    }
  } else if (in_song) {

    if (cursor == 8) {
      ;
      // adjust_lcd_screen();
    } else {
      fprintf(stderr, "lcd_move_up songlist\n");
      fprintf(stderr, "cursor: %d, selected_song: %d", cursor + 1,
              selectedsong);
      lcd_move_menu(&list_of_songs, 8, ++cursor, selectedsong);
      // adjust_lcd_screen();
    }
  }
}
void buttondown(void) {
  if (in_main) {
    if (adjust_volume || adjust_treble || adjust_bass) {
      if (adjust_volume) {
        if (volume == volumeMIN) {
          ;
        } else {
          volume = volume + 0x10;
          mp3write(0x0B, volume, volume);
        }
      }
      if (adjust_treble) {
        if (treble == trebleMIN) {
          ;
        } else {
          treble = treble - 0b10000;

          fprintf(stderr, "treble write: %x\n", treble);
          mp3write(0x02, treble, bass);
        }
      }
      if (adjust_bass) {
        if (bass == bassMIN) {
          ;
        } else {
          bass = bass - 0x10;
          fprintf(stderr, "bass write: %x\n", bass);
          mp3write(0x02, treble, bass);
        }
      }
    } else {
      if (cursor == 0) {
        ;
      } else {
        fprintf(stderr, "lcd_move_down options\n");
        lcd_move_menu(&options, 4, --cursor, -1);
        // adjust_lcd_screen();
      }
    }
  } else if (in_song) {
    if (cursor == 0) {
      ;
      // adjust_lcd_screen();
    } else {
      // adjust_lcd_screen();
      fprintf(stderr, "lcd_move_down songlist\n");
      fprintf(stderr, "cursor: %d, selected_song: %d", cursor + 1,
              selectedsong);
      lcd_move_menu(&list_of_songs, 8, --cursor,
                    selectedsong); // lcd scroll down
      fprintf(stderr, "%s\n", options[cursor]);
      // fprintf(stderr, "%s\n", options[cursor]);//
    }
    //
    fprintf(stderr, "%s\n", options[cursor]);
    // fprintf(stderr, "%s\n", options[cursor]);  }
  }
}

/// pause_play only pauses or plays the current song, regardless of what menu
/// you are on. select button lets you play a different song when you are on
/// the song menu.

void pause_play(void) {
  if (playing == true) {
    fprintf(stderr, "about to send play_sem\n");
    xSemaphoreGive(play_sem);

    playing = false;
  } else if (on_start == true) {
    xQueueSend(Q_songname, list_of_songs[0], portMAX_DELAY);
    playing = true;
    selectedsong = 0;
  } else {
    fprintf(stderr, "about to send play_sem\n");
    xSemaphoreGive(play_sem);
    playing = true;
  }
  on_start = false;
}

void select(void) {
  if (in_main) {
    if (cursor == 0) {
      in_main = false;
      in_song = true;
      cursor = 0;
      lcd_build_menu(&list_of_songs);
      // adjust_lcd_screen();
    } else if (cursor == 1) {
      in_main = true;
      adjust_volume = true;
      // adjust_lcd_screen();
    } else if (cursor == 2) {
      in_main = true;
      adjust_treble = true;
      // adjust_lcd_screen();
    } else if (cursor == 3) {
      in_main = true;
      adjust_bass = true;
      // adjust_lcd_screen();
    }
  } else if (in_song) {
    if (paused) {
      xSemaphoreGive(play_sem);
    }
    selectedsong = cursor;
    fprintf(stderr, "selected_song index: %d", selectedsong);
    lcd_now_playing(&list_of_songs, selectedsong);
    vTaskDelay(2000);
    lcd_move_menu(&list_of_songs, 8, cursor, selectedsong);
    // fprintf(stderr, "selectedsong = %c\n", list_of_songs[cursor]);
    // lcd_set_cursor(cursor % 2, strlen(list_of_songs[cursor]) + 1);
    // lcd_write_string(" <");
    xQueueSend(Q_songname, list_of_songs[cursor], portMAX_DELAY);
    playing = true;
    on_start = false;
  }
}
void back(void) {
  if (in_main) {
    if (adjust_volume) {
      adjust_volume = false;
      cursor = 1;
    } else if (adjust_treble) {
      adjust_treble = false;
      cursor = 2;
    } else if (adjust_bass) {
      adjust_bass = false;
      cursor = 3;
    } else
      ;
  } else if (in_song) {
    in_main = true;
    in_song = false;
    lcd_build_menu(&options);
    cursor = 0;
  }
}

#endif

// Used to write sci commands
void mp3write(uint8_t address, uint8_t highB, uint8_t lowB) {
  xSemaphoreTake(spi_sem, 1000);
  ssp0__set_max_clock(1000);
  while (!gpio__get(dreq)) {
    // printf("waiting for dreq in sci\n");
  }

  gpio__reset(mp3cs); // set mp3cs low (select)

  ssp0__exchange_byte(0x02);
  ssp0__exchange_byte(address);
  ssp0__exchange_byte(highB);
  ssp0__exchange_byte(lowB);

  while (!gpio__get(dreq)) {
    // printf("waiting for dreq in sci\n");
  }

  gpio__set(mp3cs); // set mp3cs high (deselect)
  ssp0__set_max_clock(6000);
  xSemaphoreGive(spi_sem);
}

void reset() {
  gpioN__set(2, 4, true);
  delay__ms(1);
  gpioN__set(2, 4, false);
}

unsigned int mp3read(unsigned char addressbyte) {
  xSemaphoreTake(spi_sem, 1000);
  ssp0__set_max_clock(1000);
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
  ssp0__set_max_clock(6000);
  xSemaphoreGive(spi_sem);
  int resultvalue = response1 << 8;
  resultvalue |= response2;
  return resultvalue;
}

void testbuttons() {
  buttondown(); // volume
  buttondown(); // treble
  select();     // adjust treble
  buttondown(); // decrease treble
  uint16_t temp = 0x0000;
  temp = mp3read(0x02); // read treble
  fprintf(stderr, "read treble: %x\n", temp);
  back();               // main
  buttondown();         // bass
  select();             // adjust bass
  buttonup();           // increase bass
  temp = mp3read(0x02); // read bass
  fprintf(stderr, "read bass: %x\n", temp);
}

void song_control_task(void *p) {
  fprintf(stderr, "in song_control\n");
  // eTaskState status;
  while (1) {
    // status = eTaskGetState(xReader);
    fprintf(stderr, "paused: %d", paused);
    if (xSemaphoreTake(play_sem, 1000)) {
      if (!paused) {
        fprintf(stderr, "suspending song\n");
        paused = true;
        vTaskSuspend(xReader);
      } else if (paused) {
        fprintf(stderr, "resuming song\n");
        paused = false;
        vTaskResume(xReader);
      } else {
        fprintf(stderr, "pausing/playing failed\n");
        fprintf(stderr, "status: %d\n", paused);
      }
    }
    vTaskDelay(10);
  }
}

void button_interrupt_task() {
  lcd_init();

  if (!isStart) {
    lcd_build_menu(&options);
    isStart = 1;
  }
  while (1) {

    if (xSemaphoreTake(lcd_sem_down, 1000)) {
      fprintf(stderr, "in up\n");
      buttondown();
    } else if (xSemaphoreTake(lcd_sem_up, 1000)) {
      fprintf(stderr, "in down\n");
      buttonup();
    } else if (xSemaphoreTake(lcd_sem_play, 1000)) {
      fprintf(stderr, "in play/pause\n");
      pause_play();
    } else if (xSemaphoreTake(lcd_sem_select, 1000)) {
      fprintf(stderr, "in select\n");
      select();
    } else if (xSemaphoreTake(lcd_sem_back, 1000)) {
      fprintf(stderr, "in main\n");
      back();
    }
//    else if (xSemaphoreTake(skip_sem, 1000)){
//        fprintf(stderr, "in skip\n");
//        skip();
//    } else if (xSemaphoreTake(previous_sem, 1000)){
//        fprintf(stderr, "in previous\n");
//        previous();
//    }
    else
      fprintf(stderr, "button idling\n");
  }
}

// void pin29_isr(void) {
//   // uart_printf__polled(UART__0, "P0.29");
//   // delay__ms(100);
//   // lcd_move_menu(&list_of_songs, 3, 0, 0); // up
//   uart_printf__polled(UART__0, "in handler 29\n");
//   xSemaphoreGiveFromISR(lcd_sem_up, NULL);
// }
// void pin30_isr(void) {
//   // uart_printf__polled(UART__0, "P0.30");
//   // delay__ms(100);
//   uart_printf__polled(UART__0, "in handler 30\n");
//   xSemaphoreGiveFromISR(lcd_sem_down, NULL);

//   // lcd_move_menu(&list_of_songs, 3, 1, 0); // down
// }

void pin01_isr(void) { // lcd_sem_down
  // uart_printf__polled(UART__0, "P0.30");
  // delay__ms(100);
//  start = clock();
//    while (!gpio__get(gpio09)) {
//        ;
//    }
//    end = clock();
//    dif = end - start;
//    printf("%d elapsed\n");
//    if(dif > 1000000){
//        xSemaphoreGiveFromISR(previous_sem);
//    }
//    else
        xSemaphoreGiveFromISR(lcd_sem_down, NULL);
    uart_printf__polled(UART__0, "in handler 01\n");
  // lcd_move_menu(&list_of_songs, 3, 1, 0); // down
}
void pin07_isr(void) { // lcd_sem_up
  // uart_printf__polled(UART__0, "P0.30");
  // delay__ms(100);
//  start = clock();
//  while (!gpio__get(gpio07)) {
//    ;
//  }
//  end = clock();
//  dif = end - start;
//  printf("%d elapsed\n");
//  if(dif > 1000000){
//      xSemaphoreGiveFromISR(skip_sem);
//  }
//  else
      xSemaphoreGiveFromISR(lcd_sem_up, NULL);
  uart_printf__polled(UART__0, "in handler 01\n");


  // lcd_move_menu(&list_of_songs, 3, 1, 0); // down
}
void pin09_isr(void) { // play
  // uart_printf__polled(UART__0, "P0.30");
  // delay__ms(100);

  uart_printf__polled(UART__0, "in handler 01\n");
  xSemaphoreGiveFromISR(lcd_sem_play, NULL);

  // lcd_move_menu(&list_of_songs, 3, 1, 0); // down
}
void pin10_isr(void) { // back
  // uart_printf__polled(UART__0, "P0.30");
  // delay__ms(100);
  uart_printf__polled(UART__0, "in handler 01\n");
  xSemaphoreGiveFromISR(lcd_sem_back, NULL);

  // lcd_move_menu(&list_of_songs, 3, 1, 0); // down
}
void pin25_isr(void) { // select
  // uart_printf__polled(UART__0, "P0.30");
  // delay__ms(100);
  uart_printf__polled(UART__0, "in handler 01\n");
  xSemaphoreGiveFromISR(lcd_sem_select, NULL);

  // lcd_move_menu(&list_of_songs, 3, 1, 0); // down
}

void button_init(void) {
  //    port_pin_s gpio30 = {0,
  //                               30}; // struct used to set input from
  //                               gpio_lab.c
  //   static port_pin_s gpio29 = {0, 29};
  // struct used to set as input from gpio_lab.c

  // void gpio__set_as_input(gpio_s gpio);
  // gpio30 = gpio__construct_with_function(GPIO__PORT_0, 30, 0);
  // gpio__set_as_input(gpio30); // set pin30 to input
  // // // gpio__set_as_output(LED2);     // sets led2 to output
  // gpio29 = gpio__construct_with_function(GPIO__PORT_0, 29, 0);
  // gpio__set_as_input(gpio29); // set pin29 to input

  // // gpio__set_as_output(LED3);     // set led3 to output
  // LPC_IOCON->P0_30 &= ~(3 << 3); // reset resistor for p30
  // LPC_IOCON->P0_30 |= (1 << 3);  // set resistor for p30
  // LPC_IOCON->P0_29 &= ~(3 << 3); // reset resistor for p29
  // LPC_IOCON->P0_29 |= (1 << 3);  // set p29
  gpio01 = gpio__construct_with_function(GPIO__PORT_0, 1, 0);
  gpio07 = gpio__construct_with_function(GPIO__PORT_0, 7, 0);
  gpio09 = gpio__construct_with_function(GPIO__PORT_0, 9, 0);
  gpio10 = gpio__construct_with_function(GPIO__PORT_0, 10, 0);
  gpio25 = gpio__construct_with_function(GPIO__PORT_0, 25, 0);
  gpio__set_as_input(gpio01);
  gpio__set_as_input(gpio07);
  gpio__set_as_input(gpio09);
  gpio__set_as_input(gpio10);
  gpio__set_as_input(gpio25);

  gpio0__attach_interrupt(1, GPIO_INTR__RISING_EDGE, pin01_isr);
  gpio0__attach_interrupt(7, GPIO_INTR__RISING_EDGE, pin07_isr);
  gpio0__attach_interrupt(9, GPIO_INTR__RISING_EDGE, pin09_isr);
  gpio0__attach_interrupt(10, GPIO_INTR__RISING_EDGE, pin10_isr);
  gpio0__attach_interrupt(25, GPIO_INTR__RISING_EDGE, pin25_isr);
  // gpio0__attach_interrupt(30, GPIO_INTR__RISING_EDGE, pin30_isr);
  // gpio0__attach_interrupt(29, GPIO_INTR__FALLING_EDGE, pin29_isr);

  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO,
                                   gpio0__interrupt_dispatcher);
  // while (1) {
  //   uart_printf__polled(UART__0, "In Main: Idle Looping\n");
  //   delay__ms(1000);
  // }
}

int main(void) {
  lcd_sem_up = xSemaphoreCreateBinary();
  lcd_sem_down = xSemaphoreCreateBinary();
  lcd_sem_play = xSemaphoreCreateBinary();
  lcd_sem_select = xSemaphoreCreateBinary();
  lcd_sem_back = xSemaphoreCreateBinary();
  play_sem = xSemaphoreCreateBinary();
  spi_sem = xSemaphoreCreateMutex();
//  skip_sem = xSemaphoreCreateBinary();
//  previous_sem = xSemaphoreCreateBinary();
  // player_sem = xSemaphoreCreateMutex();

  printf("in main\n");
  printf("populating songs...\n");
  song_list__populate();
  for (size_t song_number = 0; song_number < song_list__get_item_count();
       song_number++) {
    printf("Song %2d: %s\n", (1 + song_number),
           song_list__get_name_for_item(song_number));
  }
  // vTaskDelay(1000);
  ssp2__initialize(24000);
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
  rst = gpio__construct_with_function(GPIO__PORT_2, 2, 0);
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
  mp3write(0x02, 0x06, 0x06); // set bass

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

  // isr test
  button_init();

  // reset();
  delay__ms(10);
  xTaskCreate(mp3_reader_task, "reader", (4096 / sizeof(void *)), NULL, 1,
              &xReader);
  xTaskCreate(mp3_player_task, "player", (4096 / sizeof(void *)), NULL, 2,
              NULL);

  fprintf(stderr, "mode: %x \n", MP3Mode);
  fprintf(stderr, "status: %x \n", MP3Status);
  fprintf(stderr, "clock: %x \n", MP3Clock);
  // fprintf(stderr, "version: %x \n", version);
  fprintf(stderr, "lcd_init\n");

  xTaskCreate(button_interrupt_task, "button_interrupt",
              (4096 / sizeof(void *)), NULL, 3, NULL);
  // xTaskCreate(sj2_cli__init, "cli", (2048 / sizeof(void *)), NULL, 1,
  // NULL);
  xTaskCreate(song_control_task, "song control", (4096 / sizeof(void *)), NULL,
              3, NULL);
  fprintf(stderr, "about  write to volume\n");
  mp3write(0x0B, 0x20, 0x20);
  int tempvol = mp3read(0x0B);
  fprintf(stderr, "%x\n", tempvol);
  // testbuttons();
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
