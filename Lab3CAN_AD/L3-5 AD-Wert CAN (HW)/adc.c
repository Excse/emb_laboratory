
#include "lpc17xx.h"                              /* LPC17xx definitions    */
#include "adc.h"

/*----------------------------------------------------------------------------
  config ADC
 *----------------------------------------------------------------------------*/
void ADC_Config (void) {

  //Put your code here to perform the following tasks:
  //                   1. Enable the AD Converter in the PCONP Register
  //                2. Configure the AD Pin  -->  AD0.5 Pin
  //                3. Select AD channel, AD Frequency, enable AD function in the AD module 
}

/*----------------------------------------------------------------------------
  start ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_StartConversion (void) {
  //Put code to start an AD conversion                    /* start conversion */
  //Please remember that a new conversion must start every 100 ms 
}



/*********************************************************************************
**                            End Of File
*********************************************************************************/
