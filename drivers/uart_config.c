#include <stdint.h>
#include <msp430.h>

/**
 * @brief   Lookup table entry for UCBRSx fractional calculation.
 */
typedef struct {
    float frac;
    uint8_t ucbrs;
} ucbrs_entry;

/**
 * @brief   Fractional baud rate modulation table based on TI SLAU445 Datasheet.
 */
static const ucbrs_entry ucbrs_lut[] = {
    {0.0000f, 0x00}, {0.0529f, 0x01}, {0.0715f, 0x02}, {0.0835f, 0x04},
    {0.1001f, 0x08}, {0.1252f, 0x10}, {0.1430f, 0x20}, {0.1670f, 0x11},
    {0.2147f, 0x21}, {0.2224f, 0x22}, {0.2503f, 0x44}, {0.3000f, 0x25},
    {0.3335f, 0x49}, {0.3575f, 0x4A}, {0.3753f, 0x52}, {0.4003f, 0x92},
    {0.4286f, 0x53}, {0.4378f, 0x55}, {0.5002f, 0xAA}, {0.5715f, 0x6B},
    {0.6003f, 0xAD}, {0.6254f, 0xB5}, {0.6432f, 0xB6}, {0.6667f, 0xD6},
    {0.7001f, 0xB7}, {0.7147f, 0xBB}, {0.7503f, 0xDD}, {0.7861f, 0xED},
    {0.8004f, 0xEE}, {0.8333f, 0xBF}, {0.8464f, 0xDF}, {0.8572f, 0xEF},
    {0.8751f, 0xF7}, {0.9004f, 0xFB}, {0.9170f, 0xFD}, {0.9288f, 0xFE}
};

#define LUT_SIZE (sizeof(ucbrs_lut)/sizeof(ucbrs_lut[0]))

/**
 * @brief   Retrieves the correct UCBRSx value from the lookup table.
 */
static uint8_t get_ucbrs(float frac)
{
    for(uint8_t x = 0; x < LUT_SIZE - 1; x++)
    {
        if(frac < ucbrs_lut[x+1].frac) {
            return ucbrs_lut[x].ucbrs;
        }
    }
    return ucbrs_lut[LUT_SIZE - 1].ucbrs;
}

/**
 * @brief   Initializes the eUSCI_A module in UART mode.
 *
 * @param   module      0 for eUSCI_A0, 1 for eUSCI_A1.
 * @param   SM_CLK      SMCLK source frequency in MHz (e.g., 1, 8, 16).
 * @param   BAUD_RATE   Desired baud rate (e.g., 9600, 115200).
 */
void uart_init(uint8_t module, uint32_t SM_CLK, uint32_t BAUD_RATE) 
{
    uint8_t OS16_value = 0;
    uint32_t baud_divider = (SM_CLK * 1000000UL) / BAUD_RATE; 
    
    if (baud_divider >= 16) {
        OS16_value = 1;
    }
    
    uint16_t UCBRx  = baud_divider / 16;
    uint8_t  UCBRFx = baud_divider % 16;
    float    frac   = ((float)baud_divider / 16.0f) - UCBRx;
    uint8_t  UCBRSx = get_ucbrs(frac); 

    uint16_t modulation_val = (UCBRSx << 8) | (UCBRFx << 4) | OS16_value;

    if (module == 0) 
    {
        UCA0CTLW0 |= UCSWRST;                       
        UCA0CTLW0 |= UCSSEL__SMCLK;                 
        UCA0BR0 = UCBRx & 0xFF;                     
        UCA0BR1 = (UCBRx >> 8) & 0xFF;              
        UCA0MCTLW = modulation_val;                 
        UCA0CTLW0 &= ~UCSWRST;                      
        UCA0IE |= UCRXIE;                           
    } 
    else if (module == 1) 
    {
        UCA1CTLW0 |= UCSWRST;
        UCA1CTLW0 |= UCSSEL__SMCLK;
        UCA1BR0 = UCBRx & 0xFF;
        UCA1BR1 = (UCBRx >> 8) & 0xFF;
        UCA1MCTLW = modulation_val;
        UCA1CTLW0 &= ~UCSWRST;
        UCA1IE |= UCRXIE;
    }
}

/**
 * @brief   Transmits a null-terminated string over the selected UART module.
 *
 * @param   module      0 for eUSCI_A0, 1 for eUSCI_A1.
 * @param   s           Pointer to the string to transmit.
 */
void uart_send(uint8_t module, const char *s) 
{
    if (module == 0) 
    {
        while (*s) {
            while (!(UCA0IFG & UCTXIFG)); // Wait for TX buffer to be ready
            UCA0TXBUF = *s++;
        }
    } 
    else if (module == 1) 
    {
        while (*s) {
            while (!(UCA1IFG & UCTXIFG)); // Wait for TX buffer to be ready
            UCA1TXBUF = *s++;
        }
    }
}