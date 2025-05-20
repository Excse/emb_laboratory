
#include "LPC17xx.h"
#include "adc.h"

void Timer1_Config(void);
uint16_t AD_value;

int main(void)
{
	//SystemInit();
	Timer1_Config();
	ADC_Config();

    while (1); 
}

void Timer1_Config(void)
{
	
	LPC_SC->PCONP |= (1<<2);//enable Timer1
	//Put some code in order to enable Timer1 interrupt with priority level 1
	// and let Timer1 generate an Interrupt every 100 ms
}

void TIMER1_IRQHandler (void) 
{  
 // Put some code lines here
 //Read and store AD value 
 //Start new AD conversion
 //Clear timer1 interrupt
  
}

/************************************ EOF ***********************************/


