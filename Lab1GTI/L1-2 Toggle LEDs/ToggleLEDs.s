		AREA TASTER_LED, CODE, READONLY
		THUMB
		ALIGN
			
		INCLUDE LPC1768.inc
			
		EXPORT __main
		ENTRY
__main  PROC
	
init	LDR R0, =FIO2DIR		; Lade die Addresse von FIO2DIR in R0
		LDR R1, [R0]			; Lade den Wert an der Addresse in R1
		BIC R1, R1, #(1 << 10)	; Setze Bit 10 auf 0 (Input)
		ORR R1, R1, #0xF8		; Setze Bit 3..7 auf 1 (Output)
		STR R1, [R0]			; Speicher den Inhalt von R1 an der Addresse von R0
		
		LDR R2, =FIO2PIN		; Lade die Addresse von FIO2PIN in R2
		LDR R1, [R2]			; Lade den Wert an der Addresse in R1
		BIC R1, R1, #0xF8		; Setze Bit 3..7 auf 0 (LED ausschalten)
		STR R1, [R2]			; Speicher den Inhalt von R1 an der Addresse von R0
		
main_loop
		LDR R1, [R2]			; Auslesen der PINs
		TST R1, #(1 << 10)		; Testen ob der Knopf gedrückt worden ist
		BNE main_loop			; Falls Knopf nicht gedrückt worden ist, zurück zur main_loop
		
		LDR R3, =0x1312D0		; 10MHz * 100ms = 10'000'000 Cycles, delay-Block benötigt ca. 8 cycles pro Iteration
								; Somit benötigt man circa 1'250'000 Iterationen für 100ms (0x1312D0)
delay	LDR R1, [R2]			; Prüfe jede Iteration, ob der Knopf auch wirklich noch gedrückt ist
		TST R1, #(1 << 10)		; ...
		BNE main_loop			; ...
		SUBS R3, R3, #1			; Ziehe eins von dem Delay-Counter ab
		BNE delay				; Falls der Counter abgelaufen ist, gehe weiter, falls nicht wiederhole den Vorgang
		
wait	LDR R1, [R2]			; Warte solange, bis der Knopf nicht mehr gedrückt wird
		TST R1, #(1 << 10)		; ...
		BEQ wait				; ...
		
		EOR R1, R1, #0xF8		; Toggle alle LEDs
		STR R1, [R2]			; ...
		B main_loop				; Fange alles von vorne an
		
		ENDP
		END