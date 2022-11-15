/*
 * Practica_9_v2.c
 *
 * Created: 04/11/2022 10:25:26 a. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL

//#define F_OSC 16000000
//#define PRESCALER 64

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "my_keypad_lib.h"
#include "my_lcd_lib.h"

//uint8_t frec_clk = 16000000;
//uint8_t prescaler = 64;

uint8_t run_time = 1;
uint8_t freq = 1;
char freq_text[16];
char key;
//char test[16];

void send_frequency(void)
{
	if (freq < 10) sprintf(freq_text, "frequency:  %dKHz", freq);
	else sprintf(freq_text, "frequency: %dKHz", freq);
	lcd_goto_xy(1,0);
	lcd_send_string(freq_text);
}

int main(void)
{
	//	  Configuracion del Timer 0
	// --------------------------------------------
	TCCR0A |= (1<<WGM01) | (1<<COM0A1);
	TCCR0B |= (1<<CS00) | (1<<CS01);
	TCNT0 = 0x00;
	//OCR0A = (uint8_t)(F_OSC/(2*PRESCALER*freq*1000)) - 1;
	//OCR0A = 124 => 1KHz;
	OCR0A = (uint8_t)((125/freq) - 1);
	//OCR0A = (uint8_t)((frec_clk/(2*prescaler*freq*1000)) - 1);
	TIMSK0 |= (1<<OCIE0A);
	sei();

	//	Inicializacion del LCD y del teclado matricial
	// --------------------------------------------
	keypad_setup();
	lcd_initialization();
	lcd_goto_xy(0, 3);
	lcd_send_string("[RUNNING]");
	send_frequency();
	
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
					//sprintf(test, "%u", OCR0A);
					//lcd_send_string(test);
					lcd_send_string("[RUNNING]");
					break;
				
				// Decrementar la frecuencia
				case '*':
					if (run_time == 0)
					{
						if (freq > 1) freq--;
						else freq = 30;
						OCR0A = (uint8_t)((125/freq) - 1);
					}
					break;
				
				// Incrementar la frecuencia
				case '#':
					if (run_time == 0)
					{
						if (freq < 30) freq++;
						else freq = 1;
						OCR0A = (uint8_t)((125/freq) - 1);
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
	}
}