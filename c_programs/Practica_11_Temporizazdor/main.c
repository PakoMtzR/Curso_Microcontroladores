/*
 * Practica11.c
 *
 * Created: 26/10/2022 01:37:59 p. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "my_lcd_lib.h"

#define KEYPAD_PORT PORTK
#define KEYPAD_PIN PINK
#define KEYPAD_CONFIG DDRK

#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB

uint8_t time_init[2] = {10, 0};
uint8_t time[2] = {0, 0};//{time_init[0], time_init[1]};
	
char time_str[2][3];
char time_LCD[16];
char key;

uint8_t i = 0;
uint8_t data = 0;
uint8_t run_time = 0;
uint8_t pos_config = 1;
uint8_t config = 1;

void send_time(void)
{
	// Convertir seg, min, hrs a string
	for (uint8_t k = 0; k < 2; k++)
	{
		if (time[k] < 10) sprintf(time_str[k], "0%d", time[k]);
		else sprintf(time_str[k], "%d", time[k]);
	}

	// Concatenar el tiempo => [time]: 00:00
	sprintf(time_LCD,"[time]: %s:%s", time_str[1], time_str[0]);
	
	// Enviar el tiempo al LCD
	lcd_goto_xy(1, 0);				// Nos movemos a la linea inferior en la primera casilla
	lcd_send_string(time_LCD);		// Escribimos el tiempo en el LCD
}

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

void move_right(void)
{
	if (pos_config < 2) pos_config++;
	else pos_config = 1;
}

void move_left(void)
{
	if (pos_config > 1) pos_config--;
	else pos_config = 2;
}

void increment(void)
{
	switch(pos_config)
	{
		case 1:
			if (time_init[1] < 59) time_init[1]++;
			else time_init[1] = 0;
			break;
			
		case 2:
			if (time_init[0] < 59) time_init[0]++;
			else time_init[0] = 0;
			break;
			
		default:
			break;
	}
}

void decrement(void)
{
	switch(pos_config)
	{
		case 1:
			if (time_init[1] > 1) time_init[1]--;
			else time_init[1] = 59;
			break;
			
		case 2:
			if (time_init[0] > 1) time_init[0]--;
			else time_init[0] = 59;
			break;
			
		default:
			break;
	}
}

int main(void)
{
	//	Configuracion del timer 1
	// -------------------------------
	/* 
	En un preescalador de 1024, cada pulso tiene una duracion de 64us; esto quiere decir,
	que para generar 1 seg se requiere 15625 pulsos. Sin embargo los contadores de los
	timers 1 3 4 y 5 tiene capacidad de 16 bits y pueden contar hasta 65536 antes de su
	desbordamiento. Entonces, debemos iniciar nuestros contadores a cierto valor para obtener
	los pulsos necesarios.
		
		1 seg --> 15625 pulsos => c = 65535 - 15625 = 49910 = 0xC2F6
	*/
	TCCR1A = 0x00;
	TCCR1B |= (1<<CS12)|(1<<CS10);
	TCCR1C = 0x00;
	TCNT1 = 0xC2F6;
	TIMSK1|= (1<<TOIE1);
	sei();
	
	//	Configuracion del keypad
	// -------------------------------
	KEYPAD_CONFIG = 0x0f;	
	KEYPAD_PORT = 0xff;
	
	//	Configuracion del buzzzer
	// -------------------------------
	BUZZER_DDR |= (1<<DDB0);
	BUZZER_PORT &= ~(1<<PORTB0);
	
	//	Inicializacion del LCD
	// -------------------------------
	lcd_initialization();
	lcd_clean_display();
	lcd_goto_xy(0, 4);
	lcd_send_string("[PAUSE]");
	
	//	Tiempo inicial
	// -------------------------------
	time[0] = time_init[0];
	time[1] = time_init[1];
	send_time();
	
	
    while (1) 
    {
		_delay_ms(10);
		key = keypad();
		if (key != '\0')
		{
			switch(key)
			{
				// Pausar
				case '0':
					run_time = 0;
					lcd_clean_display();
					lcd_goto_xy(0, 4);
					lcd_send_string("[PAUSE]");
					break;
				
				// Configuracion	
				case 'B':
					run_time = 0;
					config = 1;
					lcd_clean_display();
					lcd_goto_xy(0, 3);
					lcd_send_string("[SETTING]");
					break;
					
				// Reanudar
				case 'A':
					run_time = 1;
					config = 0;
					lcd_clean_display();
					lcd_goto_xy(0, 3);
					lcd_send_string("[RUNNING]");
					break;
				
				// Moverse a la izquierda
				case '*':
					if (run_time == 0 || config == 1) move_left();
					break;
					
				// Moverse a la derecha
				case '#':
					if (run_time == 0 || config == 1) move_right();
					break;
				
				// Incrementar
				case 'C':
					if (run_time == 0 || config == 1) increment();
					break;
					
				// Decrementar	
				case 'D':
					if (run_time == 0 || config == 1) decrement();
					break;
					
				default:
					break;
			}
			
			if (config == 1)
			{
				time[0] = time_init[0];
				time[1] = time_init[1];
				BUZZER_PORT &= ~(1<<PORTB0);
				TCNT1 = 0xC2F6;
			}
			
			send_time();
		}
    }
}


ISR(TIMER1_OVF_vect)
{
	if (run_time == 1)
	{
		if (time[0] > 0) time[0]--;
		else
		{
			if (time[1] > 0) time[1]--;
			else
			{
				BUZZER_PORT ^=(1<<PORTB0);
			}
		}
		
		TCNT1 = 0xC2F6;
		send_time();
	}
}