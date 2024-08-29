;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.0 #11528 (Linux)
;--------------------------------------------------------
; PIC port for the 14-bit core
;--------------------------------------------------------
;	.file	"bingo.c"
	list	p=12f683
	radix dec
	include "p12f683.inc"
;--------------------------------------------------------
; external declarations
;--------------------------------------------------------
	extern	__moduint
	extern	__modsint
	extern	__divsint
	extern	_ANSEL
	extern	_TRISIO
	extern	_OPTION_REG
	extern	_CMCON0
	extern	_TMR0
	extern	_GPIObits
	extern	__sdcc_gsinit_startup
;--------------------------------------------------------
; global declarations
;--------------------------------------------------------
	global	_main
	global	_blink99
	global	_isNumberDrawn
	global	_generateRandomNumber
	global	_clearDisplay
	global	_showNumber
	global	_latchData
	global	_sendData
	global	_delay_ms
	global	_delay_us
	global	_drawCount
	global	_drawnNumbers
	global	_firstPress
	global	_lfsr
	global	_prevButtonState

	global PSAVE
	global SSAVE
	global WSAVE
	global STK12
	global STK11
	global STK10
	global STK09
	global STK08
	global STK07
	global STK06
	global STK05
	global STK04
	global STK03
	global STK02
	global STK01
	global STK00

sharebank udata_ovr 0x0070
PSAVE	res 1
SSAVE	res 1
WSAVE	res 1
STK12	res 1
STK11	res 1
STK10	res 1
STK09	res 1
STK08	res 1
STK07	res 1
STK06	res 1
STK05	res 1
STK04	res 1
STK03	res 1
STK02	res 1
STK01	res 1
STK00	res 1

;--------------------------------------------------------
; global definitions
;--------------------------------------------------------
;--------------------------------------------------------
; absolute symbol definitions
;--------------------------------------------------------
;--------------------------------------------------------
; compiler-defined variables
;--------------------------------------------------------
UDL_bingo_0	udata
r0x101A	res	1
r0x1019	res	1
r0x101B	res	1
r0x101C	res	1
r0x101E	res	1
r0x101D	res	1
r0x101F	res	1
r0x1020	res	1
r0x1021	res	1
r0x1023	res	1
r0x1022	res	1
r0x1025	res	1
r0x1024	res	1
r0x1027	res	1
r0x1029	res	1
r0x1028	res	1
r0x102A	res	1
r0x102B	res	1
r0x102C	res	1
_showNumber_numbers_65536_13	res	10
;--------------------------------------------------------
; initialized data
;--------------------------------------------------------

IDD_bingo_0	idata
_prevButtonState
	db	0x01	; 1


IDD_bingo_1	idata
_lfsr
	db	0xe1, 0xac	; 44257


IDD_bingo_2	idata
_firstPress
	db	0x00	; 0


IDD_bingo_3	idata
_drawnNumbers
	db	0x00	; 0
	db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00


IDD_bingo_4	idata
_drawCount
	db	0x00	; 0

;--------------------------------------------------------
; initialized absolute data
;--------------------------------------------------------
;--------------------------------------------------------
; overlayable items in internal ram 
;--------------------------------------------------------
;	udata_ovr
;--------------------------------------------------------
; reset vector 
;--------------------------------------------------------
STARTUP	code 0x0000
	nop
	pagesel __sdcc_gsinit_startup
	goto	__sdcc_gsinit_startup
;--------------------------------------------------------
; code
;--------------------------------------------------------
code_bingo	code
;***
;  pBlock Stats: dbName = M
;***
;has an exit
;functions called:
;   _clearDisplay
;   _delay_ms
;   _generateRandomNumber
;   _isNumberDrawn
;   _showNumber
;   _blink99
;   _delay_ms
;   _clearDisplay
;   _delay_ms
;   _generateRandomNumber
;   _isNumberDrawn
;   _showNumber
;   _blink99
;   _delay_ms
;6 compiler assigned registers:
;   STK00
;   r0x1028
;   r0x1029
;   r0x102A
;   r0x102B
;   r0x102C
;; Starting pCode block
S_bingo__main	code
_main:
; 2 exit points
;	.line	114; "bingo.c"	TRISIO = 0b00001000; // Configura GP3 como entrada (botón) y los demás como salida
	MOVLW	0x08
	BANKSEL	_TRISIO
	MOVWF	_TRISIO
;	.line	115; "bingo.c"	ANSEL = 0; // Desactiva los módulos analógicos
	CLRF	_ANSEL
;	.line	116; "bingo.c"	CMCON0 = 0x07; // Desactiva los comparadores
	MOVLW	0x07
	BANKSEL	_CMCON0
	MOVWF	_CMCON0
;	.line	119; "bingo.c"	OPTION_REG = 0b11010111; // Configura TMR0: reloj interno, prescaler 1:256, asignado a TMR0
	MOVLW	0xd7
	BANKSEL	_OPTION_REG
	MOVWF	_OPTION_REG
;	.line	122; "bingo.c"	clearDisplay();
	PAGESEL	_clearDisplay
	CALL	_clearDisplay
	PAGESEL	$
_00192_DS_:
;	.line	126; "bingo.c"	if (prevButtonState == 1 && BUTTON == 0) { // Si antes no estaba presionado y ahora lo está
	BANKSEL	_prevButtonState
	MOVF	_prevButtonState,W
	XORLW	0x01
	BTFSS	STATUS,2
	GOTO	_00189_DS_
	BANKSEL	_GPIObits
	BTFSC	_GPIObits,3
	GOTO	_00189_DS_
;	.line	127; "bingo.c"	if (!firstPress) {
	BANKSEL	_firstPress
	MOVF	_firstPress,W
	BTFSS	STATUS,2
	GOTO	_00186_DS_
;	.line	128; "bingo.c"	firstPress = 1; // Marcar que el botón ha sido presionado por primera vez
	MOVLW	0x01
	MOVWF	_firstPress
;	.line	129; "bingo.c"	delay_ms(30); // Pequeño retardo para evitar rebotes
	MOVLW	0x1e
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_ms
	CALL	_delay_ms
	PAGESEL	$
	GOTO	_00189_DS_
;;unsigned compare: left < lit(0xA=10), size=1
_00186_DS_:
;	.line	131; "bingo.c"	if (drawCount < 10) {
	MOVLW	0x0a
	BANKSEL	_drawCount
	SUBWF	_drawCount,W
	BTFSC	STATUS,0
	GOTO	_00183_DS_
;;genSkipc:3307: created from rifx:0x7ffdf6550d30
_00179_DS_:
;	.line	134; "bingo.c"	randomNumber = generateRandomNumber(); // Generar un nuevo número pseudoaleatorio
	PAGESEL	_generateRandomNumber
	CALL	_generateRandomNumber
	PAGESEL	$
	BANKSEL	r0x1028
	MOVWF	r0x1028
	MOVF	STK00,W
;	.line	135; "bingo.c"	} while (isNumberDrawn(randomNumber)); // Repetir hasta obtener un número no repetido
	MOVWF	r0x1029
	MOVWF	STK00
	MOVF	r0x1028,W
	PAGESEL	_isNumberDrawn
	CALL	_isNumberDrawn
	PAGESEL	$
	BANKSEL	r0x102A
	MOVWF	r0x102A
	MOVF	r0x102A,W
	BTFSS	STATUS,2
	GOTO	_00179_DS_
;	.line	137; "bingo.c"	drawnNumbers[drawCount++] = randomNumber; // Guardar el número sacado
	BANKSEL	_drawCount
	MOVF	_drawCount,W
	BANKSEL	r0x102A
	MOVWF	r0x102A
	BANKSEL	_drawCount
	INCF	_drawCount,F
	BANKSEL	r0x102A
	MOVF	r0x102A,W
	ADDLW	(_drawnNumbers + 0)
	MOVWF	r0x102A
	MOVLW	high (_drawnNumbers + 0)
	BTFSC	STATUS,0
	ADDLW	0x01
	MOVWF	r0x102B
	MOVF	r0x1029,W
	MOVWF	r0x102C
	MOVF	r0x102A,W
	BANKSEL	FSR
	MOVWF	FSR
	BCF	STATUS,7
	BANKSEL	r0x102B
	BTFSC	r0x102B,0
	BSF	STATUS,7
	MOVF	r0x102C,W
	BANKSEL	INDF
	MOVWF	INDF
;	.line	138; "bingo.c"	showNumber(randomNumber); // Mostrar el número
	BANKSEL	r0x1029
	MOVF	r0x1029,W
	MOVWF	STK00
	MOVF	r0x1028,W
	PAGESEL	_showNumber
	CALL	_showNumber
	PAGESEL	$
	GOTO	_00189_DS_
_00183_DS_:
;	.line	140; "bingo.c"	blink99(); // Parpadear 3 veces con el número 99
	PAGESEL	_blink99
	CALL	_blink99
	PAGESEL	$
;	.line	141; "bingo.c"	drawCount = 0; // Reiniciar el contador de números sacados
	BANKSEL	_drawCount
	CLRF	_drawCount
;	.line	142; "bingo.c"	firstPress = 1; // Reiniciar para esperar la siguiente pulsación del botón
	MOVLW	0x01
	BANKSEL	_firstPress
	MOVWF	_firstPress
_00189_DS_:
;	.line	147; "bingo.c"	prevButtonState = BUTTON; // Actualizar el estado previo del botón
	BANKSEL	r0x1029
	CLRF	r0x1029
	BANKSEL	_GPIObits
	BTFSS	_GPIObits,3
	GOTO	_00001_DS_
	BANKSEL	r0x1029
	INCF	r0x1029,F
_00001_DS_:
	BANKSEL	r0x1029
	MOVF	r0x1029,W
	BANKSEL	_prevButtonState
	MOVWF	_prevButtonState
;	.line	150; "bingo.c"	delay_ms(10);
	MOVLW	0x0a
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_ms
	CALL	_delay_ms
	PAGESEL	$
	GOTO	_00192_DS_
;	.line	152; "bingo.c"	}
	RETURN	
; exit point of _main

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   _showNumber
;   _delay_ms
;   _clearDisplay
;   _delay_ms
;   _showNumber
;   _delay_ms
;   _clearDisplay
;   _delay_ms
;2 compiler assigned registers:
;   r0x1027
;   STK00
;; Starting pCode block
S_bingo__blink99	code
_blink99:
; 2 exit points
;	.line	105; "bingo.c"	for (unsigned char i = 0; i < 3; i++) {
	BANKSEL	r0x1027
	CLRF	r0x1027
;;unsigned compare: left < lit(0x3=3), size=1
_00172_DS_:
	MOVLW	0x03
	BANKSEL	r0x1027
	SUBWF	r0x1027,W
	BTFSC	STATUS,0
	GOTO	_00174_DS_
;;genSkipc:3307: created from rifx:0x7ffdf6550d30
;	.line	106; "bingo.c"	showNumber(99);
	MOVLW	0x63
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_showNumber
	CALL	_showNumber
	PAGESEL	$
;	.line	107; "bingo.c"	delay_ms(100);
	MOVLW	0x64
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_ms
	CALL	_delay_ms
	PAGESEL	$
;	.line	108; "bingo.c"	clearDisplay();
	PAGESEL	_clearDisplay
	CALL	_clearDisplay
	PAGESEL	$
;	.line	109; "bingo.c"	delay_ms(100);
	MOVLW	0x64
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_ms
	CALL	_delay_ms
	PAGESEL	$
;	.line	105; "bingo.c"	for (unsigned char i = 0; i < 3; i++) {
	BANKSEL	r0x1027
	INCF	r0x1027,F
	GOTO	_00172_DS_
_00174_DS_:
;	.line	111; "bingo.c"	}
	RETURN	
; exit point of _blink99

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;7 compiler assigned registers:
;   r0x1019
;   STK00
;   r0x101A
;   r0x101B
;   r0x101C
;   r0x101D
;   r0x101E
;; Starting pCode block
S_bingo__isNumberDrawn	code
_isNumberDrawn:
; 2 exit points
;	.line	94; "bingo.c"	unsigned char isNumberDrawn(unsigned int number) {
	BANKSEL	r0x1019
	MOVWF	r0x1019
	MOVF	STK00,W
	MOVWF	r0x101A
;	.line	95; "bingo.c"	for (unsigned char i = 0; i < drawCount; i++) {
	CLRF	r0x101B
_00163_DS_:
	BANKSEL	_drawCount
	MOVF	_drawCount,W
	BANKSEL	r0x101B
	SUBWF	r0x101B,W
	BTFSC	STATUS,0
	GOTO	_00161_DS_
;;genSkipc:3307: created from rifx:0x7ffdf6550d30
;	.line	96; "bingo.c"	if (drawnNumbers[i] == number) {
	MOVF	r0x101B,W
	ADDLW	(_drawnNumbers + 0)
	MOVWF	r0x101C
	MOVLW	high (_drawnNumbers + 0)
	BTFSC	STATUS,0
	ADDLW	0x01
	MOVWF	r0x101D
	MOVF	r0x101C,W
	BANKSEL	FSR
	MOVWF	FSR
	BCF	STATUS,7
	BANKSEL	r0x101D
	BTFSC	r0x101D,0
	BSF	STATUS,7
	BANKSEL	INDF
	MOVF	INDF,W
;;1	MOVWF	r0x101E
	BANKSEL	r0x101C
	MOVWF	r0x101C
	CLRF	r0x101D
	MOVF	r0x101A,W
	XORWF	r0x101C,W
	BTFSS	STATUS,2
	GOTO	_00164_DS_
	MOVF	r0x1019,W
	XORWF	r0x101D,W
	BTFSS	STATUS,2
	GOTO	_00164_DS_
;	.line	97; "bingo.c"	return 1; // Número ya ha sido sacado
	MOVLW	0x01
	GOTO	_00165_DS_
_00164_DS_:
;	.line	95; "bingo.c"	for (unsigned char i = 0; i < drawCount; i++) {
	BANKSEL	r0x101B
	INCF	r0x101B,F
	GOTO	_00163_DS_
_00161_DS_:
;	.line	100; "bingo.c"	return 0; // Número no ha sido sacado
	MOVLW	0x00
_00165_DS_:
;	.line	101; "bingo.c"	}
	RETURN	
; exit point of _isNumberDrawn

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   __moduint
;   __moduint
;9 compiler assigned registers:
;   r0x1019
;   r0x101A
;   r0x101B
;   r0x101C
;   r0x101D
;   r0x101E
;   STK02
;   STK01
;   STK00
;; Starting pCode block
S_bingo__generateRandomNumber	code
_generateRandomNumber:
; 2 exit points
;	.line	84; "bingo.c"	unsigned lfsr_bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ^ (lfsr >> 7) ^ (lfsr >> 13)) & 1; // Añadidos más taps
	BANKSEL	_lfsr
	MOVF	_lfsr,W
	BANKSEL	r0x1019
	MOVWF	r0x1019
	BANKSEL	_lfsr
	MOVF	(_lfsr + 1),W
	BANKSEL	r0x101A
	MOVWF	r0x101A
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=0, offr=0
	BCF	STATUS,0
	BANKSEL	_lfsr
	RRF	(_lfsr + 1),W
	BANKSEL	r0x101B
	MOVWF	r0x101B
	BANKSEL	_lfsr
	RRF	_lfsr,W
	BANKSEL	r0x101C
	MOVWF	r0x101C
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=1, offr=0
	BCF	STATUS,0
	RRF	r0x101B,F
	RRF	r0x101C,F
	MOVF	r0x101C,W
	XORWF	r0x1019,F
	MOVF	r0x101B,W
	XORWF	r0x101A,F
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=0, offr=0
	BCF	STATUS,0
	BANKSEL	_lfsr
	RRF	(_lfsr + 1),W
	BANKSEL	r0x101B
	MOVWF	r0x101B
	BANKSEL	_lfsr
	RRF	_lfsr,W
	BANKSEL	r0x101C
	MOVWF	r0x101C
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=1, offr=0
	BCF	STATUS,0
	RRF	r0x101B,F
	RRF	r0x101C,F
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=1, offr=0
	BCF	STATUS,0
	RRF	r0x101B,F
	RRF	r0x101C,F
	MOVF	r0x101C,W
	XORWF	r0x1019,F
	MOVF	r0x101B,W
	XORWF	r0x101A,F
	BANKSEL	_lfsr
	SWAPF	_lfsr,W
	ANDLW	0x0f
	BANKSEL	r0x101C
	MOVWF	r0x101C
	BANKSEL	_lfsr
	SWAPF	(_lfsr + 1),W
	BANKSEL	r0x101B
	MOVWF	r0x101B
	ANDLW	0xf0
	IORWF	r0x101C,F
	XORWF	r0x101B,F
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=1, offr=0
	BCF	STATUS,0
	RRF	r0x101B,F
	RRF	r0x101C,F
	MOVF	r0x101C,W
	XORWF	r0x1019,F
	MOVF	r0x101B,W
	XORWF	r0x101A,F
	BANKSEL	_lfsr
	RLF	_lfsr,W
	RLF	(_lfsr + 1),W
	BANKSEL	r0x101C
	MOVWF	r0x101C
	CLRF	r0x101B
	RLF	r0x101B,F
	MOVF	r0x101C,W
	XORWF	r0x1019,F
	MOVF	r0x101B,W
	XORWF	r0x101A,F
	BANKSEL	_lfsr
	SWAPF	(_lfsr + 1),W
	ANDLW	0x0f
	BANKSEL	r0x101C
	MOVWF	r0x101C
	CLRF	r0x101B
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=1, offr=0
	BCF	STATUS,0
	RRF	r0x101B,F
	RRF	r0x101C,F
	MOVF	r0x101C,W
	XORWF	r0x1019,F
	MOVF	r0x101B,W
	XORWF	r0x101A,F
	MOVLW	0x01
	ANDWF	r0x1019,F
	CLRF	r0x101A
;;shiftRight_Left2ResultLit:5474: shCount=1, size=2, sign=0, same=0, offr=0
;	.line	85; "bingo.c"	lfsr = (lfsr >> 1) | (lfsr_bit << 15); // Desplazamiento con la nueva entrada
	BCF	STATUS,0
	BANKSEL	_lfsr
	RRF	(_lfsr + 1),W
	BANKSEL	r0x101B
	MOVWF	r0x101B
	BANKSEL	_lfsr
	RRF	_lfsr,W
	MOVWF	_lfsr
	BANKSEL	r0x101C
	MOVWF	r0x101C
	RRF	r0x1019,W
	CLRF	r0x101D
	RRF	r0x101D,F
;;1	CLRF	r0x101E
;;99	MOVF	r0x101C,W
	MOVF	r0x101D,W
	IORWF	r0x101B,W
	BANKSEL	_lfsr
	MOVWF	(_lfsr + 1)
;	.line	88; "bingo.c"	lfsr ^= TMR0;
	BANKSEL	_TMR0
	MOVF	_TMR0,W
	BANKSEL	r0x1019
	MOVWF	r0x1019
	CLRF	r0x101A
	MOVF	r0x1019,W
	BANKSEL	_lfsr
	XORWF	_lfsr,F
	MOVLW	0x00
	XORWF	(_lfsr + 1),F
;	.line	90; "bingo.c"	return lfsr % 100; // Retornar un valor entre 00 y 99
	MOVLW	0x64
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVF	_lfsr,W
	MOVWF	STK00
	MOVF	(_lfsr + 1),W
	PAGESEL	__moduint
	CALL	__moduint
	PAGESEL	$
	BANKSEL	r0x101A
	MOVWF	r0x101A
	MOVF	STK00,W
	MOVWF	r0x1019
	MOVWF	STK00
	MOVF	r0x101A,W
;	.line	91; "bingo.c"	}
	RETURN	
; exit point of _generateRandomNumber

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   _sendData
;   _latchData
;   _sendData
;   _latchData
;   _sendData
;   _latchData
;   _sendData
;   _latchData
;; Starting pCode block
S_bingo__clearDisplay	code
_clearDisplay:
; 2 exit points
;	.line	76; "bingo.c"	sendData(0xFF); // Todos los segmentos apagados
	MOVLW	0xff
	PAGESEL	_sendData
	CALL	_sendData
	PAGESEL	$
;	.line	77; "bingo.c"	latchData();
	PAGESEL	_latchData
	CALL	_latchData
	PAGESEL	$
;	.line	78; "bingo.c"	sendData(0xFF); // Todos los segmentos apagados
	MOVLW	0xff
	PAGESEL	_sendData
	CALL	_sendData
	PAGESEL	$
;	.line	79; "bingo.c"	latchData();
	PAGESEL	_latchData
	CALL	_latchData
	PAGESEL	$
;	.line	80; "bingo.c"	}
	RETURN	
; exit point of _clearDisplay

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   __divsint
;   __modsint
;   _sendData
;   _latchData
;   _sendData
;   _latchData
;   __divsint
;   __modsint
;   _sendData
;   _latchData
;   _sendData
;   _latchData
;8 compiler assigned registers:
;   r0x1022
;   STK00
;   r0x1023
;   STK02
;   STK01
;   r0x1024
;   r0x1025
;   r0x1026
;; Starting pCode block
S_bingo__showNumber	code
_showNumber:
; 2 exit points
;	.line	50; "bingo.c"	void showNumber(int number) {
	BANKSEL	r0x1022
	MOVWF	r0x1022
	MOVF	STK00,W
	MOVWF	r0x1023
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
;	.line	52; "bingo.c"	const unsigned char numbers[] = {
	MOVLW	0xc0
	BANKSEL	_showNumber_numbers_65536_13
	MOVWF	(_showNumber_numbers_65536_13 + 0)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0xf9
	MOVWF	(_showNumber_numbers_65536_13 + 1)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0xa4
	MOVWF	(_showNumber_numbers_65536_13 + 2)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0xb0
	MOVWF	(_showNumber_numbers_65536_13 + 3)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0x99
	MOVWF	(_showNumber_numbers_65536_13 + 4)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0x92
	MOVWF	(_showNumber_numbers_65536_13 + 5)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0x82
	MOVWF	(_showNumber_numbers_65536_13 + 6)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0xf8
	MOVWF	(_showNumber_numbers_65536_13 + 7)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0x80
	MOVWF	(_showNumber_numbers_65536_13 + 8)
;;/home/sdcc-builder/build/sdcc-build/orig/sdcc/src/pic14/gen.c:6828: size=0, offset=0, AOP_TYPE(res)=8
	MOVLW	0x90
	MOVWF	(_showNumber_numbers_65536_13 + 9)
;	.line	65; "bingo.c"	int tens = number / 10; // Obtener las decenas
	MOVLW	0x0a
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	BANKSEL	r0x1023
	MOVF	r0x1023,W
	MOVWF	STK00
	MOVF	r0x1022,W
	PAGESEL	__divsint
	CALL	__divsint
	PAGESEL	$
	BANKSEL	r0x1024
	MOVWF	r0x1024
	MOVF	STK00,W
	MOVWF	r0x1025
;	.line	66; "bingo.c"	int units = number % 10; // Obtener las unidades
	MOVLW	0x0a
	MOVWF	STK02
	MOVLW	0x00
	MOVWF	STK01
	MOVF	r0x1023,W
	MOVWF	STK00
	MOVF	r0x1022,W
	PAGESEL	__modsint
	CALL	__modsint
	PAGESEL	$
	BANKSEL	r0x1022
	MOVWF	r0x1022
	MOVF	STK00,W
	MOVWF	r0x1023
;	.line	68; "bingo.c"	sendData(numbers[tens]); // Mostrar dígito de las decenas
	MOVF	r0x1025,W
	ADDLW	(_showNumber_numbers_65536_13 + 0)
	MOVWF	r0x1025
	MOVF	r0x1024,W
	BTFSC	STATUS,0
	INCFSZ	r0x1024,W
	ADDLW	high (_showNumber_numbers_65536_13 + 0)
	MOVWF	r0x1024
	MOVF	r0x1025,W
	BANKSEL	FSR
	MOVWF	FSR
	BCF	STATUS,7
	BANKSEL	r0x1024
	BTFSC	r0x1024,0
	BSF	STATUS,7
	BANKSEL	INDF
	MOVF	INDF,W
;;1	MOVWF	r0x1026
	PAGESEL	_sendData
	CALL	_sendData
	PAGESEL	$
;	.line	69; "bingo.c"	latchData();
	PAGESEL	_latchData
	CALL	_latchData
	PAGESEL	$
;	.line	70; "bingo.c"	sendData(numbers[units]); // Mostrar dígito de las unidades
	BANKSEL	r0x1023
	MOVF	r0x1023,W
	ADDLW	(_showNumber_numbers_65536_13 + 0)
	MOVWF	r0x1023
	MOVF	r0x1022,W
	BTFSC	STATUS,0
	INCFSZ	r0x1022,W
	ADDLW	high (_showNumber_numbers_65536_13 + 0)
	MOVWF	r0x1022
	MOVF	r0x1023,W
	BANKSEL	FSR
	MOVWF	FSR
	BCF	STATUS,7
	BANKSEL	r0x1022
	BTFSC	r0x1022,0
	BSF	STATUS,7
	BANKSEL	INDF
	MOVF	INDF,W
	BANKSEL	r0x1025
	MOVWF	r0x1025
	PAGESEL	_sendData
	CALL	_sendData
	PAGESEL	$
;	.line	71; "bingo.c"	latchData();
	PAGESEL	_latchData
	CALL	_latchData
	PAGESEL	$
;	.line	72; "bingo.c"	}
	RETURN	
; exit point of _showNumber

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   _delay_us
;   _delay_us
;1 compiler assigned register :
;   STK00
;; Starting pCode block
S_bingo__latchData	code
_latchData:
; 2 exit points
;	.line	44; "bingo.c"	LOAD = 1;
	BANKSEL	_GPIObits
	BSF	_GPIObits,2
;	.line	45; "bingo.c"	delay_us(10);
	MOVLW	0x0a
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_us
	CALL	_delay_us
	PAGESEL	$
;	.line	46; "bingo.c"	LOAD = 0;
	BANKSEL	_GPIObits
	BCF	_GPIObits,2
;	.line	47; "bingo.c"	}
	RETURN	
; exit point of _latchData

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   _delay_us
;   _delay_us
;   _delay_us
;   _delay_us
;6 compiler assigned registers:
;   r0x101D
;   r0x101E
;   r0x101F
;   r0x1020
;   r0x1021
;   STK00
;; Starting pCode block
S_bingo__sendData	code
_sendData:
; 2 exit points
;	.line	32; "bingo.c"	void sendData(unsigned char data) {
	BANKSEL	r0x101D
	MOVWF	r0x101D
;	.line	33; "bingo.c"	for (int i = 0; i < 8; i++) {
	CLRF	r0x101E
	CLRF	r0x101F
;;signed compare: left < lit(0x8=8), size=2, mask=ffff
_00123_DS_:
	BANKSEL	r0x101F
	MOVF	r0x101F,W
	ADDLW	0x80
	ADDLW	0x80
	BTFSS	STATUS,2
	GOTO	_00136_DS_
	MOVLW	0x08
	SUBWF	r0x101E,W
_00136_DS_:
	BTFSC	STATUS,0
	GOTO	_00125_DS_
;;genSkipc:3307: created from rifx:0x7ffdf6550d30
;	.line	34; "bingo.c"	SDI = (data >> (7 - i)) & 0x01;
	BANKSEL	r0x101E
	MOVF	r0x101E,W
	MOVWF	r0x1020
	SUBLW	0x07
	MOVWF	r0x1020
	MOVF	r0x101D,W
	MOVWF	r0x1021
	MOVF	r0x1020,W
	SUBLW	0x00
	BTFSC	STATUS,2
	GOTO	_00138_DS_
_00137_DS_:
	BANKSEL	r0x1021
	RRF	r0x1021,F
	ADDLW	0x01
	BTFSS	STATUS,0
	GOTO	_00137_DS_
_00138_DS_:
	BANKSEL	r0x1021
	RRF	r0x1021,W
	BTFSC	STATUS,0
	GOTO	_00002_DS_
	BANKSEL	_GPIObits
	BCF	_GPIObits,0
_00002_DS_:
	BTFSS	STATUS,0
	GOTO	_00003_DS_
	BANKSEL	_GPIObits
	BSF	_GPIObits,0
_00003_DS_:
;	.line	35; "bingo.c"	SCLK = 1;
	BANKSEL	_GPIObits
	BSF	_GPIObits,1
;	.line	36; "bingo.c"	delay_us(10); // Pequeño retardo para la señal de reloj
	MOVLW	0x0a
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_us
	CALL	_delay_us
	PAGESEL	$
;	.line	37; "bingo.c"	SCLK = 0;
	BANKSEL	_GPIObits
	BCF	_GPIObits,1
;	.line	38; "bingo.c"	delay_us(10);
	MOVLW	0x0a
	MOVWF	STK00
	MOVLW	0x00
	PAGESEL	_delay_us
	CALL	_delay_us
	PAGESEL	$
;	.line	33; "bingo.c"	for (int i = 0; i < 8; i++) {
	BANKSEL	r0x101E
	INCF	r0x101E,F
	BTFSC	STATUS,2
	INCF	r0x101F,F
	GOTO	_00123_DS_
_00125_DS_:
;	.line	40; "bingo.c"	}
	RETURN	
; exit point of _sendData

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;functions called:
;   _delay_us
;   _delay_us
;5 compiler assigned registers:
;   r0x101D
;   STK00
;   r0x101E
;   r0x101F
;   r0x1020
;; Starting pCode block
S_bingo__delay_ms	code
_delay_ms:
; 2 exit points
;	.line	25; "bingo.c"	void delay_ms(unsigned int ms) {
	BANKSEL	r0x101D
	MOVWF	r0x101D
	MOVF	STK00,W
	MOVWF	r0x101E
_00113_DS_:
;	.line	26; "bingo.c"	while (ms--) {
	BANKSEL	r0x101E
	MOVF	r0x101E,W
	MOVWF	r0x101F
	MOVF	r0x101D,W
	MOVWF	r0x1020
	MOVLW	0xff
	ADDWF	r0x101E,F
	BTFSS	STATUS,0
	DECF	r0x101D,F
	MOVF	r0x1020,W
	IORWF	r0x101F,W
	BTFSC	STATUS,2
	GOTO	_00116_DS_
;	.line	27; "bingo.c"	delay_us(1000); // Cada milisegundo son 1000 microsegundos
	MOVLW	0xe8
	MOVWF	STK00
	MOVLW	0x03
	PAGESEL	_delay_us
	CALL	_delay_us
	PAGESEL	$
	GOTO	_00113_DS_
_00116_DS_:
;	.line	29; "bingo.c"	}
	RETURN	
; exit point of _delay_ms

;***
;  pBlock Stats: dbName = C
;***
;has an exit
;5 compiler assigned registers:
;   r0x1019
;   STK00
;   r0x101A
;   r0x101B
;   r0x101C
;; Starting pCode block
S_bingo__delay_us	code
_delay_us:
; 2 exit points
;	.line	18; "bingo.c"	void delay_us(unsigned int us) {
	BANKSEL	r0x1019
	MOVWF	r0x1019
	MOVF	STK00,W
	MOVWF	r0x101A
_00105_DS_:
;	.line	19; "bingo.c"	while (us--) {
	BANKSEL	r0x101A
	MOVF	r0x101A,W
	MOVWF	r0x101B
	MOVF	r0x1019,W
	MOVWF	r0x101C
	MOVLW	0xff
	ADDWF	r0x101A,F
	BTFSS	STATUS,0
	DECF	r0x1019,F
	MOVF	r0x101C,W
	IORWF	r0x101B,W
	BTFSC	STATUS,2
	GOTO	_00108_DS_
	nop	
	GOTO	_00105_DS_
_00108_DS_:
;	.line	22; "bingo.c"	}
	RETURN	
; exit point of _delay_us


;	code size estimation:
;	  418+  145 =   563 instructions ( 1416 byte)

	end
