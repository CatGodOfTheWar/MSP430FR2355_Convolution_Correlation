/**
 * @file    port_irq_config.h
 * @brief   Port Interrupt Request (IRQ) configuration for MSP430FR2355.
 */

#ifndef PORT_IRQ_CONFIG_H_
#define PORT_IRQ_CONFIG_H_

#include <stdint.h>

/**
 * @brief   Configures and enables hardware interrupts for specified port pins.
 *
 * @param   port               The GPIO port number (1, 2, 3, or 4).
 * Note: Ports 5 and 6 do not support hardware IRQs.
 * @param   pins               The pin mask (e.g., BIT1, BIT3 | BIT4).
 * @param   edge_high_to_low   1 for High-to-Low (falling edge) transition,
 * 0 for Low-to-High (rising edge) transition.
 */
void port_irq_config(uint8_t port, uint8_t pins, uint8_t edge_high_to_low);

#endif /* PORT_IRQ_CONFIG_H_ */