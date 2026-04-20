#include <msp430.h>
#include "port_irq_config.h"

/**
 * @brief   Configures and enables hardware interrupts for specified port pins.
 *
 * @param   port               The GPIO port number (1, 2, 3, or 4).
 * Note: Ports 5 and 6 do not support hardware IRQs.
 * @param   pins               The pin mask (e.g., BIT1, BIT3 | BIT4).
 * @param   edge_high_to_low   1 for High-to-Low (falling edge) transition,
 * 0 for Low-to-High (rising edge) transition.
 */
void port_irq_config(uint8_t port, uint8_t pins, uint8_t edge_high_to_low)
{
    // Safety Guard: Check if the port supports IRQs (Only P1, P2, P3, P4 do)
    if (port < 1 || port > 4) return;

    // Apply configuration based on the selected port
    switch (port) {
        case 1:
            if (edge_high_to_low) {
                P1IES |= pins;      // Falling edge
            } else {
                P1IES &= ~pins;     // Rising edge
            }
            P1IFG &= ~pins;         // Clear the interrupt flag before enabling
            P1IE |= pins;           // Enable the interrupt
            break;

        case 2:
            if (edge_high_to_low) {
                P2IES |= pins;
            } else {
                P2IES &= ~pins;
            }
            P2IFG &= ~pins;
            P2IE |= pins;
            break;

        case 3:
            if (edge_high_to_low) {
                P3IES |= pins;
            } else {
                P3IES &= ~pins;
            }
            P3IFG &= ~pins;
            P3IE |= pins;
            break;

        case 4:
            if (edge_high_to_low) {
                P4IES |= pins;
            } else {
                P4IES &= ~pins;
            }
            P4IFG &= ~pins;
            P4IE |= pins;
            break;

        default:
            break;
    }
}