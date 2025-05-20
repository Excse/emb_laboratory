    THUMB ; Directive indicating the use of UAL
    AREA Code1, CODE, READONLY, ALIGN=4

    INCLUDE    LPC1768.inc
    EXPORT     UART_init
    ; Assumption
    ; R0 = UART_PortNum ; 0 or 2
    ; R1 = UxDL = (UxDLM << 8) | UxDLL
    ; R2 = (UxFDR (7:4 MULVAL  3:0 DIVADDVAL) <<8) | UxLCR (UART_Mode) )
    ; R3 = (UxIER << 8) | UxFCR)

UART_init    PROC
    STMFD SP!,{R4-R7,R14}
    LDR   R7,=PINSEL0         ; PIN_SEL0 needed for both UART0 and UART1 modules
                              ; PIN_SEL0[5:4] = 01 enable TXD0 (UART0 TXD), PIN_SEL0[7:6] = 01 enable
                              ; RXD0 ()UART0 RXD), PIN_SEL0[31:30] = 01 enable TXD1 (UART1 TXD)
    LDR   R4, [R7]
    MOV   R5, #0x3            ; Constant value used to clear two consecutive bits in a register
    MOV   R6, #0x1            ; Constant binary value (two bits) to be written in PINxSEL Registers
    CBZ   R0, Config_UART0    ; Check if UART Port 0 or Port 2 selected
;    ANDS     R0, R0, #0x01   ; Check if UART_PortNum = R0 = 1
;    BNE     Config_UART1     ; Go configure UART1

Config_UART2
    BIC        R4, R4, R5, LSL#20  ; Clear bits 21 and 20 in PINSEL0
    ORR        R4, R4, R6, LSL#20  ; Set PINSEL0[21:20] = 01 enable TXD2

    BIC        R4, R4, R5, LSL#22  ; Clear bits 23 and 22 in PINSEL0
    ORR        R4, R4, R6, LSL#22  ; Set PINSEL0[23:22] = 01 enable RXD2

    STR        R4, [R7]            ; Store new value of PINSEL0

    LDR        R0, =PCONP
    LDR        R4, [R0]
    ORR        R4,#1<<24           ; PCONP[24]: PCUART2 UART 2 power/clock control bit
    STR        R4,[R0]

    LDR        R0, =U2FCR
    STRB       R3, [R0]
    LDR        R0, =U2IER          ; Load address of UART2 IER register in R0
    MOV        R3, R3, LSR#8
    STR        R3, [R0]            ; Includes bit 8: ABEOIntEn

    LDR        R0, =U2DLM          ; Load address of UART2 DLM register in R0
    LDR        R3, =U2DLL          ; Load address of UART2 DLL register in R3
    LDR        R4, =U2LCR          ; Load address of UART2 LCR register in R4
    LDR        R7, =U2FDR          ; Load address of UART2 FDR register in R7


;Config_UART1
;    BIC        R4, R4, R5, LSL#30  ; Clear bits 31 and 30 in PINSEL0
;    ORR        R4, R4, R6, LSL#30  ; Set bit 30 in PINSEL0
;    STR        R4, [R7]            ; Store new value of PINSEL0[31:30] = 01 enable TXD1
;    LDR        R7, =PINSEL1        ; PIN_SEL1[1:0] = 01 enable RXD1 for UART1 RXD1 (UART1 RXD)
;    LDR        R4, [R7]
;    ORR        R5, R6, R6, LSL#1
;    BIC        R4, R4, R5          ; Clear bits 1 and 0 in PINSEL1
;    ORR        R4, R4, R6          ; Set bit 0 in PINSEL1
;    STR        R4, [R7]            ; Store new value of PINSEL1

;    LDR        R0, =U1FCR
;    STRB    R3, [R0]
;    LDR        R0, =U1IER      ; Load address of UART1 IER register in R0
;    MOV        R3, R3, LSR#8
;    STR        R3, [R0]        ; Includes bit 8: ABEOIntEn

;    LDR        R0, =U1DLM      ; Load address of UART1 DLM register in R0
;    LDR        R3, =U1DLL      ; Load address of UART1 DLL register in R3
;    LDR        R4, =U1LCR      ; Load address of UART1 LCR register in R4
;    LDR        R7, =U1FDR      ; Load address of UART1 FDR register in R7

    B     U_DL_Config

Config_UART0
    BIC        R4, R4, R5, LSL#4    ; Clear bits 4 and 5 in PINSEL0
    BIC        R4, R4, R5, LSL#6    ; Clear bits 6 and 7 in PINSEL0
    ORR        R4, R4, R6, LSL#4    ; Set bit 4 in PINSEL0
    ORR        R4, R4, R6, LSL#6    ; Set bit 6 in PINSEL0
    STR        R4, [R7]             ; Store new value of PINSEL0

    LDR        R0, =U0FCR
    STRB       R3, [R0]
    LDR        R0, =U0IER           ; Load address of UART0 IER register in R0
    MOV        R3, R3, LSR#8
    STR        R3, [R0]             ; Includes bit 8: ABEOIntEn

    LDR        R0, =U0DLM           ; Load address of UART0 DLM register in R0
    LDR        R3, =U0DLL           ; Load address of UART0 DLL register in R3
    LDR        R4, =U0LCR           ; Load address of UART0 LCR register in R4
    LDR        R7, =U0FDR           ; Load address of UART0 FDR register in R7


U_DL_Config
    ORR     R2, R2, #0x80        ; Set Bit 8 (DLAB) in content of UxLCR
    STRB    R2, [R4]             ; Store UxLCR with DLAB = 1
    STRB    R1, [R3]             ; Store DLL value
    MOV     R6, R1, LSR#8        ; Shift DLM Value to bits 7:0
    STRB    R6, [R0]             ; Store DLM value
    MOV     R6, R2, LSR#8        ; Shift FDR Value to bits 7:0
    STRB    R6, [R7]             ; Store UxFDR value
    BIC     R2, R2, #0x80        ; Clear DLAB Bit from LCR
    STRB    R2, [R4]             ; Store UxLCR

    LDMFD   SP!,{R4-R7, PC}

    ENDP    ; End Procedure
    END