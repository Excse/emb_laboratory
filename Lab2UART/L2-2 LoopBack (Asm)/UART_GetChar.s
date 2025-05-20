	THUMB ; Directive indicating the use of UAL
	AREA Code1, CODE, READONLY, ALIGN=4

	INCLUDE	LPC1768.inc
	EXPORT 	UART_GetChar
	;Assumption R0 = UART_PortNum (0 or 2 for LandTiger EVB)

UART_GetChar	PROC
;LSR: RDR lesen
;wenn gesetzt, RBR lesen
	STMFD	SP!,{... , LR}
	;
	; PUT YOUR CODE HERE
	;

UART_Rx_Done
	LDMFD	SP!,{..., PC}

	ENDP		; End Procedure

	END