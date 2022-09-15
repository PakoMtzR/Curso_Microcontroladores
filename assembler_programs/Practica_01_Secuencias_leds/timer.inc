/*
 * timer.inc
 *
 *  Created: 02/09/2022 09:35:31 p. m.
 *   Author: PakoMtz
 */ 

; 16 ciclos maquina
delay_1us:	
			LDI  R18, 5
	Loop_0:	DEC  R18
			BRNE Loop_0
			NOP
			RET

; 160 ciclos maquina
delay_10us:	
			LDI  R18, 53
	Loop_1:	DEC  R18
			BRNE Loop_1
			NOP
			RET

; 1,600 ciclos maquina
delay_100us:
			LDI  R18, 3
			LDI  R19, 19
	Loop_2:	DEC  R19
			BRNE Loop_2
			DEC  R18
			BRNE Loop_2
			RET

; 8,000 ciclos maquina
delay_500us:
			LDI  R18, 11
			LDI  R19, 99
	Loop_3:	DEC  R19
			BRNE Loop_3
			DEC  R18
			BRNE Loop_3
			RET

; 16,000 ciclos maquina
delay_1ms:
			LDI  R18, 21
			LDI  R19, 199
	Loop_4:	DEC  R19
			BRNE Loop_4
			DEC  R18
			BRNE Loop_4
			RET

; 160,000 ciclos maquina
delay_10ms:
			LDI  R18, 208
			LDI  R19, 202
	Loop_5:	DEC  R19
			BRNE Loop_5
			DEC  R18
			BRNE Loop_5
			NOP
			RET

; 1,600,000 ciclos maquina
delay_100ms:
			LDI  R18, 9
			LDI  R19, 30
			LDI  R20, 229
	Loop_6:	DEC  R20
			BRNE Loop_6
			DEC  R19
			BRNE Loop_6
			DEC  R18
			BRNE Loop_6
			NOP
			RET

; 8,000,000 ciclos maquina
delay_500ms:
			LDI  R18, 41
			LDI  R19, 150
			LDI  R20, 128
	Loop_7:	DEC  R20
			BRNE Loop_7
			DEC  R19
			BRNE Loop_7
			DEC  R18
			BRNE Loop_7
			RET