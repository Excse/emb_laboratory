        AREA TASTER_LED, CODE, READONLY
        THUMB
        ALIGN
            
        INCLUDE LPC1768.inc
            
        EXPORT __main
        ENTRY
__main  PROC
    
init    LDR R0, =FIO2DIR        ; Lade die Addresse von FIO2DIR in R0
        LDR R1, [R0]            ; Lade den Wert an der Addresse in R1
        BIC R1, R1, #(1 << 10)  ; Setze Bit 10 auf 0 (Input)
        ORR R1, R1, #(1 << 2)   ; Setze Bit 2 auf 1 (Output)
        STR R1, [R0]            ; Speicher den Inhalt von R1 an der Addresse von R0
        
        LDR R2, =FIO2PIN        ; Lade die Addresse von FIO2PIN in R2
        LDR R1, [R2]            ; Lade den Wert an der Addresse in R1
        BIC R1, R1, #(1 << 2)   ; Setze Bit 2 auf 0 (LED ausschalten)
        STR R1, [R2]            ; Speicher den Inhalt von R1 an der Addresse von R0
        
main_loop
        LDR R1, [R2]            ; Auslesen der PINs
        TST R1, #(1 << 10)      ; Testen ob der Knopf gedrückt worden ist
        ITE EQ                  ; Branch mit Then & Else
        ORREQ R1, R1, #(1 << 2) ; Then: Schalte LED an
        BICNE R1, R1, #(1 << 2) ; Else: Schalte LED aus
        STR R1, [R2]            ; Wert zurück auf die PINs schreiben
        B main_loop             ; Zurück zur Mainloop
        
        ENDP
        END