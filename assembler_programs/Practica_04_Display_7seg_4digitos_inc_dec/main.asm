;
; Display_3digitos_v3.asm
;
; Created: 14/09/2022 08:05:28 a. m.
; Author : PakoMtz
;
.INCLUDE "M2560DEF.INC"
.CSEG
.ORG 0x00

; =========================================================================
;								Configuracion 	
; =========================================================================
LDI R16, LOW(RAMEND)
OUT SPL, R16
LDI R16, HIGH(RAMEND)
OUT SPH, R16

LDI R16, 0xff	; Asignamos valor maximo para configurar todo el puerto B como salida
OUT DDRA, R16	; Declaramos puerto A como salida (Display)
OUT DDRB, R16	; Declaramos puerto B como salida (Transistores)

LDI R16, 0x00	; Asignamos valor de 0 para configurar todo el puerto C como entrada
OUT DDRC, R16	; Declaramos puerto D como entrada (botones)

OUT	PORTA, R16	; Enviamos R16 por el puerto A para apagar el display

LDI R17, 0		; Registro para marcar el limite del conteo
LDI R21, 0		; Registro donde guardaremos las unidades
LDI R22, 0		; Registro donde guardaremos las decenas
LDI R23, 0		; Registro donde guardaremos las centenas
LDI R26, 0		; Registro donde guardaremos el digito a enviar

LDI	R24, 0b0000_0001	; Vincularemos el PIN0 con el boton incremento
LDI	R25, 0b0000_0010	; Vincularemos el PIN1 con el boton decremento

; =========================================================================
;							Programa principal 	
; =========================================================================
main:
	IN		R20, PINC		; Leemos el puerto C y lo guardamos en R20
	CP		R24, R20		; Comparamos R24 - R20 (0b0000_0001 - R20)
	BREQ	increment		; Si Z=1
	CP		R25, R20		; Comparamos R25 - R20 (0b0000_0010 - R20)
	BREQ	decrement		; Si Z=1
	RCALL	send_data
	RJMP	main
; =========================================================================

; =========================================================================
;								Incrementador	
; =========================================================================
increment:
	CLZ						; Reseteamos la flag Zero
	CPI		R17, 255		; Comparamos si nuestro contador llego al valor de 255
	BREQ	increment_set	; Si Z=1 (si se cumple la condicion anterior) reseteamos el contador
	INC		R17				; Si no, incrementamos 
	CPI		R21, 9			; Comparamos el valor guardado en R21 con 9 (R21 - 9)
	BREQ	increment_tens	; Si Z=1, entonces incrementa las decenas
	INC		R21				; Si no, incrementa las unidades
	loop_inc:				; Loop para no volver a incrementar si el boton lo dejan presionado
		CLZ					; Reseteamos la flag Zero
		IN		R20, PINC	; Leemos el puerto C y lo guardamos en R20
		CP		R24, R20	; Comparamos R24 - R20 (0b0000_0001 - R20)	
							; Si R20 sigue teniendo el valor 0b0000_0001 entonces el resultado sera 0
							; Y se levantara la flag Zero
		BREQ	loop_inc	; Si Z=1 repetimos el ciclo loop_inc
	RJMP main

increment_tens:
	CLZ							; Reseteamos la flag Zero
	CPI		R22, 9				; Comparamos el valor guardado en R22 con 9 (R22 - 9)
	BREQ	increment_hundreds	; Si Z=1, entonces incrementa las centenas
	INC		R22					; Si no, incrementa las decenas
	LDI		R21, 0				; Regresar a R21 con valor de 0
	RJMP	loop_inc

increment_hundreds:
	CLZ						; Reseteamos la flag Zero
	CPI		R23, 9			; Comparamos el valor guardado en R23 con 9 (R23 - 9)
	BREQ	increment_set	; Si Z=1, entonces resetemos el numero
	INC		R23				; Si no, incrementa las centenas
	LDI		R22, 0			; Regresar a R22 con valor de 0
	LDI		R21, 0			; Regresar a R21 con valor de 0
	RJMP	loop_inc

increment_set:
	CLZ						; Reseteamos la flag Zero
	LDI		R17, 0			; Resetemaos el contador
	LDI		R23, 0			; Regresar a R23 con valor de 0
	LDI		R22, 0			; Regresar a R22 con valor de 0
	LDI		R21, 0			; Regresar a R21 con valor de 0
	RJMP	loop_inc
; =========================================================================

; =========================================================================
;								Decrementador	
; =========================================================================
decrement:
	CLZ						; Reseteamos la flag Zero
	CPI		R17, 0			; Comparamos si nuestro contador llego al valor de 0
	BREQ	decrement_set	; Si Z=1 (si se cumple la condicion anterior) reseteamos el contador
	DEC		R17				; Si no, decrementamos
	CPI		R21, 0			; Comparamos el valor guardado en R21 con 0 (R21 - 0)
	BREQ	decrement_tens	; Si Z=1, entonces decrementa las decenas
	DEC		R21				; Si no, decrementa las unidades
	loop_dec:				; Loop para no volver a incrementar si el boton lo dejan presionado
		CLZ					; Reseteamos la flag Zero
		IN		R20, PINC	; Leemos el puerto C y lo guardamos en R20
		CP		R25, R20	; Comparamos R25 - R20 (0b0000_0010 - R20)	
							; Si R20 sigue teniendo el valor 0b0000_0010 entonces el resultado sera 0
							; Y se levantara la flag Zero
		BREQ	loop_dec	; Si Z=1 repetimos el ciclo loop_dec
	RJMP main

decrement_tens:
	CLZ							; Reseteamos la flag Zero
	CPI		R22, 0				; Comparamos el valor guardado en R21 con 0 (R21 - 0)
	BREQ	decrement_hundreds	; Si Z=1, entonces decrementa las decenas
	DEC		R22					; Decrementamos las decenas
	LDI		R21, 9				; Regresar a R21 con valor de 9
	RJMP	loop_dec

decrement_hundreds:
	CLZ							; Reseteamos la flag Zero
	CPI		R23, 0				; Comparamos el valor guardado en R21 con 0 (R21 - 0)
	BREQ	decrement_set		; Si Z=1, entonces resetemos el numero
	DEC		R23					; Decrementamos las centenas
	LDI		R22, 9				; Regresar a R22 con valor de 9
	LDI		R21, 9				; Regresar a R21 con valor de 9
	RJMP	loop_dec

decrement_set:
	CLZ							; Reseteamos la flag Zero
	LDI		R17, 255			; Reseteamos el contador con 255
	LDI		R23, 2				; Regresar a R23 con valor de 2
	LDI		R22, 5				; Regresar a R22 con valor de 5
	LDI		R21, 5				; Regresar a R21 con valor de 5
	RJMP	loop_dec
; =========================================================================

.INCLUDE "encoder.inc"
