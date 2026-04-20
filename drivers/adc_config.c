#include <stdint.h>
#include <msp430.h>

/**
 * @brief   Initializes the ADC (Analog-to-Digital Converter) module.
 *
 * @param   channel     Input channel (0 to 11 for external pins A0-A11).
 * (Use 12 for Temp Sensor, 13 for VCC, 14-15 for Internal).
 * @param   resolution  Resolution in bits (8, 10, or 12).
 * @param   sht         Sample-and-hold time (e.g., ADCSHT_2, ADCSHT_8).
 * @param   reference   Voltage reference (e.g., ADCSREF_0 for VCC/VSS).
 */
void adc_init(uint8_t channel, uint8_t resolution, uint16_t sht, uint16_t reference)
{
    uint16_t res_bits;

    // Safety Guard: Check if the channel is within the valid hardware range (0-15)
    if (channel > 15) return; 

    // Map integer resolution to hardware register bits
    if (resolution == 8) {
        res_bits = ADCRES_0;
    } else if (resolution == 10) {
        res_bits = ADCRES_1;
    } else {
        res_bits = ADCRES_2; // Standard 12-bit
    }

    // CRITICAL: Disable ADC conversions before modifying configuration
    ADCCTL0 &= ~ADCENC;

    // Configure Sample-and-Hold time and turn the ADC core ON
    ADCCTL0 = (ADCCTL0 & ~ADCSHT) | sht | ADCON;

    // Configure sampling timer (Pulse mode)
    ADCCTL1 |= ADCSHP;

    // Configure resolution
    ADCCTL2 = (ADCCTL2 & ~ADCRES) | res_bits;

    // Configure voltage reference and input channel
    ADCMCTL0 = reference | (channel & 0x0F);

    // Enable ADC conversion complete interrupt
    ADCIE |= ADCIE0;
}