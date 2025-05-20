    THUMB ; Directive indicating the use of UAL
    AREA Code1, CODE, READONLY, ALIGN=4

    INCLUDE    LPC1768.inc

    IMPORT UART_init
    IMPORT UART_PutChar
    IMPORT UART_GetChar

    EXPORT __main
    ENTRY
__main    PROC
    
Configure_UART
    LDR R0, =0x0002     ; UART_PortNum                
    LDR R1, =0x000B     ; UxDL  = (UxDLM << 8) | (UxDLL)
    LDR R2, =0xD303     ; (UxFDR (7:4 MULVAL  3:0 DIVADDVAL) <<8) | UxLCR (UART_Mode) ):
                        ; DLAB 0  Set Break 0  Stick Parity 0  Even Parity Select 0  
                        ; Parity Enable 0  Number of Stop Bits 0  Word Length Select 11
    LDR R3, =0x0007     ; (UxIER << 8) | UxFCR)        
                        ; TX FIFO Reset 1 ; RX FIFO Reset 1 ; FIFO Enable 1

    MOV R4, R0          ; Copy of PortNum (weil R0..R3 Scratch-Register sind, d.h. in einer gerufenen Funtion verändert werden können!)
    BL  UART_init

Loopback
    MOV R0, R4          ; Setze R0 wieder auf UART_PortNum.
    BL  UART_GetChar    ; Lese ein Zeichen.
    
    LDR R1, =0xFFFFFFFF
    CMP R0, R1          ; Teste ob in R0 der Wert 0xFFFFFFFF steht.
    BEQ Loopback        ; Falls nicht gehe zurück zur Hauptschleife.
    
    MOV R1, R0            ; Schreibe R0 nach R1 um UART_PutChar aufrufen zu können.
    MOV R0, R4          ; Setze R0 wieder auf UART_PortNum.
    BL  UART_PutChar    ; Schreibe das Zeichen erneut.

    B Loopback
    ENDP 
    END
