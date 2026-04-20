#include <stdint.h>
#include <msp430.h>

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
void gpio_config(uint8_t port, uint8_t pin_mask, uint8_t dir, uint8_t out, uint8_t sel0, uint8_t sel1)
{
    switch (port) {
        case 1:
            if (dir)  P1DIR |= pin_mask;  else P1DIR &= ~pin_mask;
            if (out)  P1OUT |= pin_mask;  else P1OUT &= ~pin_mask;
            if (sel0) P1SEL0 |= pin_mask; else P1SEL0 &= ~pin_mask;
            if (sel1) P1SEL1 |= pin_mask; else P1SEL1 &= ~pin_mask;
            break;
        case 2:
            if (dir)  P2DIR |= pin_mask;  else P2DIR &= ~pin_mask;
            if (out)  P2OUT |= pin_mask;  else P2OUT &= ~pin_mask;
            if (sel0) P2SEL0 |= pin_mask; else P2SEL0 &= ~pin_mask;
            if (sel1) P2SEL1 |= pin_mask; else P2SEL1 &= ~pin_mask;
            break;
        case 3:
            if (dir)  P3DIR |= pin_mask;  else P3DIR &= ~pin_mask;
            if (out)  P3OUT |= pin_mask;  else P3OUT &= ~pin_mask;
            if (sel0) P3SEL0 |= pin_mask; else P3SEL0 &= ~pin_mask;
            if (sel1) P3SEL1 |= pin_mask; else P3SEL1 &= ~pin_mask;
            break;
        case 4:
            if (dir)  P4DIR |= pin_mask;  else P4DIR &= ~pin_mask;
            if (out)  P4OUT |= pin_mask;  else P4OUT &= ~pin_mask;
            if (sel0) P4SEL0 |= pin_mask; else P4SEL0 &= ~pin_mask;
            if (sel1) P4SEL1 |= pin_mask; else P4SEL1 &= ~pin_mask;
            break;
        case 5:
            if (dir)  P5DIR |= pin_mask;  else P5DIR &= ~pin_mask;
            if (out)  P5OUT |= pin_mask;  else P5OUT &= ~pin_mask;
            if (sel0) P5SEL0 |= pin_mask; else P5SEL0 &= ~pin_mask;
            if (sel1) P5SEL1 |= pin_mask; else P5SEL1 &= ~pin_mask;
            break;
        case 6:
            if (dir)  P6DIR |= pin_mask;  else P6DIR &= ~pin_mask;
            if (out)  P6OUT |= pin_mask;  else P6OUT &= ~pin_mask;
            if (sel0) P6SEL0 |= pin_mask; else P6SEL0 &= ~pin_mask;
            if (sel1) P6SEL1 |= pin_mask; else P6SEL1 &= ~pin_mask;
            break;
        default:
            break;
    }
}