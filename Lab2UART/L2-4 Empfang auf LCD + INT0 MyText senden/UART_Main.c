#include <LPC17xx.h>
#include "GLCD.h"
#include <stdio.h>
#include "Mytext.h"

// UART variables:
int Selected_UART_PORT, UxDL, UxFDR_LCR, UxFCR, UxIER;
extern int IIR_Content, LSR_Content;
extern unsigned char Dummy;
extern int to_read_Baudrate;
int UxBaudrate = 0;

//UART receive + LCD output variables:
extern volatile unsigned char *LCD_Output_String;
extern volatile int Launch_LCD_Output, LCD_RefreshTick;
extern volatile int RX_MaxNumberChar, U0_rx_sw_buffer_index, U2_rx_sw_buffer_index;
int Ux_rx_sw_buffer_index;
unsigned volatile int LCD_Out_NumberLines, ScreenCounter, LineCounter;
unsigned volatile int LCD_Number_ScreenTurns;
unsigned int CharCounter, LCD_line, EndCharIndex;
unsigned char Zeile[21];
char AsciiChar [1]=" ";        // char AsciiChar [2]= {' ',0x00}; // null terminated for sprintf with %c
unsigned char LCDtext1[30];        // LCD line has 20 characters
unsigned char LCDtext2[30];

//UART transmit variables:
char *UART_TX_String;

// Systick variables:
extern int INT0_RisingEdgeDetected;
extern int INT0_PulsWidthCounter;
extern int KEY1_ScreenCounter;
int DebounceThreshold = 2;

// Timer2 variables:
extern unsigned int T2Baudrate;


// Procedures:
extern void UART_init(int UART_PortNum, int UxDL, int UxFDR_LCR, int UxIER_FCR);
extern void UART_Start_Char_Transmit(int PortNum, int NumberChar);
extern int Read_Baudrate (int Selected_UART_PORT);
extern void init_SysTick(void);
extern void init_Timer0_for_UART(void);
extern void init_NVIC(void);


int main (void) {

  int NumberChar,i;

// --- UART Init ---
//  UART baudrate selection & Mode:
//  Divisor   Fractional Divider  LCR Mode 0x03: 8 Bits, 1 stop bit, no parity, break transmission disabled
//    UxDL =5208;        UxFDR_LCR = 0x1003;        // no   Fractional Divider     300 Bd (   300,02 Bd  0,01%)
//    UxDL =3125;        UxFDR_LCR = 0x3203;        // with Fractional Divider     300 Bd (   300,00 Bd  0,00%)

        UxDL = 163;        UxFDR_LCR = 0x1003;        // no   Fractional Divider    9600 Bd (  9585,89 Bd -0,15%)
//    UxDL = 162;        UxFDR_LCR = 0x1003;        // no   Fractional Divider    9600 Bd (  9645,06 Bd  0,47%)
//    UxDL =  93;        UxFDR_LCR = 0x4303;        // with Fractional Divider    9600 Bd (  9600,61 Bd  0,01%)

//    UxDL =  41;        UxFDR_LCR = 0x1003;        // no   Fractional Divider   38400 Bd ( 38109,76 Bd    -0,76%)
//    UxDL =  40;        UxFDR_LCR = 0x1003;        // no   Fractional Divider   38400 Bd ( 39062,50 Bd  1,73%)
//    UxDL =  25;        UxFDR_LCR = 0x8503;        // with Fractional Divider   38400 Bd ( 38461,54 Bd  0,16%)

//    UxDL =  14;        UxFDR_LCR = 0x1003;        // no   Fractional Divider  115200 Bd (111607,14 Bd    -3,12%)
//    UxDL =  13;        UxFDR_LCR = 0x1003;        // no   Fractional Divider  115200 Bd (120192,31 Bd  4,33%)
//    UxDL =  11;        UxFDR_LCR = 0xD303;        // with Fractional Divider  115200 Bd (115411,93 Bd  0,18%)

    UxIER = 0x01; // RX Interrupt enabled
    UxFCR = 0xC3;    // FIFO enabled, (TX FIFO cleared, RX FIFI cleared), RX FIFO trigger level = 14

    Selected_UART_PORT = 0;
    UART_init (Selected_UART_PORT , UxDL, UxFDR_LCR, ((UxIER << 8) | UxFCR));

    /******************************************************************************
          AUTO-BAUD  Measurement (in L2-6)
    *******************************************************************************/
/*
    LPC_UART0->ACR = (1<<0)|(1<<2);    //UART0  Auto-Baud start and Restart //Mode 0
    LPC_UART0->IER |= (1<<8);                //UART0  set ABEOIntEn
    LPC_UART2->ACR = (1<<0)|(1<<2);    //UART2  Auto-Baud start and Restart //Mode 0
    LPC_UART0->IER |= (1<<8);                //UART0  set ABEOIntEn  */
    
    // GPIO for buttons INT0, KEY1, KEY2
    LPC_PINCON->PINSEL4 &= ~(3 << 20);     //Clear the bits to be updated  //PORT2.10 (INT0 Pin) as GPIO  -> button INT0 for UART send string
    LPC_PINCON->PINSEL4 &= ~(3 << 22);     //Clear the bits to be updated  //PORT2.11 (KEY1 Pin) as GPIO  -> button KEY1 to select debug screen
    LPC_PINCON->PINSEL4 &= ~(3 << 24);     //Clear the bits to be updated  //PORT2.12 (KEY2 Pin) as GPIO  -> button KEY2 to reset T2Baudrate

    init_SysTick( );
    init_Timer0_for_UART();
    init_NVIC();


    GLCD_Init();                                    /* Initialize the GLCD */
    GLCD_Clear(White);                              /* Clear the GLCD */
    GLCD_SetBackColor(White);             /* Set the Back Color */
    GLCD_SetTextColor(Blue);              /* Set the Text Color */
    GLCD_DisplayString(3, 0, (unsigned char*)"Tastenbelegung:");        // Info Screen before first serial receive
    GLCD_DisplayString(5, 0, (unsigned char*)"KEY1: Debug Screen");
    GLCD_DisplayString(6, 0, (unsigned char*)"KEY2: Timer2 Reset");
    GLCD_DisplayString(7, 0, (unsigned char*)"INT0: Send String");
    to_read_Baudrate = 1;        // Read_Baudrate() to be executed

    while (1)  {

        // LCD Blue Debug Screen:
      GLCD_SetBackColor(Blue);                        /* Set the Back Color */
        GLCD_SetTextColor(White);                       /* Set the Text Color */

        // Prepare data for Blue Debug Screen:
        if (to_read_Baudrate)
            UxBaudrate = Read_Baudrate(Selected_UART_PORT);
        to_read_Baudrate = 0;        // Read_Baudrate() is done
        if (Selected_UART_PORT==0)  Ux_rx_sw_buffer_index = U0_rx_sw_buffer_index;
        else                        Ux_rx_sw_buffer_index = U2_rx_sw_buffer_index;

        // Copy Dummy character to a string for sprintf ascii output of a single char with %c
        AsciiChar[0]=Dummy;        // Dummy character null terminated for sprintf with %c
        // Replace special characters with readable characters:
        if (Dummy < 0x20)            // control characters 0x00 .. 0x1F
            AsciiChar[0]=0x88;    // arrow left
        if (Dummy > 0x8F)            // special characters 0x90 .. 0xFF
            AsciiChar[0]=0x8A;    // arrow right
        if (Dummy == 0x0A)            // control character 0x0A = LineFeed
            AsciiChar[0]=0x86;    // arrow down

        //AsciiChar[1] = 0x00;    // String Termination

        // LCD Blue Screen - Line 1
        if      (KEY1_ScreenCounter==0){        // LCD Blue Screen: S0  Welcome
            sprintf((char*)LCDtext1, "EMB Versuch 2: UART%d ", Selected_UART_PORT );
        }
        else if (KEY1_ScreenCounter==1){        // LCD Blue Screen: S1  T2Bd
            sprintf((char*)LCDtext1, "S%-2d T2Bd=%-6d     ", KEY1_ScreenCounter, T2Baudrate);
        }
        else if (KEY1_ScreenCounter==2){        // LCD Blue Screen: S2  IIR + LSR Content
            sprintf((char*)LCDtext1, "S%-1d P%-2dIIR=%03X LSR=%02X ", KEY1_ScreenCounter, INT0_PulsWidthCounter, IIR_Content, LSR_Content);
        }
        
        sprintf((char*)LCDtext2, "B%-3dU%dBd=%-6d %02X %c ", Ux_rx_sw_buffer_index, Selected_UART_PORT, UxBaudrate, Dummy, *AsciiChar);  // LCD Blue Screen - Line 2

        // LCD Blue Screen for Debug Output
        GLCD_SetBackColor(Blue);              /* Set the Back Color */
        GLCD_SetTextColor(White);             /* Set the Text Color */
        GLCD_DisplayString(0, 0, LCDtext1);        /* print string to LCD */
        GLCD_DisplayString(1, 0, LCDtext2);        /* print string to LCD */

        // LCD White Screen for received characters:
        GLCD_SetBackColor(White);             /* Set the Back Color */
        GLCD_SetTextColor(Blue);              /* Set the Text Color */

        if (Dummy == 0x32)        // received character
            __NOP();    // set breakpoint here to stop at a specific character received

        // Shall Data on Display be updated?
        if (Launch_LCD_Output){
            LCD_RefreshTick = 0;
            LCD_Out_NumberLines = (int) ((RX_MaxNumberChar + (20 -1)) /20);     // Assuming 20 Chars per Line
            LCD_Number_ScreenTurns = (int) ((LCD_Out_NumberLines + (6 -1))/ 6); // Assuming 6 lines for data per screen

                //Clear Lines 2 to 9
                for (i=2;i < 9; i++){
                        GLCD_ClearLn(i);
                }

            for (ScreenCounter = 0; ScreenCounter < LCD_Number_ScreenTurns; ScreenCounter++){
                for (LineCounter= 6*ScreenCounter; ((LineCounter < (6*(ScreenCounter+1))) &&
                                                                (LineCounter < LCD_Out_NumberLines )) ; LineCounter++){
                    if (( LineCounter < (LCD_Out_NumberLines - 1)) && ( LCD_Out_NumberLines > 1)){ //Not yet on last line
                        for (CharCounter = 0; CharCounter < 20; CharCounter++){
                            Zeile[CharCounter] = *(LCD_Output_String + LineCounter*20 + CharCounter);
                        }
                    }
                    else {
                        EndCharIndex = RX_MaxNumberChar - 20*(LCD_Out_NumberLines - 1);
                        for (CharCounter = 0; CharCounter < EndCharIndex; CharCounter++){
                            Zeile[CharCounter] = *(LCD_Output_String + LineCounter*20 + CharCounter);
                        }
                        if ( CharCounter == EndCharIndex ){
                            for (CharCounter = EndCharIndex; CharCounter < 20; CharCounter++)
                                        Zeile[CharCounter] = 0;
                        }
                    }
                    LCD_line = (LineCounter%6) + 3;
                    GLCD_DisplayString(LCD_line, 0, Zeile);
                }
                if ( ScreenCounter < (LCD_Number_ScreenTurns-1)){

                    while(LCD_RefreshTick ==0);
                    // Clear lines 2 to 9 to prepare new screen
                    for (i=2;i < 9; i++){
                        GLCD_ClearLn(i);
                        LCD_RefreshTick =0;
                    }
                }
            }
            Launch_LCD_Output = 0;
        }

        //If button INT0 has been pressed: sent data over UARTx
        if ((INT0_RisingEdgeDetected==1) && (INT0_PulsWidthCounter > DebounceThreshold)){
            UART_TX_String = MyText;
            NumberChar = 0;
            while(*UART_TX_String++) NumberChar++;
            UART_TX_String =     MyText;
            if ( NumberChar > 0)
                UART_Start_Char_Transmit(Selected_UART_PORT, NumberChar);
        }
        INT0_RisingEdgeDetected = 0;
    }
} // end of main
