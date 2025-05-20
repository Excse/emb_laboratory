
#include <stdio.h>
#include <LPC17xx.H>  

// Put some lines here  e.g. DEFINE ...

unsigned char MyChar;
unsigned int PortNum, UDL, FDRvalue, UART_mode, IntEnableValue, FIFO_mode, Temp;
// PortNum = Port Number excepted to be 0 or 2 for the the LandTiger EVB
// UDL = U_DLM*256 + U_DLL to set the data rate
// FDRvalue  = Content of the register UxFDR (Fractional Divider)
// UART_mode = Content of the register UxLCR without the DLAB bit
// IntEnableValue = Content of the register UxIER
// FIFO_mode = Content of the register UxFCR (FIFO Control Register)



int main(void){

// Put your code here

}

