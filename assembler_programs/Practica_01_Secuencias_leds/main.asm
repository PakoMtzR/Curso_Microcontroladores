;
; practica01.asm
;
; Created: 06/09/2022 09:09:22 a. m.
; Author : PakoMtz
;


.INCLUDE "M2560DEF.INC"		; Cargamos la libreria de arduino mega
.CSEG						; Abrir un segmento en la memoria flash
.ORG 0x00					; Empezamos en la direccion 0 de la flash

; Inicializacion del stack-pointer y otras
LDI R16, LOW(RAMEND)
OUT SPL, R16
LDI R16, HIGH(RAMEND)
OUT SPH, R16

LDI R16, 0xff				; Declaramos R16 en alto
OUT DDRB, R16				; Declaramos el puerto DDRB como salida

; loop principal
main:
	RCALL blink
	RCALL increment
	RCALL decrement
	RCALL leds_1
	RCALL leds_2
	RCALL leds_3
	RCALL leds_4
	RCALL leds_5
	RJMP main				; Repetimos el ciclo con ayuda de la bandera "main"

.INCLUDE "timer.inc"


blink:
	LDI R21, 10
	loop_blink:
		LDI R16, 0xff			; Declaramos todos los bits de R16 en alto
		OUT PORTB, R16			; Enviamos al puerto B
		RCALL delay_100ms			; Llamamos la funcion delay (timer.inc)
		LDI R16, 0x00			; Declaramos todos los bits de R16 en bajo
		OUT PORTB, R16			; Enviamos al puerto B
		RCALL delay_100ms			; Llamamos la funcion delay (timer.inc)
		DEC R21
		BRNE loop_blink
	RET

increment:
	RCALL	delay_100ms		; Mantenemos encendido el led por cierto tiempo
	CLZ						; Reseteamos la bandera Z porque es modificada en el delay
	INC		R16				; Incrementamos el valor de R16
	OUT		PORTB, R16		; Enviamos el valor de R16 para prender los leds
	BRNE	increment		; BRNE --> si Z=0 -> repite el ciclo
	RET

decrement:
	RCALL	delay_100ms		; Mantenemos encendido el led por cierto tiempo
	CLZ						; Reseteamos la bandera Z porque es modificada en el delay
	DEC 	R16				; Decrementamos el valor de R16
	OUT		PORTB, R16		; Enviamos el valor de R16 para prender los leds
	BRNE	decrement		; BRNE --> si Z=0 -> repite el ciclo
	RET

leds_1:
	LDI R23, 5
	loop_led1:
		LDI		R16, 0x0f			; Dato para prender los primeros 4 leds
		RCALL	blink_4leds			
		LDI		R16, 0xf0			; Dato para prender los ultimos 4 leds
		RCALL	blink_4leds
		DEC		R23
		BRNE	loop_led1
	RET

blink_4leds:
	LDI	R21, 0
	LDI R22, 3		; Numero de parpadeos que hara el led antes de que cambie de estado
	loop:	OUT		PORTB, R21	; Apagamos todos los leds
			RCALL	delay_100ms	; Llamamos la funcion delay (timer.inc)
			OUT		PORTB, R16	; Prendemos los primeros o ultimos 4 leds
			RCALL	delay_100ms	; Llamamos la funcion delay (timer.inc)
			OUT		PORTB, R21	; Apagamos todos los leds
			RCALL	delay_100ms	; Llamamos la funcion delay (timer.inc)
			DEC		R22
			BRNE	loop		; Realizamos nuevamente la secuencia segun el numero de parpadeos antes declarados
	RET

leds_2:
	LDI R21, 5
	loop_led2:
		LDI		R16, 0xaa
		RCALL	turn_on
		LDI		R16, 0x55
		RCALL	turn_on
		DEC		R21
		BRNE	loop_led2
	RET

leds_3:
	LDI R21, 5
	loop_led3:
		LDI		R16, 0x18	; 0 0 0 1 1 0 0 0 <-- Leds encendidos	
		RCALL	turn_on
		LDI		R16, 0x24	; 0 0 1 0 0 1 0 0 <-- Leds encendidos	
		RCALL	turn_on
		LDI		R16, 0x42	; 0 1 0 0 0 0 1 0 <-- Leds encendidos	
		RCALL	turn_on
		LDI		R16, 0x81	; 1 0 0 0 0 0 0 1 <-- Leds encendidos	
		RCALL	turn_on
		LDI		R16, 0x42	; 0 1 0 0 0 0 1 0 <-- Leds encendidos	
		RCALL	turn_on
		LDI		R16, 0x24	; 0 0 1 0 0 1 0 0 <-- Leds encendidos
		RCALL	turn_on
		DEC		R21
		BRNE	loop_led3
	RET

leds_4:
	LDI R21, 5
	loop_led4:
		LDI		R16, 0x01		; 0 0 0 0 0 0 0 1 <--
		RCALL	led_shift_left
		LDI		R16, 0x40		; 1 0 0 0 0 0 0 0 -->
		RCALL	led_shift_right
		DEC		R21
		BRNE	loop_led4
	RET

leds_5:
	LDI R21, 5
	loop_led5:
		LDI		R16, 0x01		; 
		RCALL	fill
		LDI		R16, 0xff		; 
		RCALL	clean
		DEC		R21
		BRNE	loop_led5
	RET

fill:
	OUT		PORTB, R16		; Enviamos datos al puerto
	RCALL	delay_100ms		; Mantenemos los leds prendidos cierto tiempo
	LSL		R16				; LSL --> Desplazamiento de bits a la izquierda
	INC		R16
	BRSH	fill			; Si no hay desbordamiento de bits repetimos el ciclo
	RET

clean:
	OUT		PORTB, R16		; Enviamos datos al puerto
	RCALL	delay_100ms		; Mantenemos los leds prendidos cierto tiempo
	DEC		R16
	LSR		R16				; LSR --> Desplazamiento de bits a la derecha
	BRCC	clean			; Si no hay desbordamiento de bits repetimos el ciclo
	RET

led_shift_left:
	OUT		PORTB, R16		; Enviamos datos al puerto
	RCALL	delay_100ms		; Mantenemos los leds prendidos cierto tiempo
	LSL		R16				; LSL --> Desplazamiento de bits a la izquierda
	BRSH	led_shift_left	; Si no hay desbordamiento de bits repetimos el ciclo
	RET

led_shift_right:
	OUT		PORTB, R16		; Enviamos datos al puerto
	RCALL	delay_100ms		; Mantenemos los leds prendidos cierto tiempo
	LSR		R16				; LSR --> Desplazamiento de bits a la derecha
	BRCC	led_shift_right	; Si no hay desbordamiento de bits repetimos el ciclo
	RET

turn_on:
	OUT		PORTB, R16	; Enviamos datos al puerto
	RCALL	delay_100ms ; Retardo
	RET