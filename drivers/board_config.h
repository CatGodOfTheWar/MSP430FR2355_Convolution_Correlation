/**
 * @file    board_config.h
 * @brief   Fundamental board initialization for MSP430FR series.
 */

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

/**
 * @brief   Initializes fundamental board settings (WDT stop and GPIO unlock).
 */
void board_init(void);

#endif /* BOARD_CONFIG_H_ */