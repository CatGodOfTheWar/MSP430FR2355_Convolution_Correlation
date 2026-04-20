#include <msp430.h>

/**
 * @brief   Initializes fundamental board settings.
 *
 * @details Stops the Watchdog Timer (WDT) to prevent unintended resets during 
 * lengthy initialization routines (like clock calibration). 
 * Clears the LOCKLPM5 bit to unlock GPIO pins from their high-impedance 
 * state, which is the default safety behavior after power-on for FRAM devices.
 */
void board_init(void) 
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;
}