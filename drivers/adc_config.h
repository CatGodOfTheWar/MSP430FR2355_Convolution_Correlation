/**
 * @file    adc_config.h
 * @brief   ADC (Analog-to-Digital Converter) initialization for MSP430FR2355.
 */

#ifndef ADC_CONFIG_H_
#define ADC_CONFIG_H_

#include <stdint.h>

/**
 * @brief   Initializes the ADC module.
 *
 * @param   channel     Input channel (0 to 11 for external pins A0-A11).
 * (Use 12 for Temp Sensor, 13 for VCC, 14-15 for Internal).
 * @param   resolution  Resolution in bits (8, 10, or 12).
 * @param   sht         Sample-and-hold time (e.g., ADCSHT_2, ADCSHT_8).
 * @param   reference   Voltage reference (e.g., ADCSREF_0 for VCC/VSS).
 */
void adc_init(uint8_t channel, uint8_t resolution, uint16_t sht, uint16_t reference);

#endif /* ADC_CONFIG_H_ */