
#include <stdio.h>
#include <LPC17xx.H>  

#define INVALID_CHAR 0xFFFFFFFF
#define UART_PORT    0x0000
#define UART_DL      0x000B
#define UART_FDR_LCR 0xD303
#define UART_IER_FCR 0x0007

extern void UART_init(int UART_PortNum, int UxDL, int UxFDR_LCR, int UxIER_FCR);

extern void UART_PutChar(int UART_PortNum, char Char);

extern char UART_GetChar(int UART_PortNum);

int main(void){
    UART_init(UART_PORT, UART_DL, UART_FDR_LCR, UART_IER_FCR);
    
    while(1) {
        char read = UART_GetChar(UART_PORT);
        if(read == INVALID_CHAR) continue;
        
        UART_PutChar(UART_PORT, read);
    }
}

