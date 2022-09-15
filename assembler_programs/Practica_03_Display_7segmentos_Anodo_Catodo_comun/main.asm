;
; c_anodo.asm
;
; Created: 07/09/2022 09:36:46 p. m.
; Author : Manuel de Jesús
;

.INCLUDE "M2560DEF.INC"		
.CSEG						; segmento en la memoria flash
.ORG 0x00					; direccion 0 de la flash

LDI R16, 0xff				; Declaramos R16 en alto
OUT DDRB, R16				; Declaramos el puerto DDRB como salida
LDI R16, 0xff
OUT DDRC, R16

main:
	LDI		R16, 0x3F		
	RCALL	c_catodo
	LDI		R16, 0x06		
	RCALL	c_catodo
	LDI		R16, 0x5B		
	RCALL	c_catodo
	LDI		R16, 0x4F		
	RCALL	c_catodo
	LDI		R16, 0x66		
	RCALL	c_catodo
	LDI		R16, 0x6D	
	RCALL	c_catodo
	LDI		R16, 0x7D	
	RCALL	c_catodo
	LDI		R16, 0x07	
	RCALL	c_catodo
	LDI		R16, 0x7F	
	RCALL	c_catodo
	LDI		R16, 0xC0		
	RCALL	c_anodo
	LDI		R16, 0xF9		
	RCALL	c_anodo
	LDI		R16, 0xA4		
	RCALL	c_anodo
	LDI		R16, 0xB0		
	RCALL	c_anodo
	LDI		R16, 0x99		
	RCALL	c_anodo
	LDI		R16, 0x92	
	RCALL	c_anodo
	LDI		R16, 0x82	
	RCALL	c_anodo
	LDI		R16, 0xF8	
	RCALL	c_anodo
	LDI		R16, 0x80	
	RCALL	c_anodo
	RJMP main	

.INCLUDE "retardos.inc"

c_catodo:
	OUT		PORTB, R16	; datos al puerto
	RCALL	delay_500ms 
	RET	

c_anodo:
	OUT		PORTC, R16	; datos al puerto
	RCALL	delay_500ms 
	RET

