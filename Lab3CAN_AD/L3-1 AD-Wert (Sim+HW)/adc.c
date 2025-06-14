#include "lpc17xx.h"                              /* LPC17xx definitions    */
#include "adc.h"

#define ADC_MAX_VALUE 0xFFF // Max 12bit Auflösung

uint16_t AD_value = 0;

/*----------------------------------------------------------------------------
  config ADC
 *----------------------------------------------------------------------------*/
void ADC_Config(void) {
    // 1. Enable the AD Converter in the PCONP Register
    LPC_SC->PCONP |= (1 << 12);
    
    // 2. Configure the AD Pin  -->  AD0.5 Pin
    LPC_PINCON->PINSEL3 &= ~(3UL << 30);
    LPC_PINCON->PINSEL3 |=  (3UL << 30);
    
    // 3. Select AD channel, AD Frequency, enable AD function in the AD module 
    LPC_ADC->ADCR = (1 << 5) | (4 << 8) | (1 << 21);
}

/*----------------------------------------------------------------------------
  start ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_StartConversion(void) {
    LPC_ADC->ADCR &= ~(7 << 24); // START-Bits löschen
    LPC_ADC->ADCR |=  (1 << 24); // START = 001 = Start per Software
    
    // Warten bis DONE-Bit gesetzt (bit 31 in ADGDR)
    while (!(LPC_ADC->ADGDR & (1U << 31)));

    // Wert auslesen: Bits 6 bis 15 enthalten den 12-Bit AD-Wert
    AD_value = (LPC_ADC->ADGDR >> 4) & ADC_MAX_VALUE;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
