    THUMB ; Directive indicating the use of UAL
    AREA Code1, CODE, READONLY, ALIGN=4

    INCLUDE    LPC1768.inc
    EXPORT    UART_PutChar
    ;Assumption R0 = UART_PortNum (0 or 2 for LandTiger EVB), R1 = Char to be sent

UART_PutChar    PROC
    STMFD    SP!,{R4,R5, LR}
    CBZ     R0, UART_Port0    ; Check if UART Port 0 or Port 2 selected

UART_Port2
    LDR        R3,=U2LSR    ; UART Port 2 selected
    LDR        R4,=U2THR
    B Wait_TX

UART_Port0
    LDR        R3,=U0LSR    ; UART Port 0 selected
    LDR        R4,=U0THR

Wait_TX
    LDR        R5, [R3]    ; Content of UxLSR loaded in R3
    ANDS    R5, R5, #2_00100000    ; Test bit 5 of UxLSR
    BEQ        Wait_TX
    STR        R1, [R4]    ; Write new Char to UxTHR since UxTHR is empty

UART_Tx_Done
    LDMFD    SP!,{R4,R5, PC}

;    MOV        R15, R14    ; Return from branch with link
;    NOP                ; To avoid warning: Added 2 bytes of padding at address ...

    ENDP        ; End Procedure
    END