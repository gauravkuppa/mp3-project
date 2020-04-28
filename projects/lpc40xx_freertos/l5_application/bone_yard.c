/**
static SemaphoreHandle_t switch_press_indication;
void gpio_interrupt(void);
void sleep_on_sem_task(void *p);
void pin29_isr(void) {
  uart_printf__polled(UART__0, "P0.29");
  delay__ms(100);
}
void pin30_isr(void) {
  uart_printf__polled(UART__0, "P0.30");
  delay__ms(100);
}

typedef struct {
  float f1; // 4 bytespin30_isr
  char c2;
} __attribute__((packed)) my_s;

typedef struct {
  /* First get gpio0 driver to work only, and if you finish it
   * you can do the extra credit to also make it work for other Ports
   *
  uint8_t port;
  uint8_t pin;
} port_pin_s;

typedef struct {
  /* First get gpio0 driver to work only, and if you finish20001b70

void led_task(void *pvParameters) {
  // Choose one of the onboard LEDS by looking into schematics and write code
for the below

  // 0) Set the IOCON MUX function select pins to 000
  *(uint32_t *)(0x4002C0E8) &= ~(0b111);
  const uint32_t p1_26 = (1 << 26);
  LPC_GPIO1->DIR |= p1_26;

  // 1) Set the DIR register bit for the LED port pin
  while (true) {
    // 2) Set PIN register bit to 0 to turn ON LED (led may be active low)
    LPC_GPIO1->PIN &= ~p1_26;
    vTaskDelay(500);

    // 3) Set PIN register bit to 1 to turn OFF LED
    LPC_GPIO1->PIN |= p1_26;
    vTaskDelay(500);
  }
}**/
/**


void led_task_any_led(void *task_parameter) {
  // Type-cast the paramter that was passed from xTaskCreate()
  pin_s led = *(pin_s *)(task_parameter);
  uint8_t port_num;
  uint8_t pin_num;

  if (led.pin == 1) {
    LPC_IOCON->P2_3 &= ~(0b111);
    port_num = 2;
    pin_num = 3;
  } else if (led.pin == 2) {
    LPC_IOCON->P1_26 &= ~(0b111);
    port_num = 1;
    pin_num = 26;
  } else if (led.pin == 3) {
    LPC_IOCON->P1_24 &= ~(0b111);
    port_num = 1;
    pin_num = 24;
  } else if (led.pin == 4) {
    LPC_IOCON->P1_18 &= ~(0b111);
    port_num = 1;
    pin_num = 18;
  } else {
    return;
  }

  gpioN__set_as_output(port_num, pin_num);
  while (true) {
    gpioN__set_low(port_num, pin_num);
    vTaskDelay(500);
    gpioN__set_high(port_num, pin_num);
    vTaskDelay(500);
  }
}

void led_task_any_sempahores(void *task_parameter) {
  port_pin_s *led = (port_pin_s *)task_parameter;
  uint8_t port_num = led->port;
  uint8_t pin_num = led->pin;
  gpioN__set_as_output(port_num, pin_num);

  while (true) {
    // Note: There is no vTaskDelay() here, but we use sleep mechanism while
waiting for the binary semaphore (signal) if
(xSemaphoreTake(switch_press_indication, 1000)) {

      gpioN__set_low(port_num, pin_num);
      vTaskDelay(500);
      gpioN__set_high(port_num, pin_num);
      vTaskDelay(500);

      puts("LED blinked!");
    } else {while switch press indication for 1000ms");
    }
  }
}

void switch_task(void *task_parameter) {
  port_pin_s *toggle_switch = (port_pin_s *)task_parameter;
  uint8_t port = toggle_switch->port;
  uint8_t pin = toggle_switch->pin;
  gpioN__set_as_input(port, pin);
  while (true) {
    // TODO: If switch pressed, set the binary semaphore
20001b70
      xSemaphoreGive(switch_press_indication);
    }

    // Task should always sleep otherwise they will use 100% CPU
    // This #include "adc.h"

#include "FreeRTOS.h"
#include "task.h"

void adc_task(void *p) {
  adc__initialize();

  // TODO This is the function you need to add to adc.h
  // You can configure burst mode for just the channel you are using
  adc__enable_burst_mode();

  // Configure a pin, such as P1.31 with FUNC 011 to route this pin as ADC
channel 5
  // You can use gpio__construct_with_function() API from gpio.h
  pin_configure_adc_channel_as_io_pin(); // TODO You need to write this function

  while (1) {
    // Get the ADC reading using a new routine you created to read an ADC burst
reading
    // TODO: You need to write the implementation of this function
    const uint16_t adc_value =
adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_2);

    vTaskDelay(100);
  }
}
**/
// which pins will be damaged if one of the damaged if one of the lines is low?
// MISO
// TODO: Implement Adesto flash memory CS signal as a GPIO driver
/**void adesto_cs(void) {
  LPC_GPIO1->PIN &= ~(1 << 10);
  LPC_GPIO0->PIN &= ~(1 << 6);
  // gpioN__set_as_input(1, 10);//LPC_GPIO1->SET &= ~(1 << 10);
}
void adesto_ds(void) {
  LPC_GPIO1->PIN |= (1 << 10);
  LPC_GPIO1->PIN |= (1 << 10);
  LPC_GPIO0->PIN |= (1 << 6);
  LPC_GPIO0->PIN |= (1 << 6);
  // LPC_GPIO1->SET |= (1 << 10); // review this again
};

// TODO: Study the Adesto flash 'Manufacturer and Device ID' section
typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;

// TODO: Implement the code to read Adesto flash memory signature
// TODO: Create struct of type 'adesto_flash_id_s' and return it
adesto_flash_id_s adesto_read_signature(void) {

  adesto_flash_id_s data;
  adesto_cs();

  // Send opcode and read bytes
  // TODO: Populate members of the 'adesto_flash_id_s' struct
  uint8_t op_code_return = ssp__exchange_byte(0x9f);
  data.manufacturer_id = ssp__exchange_byte(0xff);
  // fprintf(stderr, "Manu ID: %x", data.manufacturer_id);
  data.device_id_1 = ssp__exchange_byte(0xff);
  data.device_id_2 = ssp__exchange_byte(0xff);
  data.extended_device_id = ssp__exchange_byte(0xff);
  printf("\nManufacturer ID: %x\nDevice ID 1: %x\nDevice ID 2: %x\nExtended
Device ID: %x", data.manufacturer_id, data.device_id_1, data.device_id_2,
data.extended_device_id);

  adesto_ds();

  return data;
  //}
}

todo_configure_your_ssp1_pin_functions() {

  gpio__construct_with_function(GPIO__PORT_1, 0, GPIO__FUNCTION_4); // SSP2, SCK
  gpio__construct_with_function(GPIO__PORT_1, 4, GPIO__FUNCTION_4); // SSP2,
MISO gpio__construct_with_function(GPIO__PORT_1, 1, GPIO__FUNCTION_4); // SSP2,
MOSI gpioN__set_as_output(1, 10);
}

void spi_task(void *p) {

  const uint32_t spi_clock_mhz = 24;
  fprintf(stderr, "%d", spi_clock_mhz);
  ssp__init(spi_clock_mhz);

  // From the LPC schematics pdf, find the pin numbers connected to flash memory
  // Read table 84 from LPC User Manual and configure PIN functions for SPI2
pins
  // You can use gpio__construct_with_function() API from gpio.h
  //
  // Note: Configure only SCK2, MOSI2, MISO2.
  // CS will be a GPIO output pin(configure and setup direction)
  todo_configure_your_ssp1_pin_functions();

  while (1) {
    adesto_flash_id_s id = adesto_read_signature();
    // TODO: printf the members of the 'adesto_flash_id_s' struct
    printf("\nManufacturer ID: %x\nDevice ID 1: %x\nDevice ID 2: %x\nExtended
Device ID: %x", id.manufacturer_id, id.device_id_1, id.device_id_2,
id.extended_device_id); vTaskDelay(500);
  }
}
void spi_id_verification_task(void *p) {
  while (1) {
    adesto_flash_id_s id;
    if (xSemaphoreTake(spi_bus_mutex, 1000)) {
      id = adesto_read_signature();
      xSemaphoreGive(spi_bus_mutex);
    }
    // When we read a manufacturer ID we do not expect, we will kill this task
    if (id.manufacturer_id != 0x1F) {
      fprintf(stderr, "Manufacturer ID read failure\n");
      vTaskSuspend(NULL); // Kill this task
    }
  }
}**/
/**void uart_read_task(void *p) {
  while (1) {
    // TODO: Use uart_lab__polled_get() function and printf the received value
    fprintf(stderr, "in read task\n");

    char val;
    if (uart_lab__polled_get(UART_2, &val)) {

      fprintf(stderr, "get char %c\n", val);
    }

    vTaskDelay(500);
  }
}

void uart_write_task(void *p) {
  while (1) {
    // TODO: Use uart_lab__polled_put() function and send a value
    fprintf(stderr, "in write\n ");
    bool put = uart_lab__polled_put(UART_2, 'h');
    // fprintf(stderr, "bool put: %d\n", put);
    vTaskDelay(500);
  }
}
// This task is done for you, but you should understand what this code is doing
void board_1_sender_task(void *p) {
  char number_as_string[16] = {0};

  while (true) {
    const int number = rand();
    sprintf(number_as_string, "%i", number);

    // Send one char at a time to the other board including terminating NULL
char for (int i = 0; i <= strlen(number_as_string); i++) {
      uart_lab__polled_put(0, number_as_string[i]);
      printf("Sent: %c\n", number_as_string[i]);
    }

    printf("Sent: %i over UART to the other board\n", number);
    vTaskDelay(3000);
  }
}

void board_2_receiver_task(void *p) {
  char number_as_string[16] = {0};
  int counter = 0;
  while (true) {
    printf("In while loop.");
    char byte = 0;typedef char songname_t[16];

QueueHandle_t Q_songname;
QueueHandle_t Q_songdata;

void main(void) {
  Q_songname = xQueueCreate(1, sizeof(songname));
  Q_songdata = xQueueCreate(1, 512);
}

// Reader tasks receives song-name over Q_songname to start reading it
void mp3_reader_task(void *p) {
  songname name;
  char bytes_512[512];

  while(1) {
    xQueueReceive(Q_songname, &name[0], portMAX_DELAY);
    printf("Received song to play: %s\n", name);

    open_file();
    while (!file.end()) {
      read_from_file(bytes_512);
      xQueueSend(Q_songdata, &bytes_512[0], portMAX_DELAY);
    }
    close_file();
  }
}

// Player task receives song data over Q_songdata to send it to the MP3 decoder
void mp3_player_task(void *p) {
  char bytes_512[512];

  while (1) {
    xQueueReceive(Q_songdata,  &bytes_512[0], portMAX_DELAY);
    for (int i = 0; i < sizeof(bytes_512); i++) {
      while (!mp3_decoder_needs_data()) {
        vTaskDelay(1);
      }

      spi_send_to_mp3_decoder(bytes_512[i]);
    }
  }
}
    uart_lab__get_char_from_queue(&byte, portMAX_DELAY);
    printf("Received: %c\n", byte);
    // This is the last char, so print the number
    if ('\0' == byte) {
      number_as_string[counter] = '\0';
      counter = 0;
      printf("Received this number from the other board: %s\n",
number_as_string);
    }
    // We have not yet received the NULL '\0' char, so buffer the data
    else {
      number_as_string[counter] = byte;
      if (counter < 16)
        counter++;
      else
        counter = 0;
      // TODO: Store data to number_as_string[] array one char at a time
      // Hint: Use counter as an index, and increment it as long as we do not
reach max value of 16
    }
  }
}**/

/**typedef enum { switch__off, switch__on } switch_e;

// TODO: Create this task at PRIORITY_LOW
void producer(void *p) {
  switch_e switch_value = 0;
  while (1) {
    // This xQueueSend() will internally switch context to "consumer" task
because it is higher priority than this
    // "producer" task Then, when the consumer task sleeps, we will resume out
of xQueueSend()and go over to the next
    // line

    // TODO:

    switch_value = (switch_e)gpioN__get_level(0, 30);

    // TODO: Print a message before xQueueSend()
    printf("Sending...%d\n", switch_value);
    // Note: Use printf() and not fprintf(stderr, ...) because stderr is a
polling printf xQueueSend(switch_queue, &switch_value, 0);
    // TODO: Print a message after xQueueSend()
    printf("Sent!\n");

    vTaskDelay(1000);
  }
}

// TODO: Create this task at PRIORITY_HIGH
void consumer(void *p) {
  int x;
  while (1) {
    printf("Waiting...\n");
    // TODO: Print a message before xQueueReceive()
    xQueueReceive(switch_queue, &x, portMAX_DELAY);
    // TODO: Print a message after xQueueReceive()
    printf("Recieved\n");
  }
}**/
/**#define BIT_1 (1 << 1) // producer task
#define BIT_2 (1 << 2) // consumer task

// Sample code to write a file to the SD Card
void write_file_using_fatfs_pi(acceleration__axis_data_s *sensor_value) {
  const char *filename = "sensor.txt";
  FIL file; // File handle
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_OPEN_APPEND | FA_WRITE));

  if (FR_OK == result) {extern QueueHandle_t Q_songname;
    char string[64];
    sprintf(string, "%li, %i, %i, %i\n", xTaskGetTickCount(), sensor_value->x,
sensor_value->y, sensor_value->z); if (FR_OK == f_write(&file, string,
strlen(string), &bytes_written)) {
      // printf("sent");
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    printf("ERROR: Failed to open: %s because %d\n", filename, result);
  }
}

void producer_task(void *params) {
  while (1) { // Assume 100ms loop - vTaskDelay(100)
    // Sample code:

    // fprintf(stderr, "Send Acceleration x: %d, y: %d, z: %d\n",
sensor_value.x, sensor_value.y, sensor_value.z);

    acceleration__axis_data_s calculated_avg;
    for (int i = 0; i < 100; i++) {
      acceleration__axis_data_s sensor_value = acceleration__get_data();
      calculated_avg.x += sensor_value.x;
      calculated_avg.y += sensor_value.y;
      calculated_avg.z += sensor_value.z;
      vTaskDelay(1);
    }
    calculated_avg.x = calculated_avg.x / 100;
    calculated_avg.y = calculated_avg.y / 100;
    calculated_avg.z = calculated_avg.z / 100;
    // write_file_using_fatfs_pi();
    xQueueSend(sensor_data_queue, &calculated_avg, 0);
    uxBits = xEventGroupSetBits(xEventGroup, BIT_1);
    // vTaskDelay(100);
  }
}

void consumer_task(void *params) {
  while (1) { // Assume 100ms loop
    // No need to use vTaskDelay() because the consumer will consume as fast as
production rate
    // because we should block on xQueueReceive(&handle, &item, portMAX_DELAY);
    // Sample code:
    acceleration__axis_data_s sensor_value;
    xQueueReceive(sensor_data_queue, &sensor_value, 100); // Wait forever for an
item write_file_using_fatfs_pi(&sensor_value);
    // fprintf(stderr, "Receive Acceleration );
    uxBits = xEventGroupSetBits(xEventGroup, BIT_2);
  }
}

void watchdog_task(void *params) {
  fprintf(stderr, "In\n");
  while (1) {
    // ...
    // vTaskDelay(200);
    // We either should vTaskDelay, but for better robustness, we should
    // block on xEventGroupWaitBits() for slightly more than 100ms because
    // of the expected production rate of the producer() task and its check-in
    // fprintf(stderr, "In while\n");
    vTaskDelay(500);
    uxBits = xEventGroupWaitBits(xEventGroup, BIT_1 | BIT_2, pdTRUE, pdFALSE,
205 / portTICK_PERIOD_MS); if ((uxBits & (BIT_1 | BIT_2)) == (BIT_1 | BIT_2)) {
      // xEventGroupWaitBits() returned because both bits were set.
      fprintf(stderr, "producer and consumer working\n");

    } else {
      if ((uxBits & BIT_1) != (BIT_1)) {
        fprintf(stderr, "producer is suspended\n");
      }
      if ((uxBits & BIT_2) != (BIT_2)) {
        fprintf(stderr, "consumer is suspended\n");
      }
    }
    printf("uxBits: %ld\n", uxBits);
  }
}**/

#if 0
main
/**i2c__init_slave(0x86);
  LPC_IOCON->P0_10 &= ~(3 << 3);
  LPC_IOCON->P0_11 &= ~(3 << 3);
  i2c__initialize(I2C__2, 9600, 96000000);
  uart_lab__init(UART_3, 96000000, 9600);
  fprintf(stderr, "init\n");
  gpio__construct_with_function(GPIO__PORT_4, 28, GPIO__FUNCTION_2); // TX
  gpio__construct_with_function(GPIO__PORT_4, 29, GPIO__FUNCTION_2); // RX
  fprintf(stderr, "pin select");
  xTaskCreate(uart_read_task, "uart", (2048 / sizeof(void *)), NULL, 1, NULL);
  fprintf(stderr, "in\n");
  write_file_using_fatfs_pi(false, "write_file_using_fatfs_pi\n");
  fprintf(stderr, "print1");
  char print[64];
  sprintf(print, "it is ye time and my name is %d digits long", 11);
  write_file_using_fatfs_pi(true, print);
  int x = 1;
  int *y = &x;
  char *p = "hello";
  fprintf(stderr, "in main: %s\n", p);
  write_file_using_fatfs_pi(true, p);
  fprintf(stderr, "print2\n");**/
  /**
   * Note: When another Master interacts with us, it will invoke the I2C
   *interrupt .      which will then invoke our i2c_slave_callbacks_*() from
   *above And thus, we only need to react to the changes of memory
   */

#endif

/**
vo
id pin_configure_pwm_channel_as_io_pin() {
  LPC_IOCON->P2_1 &= ~(3 << 0);
  LPC_IOCON->P2_1 |= (1 << 0);

  // gpio_s gpio = gpio__construct_with_function(GPIO__PORT_2, pin,
GPIO__FUNCTION_1);
  // gpio__set_as_output(gpio);
}

void pwm_task(void *p) {
  pwm1__init_single_edge(1000);

  // Locate a GPIO pin that a PWM channel will control
  // NOTE You can use gpio__construct_with_function() API from gpio.h
  // TODO Write this function yourself
  pin_configure_pwm_channel_as_io_pin();

  // We only need to set PWM configuration once, and the HW will drive
  // the GPIO at 1000Hz, and control set its duty cycle to 50%
  pwm1__set_duty_cycle(PWM1__2_1, 50);

  // Continue to vary the duty cycle in the loop
  uint8_t percent = 0;
  int adc_reading = 0;

  while (1) {
    // Implement code to receive potentiometer value from queue
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      double reading = (double)((adc_reading / 4095) * 100);
      pwm1__set_duty_cycle(PWM1__2_1, reading);

      fprintf(stderr, "recieved: %f\n", reading);
      vTaskDelay(100);
    }

    // We do not need task delay because our queue API will put task to sleep
when there is no data in the queue
    // vTaskDelay(100);
  }
}

void pin_configure_adc_channel_as_io_pin() {
  LPC_IOCON->P0_25 &= ~(1 << 7);
  LPC_IOCON->P0_25 &= ~(7 << 0);
  LPC_IOCON->P0_25 |= (1 << 0);
}

void adc_task(void *p) {
  adc__initialize();

  // TODO This is the function you need to add to adc.h
  // You can configure burst mode for just the channel you are using
  adc__enable_burst_mode();

  // Configure a pin, such as P1.31 with FUNC 011 to route this pin as ADC
channel 5
  // You can use gpio__construct_with_function() API from gpio.h
  pin_configure_adc_channel_as_io_pin(); // TODO You need to write this function

  int adc_reading = 0; // Note that this 'adc_reading' is not the same variable
as the one from adc_task while (1) {
    // Implement code to send potentiometer value on the queue
    // a) read ADC input to 'int adc_reading'
    // b) Send to queue: xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    const uint16_t adc_reading =
adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_2); int reading =
(int)adc_reading; fprintf(stderr, "reading: %d\n", reading);
    xQueueSend(adc_to_pwm_task_queue, &reading, 0);
    vTaskDelay(100);
  }
}**/

// Step 2:
/**void gpio_interrupt(void) { // how to crash interrupt
  // a) Clear Port0/2 interrupt using CLR0 or CLR2 registers
  // b) Use fprintf(stderr) or blink and LED here to test your ISR
  uart_printf__polled(UART__0, "interrupt port 0\n");
  if (LPC_GPIOINT->IO0IntStatF != 0) {
    // uart_printf__polled("interrupt port 0\n");
    xSemaphoreGiveFromISR(switch_press_indication, NULL);
    LPC_GPIOINT->IO0IntClr |= (1 << 30);
    // vTaskDelay(100);
    uart_printf__polled(UART__0, "cleared port 0\n");
  }
}

void sleep_on_sem_task(void *p) {
  while (1) {
    // Use xSemaphoreTake with forever delay and blink an LED when you get the
signal xSemaphoreTakeFromISR(switch_press_indication, portMAX_DELAY);
    // fprintf(stderr, "sleep!");
  }
}**/