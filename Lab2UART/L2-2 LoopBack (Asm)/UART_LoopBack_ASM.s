	THUMB ; Directive indicating the use of UAL
	AREA Code1, CODE, READONLY, ALIGN=4

	INCLUDE	LPC1768.inc

	IMPORT UART_init
	IMPORT UART_PutChar
	IMPORT UART_GetChar

	EXPORT __main
	ENTRY
__main	PROC

	;
	; PUT YOUR CODE HERE
	;

	ENDP	; End Procedure

	END