/**
 * @file    clock_config.h
 * @brief   Clock System (CS) initialization and DCO calibration for MSP430FR series.
 */

#ifndef CLOCK_CONFIG_H_
#define CLOCK_CONFIG_H_

#include <stdint.h>
#include <msp430.h> // Ensure the device-specific header is included

/**
 * @brief   Fine software calibration for the DCO.
 * @note    Blocks execution until the FLL stabilizes.
 */
void Software_Trim(void);

/**
 * @brief   Initializes the microcontroller's Clock System (CS) and FRAM wait states.
 *
 * @param   MCLK_FREQ  Target Master Clock (MCLK) in MHz (Supported: 1, 2, 4, 8, 12, 16, 20, 24).
 * @param   DIVSM      Sub-Main Clock (SMCLK) division factor (Supported: 1, 2, 4, 8).
 */
void clock_init(uint8_t MCLK_FREQ, uint8_t DIVSM);

#endif /* CLOCK_CONFIG_H_ */