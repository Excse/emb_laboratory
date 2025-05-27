;***************************************************************************************
; Assembler Include File: Definition of peripheral registers for Cortex-M3 NXP LPC1768 *
; from: LPC176x,5x User manual (UM10360) v4.1    Pages: p.xxx/851                      *
; Hochschule Mannheim / Institut für Embedded Systems / Bernhard Pohlner               *
; Version 2019-1118                                                                    *
; for:                                                                                 *
; Chapter  4: LPC176x/5x Clocking and power control           p. 32/851                *
; Chapter  8: LPC176x/5x Pin connect block                    p.117/851                *
; Chapter  9: LPC176x/5x General Purpose Input/Output (GPIO)  p.132-139/851            *
; Chapter 14: LPC176x/5x UART0/2/3                            p.310/851                *
; Chapter 15: LPC176x/5x UART1                                p.330/851                *
; Chapter 21: LPC176x/5x Timer 0/1/2/3                        p.503/851                *
;***************************************************************************************
; Name		EQU	Address    ; Access		Reset Value		Description

; Chapter  4: LPC176x/5x Clocking and power control           p. 32/851

			; Clock source selection
CLKSRCSEL	EQU	0x400FC10C ; R/W 0  Clock Source Select Register
			; Phase Locked Loop (PLL0, Main PLL)
PLL0CON		EQU	0x400FC080 ; R/W 0  PLL0 Control Register
PLL0CFG		EQU	0x400FC084 ; R/W 0  PLL0 Configuration Register
PLL0STAT	EQU	0x400FC088 ; RO  0  PLL0 Status Register
PLL0FEED	EQU	0x400FC08C ; WO NA  PLL0 Feed Register
			; Phase Locked Loop (PLL1, USB PLL)
PLL1CON		EQU	0x400FC0A0 ; R/W 0  PLL1 Control Register
PLL1CFG		EQU	0x400FC0A4 ; R/W 0  PLL1 Configuration Register
PLL1STAT	EQU	0x400FC0A8 ; PLL1 Status Register RO 0
PLL1FEED	EQU	0x400FC0AC ; PLL1 Feed Register WO NA
			; Clock dividers
CCLKCFG		EQU	0x400FC104 ; R/W 0  CPU Clock Configuration Register
USBCLKCFG	EQU	0x400FC108 ; R/W 0  USB Clock Configuration Register
PCLKSEL0	EQU	0x400FC1A8 ; R/W 0  Peripheral Clock Selection register 0.
PCLKSEL1	EQU	0x400FC1AC ; R/W 0  Peripheral Clock Selection register 1.
			; Power control
PCON		EQU	0x400FC0C0 ; R/W 0  Power Control Register						p. 63/851
PCONP		EQU	0x400FC0C4 ; R/W 0x03BE  Power Control for Peripherals Register	p. 65/851
			; Utility
CLKOUTCFG	EQU	0x400FC1C8 ; R/W 0  Clock Output Configuration Register

; Chapter 8: LPC176x/5x Pin connect block  p.117/851

PINSEL0		EQU	0x4002C000 ; R/W 0  Pin function select register 0
PINSEL1		EQU	0x4002C004 ; R/W 0  Pin function select register 1
PINSEL2		EQU	0x4002C008 ; R/W 0  Pin function select register 2
PINSEL3		EQU	0x4002C00C ; R/W 0  Pin function select register 3
PINSEL4		EQU	0x4002C010 ; R/W 0  Pin function select register 4
PINSEL7		EQU	0x4002C01C ; R/W 0  Pin function select register 7
PINSEL8		EQU	0x4002C020 ; R/W 0  Pin function select register 8
PINSEL9		EQU	0x4002C024 ; R/W 0  Pin function select register 9
PINSEL10	EQU	0x4002C028 ; R/W 0  Pin function select register 10
PINMODE0	EQU	0x4002C040 ; R/W 0  Pin mode select register 0
PINMODE1	EQU	0x4002C044 ; R/W 0  Pin mode select register 1
PINMODE2	EQU	0x4002C048 ; R/W 0  Pin mode select register 2
PINMODE3	EQU	0x4002C04C ; R/W 0  Pin mode select register 3
PINMODE4	EQU	0x4002C050 ; R/W 0  Pin mode select register 4
PINMODE5	EQU	0x4002C054 ; R/W 0  Pin mode select register 5
PINMODE6	EQU	0x4002C058 ; R/W 0  Pin mode select register 6
PINMODE7	EQU	0x4002C05C ; R/W 0  Pin mode select register 7
PINMODE9	EQU	0x4002C064 ; R/W 0  Pin mode select register 9

PINMODE_OD0	EQU	0x4002C068 ; R/W 0  Open drain mode control register 0
PINMODE_OD1	EQU	0x4002C06C ; R/W 0  Open drain mode control register 1
PINMODE_OD2	EQU	0x4002C070 ; R/W 0  Open drain mode control register 2
PINMODE_OD3	EQU	0x4002C074 ; R/W 0  Open drain mode control register 3
PINMODE_OD4	EQU	0x4002C078 ; R/W 0  Open drain mode control register 4

I2CPADCFG	EQU	0x4002C07C ; R/W 0  I2C Pin Configuration register

; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.132/851

FIO0DIR		EQU	0x2009C000 ; R/W 0  FIODIR Fast GPIO Port Direction control register.
FIO1DIR		EQU	0x2009C020 ; This register individually controls the direction of each port pin.
FIO2DIR		EQU	0x2009C040
FIO3DIR		EQU	0x2009C060
FIO4DIR		EQU	0x2009C080

FIO0MASK	EQU	0x2009C010 ; R/W 0  FIOMASK Fast Mask register for port.
FIO1MASK	EQU	0x2009C030 ; Writes, sets, clears, and reads to port
FIO2MASK	EQU	0x2009C050 ; (done via writes to FIOPIN, FIOSET, and FIOCLR, and reads of FIOPIN)
FIO3MASK	EQU	0x2009C070 ; alter or return only the bits enabled by zeros in this register.
FIO4MASK	EQU	0x2009C090

FIO0PIN		EQU	0x2009C014 ; R/W 0  FIOPIN Fast Port Pin value register using FIOMASK.
FIO1PIN		EQU	0x2009C034 ; The current state of digital port pins can be read from this register,
FIO2PIN		EQU	0x2009C054 ; regardless of pin direction or alternate function selection
FIO3PIN		EQU	0x2009C074 ; (as long as pins are not configured as an input to ADC).
FIO4PIN		EQU	0x2009C094 ; The value read is masked by ANDing with inverted FIOMASK.
			; Writing to this register places corresponding values in all bits enabled by zeros in FIOMASK.
			;
			; Important: if an FIOPIN register is read, its bit(s) masked with 1 in the FIOMASK register
			; will be read as 0 regardless of the physical pin state.

FIO0SET		EQU	0x2009C018 ; R/W 0  FIOSET Fast Port Output Set register using FIOMASK.
FIO1SET		EQU	0x2009C038 ; This register controls the state of output pins.
FIO2SET		EQU	0x2009C058 ; Writing 1s produces highs at the corresponding port pins.
FIO3SET		EQU	0x2009C078 ; Writing 0s has no effect.
FIO4SET		EQU	0x2009C098 ; Only bits enabled by 0 in FIOMASK can be altered.
			; Reading this register returns the current contents of the port output register.

FIO0CLR		EQU	0x2009C01C ; WO  0  FIOCLR Fast Port Output Clear register using FIOMASK.
FIO1CLR		EQU	0x2009C03C ; This register controls the state of output pins.
FIO2CLR		EQU	0x2009C05C ; Writing 1s produces lows at the corresponding port pins.
FIO3CLR		EQU	0x2009C07C ; Writing 0s has no effect.
FIO4CLR		EQU	0x2009C09C ; Only bits enabled by 0 in FIOMASK can be altered.


; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.133/851


IO0IntEnR	EQU	0x40028090 ; R/W 0  IntEnR GPIO Interrupt Enable for Rising edge.
IO2IntEnR	EQU	0x400280B0

IO0IntEnF	EQU	0x40028094 ; R/W 0  IntEnF GPIO Interrupt Enable for Falling edge.  Druckfehler: IO0IntEnR
IO2IntEnF	EQU	0x400280B4 ;                                                                    IO2IntEnR

IO0IntStatR	EQU	0x40028084 ; RO  0  IntStatR GPIO Interrupt Status for Rising edge.
IO2IntStatR	EQU	0x400280A4

IO0IntStatF	EQU	0x40028088 ; RO  0  IntStatF GPIO Interrupt Status for Falling edge.
IO2IntStatF	EQU	0x400280A8

IO0IntClr	EQU	0x4002808C ; WO  0  IntClr GPIO Interrupt Clear.
IO2IntClr	EQU	0x400280AC

IOIntStatus	EQU	0x40028080 ; RO  0  IntStatus GPIO overall Interrupt Status.


; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.134/851


FIO0DIR0	EQU	0x2009C000 ; R/W 0x00  FIOxDIR0 Fast GPIO Port x Direction control register 0.
FIO1DIR0	EQU	0x2009C020 ; Bit 0 in FIOxDIR0 register corresponds to pin Px.0 … bit 7 to pin Px.7.
FIO2DIR0	EQU	0x2009C040 ; 8 (byte)
FIO3DIR0	EQU	0x2009C060
FIO4DIR0	EQU	0x2009C080

FIO0DIR1	EQU	0x2009C001 ; R/W 0x00  FIOxDIR1 Fast GPIO Port x Direction control register 1.
FIO1DIR1	EQU	0x2009C021 ; Bit 0 in FIOxDIR1 register corresponds to pin Px.8 … bit 7 to pin Px.15.
FIO2DIR1	EQU	0x2009C041 ; 8 (byte)
FIO3DIR1	EQU	0x2009C061
FIO4DIR1	EQU	0x2009C081

FIO0DIR2	EQU	0x2009C002 ; R/W 0x00  FIOxDIR2 Fast GPIO Port x Direction control register 2.
FIO1DIR2	EQU	0x2009C022 ; Bit 0 in FIOxDIR2 register corresponds to pin Px.16 … bit 7 to pin Px.23.
FIO2DIR2	EQU	0x2009C042 ; 8 (byte)
FIO3DIR2	EQU	0x2009C062
FIO4DIR2	EQU	0x2009C082

FIO0DIR3	EQU	0x2009C003 ; R/W 0x00  FIOxDIR3 Fast GPIO Port x Direction control register 3.
FIO1DIR3	EQU	0x2009C023 ; Bit 0 in FIOxDIR3 register corresponds to pin Px.24 … bit 7 to pin Px.31.
FIO2DIR3	EQU	0x2009C043 ; 8 (byte)
FIO3DIR3	EQU	0x2009C063
FIO4DIR3	EQU	0x2009C083

FIO0DIRL	EQU	0x2009C000 ; R/W 0x0000  FIOxDIRL Fast GPIO Port x Direction control Lower half-word register.
FIO1DIRL	EQU	0x2009C020 ; Bit 0 in FIOxDIRL register corresponds to pin Px.0 … bit 15 to pin Px.15.
FIO2DIRL	EQU	0x2009C040 ; 16 (half-word)
FIO3DIRL	EQU	0x2009C060
FIO4DIRL	EQU	0x2009C080

FIO0DIRU	EQU	0x2009C002 ; R/W 0x0000  FIOxDIRU Fast GPIO Port x Direction control Upper half-word register.
FIO1DIRU	EQU	0x2009C022 ; Bit 0 in FIOxDIRU register corresponds to Px.16 … bit 15 to Px.31.
FIO2DIRU	EQU	0x2009C042 ; 16 (half-word)
FIO3DIRU	EQU	0x2009C062
FIO4DIRU	EQU	0x2009C082

; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.135/851

FIO0SET0	EQU	0x2009C018 ; R/W 0x00  FIOxSET0 Fast GPIO Port x output Set register 0.
FIO1SET0	EQU	0x2009C038 ; Bit 0 in FIOxSET0 register corresponds to pin Px.0 … bit 7 to pin Px.7.
FIO2SET0	EQU	0x2009C058 ; 8 (byte)
FIO3SET0	EQU	0x2009C078
FIO4SET0	EQU	0x2009C098

FIO0SET1	EQU	0x2009C019 ; R/W 0x00  FIOxSET1 Fast GPIO Port x output Set register 1.
FIO1SET1	EQU	0x2009C039 ; Bit 0 in FIOxSET1 register corresponds to pin Px.8 … bit 7 to pin Px.15.
FIO2SET1	EQU	0x2009C059 ; 8 (byte)
FIO3SET1	EQU	0x2009C079
FIO4SET1	EQU	0x2009C099

FIO0SET2	EQU	0x2009C01A ; R/W 0x00  FIOxSET2 Fast GPIO Port x output Set register 2.
FIO1SET2	EQU	0x2009C03A ; Bit 0 in FIOxSET2 register corresponds to pin Px.16 … bit 7 to pin Px.23.
FIO2SET2	EQU	0x2009C05A ; 8 (byte)
FIO3SET2	EQU	0x2009C07A
FIO4SET2	EQU	0x2009C09A

FIO0SET3	EQU	0x2009C01B ; R/W 0x00  FIOxSET3 Fast GPIO Port x output Set register 3.
FIO1SET3	EQU	0x2009C03B ; Bit 0 in FIOxSET3 register corresponds to pin Px.24 … bit 7 to pin Px.31.
FIO2SET3	EQU	0x2009C05B ; 8 (byte)
FIO3SET3	EQU	0x2009C07B
FIO4SET3	EQU	0x2009C09B

FIO0SETL	EQU	0x2009C018 ; R/W 0x0000  FIOxSETL Fast GPIO Port x output Set Lower half-word register.
FIO1SETL	EQU	0x2009C038 ; Bit 0 in FIOxSETL register corresponds to pin Px.0 … bit 15 to pin Px.15.
FIO2SETL	EQU	0x2009C058 ; 16 (half-word)
FIO3SETL	EQU	0x2009C078
FIO4SETL	EQU	0x2009C098

FIO0SETU	EQU	0x2009C01A ; R/W 0x0000  FIOxSETU Fast GPIO Port x output Set Upper half-word register.
FIO1SETU	EQU	0x2009C03A ; Bit 0 in FIOxSETU register corresponds to Px.16 … bit 15 to Px.31.
FIO2SETU	EQU	0x2009C05A ; 16 (half-word)
FIO3SETU	EQU	0x2009C07A
FIO4SETU	EQU	0x2009C09A

; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.136+137/851

FIO0CLR0	EQU	0x2009C01C ; WO  0x00  FIOxCLR0 Fast GPIO Port x output Clear register 0.
FIO1CLR0	EQU	0x2009C03C ; Bit 0 in FIOxCLR0 register corresponds to pin Px.0 … bit 7 to pin Px.7.
FIO2CLR0	EQU	0x2009C05C ; 8 (byte)
FIO3CLR0	EQU	0x2009C07C
FIO4CLR0	EQU	0x2009C09C

FIO0CLR1	EQU	0x2009C01D ; WO  0x00  FIOxCLR1 Fast GPIO Port x output Clear register 1.
FIO1CLR1	EQU	0x2009C03D ; Bit 0 in FIOxCLR1 register corresponds to pin Px.8 … bit 7 to pin Px.15.
FIO2CLR1	EQU	0x2009C05D ; 8 (byte)
FIO3CLR1	EQU	0x2009C07D
FIO4CLR1	EQU	0x2009C09D

FIO0CLR2	EQU	0x2009C01E ; WO  0x00  FIOxCLR2 Fast GPIO Port x output Clear register 2.
FIO1CLR2	EQU	0x2009C03E ; Bit 0 in FIOxCLR2 register corresponds to pin Px.16 … bit 7 to pin Px.23.
FIO2CLR2	EQU	0x2009C05E ; 8 (byte)
FIO3CLR2	EQU	0x2009C07E
FIO4CLR2	EQU	0x2009C09E

FIO0CLR3	EQU	0x2009C01F ; WO  0x00  FIOxCLR3 Fast GPIO Port x output
FIO1CLR3	EQU	0x2009C03F ; Clear register 3. Bit 0 in FIOxCLR3 register corresponds to pin Px.24 … bit 7 to pin Px.31.
FIO2CLR3	EQU	0x2009C05F ; 8 (byte)
FIO3CLR3	EQU	0x2009C07F
FIO4CLR3	EQU	0x2009C09F

FIO0CLRL	EQU	0x2009C01C ; WO  0x0000  FIOxCLRL Fast GPIO Port x output Clear Lower half-word register.
FIO1CLRL	EQU	0x2009C03C ; Bit 0 in FIOxCLRL register corresponds to pin Px.0 … bit 15 to pin Px.15.
FIO2CLRL	EQU	0x2009C05C ; 16 (half-word)
FIO3CLRL	EQU	0x2009C07C
FIO4CLRL	EQU	0x2009C09C

FIO0CLRU	EQU	0x2009C01E ; WO  0x0000  FIOxCLRU Fast GPIO Port x output eaClr Upper half-word register.
FIO1CLRU	EQU	0x2009C03E ; Bit 0 in FIOxCLRU register corresponds to pin Px.16 … bit 15 to Px.31.
FIO2CLRU	EQU	0x2009C05E ; 16 (half-word)
FIO3CLRU	EQU	0x2009C07E
FIO4CLRU	EQU	0x2009C09E

; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.138+139/851

FIO0PIN0	EQU	0x2009C014 ; R/W 0x00  FIOxPIN0 Fast GPIO Port x Pin value register 0.
FIO1PIN0	EQU	0x2009C034 ; Bit 0 in FIOxPIN0 register corresponds to pin Px.0 … bit 7 to pin Px.7.
FIO2PIN0	EQU	0x2009C054 ; 8 (byte)
FIO3PIN0	EQU	0x2009C074
FIO4PIN0	EQU	0x2009C094

FIO0PIN1	EQU	0x2009C015 ; R/W 0x00  FIOxPIN1 Fast GPIO Port x Pin value register 1.
FIO1PIN1	EQU	0x2009C035 ; Bit 0 in FIOxPIN1 register corresponds to pin Px.8 … bit 7 to pin Px.15.
FIO2PIN1	EQU	0x2009C055 ; 8 (byte)
FIO3PIN1	EQU	0x2009C075
FIO4PIN1	EQU	0x2009C095

FIO0PIN2	EQU	0x2009C016 ; R/W 0x00  FIOxPIN2 Fast GPIO Port x Pin value register 2.
FIO1PIN2	EQU	0x2009C036 ; Bit 0 in FIOxPIN2 register corresponds to pin Px.16 … bit 7 to pin Px.23.
FIO2PIN2	EQU	0x2009C056 ; 8 (byte)
FIO3PIN2	EQU	0x2009C076
FIO4PIN2	EQU	0x2009C096

FIO0PIN3	EQU	0x2009C017 ; R/W 0x00  FIOxPIN3 Fast GPIO Port x Pin value register 3.
FIO1PIN3	EQU	0x2009C037 ; Bit 0 in FIOxPIN3 register corresponds to pin Px.24 … bit 7 to pin Px.31.
FIO2PIN3	EQU	0x2009C057 ; 8 (byte)
FIO3PIN3	EQU	0x2009C077
FIO4PIN3	EQU	0x2009C097

FIO0PINL	EQU	0x2009C014 ; R/W 0x0000  FIOxPINL Fast GPIO Port x Pin value Lower half-word register.
FIO1PINL	EQU	0x2009C034 ; Bit 0 in FIOxPINL register corresponds to pin Px.0 … bit 15 to pin Px.15.
FIO2PINL	EQU	0x2009C054 ; 16 (half-word)
FIO3PINL	EQU	0x2009C074
FIO4PINL	EQU	0x2009C094

FIO0PINU	EQU	0x2009C016 ; R/W 0x0000  FIOxPINU Fast GPIO Port x Pin value Upper half-word register.
FIO1PINU	EQU	0x2009C036 ; Bit 0 in FIOxPINU register corresponds to pin Px.16 … bit 15 to Px.31.
FIO2PINU	EQU	0x2009C056 ; 16 (half-word)
FIO3PINU	EQU	0x2009C076
FIO4PINU	EQU	0x2009C096

; Chapter 9: LPC176x/5x General Purpose Input/Output (GPIO)  p.138+139/851

FIO0MASK0	EQU	0x2009C010 ; R/W 0x0  FIOxMASK0 Fast GPIO Port x Mask register 0.
FIO1MASK0	EQU	0x2009C030 ; Bit 0 in FIOxMASK0 register corresponds to pin Px.0 … bit 7 to pin Px.7.
FIO2MASK0	EQU	0x2009C050 ; 8 (byte)
FIO3MASK0	EQU	0x2009C070
FIO4MASK0	EQU	0x2009C090

FIO0MASK1	EQU	0x2009C011 ; R/W 0x0  FIOxMASK1 Fast GPIO Port x Mask register 1.
FIO1MASK1	EQU	0x2009C031 ; Bit 0 in FIOxMASK1 register corresponds to pin Px.8 … bit 7 to pin Px.15.
FIO2MASK1	EQU	0x2009C051 ; 8 (byte)
FIO3MASK1	EQU	0x2009C071
FIO4MASK1	EQU	0x2009C091

FIO0MASK2	EQU	0x2009C012 ; R/W 0x0  FIOxMASK2 Fast GPIO Port x Mask register 2.
FIO1MASK2	EQU	0x2009C032 ; Bit 0 in FIOxMASK2 register corresponds to pin Px.16 … bit 7 to pin Px.23.
FIO2MASK2	EQU	0x2009C052 ; 8 (byte)
FIO3MASK2	EQU	0x2009C072
FIO4MASK2	EQU	0x2009C092

FIO0MASK3	EQU	0x2009C013 ; R/W 0x0  FIOxMASK3 Fast GPIO Port x Mask register 3.
FIO1MASK3	EQU	0x2009C033 ; Bit 0 in FIOxMASK3 register corresponds to pin Px.24 … bit 7 to pin Px.31.
FIO2MASK3	EQU	0x2009C053 ; 8 (byte)
FIO3MASK3	EQU	0x2009C073
FIO4MASK3	EQU	0x2009C093

FIO0MASKL	EQU	0x2009C010 ; R/W 0x0  FIOxMASKL Fast GPIO Port x Mask Lower half-word register.
FIO1MASKL	EQU	0x2009C030 ; Bit 0 in FIOxMASKL register corresponds to pin Px.0 … bit 15 to pin Px.15.
FIO2MASKL	EQU	0x2009C050 ; 16 (half-word)
FIO3MASKL	EQU	0x2009C070
FIO4MASKL	EQU	0x2009C090

FIO0MASKU	EQU	0x2009C012 ; R/W 0x0  FIOxMASKU Fast GPIO Port x Mask Upper half-word register.
FIO1MASKU	EQU	0x2009C032 ; Bit 0 in FIOxMASKU register corresponds to pin Px.16 … bit 15 to Px.31.
FIO2MASKU	EQU	0x2009C052 ; 16 (half-word)
FIO3MASKU	EQU	0x2009C072
FIO4MASKU	EQU	0x2009C092

; Chapter 14: LPC176x/5x UART0/2/3  p.310/851

U0RBR		EQU	0x4000C000 ; RO  NA    RBR (when DLAB =0) Receiver Buffer Register.
U2RBR		EQU	0x40098000 ; Contains the next received character to be read.
U3RBR		EQU	0x4009C000 ; RBR 7:0  contains the oldest received byte in the UARTn Rx FIFO.  p.311/851

U0THR		EQU	0x4000C000 ; WO  NA    THR (when DLAB =0) Transmit Holding Register.
U2THR		EQU	0x40098000 ; The next character to be transmitted is written here.
U3THR		EQU	0x4009C000 ; THR 7:0  Writing causes the data to be stored in the UARTn transmit FIFO.
			; The byte will be sent when it reaches the bottom of the FIFO and the transmitter is available.  p.311/851

U0DLL		EQU	0x4000C000 ; R/W 0x01  DLL (when DLAB =1) Divisor Latch LSB.
U2DLL		EQU	0x40098000 ; Least significant byte of the baud rate divisor value.
U3DLL		EQU	0x4009C000 ; The full divisor is used to generate a baud rate from the fractional rate divider.
			; UnDLL:  DLLSB 7:0  Divisor Latch LSB  p.312/851

U0DLM		EQU	0x4000C004 ; R/W 0x00  DLM (when DLAB =1) Divisor Latch MSB.
U2DLM		EQU	0x40098004 ; Most significant byte of the baud rate divisor value.
U3DLM		EQU	0x4009C004 ; The full divisor is used to generate a baud rate from the fractional rate divider.
			; UnDLM:  DLMSB 7:0  Divisor Latch MSB  p.312/851

U0IER		EQU	0x4000C004 ; R/W 0x00  IER (when DLAB =0) Interrupt Enable Register.
U2IER		EQU	0x40098004 ; Contains individual interrupt enable bits for the 7 potential UART interrupts.
U3IER		EQU	0x4009C004 ;
			; IER:  ABTOIntEn 9   ABEOIntEn 8   Reserved 7:3   RX Line Status Interrupt Enable 2   THRE Interrupt Enable 1   RBR Interrupt Enable 0  p.312/851

U0IIR		EQU	0x4000C008 ; RO  0x01  IIR Interrupt ID Register.
U2IIR		EQU	0x40098008 ; Identifies which interrupt(s) are pending.
U3IIR		EQU	0x4009C008 ; IIR:  ABTOInt 9   ABEOInt 8   FIFO Enable 7:6   Reserved 5:4   IntId 3:1   IntStatus 0  p.313/851

U0FCR		EQU	0x4000C008 ; WO  0x00  FCR FIFO Control Register.
U2FCR		EQU	0x40098008 ; Controls UART FIFO usage and modes.
U3FCR		EQU	0x4009C008 ; FCR:  RX Trigger 7:6   Reserved 5:4
			; DMA Mode Select 3   TX FIFO Reset 2   RX FIFO Reset 1   FIFO Enable 0  p.315/851

U0LCR		EQU	0x4000C00C ; R/W 0x00  LCR Line Control Register.
U2LCR		EQU	0x4009800C ; Contains controls for frame formatting and break generation.
U3LCR		EQU	0x4009C00C ; LCR:  Divisor Latch Access Bit (DLAB) 7   Break Control 6   Parity Select 5:4
			; Parity Enable 3   Stop Bit Select 2   Word Length Select 1:0
			; Word Length Select: 00=5-bit character length ... 11=8-bit character length  p.316/851

U0LSR		EQU	0x4000C014 ; RO  0x60  LSR Line Status Register.
U2LSR		EQU	0x40098014 ; Contains flags for transmit and receive status, including line errors.
U3LSR		EQU	0x4009C014 ; LSR:  Error in RX FIFO (RXFE) 7   Transmitter Empty (TEMT) 6   Transmitter Holding Register Empty (THRE)) 5   Break Interrupt (BI) 4
			; Framing Error (FE) 3   Parity Error (PE) 2   Overrun Error (OE) 1   Receiver Data Ready (RDR) 0  p.316-318/851

U0SCR		EQU	0x4000C01C ; R/W 0x00  SCR Scratch Pad Register.
U2SCR		EQU	0x4009801C ; 8-bit temporary storage for software.
U3SCR		EQU	0x4009C01C ; SCR:  Pad 7:0  A readable, writable byte.  p.318/851

U0ACR		EQU	0x4000C020 ; R/W 0x00  ACR Auto-baud Control Register.
U2ACR		EQU	0x40098020 ; Contains controls for the auto-baud feature.
U3ACR		EQU	0x4009C020 ; ACR:  AutoRestart 2   Mode 1   Start 0  p.316-318/851

U0ICR		EQU	0x4000C024 ; R/W 0x00  ICR IrDA Control Register.
U2ICR		EQU	0x40098024 ; Enables and configures the IrDA mode.
U3ICR		EQU	0x4009C024 ;

U0FDR		EQU	0x4000C028 ; R/W 0x10  FDR Fractional Divider Register.
U2FDR		EQU	0x40098028 ; Generates a clock input for the baud rate divider.
U3FDR		EQU	0x4009C028 ;

U0TER		EQU	0x4000C030 ; R/W 0x80  TER Transmit Enable Register.
U2TER		EQU	0x40098030 ; Turns off UART transmitter for use with software flow control.
U3TER		EQU	0x4009C030 ;

; Chapter 15: LPC176x/5x UART1  p.330/851

U1RBR		EQU	0x40010000 ; RO NA  RBR (when DLAB =0) Receiver Buffer Register.
						   ; Contains the next received character to be read.

U1THR		EQU	0x40010000 ; WO NA  THR (when DLAB =0) Transmit Holding Register.
						   ; The next character to be transmitted is written here.

U1DLL		EQU	0x40010000 ; R/W 0x01  DLL (when DLAB =1) Divisor Latch LSB.
						   ; Least significant byte of the baud rate divisor value.
						   ; The full divisor is used to generate a baud rate from the fractional rate divider.

U1DLM		EQU	0x40010004 ; R/W 0x00  DLM (when DLAB =1) Divisor Latch MSB.
						   ; Most significant byte of the baud rate divisor value.
						   ; The full divisor is used to generate a baud rate from the fractional rate divider.

U1IER		EQU	0x40010004 ; R/W 0x00  IER (when DLAB =0) Interrupt Enable Register.
						   ; Contains individual interrupt enable bits for the 7 potential UART1 interrupts.

U1IIR		EQU	0x40010008 ; RO 0x01  IIR	Interrupt ID Register.
						   ; Identifies which interrupt(s) are pending.

U1FCR		EQU	0x40010008 ; WO 0x00  FIFO Control Register. Controls UART1 FIFO usage and modes.

U1LCR		EQU	0x4001000C ; R/W 0x00  LCR Line Control Register.
						   ; Contains controls for frame formatting and break generation.

U1MCR		EQU	0x40010010 ; R/W 0x00  MCR Modem Control Register.
						   ; Contains controls for flow control handshaking and loopback mode.

U1LSR		EQU	0x40010014 ; RO  0x60  LSR Line Status Register.
						   ; Contains flags for transmit and receive status, including line errors.

U1MSR		EQU	0x40010018 ; RO 0x00  MSR Modem Status Register.
						   ; Contains handshake signal status flags.

U1SCR		EQU	0x4001001C ; R/W 0x00  SCR Scratch Pad Register. 8-bit temporary storage for software.

U1ACR		EQU	0x40010020 ; R/W 0x00  ACR Auto-baud Control Register.
						   ; Contains controls for the auto-baud feature.

U1FDR		EQU	0x40010028 ; R/W 0x10  FDR Fractional Divider Register.
						   ; Generates a clock input for the baud rate divider.

U1TER		EQU	0x40010030 ; R/W 0x80  TER Transmit Enable Register.
						   ; Turns off UART transmitter for use with software flow control.

U1RS485CTRL	EQU	0x4001004C ; R/W 0x00  RS-485/EIA-485 Control.
						   ; Contains controls to configure various aspects of RS-485/EIA-485 modes.

U1ADRMATCH	EQU	0x40010050 ; R/W 0x00  RS-485/EIA-485 address match.
						   ; Contains the address match value for RS-485/EIA-485 mode.

U1RS485DLY	EQU	0x40010054 ; R/W 0x00  RS-485/EIA-485 direction control delay.

; Chapter 21: LPC176x/5x Timer 0/1/2/3                        p.503/851

T0IR	EQU	0x40004000 ; R/W 0  IR Interrupt Register.
T1IR	EQU	0x40008000 ; The IR can be written to clear interrupts.
T2IR	EQU	0x40090000 ; The IR can be read to identify
T3IR	EQU	0x40094000 ; which of eight possible interrupt sources are pending.

T0TCR	EQU	0x40004004 ; R/W 0  TCR Timer Control Register.
T1TCR	EQU	0x40008004 ; The TCR is used to control the Timer Counter functions.
T2TCR	EQU	0x40090004 ; The Timer Counter can be disabled or reset through the TCR.
T3TCR	EQU	0x40094004 ;

T0TC	EQU	0x40004008 ; R/W 0  TC Timer Counter.
T1TC	EQU	0x40008008 ; The 32-bit TC is incremented every PR+1 cycles of PCLK.
T2TC	EQU	0x40090008 ; The TC is controlled through the TCR.
T3TC	EQU	0x40094008 ;

T0PR	EQU	0x4000400C ; R/W 0  PR Prescale Register.
T1PR	EQU	0x4000800C ; When the Prescale Counter (below) is equal to this value,
T2PR	EQU	0x4009000C ; the next clock increments the TC and clears the PC.
T3PR	EQU	0x4009400C ;

T0PC	EQU	0x40004010 ; R/W 0  PC Prescale Counter.
T1PC	EQU	0x40008010 ; The 32-bit PC is a counter which is incremented to the value stored in PR.
T2PC	EQU	0x40090010 ; When the value in PR is reached, the TC is incremented and the PC is cleared.
T3PC	EQU	0x40094010 ; The PC is observable and controllable through the bus interface.

T0MCR	EQU	0x40004014 ; R/W 0  MCR Match Control Register.
T1MCR	EQU	0x40008014 ; The MCR is used to control if an interrupt is generated
T2MCR	EQU	0x40090014 ; and if the TC is reset when a Match occurs.
T3MCR	EQU	0x40094014 ;

T0MR0	EQU	0x40004018 ; R/W 0  MR0 Match Register 0.
T1MR0	EQU	0x40008018 ; MR0 can be enabled through the MCR to reset the TC, stop both the TC and PC,
T2MR0	EQU	0x40090018 ; and/or generate an interrupt every time MR0 matches the TC.
T3MR0	EQU	0x40094018 ;

T0MR1	EQU	0x4000401C ; R/W 0  MR1 Match Register 1.
T1MR1	EQU	0x4000801C ; See MR0 description.
T2MR1	EQU	0x4009001C ;
T3MR1	EQU	0x4009401C ;

T0MR2	EQU	0x40004020 ; R/W 0  MR2 Match Register 2.
T1MR2	EQU	0x40008020 ; See MR0 description.
T2MR2	EQU	0x40090020 ;
T3MR2	EQU	0x40094020 ;

T0MR3	EQU	0x40004024 ; R/W 0  MR3 Match Register 3.
T1MR3	EQU	0x40008024 ; See MR0 description.
T2MR3	EQU	0x40090024 ;
T3MR3	EQU	0x40094024 ;

T0CCR	EQU	0x40004028 ; R/W 0  CCR Capture Control Register.
T1CCR	EQU	0x40008028 ; The CCR controls which edges of the capture inputs are used
T2CCR	EQU	0x40090028 ; to load the Capture Registers and whether
T3CCR	EQU	0x40094028 ; or not an interrupt is generated when a capture takes place.

T0CR0	EQU	0x4000402C ; RO 0  CR0 Capture Register 0.
T1CR0	EQU	0x4000802C ; CR0 is loaded with the value of TC when there is an event
T2CR0	EQU	0x4009002C ; on the CAPn.0 (CAP0.0 or CAP1.0 respectively) input.
T3CR0	EQU	0x4009402C ;

T0CR1	EQU	0x40004030 ; RO 0  CR1 Capture Register 1.
T1CR1	EQU	0x40008030 ; See CR0 description.
T2CR1	EQU	0x40090030 ;
T3CR1	EQU	0x40094030 ;

T0EMR	EQU	0x4000403C ; R/W 0  EMR External Match Register.
T1EMR	EQU	0x4000803C ; The EMR controls the external match pins
T2EMR	EQU	0x4009003C ; MATn.0-3 (MAT0.0-3 and MAT1.0-3 respectively).
T3EMR	EQU	0x4009403C ;

T0CTCR	EQU	0x40004070 ; R/W 0  CTCR Count Control Register.
T1CTCR	EQU	0x40008070 ; The CTCR selects between Timer and Counter mode,
T2CTCR	EQU	0x40090070 ; and in Counter mode selects the signal and edge(s) for counting.
T3CTCR	EQU	0x40094070 ;


	END