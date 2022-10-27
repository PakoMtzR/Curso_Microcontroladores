/*
 * Practica_9.c
 *
 * Created: 26/10/2022 03:19:13 a. m.
 * Author : PakoMtz
 */ 

#define  F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
//#include <math.h>
#include "my_lcd_lib.h"

#define KEYPAD_PORT PORTK
#define KEYPAD_PIN PINK
#define KEYPAD_CONFIG DDRK

#define ARDUINO_FREQ 16000000
#define PRESCALER 256

uint8_t data = 0;
uint8_t run_time = 1;
uint8_t freq = 1;
char freq_text[16];
char key;

char keypad(void)
{
	char const keys[4][4] = {	{'1','2','3','A'},
								{'4','5','6','B'},
								{'7','8','9','C'},
								{'*','0','#','D'}	};
	
	for (uint8_t row = 0; row < 4; row++)
	{
		KEYPAD_PORT = ~(1<<row);
		_delay_us(10);
		data = KEYPAD_PIN & 0xf0;
		
		if (data != 0xf0)
		{
			_delay_ms(200);
			switch(data)
			{
				case 0x70: return keys[row][3];
				case 0xb0: return keys[row][2];
				case 0xd0: return keys[row][1];
				case 0xe0: return keys[row][0];
			}
		}
	}
	return '\0';	// Caracter nulo
}

void send_frequency(void)
{
	if (freq < 10) sprintf(freq_text, "frequency:  %dKHz", freq);
	else sprintf(freq_text, "frequency: %dKHz", freq);
	lcd_goto_xy(1,0);
	lcd_send_string(freq_text);
}

int main(void)
{
	//	  Configuracion del inicial del timer 0
	// --------------------------------------------
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS02); 
	TCNT0 = 0x00;
	OCR0A = (uint8_t)((ARDUINO_FREQ/(PRESCALER*freq*1000)) - 1);
	TIMSK0 |= (1<<OCIE0A);
	sei();
	
	//			Inicializacion del LCD
	// --------------------------------------------
	lcd_initialization();
	lcd_goto_xy(0, 3);
	lcd_send_string("[RUNNING]");
	send_frequency();
	
	//			Configuracion del keypad
	// --------------------------------------------
	KEYPAD_CONFIG = 0x0f;
	KEYPAD_PORT = 0xff;
	
	//		Configuracion del puerto de salida
	// --------------------------------------------
	DDRL |= (1<<DDL2);
	PORTL &= ~(1<<PORTL2);
	
    while (1) 
    {
		//		Lectura del teclado matricial
		// --------------------------------------------
		_delay_ms(10);
		key = keypad();
		if (key != '\0')
		{
			switch(key)
			{
				// Pausar el reloj
				case 'A':
					run_time = 0;
					lcd_clean_display();
					lcd_goto_xy(0, 4);				
					lcd_send_string("[PAUSE]");
					break;
				
				// Reanudar	
				case 'B':
					run_time = 1;
					lcd_clean_display();
					lcd_goto_xy(0, 3);
					lcd_send_string("[RUNNING]");
					break;
				
				// Decrementar la frecuencia	
				case '*':
					if (run_time == 0)
					{
						if (freq > 1) freq--;
						else freq = 30;
						OCR0A = (uint8_t)((ARDUINO_FREQ/(PRESCALER*freq*1000)) - 1);
					}
					break;
				
				// Incrementar la frecuencia	
				case '#':
					if (run_time == 0)
					{
						if (freq < 30) freq++;
						else freq = 1;
						OCR0A = (uint8_t)((ARDUINO_FREQ/(PRESCALER*freq*1000)) - 1);
					}
					break;
					
				default:
					break;
			}
			send_frequency();
		}
    }
}

ISR(TIMER0_COMPA_vect)
{
	if (run_time == 1)
	{
		PORTL ^=(1<<PORTL2);
		//TCNT0 = 0x00;
	}
}