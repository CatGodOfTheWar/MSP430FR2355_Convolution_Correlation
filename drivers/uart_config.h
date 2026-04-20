/**
 * @file    uart.h
 * @brief   Hardware UART initialization and transmission functions for MSP430FR series.
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

/**
 * @brief   Initializes the eUSCI_A module in UART mode.
 *
 * @param   module      0 for eUSCI_A0, 1 for eUSCI_A1.
 * @param   SM_CLK      SMCLK source frequency in MHz (e.g., 1, 8, 16).
 * @param   BAUD_RATE   Desired baud rate (e.g., 9600, 115200).
 */
void uart_init(uint8_t module, uint32_t SM_CLK, uint32_t BAUD_RATE);

/**
 * @brief   Transmits a null-terminated string over the selected UART module.
 *
 * @param   module      0 for eUSCI_A0, 1 for eUSCI_A1.
 * @param   s           Pointer to the string to transmit.
 */
void uart_send(uint8_t module, const char *s);

#endif /* UART_H_ */