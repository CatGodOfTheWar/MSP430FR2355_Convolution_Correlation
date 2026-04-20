/**
 * @file    gpio_config.h
 * @brief   General Purpose I/O configuration for MSP430FR series.
 */

#ifndef GPIO_CONFIG_H_
#define GPIO_CONFIG_H_

#include <stdint.h>

/**
 * @brief   Configures the direction, output state, and alternate functions of a GPIO port.
 *
 * @param   port      Port number (1 to 6).
 * @param   pin_mask  The pin(s) to configure (e.g., BIT0, BIT2 | BIT3).
 * @param   dir       0 for Input, 1 for Output.
 * @param   out       0 for Low, 1 for High (or Pull-down/Pull-up if input).
 * @param   sel0      PxSEL0 register state (0 or 1).
 * @param   sel1      PxSEL1 register state (0 or 1).
 */
void gpio_config(uint8_t port, uint8_t pin_mask, uint8_t dir, uint8_t out, uint8_t sel0, uint8_t sel1);

#endif /* GPIO_CONFIG_H_ */