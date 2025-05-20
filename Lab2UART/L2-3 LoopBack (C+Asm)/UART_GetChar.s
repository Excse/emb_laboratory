    THUMB ; Directive indicating the use of UAL
    AREA Code1, CODE, READONLY, ALIGN=4

    INCLUDE    LPC1768.inc
    EXPORT     UART_GetChar
        
    ;Assumption R0 = UART_PortNum (0 or 2 for LandTiger EVB)

UART_GetChar    PROC
    STMFD SP!, {R4-R6, LR}
    CBZ   R0, UART_Port0   ; Check if UART Port 0 or Port 2 selected

UART_Port2
    LDR R4, =U2LSR         ; UART Port 2 selected
    LDR R5, =U2RBR           ; UART Port 2 RBR
    B   Check_RDR

UART_Port0
    LDR R4, =U0LSR         ; UART Port 0 selected
    LDR R5, =U0RBR           ; UART Port 0 RBR
    
Check_RDR
    LDRB R6, [R4]          ; Lade Inhalt des LSR
    TST  R6, #0x0001       ; Prüfe Bit 0 (RDR - Receiver Data Ready)
    BEQ  No_Data
    
Has_Data
    LDRB R0, [R5]
    B    UART_Rx_Done
    
No_Data
    LDR R0, =0xFFFFFFFF    ; Kein Zeichen, somit -1 zurückgeben

UART_Rx_Done
    LDMFD SP!, {R4-R6, PC}
    ENDP
    END