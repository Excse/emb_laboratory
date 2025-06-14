#include "LPC17xx.h"
#include "adc.h"

#define RELOAD_VALUE         249999    // Reload value for 10ms (25MHz * 10ms = 250000) 

void Timer1_Config(void);

int main(void) {
    Timer1_Config();
    ADC_Config();

    while (1); 
}

void Timer1_Config(void) {
    LPC_SC->PCONP |= (1 << 2); // Enable Timer1

    // Put some code in order to enable Timer1 interrupt with priority level 1    
    NVIC_SetPriority(TIMER1_IRQn, 1);
    NVIC_EnableIRQ(TIMER1_IRQn);

    // and let Timer1 generate an Interrupt every 10ms
    LPC_TIM1->MR0 = RELOAD_VALUE;        // Zeit[s] * 25MHz = Cycles
    LPC_TIM1->MCR = (1 << 0) | (1 << 1); // MR0I + MR0R
    LPC_TIM1->TCR = 1;                   // Timer starten
}

void TIMER1_IRQHandler(void) {  
    // Start new AD conversion
    ADC_StartConversion();
    
    // Clear timer1 interrupt
    LPC_TIM1->IR = (1 << 0);  //Clear interrupt
}

/************************************ EOF ***********************************/