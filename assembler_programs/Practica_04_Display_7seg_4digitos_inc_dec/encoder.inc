
; =========================================================================
;					Rutinas para habilitar los transistores
; =========================================================================
enable_digit_1:
	LDI	R16,	0x01 
	OUT PORTB,	R16		; Encendemos el pin 0 del puerto B
	RET	

enable_digit_2:
	LDI	R16,	0x02 
	OUT PORTB,	R16		; Encendemos el pin 1 del puerto B
	RET	

enable_digit_3:
	LDI	R16,	0x04
	OUT PORTB,	R16		; Encendemos el pin 2 del puerto B
	RET	
; =========================================================================

; =========================================================================
;						Rutinas para el envio de datos
; =========================================================================
; -------------------------------------------------------------------------
; Envio de datos
; -------------------------------------------------------------------------
send_data:
	MOV		R26, R21
	RCALL	enable_digit_1
	RCALL	send_digit
	RCALL	delay_1ms

	MOV		R26, R22
	RCALL	enable_digit_2
	RCALL	send_digit
	RCALL	delay_1ms

	MOV		R26, R23
	RCALL	enable_digit_3
	RCALL	send_digit
	RCALL	delay_1ms
	RET

send_digit:
	CPI		R26, 0
	BREQ	send_0
	CPI		R26, 1
	BREQ	send_1
	CPI		R26, 2
	BREQ	send_2
	CPI		R26, 3
	BREQ	send_3
	CPI		R26, 4
	BREQ	send_4
	CPI		R26, 5
	BREQ	send_5
	CPI		R26, 6
	BREQ	send_6
	CPI		R26, 7
	BREQ	send_7
	CPI		R26, 8
	BREQ	send_8
	CPI		R26, 9
	BREQ	send_9
	next_digit:
		RET

; -------------------------------------------------------------------------
; Codificador para el display
; Enviar: 0b0011111 => Visualizar: 0
; -------------------------------------------------------------------------
send_0:
	LDI		R16,	0x3F ; 0b0001_1111 => 0
	OUT		PORTA,	R16		
	RJMP	next_digit

send_1:
	LDI		R16,	0x06 ; 0b0000_0110 =>	1
	OUT		PORTA,	R16
	RJMP	next_digit

send_2:
	LDI		R16,	0x5B ; 0b0101_1011 =>	2
	OUT		PORTA,	R16	
	RJMP	next_digit

send_3:
	LDI		R16,	0x4F ; 0b0100_1111 =>	3
	OUT		PORTA,	R16
	RJMP	next_digit

send_4:
	LDI		R16,	0x66 ; 0b0110_0110 =>	4
	OUT		PORTA,	R16		
	RJMP	next_digit

send_5:
	LDI		R16,	0x6D ; 0b0110_1101 =>	5
	OUT		PORTA,	R16
	RJMP	next_digit

send_6:
	LDI		R16,	0x7D ; 0b0111_1101 =>	6
	OUT		PORTA,	R16
	RJMP	next_digit

send_7:
	LDI		R16,	0x07 ; 0b0000_0111 =>	7
	OUT		PORTA,	R16
	RJMP	next_digit

send_8:	
	LDI		R16,	0x7F ; 0b0111_1111 =>	8
	OUT		PORTA,	R16
	RJMP	next_digit

send_9:
	LDI		R16,	0x6F ; 0b0110_1111 =>	9
	OUT		PORTA,	R16
	RJMP	next_digit	
; =========================================================================



; =========================================================================
;									TIMER
; =========================================================================
delay_1us:					; 16 ciclos maquina
			LDI  R18, 5
	Loop_0:	DEC  R18
			BRNE Loop_0
			NOP
			RET

delay_10us:					; 160 ciclos maquina
			LDI  R18, 53
	Loop_1:	DEC  R18
			BRNE Loop_1
			NOP
			RET

delay_100us:				; 1,600 ciclos maquina
			LDI  R18, 3
			LDI  R19, 19
	Loop_2:	DEC  R19
			BRNE Loop_2
			DEC  R18
			BRNE Loop_2
			RET

delay_500us:				; 8,000 ciclos maquina
			LDI  R18, 11
			LDI  R19, 99
	Loop_3:	DEC  R19
			BRNE Loop_3
			DEC  R18
			BRNE Loop_3
			RET

delay_1ms:					; 16,000 ciclos maquina
			LDI  R18, 21
			LDI  R19, 199
	Loop_4:	DEC  R19
			BRNE Loop_4
			DEC  R18
			BRNE Loop_4
			RET

delay_10ms:					; 160,000 ciclos maquina
			LDI  R18, 208
			LDI  R19, 202
	Loop_5:	DEC  R19
			BRNE Loop_5
			DEC  R18
			BRNE Loop_5
			NOP
			RET
; =========================================================================