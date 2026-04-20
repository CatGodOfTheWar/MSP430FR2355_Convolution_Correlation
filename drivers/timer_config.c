#include <stdint.h>
#include <msp430.h>

/**
 * @brief   Initializes a Timer_B module in UP mode.
 *
 * @param   mclk_freq     MCLK frequency in MHz (e.g., 8, 16, 24).
 * @param   smclk_div     SMCLK division factor (1, 2, 4, 8).
 * @param   timer_select  Timer_B instance (0, 1, 2, or 3).
 * @param   ccrx_select   Capture/Compare channel (0-6).
 * @param   t_int_us      Desired interrupt period in MICROSECONDS.
 *
 * @return  The actual value loaded into the CCRx register, or 0 on error.
 */
uint16_t timer_init(uint32_t mclk_freq, uint32_t smclk_div, uint8_t timer_select, uint8_t ccrx_select, uint32_t t_int_us) 
{
    // Direct and precise calculation of required ticks
    uint32_t ticks = (mclk_freq * t_int_us) / smclk_div;
    uint16_t id_bits = ID__1;
    uint16_t tbidex_bits = 0; 
    uint16_t prescaler = 1;

    // Automatic hardware prescaler calculation
    while ((ticks / prescaler) > 65536) { // 65536 because we will subtract 1 at the end
        prescaler *= 2; 
        if (prescaler > 64) return 0; // Exceeds hardware capacity
    }

    if (prescaler <= 8) {
        id_bits = (prescaler == 1) ? ID__1 : (prescaler == 2) ? ID__2 : (prescaler == 4) ? ID__4 : ID__8;
        tbidex_bits = 0; 
    } else {
        id_bits = ID__8;
        tbidex_bits = (prescaler / 8) - 1; 
    }

    // Final calculation for UP mode (counts from 0 to ticks-1)
    uint16_t period = (uint16_t)(ticks / prescaler) - 1;
    
    // Hardware configuration: UP mode
    uint16_t ctl_config = CNTL_0 | TBSSEL__SMCLK | MC__UP | id_bits | TBCLR;

    switch (timer_select) {
        case 0:
            TB0EX0 = tbidex_bits;
            if (ccrx_select == 0) { TB0CCTL0 = CCIE; TB0CCR0 = period; }
            else if (ccrx_select == 1) { TB0CCTL1 = CCIE; TB0CCR1 = period; }
            else if (ccrx_select == 2) { TB0CCTL2 = CCIE; TB0CCR2 = period; }
            TB0CTL = ctl_config;
            break;
        case 1:
            TB1EX0 = tbidex_bits;
            if (ccrx_select == 0) { TB1CCTL0 = CCIE; TB1CCR0 = period; }
            else if (ccrx_select == 1) { TB1CCTL1 = CCIE; TB1CCR1 = period; }
            else if (ccrx_select == 2) { TB1CCTL2 = CCIE; TB1CCR2 = period; }
            TB1CTL = ctl_config;
            break;
        case 2:
            TB2EX0 = tbidex_bits;
            if (ccrx_select == 0) { TB2CCTL0 = CCIE; TB2CCR0 = period; }
            else if (ccrx_select == 1) { TB2CCTL1 = CCIE; TB2CCR1 = period; }
            else if (ccrx_select == 2) { TB2CCTL2 = CCIE; TB2CCR2 = period; }
            TB2CTL = ctl_config;
            break;
        case 3:
            TB3EX0 = tbidex_bits;
            if (ccrx_select == 0) { TB3CCTL0 = CCIE; TB3CCR0 = period; }
            else if (ccrx_select == 1) { TB3CCTL1 = CCIE; TB3CCR1 = period; }
            else if (ccrx_select == 2) { TB3CCTL2 = CCIE; TB3CCR2 = period; }
            else if (ccrx_select == 3) { TB3CCTL3 = CCIE; TB3CCR3 = period; }
            else if (ccrx_select == 4) { TB3CCTL4 = CCIE; TB3CCR4 = period; }
            else if (ccrx_select == 5) { TB3CCTL5 = CCIE; TB3CCR5 = period; }
            else if (ccrx_select == 6) { TB3CCTL6 = CCIE; TB3CCR6 = period; }
            TB3CTL = ctl_config;
            break;
    }

    return period;
}