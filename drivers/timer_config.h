/**
 * @file    timer_config.h
 * @brief   Timer_B initialization and configuration for MSP430FR series.
 */

#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_

#include <stdint.h>

/**
 * @brief   Initializes a Timer_B module in Continuous mode with interrupt generation.
 *
 * @param   mclk_freq     MCLK frequency in MHz (e.g., 24).
 * @param   smclk_div     SMCLK division factor (1, 2, 4, or 8).
 * @param   timer_select  Timer_B instance (0, 1, 2, or 3).
 * @param   ccrx_select   Capture/Compare channel to enable CCIE for (0-6).
 * @param   t_int         Desired interrupt period (in milliseconds).
 *
 * @return  The actual value (ticks) loaded into the CCRx register, or 0 on error.
 */
uint16_t timer_init(uint32_t mclk_freq, uint32_t smclk_div, uint8_t timer_select, uint8_t ccrx_select, uint32_t t_int);

#endif /* TIMER_CONFIG_H_ */