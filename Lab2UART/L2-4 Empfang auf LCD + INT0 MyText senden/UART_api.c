/*****************************************************************************
**   UART_api.c
**   in Lab2 of the lecture EMB
**   History
**   2015-0514  ver 0.01  LDK / HS Mannheim
**   2024-0617  ver 0.02  Poh / HS Mannheim
**   2025-0509  ver 0.03  Poh / TH Mannheim
*****************************************************************************/

#include "LPC17xx.h"
#include <stdio.h>

#define Systick_RELOAD_VALUE  10000000-1  // Set Systick Reload Register for 100ms interrupts
#define LCDout_ZeitDauer_Sek  3

// UART variables:
extern int Selected_UART_PORT;
int IIR_Content, LSR_Content;        // last content of UxIIR and UxLSR
unsigned char Dummy;        // last character of serial transmission
int AutoBaud_DLL, AutoBaud_DLM, AutoBaud_DL, AutoBaudrate, to_read_Baudrate;

//UART receive + LCD output variables:
volatile int Launch_LCD_Output =0;
volatile int LCD_RefreshTick = 0;
int New_UART_Data_Received = 0;
int volatile RX_MaxNumberChar;
//static unsigned char *LCD_Output_String;
volatile unsigned char *LCD_Output_String;

int U0_RX_NumberOfChar, U0_rx_sw_buffer_index;
int U2_RX_NumberOfChar, U2_rx_sw_buffer_index;

int U0_tx_sw_buffer_index, U0_TX_NumberOfChar;
int U2_tx_sw_buffer_index, U2_TX_NumberOfChar;

unsigned char UART0_ReceivedBytes[500];
unsigned char UART2_ReceivedBytes[500];

//UART transmit variables:
//unsigned char UART_TX_String[500];
extern char *UART_TX_String;

int U0_TX_Running =0;
int U2_TX_Running =0;


// Systick variables:
int New_INT0_ButtonState, Old_INT0_ButtonState = (1 << 10);    // P2.10 INT1 button
int INT0_RisingEdgeDetected = 0;
int INT0_FallingEdgeDetected = 0;
int INT0_PulsWidthCounter;
//int INT0_PhaseValue =0;
int New_KEY1_ButtonState, Old_KEY1_ButtonState = (1 << 11);    // P2.11 KEY1 button
int KEY1_ScreenCounter = 0;    // Initialize for welcome screen
int New_KEY2_ButtonState, Old_KEY2_ButtonState = (1 << 12);    // P2.12 KEY2 button


// Timer 2 Capture Measurement:
unsigned int T2_MinimumMeasurement = 0x7FFFFFFF;    // highest value for minimum
unsigned int T2Baudrate = 0;


// Procedures:
extern void UART_PutChar(int UART_PortNum,int newChar);
extern int UART_GetChar(int UART_PortNum);

void UART0_IRQHandler(void);
void UART2_IRQHandler(void);
void UART_Start_Char_Transmit(int PortNum, int NumberChar);
int Read_Baudrate (int UART_PortNum);

void SysTick_Handler(void);    // Button Sampling
void TIMER0_IRQHandler (void);    // Time in seconds for LCD frames (received characters)
//void TIMER2_IRQHandler (void);    // Capture measurement of baudrate

void init_SysTick(void);
void init_Timer0_for_UART(void);
//void init_Timer2(void);

void init_NVIC(void);        // Interrupt enable


/*****************************************************************************
** Function name:        UART0_IRQHandler
**
** Descriptions:        UART0 interrupt service routine
**
** Parameters:            None
** Returned value:        None
**
*****************************************************************************/
void UART0_IRQHandler(void){
    IIR_Content = LPC_UART0->IIR;

    //Read U0LSR and consequently RLS interrupt is automatically cleared.
    LSR_Content = LPC_UART0->LSR;
    
    Selected_UART_PORT = 0;

    if ( IIR_Content & 0x100){    // ABEOInt: Auto Baud End Of Interrupt
        LPC_UART0->ACR |= (1<<8);    // ABEOIntClr
        to_read_Baudrate = 1;            // Read_Baudrate() to be executed
    }
    else if ( IIR_Content & 0x200){    // ABTOInt: Auto Baud Time-Out Interrupt
        LPC_UART0->ACR |= (1<<2);    // AutoRestart
        LPC_UART0->ACR |= (1<<9);    // ABTOIntClr
        to_read_Baudrate = 1;            // Read_Baudrate() to be executed
    }


    switch(IIR_Content & ((1<<3)|(1<<2)|(1<<1))){  // Interrupt Identification Bits = UnIIR[3:1]

        case 0x06: /* Receive Line Status (RLS)*/
                // At least one RX error OE(Overrun Error) or PE(Parity Error) or
                // FE(Frame Error) or BI(Break Interrupt) has occurred
                // Put your code here if RLS error handling required
            if (LSR_Content & 1)
                    Dummy = LPC_UART0->LSR; //Remove erroneous byte, if any
            break;

        case 0x04: /* Receive Data Available (RDA)  or*/
        case 0x0C: /*Character Time-Out Indicator (CTI) */
            while (LPC_UART0->LSR & 0x01){
                Dummy = UART_GetChar(0);
                if (Dummy == 0x0A ){  // on LineFeed (LF) as end character
                    // of a string given in a terminal, LCD output is to be performed
                    U0_RX_NumberOfChar = U0_rx_sw_buffer_index;
                    U0_rx_sw_buffer_index = 0;
                    New_UART_Data_Received = 1;
                    //Prepare the pointer for LCD output
                    //LCD_U0_ReceivedBytes = &UART0_ReceivedBytes[0];
                }
                else
                    if (Dummy != 0x0D)  // ignore Carriage Return (CR)
                        UART0_ReceivedBytes[U0_rx_sw_buffer_index++] = Dummy;
            }
            break;

        case 0x02:
            if (U0_TX_Running){
            while (( LPC_UART0->LSR & 0x20) && (U0_tx_sw_buffer_index < (U0_TX_NumberOfChar-2))) {
                Dummy = UART_TX_String[U0_tx_sw_buffer_index++];
                UART_PutChar(0, Dummy);
                }
            }
            if     (U0_tx_sw_buffer_index == U0_TX_NumberOfChar-2){
                U0_TX_Running = 0;
                LPC_UART0->IER &= ~(1<<1);
            }
            break;

        default: Dummy++;    // other cases: change last Dummy

    }

    if (Dummy == 0x32)        // received character
        __NOP();    // set breakpoint here to stop at a specific character received

} // end of UART0_IRQHandler


/*****************************************************************************
** Function name:        UART2_IRQHandler
**
** Descriptions:        UART2 interrupt service routine
**
** Parameters:            None
** Returned value:        None
**
*****************************************************************************/
void UART2_IRQHandler(void){
    IIR_Content = LPC_UART2->IIR;

    //Read U2LSR and consequently RLS interrupt is automatically cleared.
    LSR_Content = LPC_UART2->LSR;
    
    Selected_UART_PORT = 2;

    if ( IIR_Content & 0x100){    // ABEOInt: Auto Baud End Of Interrupt
        LPC_UART2->ACR |= (1<<8);    // ABEOIntClr
        to_read_Baudrate = 1;            // Read_Baudrate() to be executed
    }
    else if ( IIR_Content & 0x200){    // ABTOInt: Auto Baud Time-Out Interrupt
        LPC_UART2->ACR |= (1<<2);    // AutoRestart
        LPC_UART2->ACR |= (1<<9);    // ABTOIntClr
        to_read_Baudrate = 1;            // Read_Baudrate() to be executed
    }

    switch(IIR_Content & ((1<<3)|(1<<2)|(1<<1))){  // Interrupt Identification Bits = UnIIR[3:1]

        case 0x06: /* Receive Line Status (RLS)*/
            // At least one RX error OE(Overrun Error) or PE(Parity Error) or
            // FE(Frame Error) or BI(Break Interrupt) has occurred
            // Put your code here if RLS error handling required
            if (LSR_Content & 1)
                Dummy = LPC_UART2->LSR; //Remove erroneous byte, if any
            break;

        case 0x04: /* Receive Data Available (RDA)  or*/
        case 0x0C: /*Character Time-Out Indicator (CTI) */
            while (LPC_UART2->LSR & 0x01){
                Dummy = UART_GetChar(2);
                if (Dummy == 0x0A ){  // on LineFeed (LF) as end character
                    // of a string given in a terminal, LCD output is to be performed
                    U2_RX_NumberOfChar = U2_rx_sw_buffer_index;
                    U2_rx_sw_buffer_index = 0;
                    New_UART_Data_Received = 1;
                    //Prepare the pointer for LCD output
                    //LCD_U0_ReceivedBytes = &UART0_ReceivedBytes[0];
                }
                else
                    if (Dummy != 0x0D)  // ignore Carriage Return (CR)
                        UART2_ReceivedBytes[U2_rx_sw_buffer_index++] = Dummy;
            }
            break;

        case 0x02:
            if (U2_TX_Running){
                while (( LPC_UART2->LSR & 0x20) && (U2_tx_sw_buffer_index < (U2_TX_NumberOfChar-2))) {
                    Dummy = UART_TX_String[U2_tx_sw_buffer_index++];
                    UART_PutChar(2, Dummy);
                }
            }
            if     (U2_tx_sw_buffer_index == U2_TX_NumberOfChar-2){
                U2_TX_Running = 0;
                LPC_UART2->IER &= ~(1<<1);
            }
            break;

        default: Dummy++;    // other cases: change last Dummy

    }
    if (Dummy == 0x32)        // received character
        __NOP();    // set breakpoint here to stop at a specific character received

} // end of UART2_IRQHandler


/*****************************************************************************
** Function name:        UART_Start_Char_Transmit
**
** Descriptions: This function sends the first 2 characters per UART and
**               enables UART TX interrupt afterwards
**
** Parameters:            PortNum = 0 (for UART0) 
**                            2 (for UART2)
**                  TX_String = String to be sent
**                  NumberChar
** Returned value:        None
**
*****************************************************************************/
void UART_Start_Char_Transmit(int PortNum, int NumberChar){
    volatile unsigned  int *UxLSR;
    volatile unsigned int *UxIER;

    if (PortNum == 0)                    // UART0 Selected
        {
            UxLSR = (volatile unsigned int*)(&(LPC_UART0->LSR));
            UxIER = &(LPC_UART0->IER);
            if (NumberChar > 2){
                U0_TX_Running  = 1;
                U0_TX_NumberOfChar = NumberChar;
                U0_tx_sw_buffer_index = 0;
            }
        }
    else if (PortNum == 2)        // UART2 Selected
        {
            UxLSR = (volatile unsigned int*) &(LPC_UART2->LSR);
            UxIER = &(LPC_UART2->IER);
            if (NumberChar > 2){
                U2_TX_Running  = 1;
                U2_TX_NumberOfChar = NumberChar;
                U2_tx_sw_buffer_index = 0;
            }
        }

    if ( *UxLSR & 0x20 ){  // THRE==1
        if ( NumberChar >= 2) {
            UART_PutChar(PortNum, *UART_TX_String++); // Send first char without interrupt
            UART_PutChar(PortNum, *UART_TX_String); // Send second char without interrupt
            if ( NumberChar > 2){
                *UxIER  |= 0x02;
                NumberChar = NumberChar - 2;
                UART_TX_String++;
            }
      }
      else
            if ( NumberChar == 1){
                UART_PutChar(PortNum, *UART_TX_String);
                *UART_TX_String = 0;
            }
    }
} // end of UART_Start_Char_Transmit


/*****************************************************************************
** Function name:        Read_Baudrate
**
** Descriptions: This function reads the baudrate defined by the DLM, DLL and FDR
**               registers and returns the calculated value.
**
** Parameters:            PortNum = 0 (for UART0) or 2 (for UART2)
**
** Returned value:        Baudrate of UART_PortNum
**
*****************************************************************************/
int Read_Baudrate (int UART_PortNum){        // Read the UART baudrate
    int Baud_DLL, Baud_DLM, FDR_MulVal, FDR_DivAddVal, Baudrate;
    if (UART_PortNum == 0)    // UART0 Selected
        {
            LPC_UART0->LCR = (LPC_UART0->LCR) | (1<<7);        // set DLAB
            Baud_DLL = LPC_UART0->DLL;
            Baud_DLM = LPC_UART0->DLM;
            FDR_MulVal     = (LPC_UART0->FDR & 0xF0) >>4 ;
            FDR_DivAddVal  = (LPC_UART0->FDR & 0x0F);
            LPC_UART0->LCR = (LPC_UART0->LCR) & ~(1<<7);    // clear DLAB
        }
        else                                    // UART2 Selected
        {
            LPC_UART2->LCR = (LPC_UART2->LCR) | (1<<7);        // set DLAB
            Baud_DLL = LPC_UART2->DLL;
            Baud_DLM = LPC_UART2->DLM;
            FDR_MulVal     = (LPC_UART2->FDR & 0xF0) >>4 ;
            FDR_DivAddVal  = (LPC_UART2->FDR & 0x0F);
            LPC_UART2->LCR = (LPC_UART2->LCR) & ~(1<<7);    // clear DLAB
        }

        Baudrate = 25000000 / (16 * ((Baud_DLM<<8) | Baud_DLL)*(1+((float)FDR_DivAddVal/FDR_MulVal)));
        return(Baudrate);
        
} // end of Read_Baudrate



/*****************************************************************************
** Function name:        SysTick_Handler
**
** Descriptions:        IRQ Handler for the System Timer
**                  Used to sample the state of several buttons:
**                    INT0: Launch a transmission of characters
**                    KEY1: Switch the blue debug screen on LCD
**                    KEY2: Reset the capture measurement of baudrate
**
** Parameters:            None
** Returned value:        None
**
*****************************************************************************/
void SysTick_Handler(void){    // Button Sampling
    // For sampling INT0 button
    New_INT0_ButtonState =  ( (1 << 10) & LPC_GPIO2->FIOPIN );    // P2.10 INT0 button
    if (New_INT0_ButtonState){
        //INT0_PhaseValue = 1;
        if (Old_INT0_ButtonState != New_INT0_ButtonState)
                INT0_RisingEdgeDetected = 1;
        else
                INT0_RisingEdgeDetected = 0;
    }
    else {
        //INT0_PhaseValue = 0;
        if (Old_INT0_ButtonState != New_INT0_ButtonState){
            INT0_FallingEdgeDetected = 1;
            INT0_PulsWidthCounter = 0;
        }
        else {
            INT0_FallingEdgeDetected = 0;
            INT0_PulsWidthCounter++;
        }
    }
    Old_INT0_ButtonState = New_INT0_ButtonState;

    // For sampling KEY1 button
    New_KEY1_ButtonState =  ( (1 << 11) & LPC_GPIO2->FIOPIN );    // P2.11 KEY1 button
    if (New_KEY1_ButtonState)        // released
        if (Old_KEY1_ButtonState != New_KEY1_ButtonState){
            if (++KEY1_ScreenCounter > 2) KEY1_ScreenCounter = 1;
        }

    Old_KEY1_ButtonState = New_KEY1_ButtonState;

    // For sampling KEY2 button (Reset T2Bd)
    New_KEY2_ButtonState =  ( (1 << 12) & LPC_GPIO2->FIOPIN );    // P2.12 KEY2 button
    if (!New_KEY2_ButtonState)        // pressed
        if (Old_KEY2_ButtonState != New_KEY2_ButtonState){
            T2_MinimumMeasurement = 0x0FFFFFFF;    // highest value for minimum to enable new capture measurement of lower baudrate
            T2Baudrate = 0;        // Reset T2Bd
            LPC_UART0->ACR = (1<<0)|(1<<2);    //UART0  Auto-Baud start and Restart //Mode 0
            LPC_UART0->IER |= (1<<8);                //UART0  set ABEOIntEn
            LPC_UART2->ACR = (1<<0)|(1<<2);    //UART2  Auto-Baud start and Restart //Mode 0
            LPC_UART2->IER |= (1<<8);                //UART2  set ABEOIntEn
        }

    Old_KEY2_ButtonState = New_KEY2_ButtonState;

} // end of SysTick_Handler


/*****************************************************************************
**   TIMER0 Interrupt used sequencing data outputs to GLCD Display Module
**   in Lab2 of the lecture EMB
**   Functions: TIMER0_IRQHandler
**   LDK / HS Mannheim
**   History
**   2015.05.14  ver 0.01
*****************************************************************************/
void TIMER0_IRQHandler (void){    // Time in seconds for LCD frames (received characters)

    if (New_UART_Data_Received){

        if (Selected_UART_PORT==0)  {                // Selected_UART_PORT==0
            //LCD_Output_String = LCD_U1_ReceivedBytes;
            LCD_Output_String = UART0_ReceivedBytes;
            RX_MaxNumberChar =  U0_RX_NumberOfChar;
        }
        else if (Selected_UART_PORT==2)  {    // Selected_UART_PORT==2
            //LCD_Output_String = LCD_U2_ReceivedBytes;
            LCD_Output_String = UART2_ReceivedBytes;
            RX_MaxNumberChar =  U2_RX_NumberOfChar;
        }

        New_UART_Data_Received = 0;
        Launch_LCD_Output = 1;
    }
    LCD_RefreshTick = 1;
    LPC_TIM0->IR = (1 << 0);  //Clear interrupt

} // end of TIMER0_IRQHandler


/*****************************************************************************
** Function name:        TIMER2_IRQHandler                    --> your task in L2-5
**
** Descriptions:        TIMER2 interrupt service routine for capture measurement of UART baudrate
**                  Uses CAP2.1 input (port P0.5)
**
** Parameters:            None
** Returned value:        Return only in global variable "T2Baudrate"
**
*****************************************************************************/
void TIMER2_IRQHandler (void){    // Capture measurement of baudrate
    // Put your code here:
    //  :

}//end of TIMER2_IRQHandler


/*****************************************************************************
** Function name:        init_SysTick
**
** Descriptions:        Initialization of the System Timer SysTick
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
void init_SysTick(void){
    // Configure SysTick to trigger every millisecond using the CPU Clock
    SysTick->CTRL = 0;                    // Disable the SysTick Module
    /* A clock frequency of 100 MHz is assumed  */
    SysTick->LOAD = Systick_RELOAD_VALUE;         // Set the Reload Register for 100ms interrupts
    NVIC_SetPriority(SysTick_IRQn, 1);    // Set the interrupt priority to least urgency
    SysTick->VAL = 0;                     // Clear the Current Value register
    SysTick->CTRL = (1<<0)|(1<<1)|(1<<2);        // Enable SysTick, Enable SysTick Exceptions, Use CPU Clock
}


/*****************************************************************************
** Function name:        init_Timer0_for_UART                 --> your task in L2-4
**
** Descriptions:        Initialization of Timer0 (Time in seconds for LCD frames)
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
void init_Timer0_for_UART(void) {
    LPC_TIM0->MR0 = LCDout_ZeitDauer_Sek * 25000000; // Zeit[s] * 25MHz = Cycles
    LPC_TIM0->MCR = (1 << 0) | (1 << 1);             // MR0I + MR0R
    LPC_TIM0->TCR = 1;                               // Timer starten
} // end of init_Timer0_for_UART


/*****************************************************************************
** Function name:        init_Timer2                          --> your task in L2-5
**
** Descriptions:        Initialization of Timer2 (Capture measurement of baudrate)
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
void init_Timer2(void){
    // Put your code here:
    //  :

}//end of init_Timer2


/*****************************************************************************
** Function name:        init_NVIC                            --> your task in L2-4
**
** Descriptions:        Initialization of NVIC (Nested Vectored Interrupt Controller)
**
** parameters:            None
** Returned value:        None
**
*****************************************************************************/
void init_NVIC(void){        // Interrupt enable
    NVIC_SetPriority(TIMER0_IRQn, 3);
    NVIC_EnableIRQ(TIMER0_IRQn);
    
    NVIC_SetPriority(UART0_IRQn, 2);
    NVIC_EnableIRQ(UART0_IRQn);
    
    NVIC_SetPriority(UART2_IRQn, 2);
    NVIC_EnableIRQ(UART2_IRQn);
} //end of init_NVIC

