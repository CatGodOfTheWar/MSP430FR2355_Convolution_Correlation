// ============================================================================
//  MSP430 Project - Convolution & Correlation  
//  S1(n), f1 = 1000 Hz, A1 = 3.3V, Nep = 4096 
//  S2(n), f2 = 1000 Hz, A2 = 3.3V, Nep = 4096
//
//                  MSP430FR2355
//             -----------------
//         /|\|             XIN|-
//          | |                |
//          --|RST         XOUT|-
//            |                |
//            |    P4.3/UCA1TXD| -> PC (UART TX)
//            |      P1.1/A1/S1| <- Analog Input Signal
//            |      P1.2/A2/S2| <- Analog Input Signal
//            |            P6.6| -> LED (ON)Convolution (OFF)Correlation
//            |            P1.0| -> LED (ON)Correlation (OFF)Convolution
//            |            P2.3| <- Btn Convolution(ON)
//            |            P4.1| <- Btn Correlation(ON)
// ============================================================================

#include <msp430.h>
#include <stdint.h>
#include "drivers/board_config.h"
#include "drivers/gpio_config.h"
#include "drivers/port_irq_config.h"
#include "drivers/clock_config.h"
#include "drivers/uart_config.h"
#include "drivers/timer_config.h"
#include "drivers/adc_config.h"

//*************************************************
// Machine states
typedef enum {
    STATE_ACQUIRING,  
    STATE_CALCULATE, 
    STATE_TRANSMITTING 
} SystemState;
volatile SystemState machine_state = STATE_ACQUIRING;

//*************************************************
// Defines
#define BUFFER_SIZE_S1_S2 100
#define BUFFER_SIZE_CONVOLUTION_CORRELATION 199

//*************************************************
// Global Variables
volatile uint16_t adc_buffer_S1[BUFFER_SIZE_S1_S2];
volatile uint16_t adc_buffer_S2[BUFFER_SIZE_S1_S2];
volatile uint16_t convolution_buffer[BUFFER_SIZE_CONVOLUTION_CORRELATION];
volatile uint16_t correlation_buffer[BUFFER_SIZE_CONVOLUTION_CORRELATION];
volatile uint16_t channel = 1;
volatile uint16_t s1_buffer_index = 0;
volatile uint16_t s2_buffer_index = 0;
volatile uint16_t convolutin_buffer_index = 0;
volatile uint16_t correlation_buffer_index = 0;
volatile uint16_t is_convolution = 1;

//*************************************************
// Calculation functions
/**
 * @brief Computes the linear convolution of two acquired signals.
 * * This function processes two input arrays (S1 and S2), removes their DC offset 
 * to operate in the bipolar domain, performs the Multiply-Accumulate (MAC) operation,
 * scales the result down, and restores the DC offset for UART transmission.
 */
void calculate_convolution() {
    for (uint16_t i = 0; i < BUFFER_SIZE_CONVOLUTION_CORRELATION; i++) {
        // Accumulator must be a 32-bit signed integer to prevent overflow and handle negative values
        int32_t sum = 0; 
        
        for (uint16_t j = 0; j < BUFFER_SIZE_S1_S2; j++) {
            // Check boundaries to ensure we only multiply overlapping window elements
            if ((i >= j) && ((i - j) < BUFFER_SIZE_S1_S2)) {
                // 1. Remove DC offset: Center the 12-bit ADC signal around 0 (2048 is the midpoint of 4095)
                int16_t val_S1 = (int16_t)adc_buffer_S1[j] - 2048;
                int16_t val_S2 = (int16_t)adc_buffer_S2[i - j] - 2048;
                
                // 2. MAC Operation: Multiply the zero-mean signals and accumulate
                sum += (int32_t)val_S1 * val_S2;
            }
        }
        
        // 3. Scale the massive accumulated sum down (bit shift by 17 is equivalent to dividing by ~131000)
        // Then, restore the 12-bit positive DC offset (1.65V) for correct plotting on the PC
        int32_t scaled_result = (sum >> 17) + 2048;
        
        // 4. Clipping: Clamp the values to strictly stay within the 12-bit unsigned resolution (0 - 4095)
        if (scaled_result > 4095) scaled_result = 4095;
        if (scaled_result < 0) scaled_result = 0;
        
        // Store the finalized 16-bit value into the output buffer
        convolution_buffer[i] = (uint16_t)scaled_result;
    }
}

/**
 * @brief Computes the cross-correlation of two acquired signals.
 * * Similar to convolution, but slides S2 across S1 without reversing it. 
 * Used to determine the similarity or phase delay between the two signals.
 */
void calculate_correlation() {
    for (uint16_t i = 0; i < BUFFER_SIZE_CONVOLUTION_CORRELATION; i++) {
        int32_t sum = 0;
        
        // Calculate the relative shift (lag) between the two signal windows
        int16_t lag = i - (BUFFER_SIZE_S1_S2 - 1); 
        
        for (uint16_t j = 0; j < BUFFER_SIZE_S1_S2; j++) {
            // Calculate the shifted index for the second signal
            int16_t k = j - lag;
            
            // Check if the shifted index falls within the valid buffer boundaries
            if (k >= 0 && k < BUFFER_SIZE_S1_S2) {
                // 1. Remove DC offset to bring signals into the signed/bipolar domain
                int16_t val_S1 = (int16_t)adc_buffer_S1[j] - 2048;
                int16_t val_S2 = (int16_t)adc_buffer_S2[k] - 2048;
                
                // 2. Multiply and accumulate
                sum += (int32_t)val_S1 * val_S2;
            }
        }
        
        // 3. Apply heavy scaling to fit the result back into 12 bits, and add the baseline offset
        int32_t scaled_result = (sum >> 17) + 2048;
        
        // 4. Hard clipping to prevent integer underflow/overflow artifacts on the UART side
        if (scaled_result > 4095) scaled_result = 4095;
        if (scaled_result < 0) scaled_result = 0;
        
        correlation_buffer[i] = (uint16_t)scaled_result;
    }
}


//*************************************************
int main(void)
{
    // System & GPIO Initialization 
    board_init();
    // UART TX/RX mapping on P4.3
    gpio_config(4, BIT3, 0, 0, 1, 0);
    // Visual indicators (LEDs)
    gpio_config(6, BIT6, 1, 1, 0, 0); // P6.6
    gpio_config(1, BIT0, 1, 1, 0, 0); // P1.0
    // Hardware buttons 
    gpio_config(2, BIT3, 0, 1, 0, 0);
    P2REN |= BIT3;
    gpio_config(4, BIT1, 0, 1, 0, 0);
    P4REN |= BIT1; 
    // Enable falling-edge interrupts for buttons
    port_irq_config(2, BIT3, 1);
    port_irq_config(4, BIT1, 1);

    // Clock & Peripheral Initialization 
    // Configure Master Clock (MCLK) and Sub-Main Clock (SMCLK) to 24 MHz
    clock_init(24, 1);
    // Setup Timer B0 to trigger ADC conversions based on CCR0 intervals
    timer_init(24, 1, 0, 0, 960);
    // Setup ADC: Channel A1, 12-bit resolution, 8-clock sample-and-hold, standard VCC/VSS ref
    adc_init(1, 12, ADCSHT_2, ADCSREF_0);
    // Setup UART eUSCI_A1: 24MHz source clock, 115200 baud rate
    uart_init(1, 24, 115200);

    // Enable Global Interrupts (GIE)
    __bis_SR_register(GIE);

    // Infinite Control Loop 
    while (1) {
        switch (machine_state) {
            case STATE_ACQUIRING:
                if (s1_buffer_index >= BUFFER_SIZE_S1_S2 && s2_buffer_index >= BUFFER_SIZE_S1_S2) {
                    machine_state = STATE_CALCULATE;
                    TB0CTL &= ~MC__UP;
                }
                break;

            case STATE_CALCULATE:
                if (is_convolution) {
                    calculate_convolution();
                } else {
                    calculate_correlation();
                }
                s1_buffer_index = 0;
                s2_buffer_index = 0;
                machine_state = STATE_TRANSMITTING;
                break;

            case STATE_TRANSMITTING:
                if (is_convolution) {
                    // 1. Synchronization header for Convolution
                    while (!(UCA1IFG & UCTXIFG)) {} UCA1TXBUF = 0xAA;
                    while (!(UCA1IFG & UCTXIFG)) {} UCA1TXBUF = 0xBB;

                    // 2. Transmission Convolution
                    for (uint16_t i = 0; i < BUFFER_SIZE_CONVOLUTION_CORRELATION; i++) {
                        while (!(UCA1IFG & UCTXIFG));
                        UCA1TXBUF = (uint8_t)(convolution_buffer[i] >> 8);
                        while (!(UCA1IFG & UCTXIFG));
                        UCA1TXBUF = (uint8_t)(convolution_buffer[i] & 0xFF);
                    }
                } else {
                    // 1. Synchronization header for Correlation
                    while (!(UCA1IFG & UCTXIFG)) {} UCA1TXBUF = 0xCC;
                    while (!(UCA1IFG & UCTXIFG)) {} UCA1TXBUF = 0xDD;

                    // 2. Transmission Correlation
                    for (uint16_t i = 0; i < BUFFER_SIZE_CONVOLUTION_CORRELATION; i++) {
                        while (!(UCA1IFG & UCTXIFG));
                        UCA1TXBUF = (uint8_t)(correlation_buffer[i] >> 8);
                        while (!(UCA1IFG & UCTXIFG));
                        UCA1TXBUF = (uint8_t)(correlation_buffer[i] & 0xFF);
                    }
                }

                // 3. Reset
                convolutin_buffer_index = 0;
                correlation_buffer_index = 0;
                machine_state = STATE_ACQUIRING;
                TB0CTL |= MC__UP; 
                break;
        }
    }
    return 0;
}

//*************************************************
// ISR ADC - Fast acquisition only
__attribute__((interrupt(ADC_VECTOR)))
void ADC_ISR(void) {
    if (__even_in_range(ADCIV, ADCIV_ADCIFG) == ADCIV_ADCIFG) {
        channel = ADCMCTL0 & 0x000F;
        switch(channel) 
        {
            case 1:
                if (s1_buffer_index < BUFFER_SIZE_S1_S2) {
                    adc_buffer_S1[s1_buffer_index++] = ADCMEM0;
                }
                ADCCTL0 &= ~ADCENC; 
                ADCMCTL0 = (ADCMCTL0 & ~0x000F) | 2; 
                break;
            case 2:
                if (s2_buffer_index < BUFFER_SIZE_S1_S2) {
                    adc_buffer_S2[s2_buffer_index++] = ADCMEM0;
                }
                ADCCTL0 &= ~ADCENC; 
                ADCMCTL0 = (ADCMCTL0 & ~0x000F) | 1; 
                break;
            default: 
                break;
        }
    }
}

//*************************************************
// ISR Timer B0 - Triggers ADC conversion
__attribute__((interrupt(TIMER0_B0_VECTOR)))
void Timer_B(void) {
    if (machine_state == STATE_ACQUIRING) {
        ADCCTL0 |= ADCENC | ADCSC; 
    }
}

//*************************************************
// ISR Port2 
__attribute__((interrupt(PORT2_VECTOR)))
void Port2_ISR(void) {
    P2IFG &= ~BIT3;
    P1OUT &= ~BIT0;
    P6OUT |= BIT6;
    is_convolution = 1;
}

//*************************************************
// ISR Port4 
__attribute__((interrupt(PORT4_VECTOR)))
void Port4_ISR(void) {
    P4IFG &= ~BIT1;
    P1OUT |= BIT0;
    P6OUT &= ~BIT6;
    is_convolution = 0;
}
