#pragma once
#include "uart_lab.h"
#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "queue.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static QueueHandle_t your_uart_rx_queue;

// Private function of our uart_lab.c
static void your_receive_interrupt(void) {
  // TODO: Read the IIR register to figure out why you got interrupted
  // uint4_t status = LPC_UART2->IIR & 0x0E;
  // TODO: Based on IIR status, read the LSR register to confirm if there is data to be read
  if (((LPC_UART2->IIR >> 1 & 0x7) == (0x2)) &&
      (LPC_UART2->LSR & (1 << 0))) { // if interrupt pending and data is to be read
    // TODO: Based on LSR status, read the RBR register and input the data to the RX Queue
    const char byte = LPC_UART2->RBR;
    fprintf(stderr, "Data recieved: %c\n", byte);
    xQueueSendFromISR(your_uart_rx_queue, &byte, NULL);
  }
}

// Public function to enable UART interrupt
// TODO Declare this at the header file
void uart__enable_receive_interrupt(uart_number_e uart_number) {
  // TODO: Use lpc_peripherals.h to attach your interrupt
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART2, your_receive_interrupt);

  // TODO: Enable UART receive interrupt by reading the LPC User manual
  LPC_UART2->IER |= (1 << 0);

  // Hint: Read about the IER register

  // TODO: Create your RX queue
  your_uart_rx_queue = xQueueCreate(10, sizeof(char));
}

// Public function to get a char from the queue (this function should work without modification)
// TODO: Declare this at the header file
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
  return xQueueReceive(your_uart_rx_queue, input_byte, timeout);
}

void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
  // Refer to LPC User manual and setup the register bits correctly
  // The first page of the UART chapter has good instructions
  // a) Power on Peripheral
  // b) Setup DLL, DLM, FDR, LCR registers

  // using UART2
  // LPC_SC->PCONP |= (1 << 23);

  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__UART2);
  uint16_t div = peripheral_clock / (16 * baud_rate);
  LPC_UART2->LCR |= (1 << 7);
  LPC_UART2->DLM |= (div >> 8 & 0xFF);
  LPC_UART2->DLL |= (div >> 0 & 0xFF);
  LPC_UART2->FDR = (1 << 4);
  LPC_UART2->LCR &= ~(1 << 7);
  LPC_UART2->LCR &= ~(1 << 3);
  LPC_UART2->LCR |= (3 << 0);  // WLS
  LPC_UART2->LCR &= ~(1 << 2); // two stop bits

  /**lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__UART2);

  LPC_UART2->LCR |= (3 << 0);
  LPC_UART2->LCR &= ~(1 << 2);
  LPC_UART2->LCR &= ~(1 << 3);
  LPC_UART2->LCR |= (1 << 7);

  uint16_t div = peripheral_clock / (16 * baud_rate);**/

  // i don't think i have to set FDR, bc reset values are right
}

bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {
  // a) Check LSR for Receive Data Ready
  // b) Copy data from RBR register to input_byte
  /**fprintf(stderr, "in get");
  if (LPC_UART2->LSR & 0xFF) {
    const uint32_t mask = (1 << 0);
    fprintf(stderr, "ready");
    while (!(LPC_UART2->LSR & mask))
      ;
    fprintf(stderr, "transmission");
    *input_byte = LPC_UART2->RBR;
    fprintf(stderr, "read");
    return true;
  }
  return false;**/

  // b) Copy data from RBR register to input_byte
  fprintf(stderr, "in get LSR:%x\n", LPC_UART2->LSR);
  if (LPC_UART2->LSR & (1 << 0)) {
    fprintf(stderr, "ready to load\n");
    *input_byte = LPC_UART2->RBR & 0xFF;
    fprintf(stderr, "RBR:%c", LPC_UART2->RBR);
    return true;
  }
  return false;
}

bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
  // a) Check LSR for Transmit Hold Register Empty
  // b) Copy output_byte to THR register
  fprintf(stderr, "in\n");
  const uint32_t empty = (1 << 5);

  if ((LPC_UART2->LSR & empty)) {
    LPC_UART2->THR = output_byte;
    fprintf(stderr, "in2\n");
    fprintf(stderr, "%c\n", output_byte);
    return true;
  }

  return false;
}