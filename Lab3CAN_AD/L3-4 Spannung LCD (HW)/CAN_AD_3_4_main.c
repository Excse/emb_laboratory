/*----------------------------------------------------------------------------
* Name:    CAN_AD_3_4_main.c
* Purpose: main File for LPC1768 Microcontrollers        
* Version: Version 1.0 / 02.05.2019 (KBL, HS-Mannheim)
*----------------------------------------------------------------------------*/


#include "LPC17xx.h"
#include "adc.h"
#include "GLCD.h"
#include <stdio.h>

#define RELOAD_VALUE         249999    // Reload value for 10ms (25MHz * 10ms = 250000) 

void Timer1_Config(void);

const uint16_t My_CAN_ID = 0x701;

extern uint16_t AD_value;
extern uint8_t AD_New_Value_Ready;

float AD_Voltage = 0.0;
char NewLine[21], AD_Val_String[6];

int main(void) {
    GLCD_Init();              /* Initialize the GLCD */
    GLCD_Clear(White);        /* Clear the GLCD */
    GLCD_SetBackColor(Blue);  /* Set the Back Color */
    GLCD_SetTextColor(White); /* Set the Text Color */

    GLCD_DisplayString(0, 0, "  EMB Lab3: CAN/ADC ");
    sprintf(NewLine, "   Local ID: %#3x  ", My_CAN_ID);
    GLCD_DisplayString(1, 0, (unsigned char *) NewLine);
    sprintf(NewLine, "Local ADValue: %#.3f", AD_Voltage);
    GLCD_DisplayString(2, 0, (unsigned char *) NewLine);    

    Timer1_Config();
    ADC_Config();
    ADC_StartConversion();

    AD_New_Value_Ready = 0;

    while (1) {
        if(AD_New_Value_Ready) {
            GLCD_SetBackColor(Blue);  /* Set the Back Color */
            GLCD_SetTextColor(White); /* Set the Text Color */
            
            AD_Voltage = ((float) AD_value * 3.3f) / (float) 0xFFF;
            sprintf(NewLine, "Local ADValue: %#.3f", AD_Voltage);
            GLCD_DisplayString(2, 0, (unsigned char *) NewLine);    
            
            AD_New_Value_Ready = 0;
        }
    }
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
    LPC_TIM1->IR = (1 << 0);
}

/************************************ EOF ***********************************/


