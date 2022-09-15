;
; practica_02_inc-dec-binario-boton.asm
;
; Created: 07/09/2022 12:53:56 p. m.
; Author : PakoMtz
;

.INCLUDE "M2560DEF.INC"
.CSEG
.ORG 0x00

; Inicializacion del stack-pointer y otras
LDI R16, LOW(RAMEND)
OUT SPL, R16
LDI R16, HIGH(RAMEND)
OUT SPH, R16


LDI R16, 0xff	; Asignamos valor maximo para configurar todo el puerto B como salida
OUT DDRB, R16	; Declaramos puerto B como salida
LDI R16, 0x00	; Asignamos valor de 0 para configurar todo el puerto D como entrada
OUT DDRC, R16	; Declaramos puerto C como entrada (botones)
OUT	PORTB, R16	; Enviamos R16 por el puerto B para apagar los leds

LDI	R21, 0b0000_0001	; Vincularemos el PIN0 con el boton incremento
LDI	R22, 0b0000_0010	; Vincularemos el PIN1 con el boton decremento

main:
	IN		R24, PINC		; Leemos el puerto C y lo guardamos en R24
	CP		R21, R24		; Comparamos R21 - R24 (0b0000_0001 - R24)
							; Si R24 = 0b0000_0000, entonces no se levantara ninguna flag
							; Si R24 = 0b0000_0001, entonces se levantara la flag Zero
							; Si R24 = 0b0000_0010, entonces se levantara la flag Negative
	BREQ	increment_btn	; Si Z=1
	BRMI	decrement_btn	; Si N=1
	RJMP	main

increment_btn:
	CLZ						; Reseteamos la flag Zero
	INC		R16				; Incrementamos el valor de R16
	OUT		PORTB, R16		; Enviamos el valor de R16 al puerto B para encender los leds

	loop_inc:				; Loop para no volver a incrementar si el boton lo dejan presionado
		CLZ					; Reseteamos la flag Zero
		IN		R24, PINC	; Leemos el puerto C y lo guardamos en R24
		CP		R21, R24	; Comparamos R21 - R24 (0b0000_0001 - R24)	
							; Si R24 sigue teniendo el valor 0b0000_0001 entonces el resultado sera 0
							; Y se levantara la flag Zero
		BREQ	loop_inc	; Si Z=1 repetimos el ciclo loop_inc
	RJMP	main			; R24 != 0b0000_0001 => Volvemos al main

decrement_btn:
	CLN						; Reseteamos la flag Negative
	DEC 	R16				; Decrementamos el valor de R16
	OUT		PORTB, R16		; Enviamos el valor de R16 al puerto B para encender los leds
	
	loop_dec:				; Loop para no volver a decrementar si el boton lo dejan presionado
		CLZ					; Reseteamos la flag Zero
		IN		R24, PINC	; Leemos el puerto C y lo guardamos en R24
		CP		R22, R24	; Comparamos R22 - R24 (0b0000_0010 - R24)
							; Si R24 sigue teniendo el valor 0b0000_0010 entonces el resultado sera 0
							; Y se levantara la flag Zero
		BREQ	loop_dec	; Si Z=1 repetimos el ciclo loop_dec
	RJMP	main			; R24 != 0b0000_0010 => Volvemos al main