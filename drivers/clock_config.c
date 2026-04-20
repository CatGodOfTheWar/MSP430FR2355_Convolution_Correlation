#include "clock_config.h"

/**
 * @name    Clock Configuration State
 * @brief   Internal variables used to track the current clock settings.
 * @{
 */
static uint16_t MCLK = 1;
static uint16_t FLL = 0;
static uint16_t DIVSMCLK = 0;
/** @} */

void software_trim(void)
{
    uint16_t oldDcoTap = 0xFFFF;
    uint16_t newDcoTap = 0xFFFF;
    uint16_t newDcoDelta = 0xFFFF;
    uint16_t bestDcoDelta = 0xFFFF;
    uint16_t csCtl0Copy = 0;
    uint16_t csCtl1Copy = 0;
    uint16_t csCtl0Read = 0;
    uint16_t csCtl1Read = 0;
    uint8_t dcoFreqTrim = 3;
    uint8_t endLoop = 0;

    do
    {
        CSCTL0 = 0x0100;
        
        do {
            CSCTL7 &= ~DCOFFG;
        } while (CSCTL7 & DCOFFG);

        /* 
            Wait FLL lock status (FLLUNLOCK) to be stable. 
            TI suggests waiting 24 cycles of divided FLL reference clock. 
        */
        uint16_t k;
        for (k = 0; k < (3000 * MCLK); k++) {
            __no_operation();
        } 

        while ((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;
        csCtl1Read = CSCTL1;

        oldDcoTap = newDcoTap;
        newDcoTap = csCtl0Read & 0x01FF;
        dcoFreqTrim = (csCtl1Read & 0x0070) >> 4;

        if (newDcoTap < 256)
        {
            newDcoDelta = 256 - newDcoTap;
            if ((oldDcoTap != 0xFFFF) && (oldDcoTap >= 256)) {
                endLoop = 1;
            } else {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim << 4);
            }
        }
        else
        {
            newDcoDelta = newDcoTap - 256;
            if (oldDcoTap < 256) {
                endLoop = 1;
            } else {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim << 4);
            }
        }

        if (newDcoDelta < bestDcoDelta)
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    } while (endLoop == 0);

    CSCTL0 = csCtl0Copy;
    CSCTL1 = csCtl1Copy;
    while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
}

void clock_init(uint8_t MCLK_FREQ, uint8_t DIVSM) 
{
    switch(MCLK_FREQ) {
        case 1:  MCLK = DCORSEL_0; FLL = 29;  break;
        case 2:  MCLK = DCORSEL_1; FLL = 60;  break;
        case 4:  MCLK = DCORSEL_2; FLL = 121; break;
        case 8:  MCLK = DCORSEL_3; FLL = 243; break;
        case 12: MCLK = DCORSEL_4; FLL = 365; break;
        case 16: MCLK = DCORSEL_5; FLL = 487; break;
        case 20: MCLK = DCORSEL_6; FLL = 609; break;
        case 24: MCLK = DCORSEL_7; FLL = 731; break;
        default: MCLK = DCORSEL_0; FLL = 29;  break;
    }
    
    /* FRAM Wait States Configuration */
    if(FLL < 366) {
        FRCTL0 = FRCTLPW | NWAITS_0;
    } else if (FLL < 488) {
        FRCTL0 = FRCTLPW | NWAITS_1;
    } else {
        FRCTL0 = FRCTLPW | NWAITS_2;
    }

    __bis_SR_register(SCG0);
    CSCTL3 |= SELREF__REFOCLK;
    CSCTL0 = 0;
    CSCTL1 = MCLK;
    CSCTL2 = FLLD_0 + FLL;
    __delay_cycles(3);  
    __bic_SR_register(SCG0);

    software_trim(); 
    
    while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;

    CSCTL5 &= ~(DIVS_0 | DIVS_1 | DIVS_2 | DIVS_3);
    switch(DIVSM) {
        case 1: DIVSMCLK = DIVS_0; break;
        case 2: DIVSMCLK = DIVS_1; break;
        case 4: DIVSMCLK = DIVS_2; break;
        case 8: DIVSMCLK = DIVS_3; break;
        default: DIVSMCLK = DIVS_0; break;
    }
    CSCTL5 |= DIVSMCLK;
}