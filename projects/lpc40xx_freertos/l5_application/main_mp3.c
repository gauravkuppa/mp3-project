#include <stdio.h>

#include "FreeRTOS.h"
#include "app_cli.h"
#include "ff.h"
#include "queue.h"

/**void main(void) {
  Q_songname = xQueueCreate(1, sizeof(songname_t));
  Q_songdata = xQueueCreate(1, 512);
  xTaskCreate(mp3_reader_task, "reader", (4000) / sizeof(void *), NULL, 1,
NULL);
}**/