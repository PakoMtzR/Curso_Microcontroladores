/*
 * Reloj_Calendario.c
 *
 * Created: 15/10/2022 09:32:16 p. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "my_lcd_lib.h"

#define KEYPAD_PORT PORTK
#define KEYPAD_PIN PINK
#define KEYPAD_CONFIG DDRK

/******************************************************************************
							DECLARACIOND DE VARIABLES
******************************************************************************/
uint8_t time[3] = {50, 59, 17};	// {seconds, minutes, hours};
uint8_t date[3] = {1, 10, 2};	// {days, month, years};

char time_str[3][3];	// --> [3][3] -> {"SS\0", "MM\0", "HH\0"}  (\0 caracter nulo)
char date_str[3][3];	// --> [3][3] -> {"DD\0", "MM\0", "YY\0"}  (\0 caracter nulo)

// Cadenas de texto para enviar al LCD (Dia y la hora)
char time_LCD[16];
char date_LCD[16];

uint8_t data = 0;
char key;

uint8_t i = 0;			// Variable para el contador del LCD
uint8_t run_time = 1;	// Variable para pausar o reanudar la interrupcion de reloj

uint8_t pos_config = 1;
/******************************************************************************
							DECLARACION DE FUNCIONES
******************************************************************************/
void send_time(void);
void send_date(void);
char keypad(void);
void move_left(void);
void move_right(void);
void decrement(void);
void increment(void);

int main(void)
{
	//		Configuracion de interrupciones para el timer
	// ---------------------------------------------------------
	TCCR0A = 0x00;						// timer 0 --> [Modo]: Normal
	TCCR0B |= (1<<CS00) | (1<<CS02);	// clkI/O/(No prescaling)
	TCNT0 = 0x00;						// Iniciar registro contador a 0 (cout = 0)
	TIMSK0 |= (1<<TOIE0);
	sei();								// Habilitamos las interrupciones

	/***********************************************************
						INICIALIZACION DEL LCD
	***********************************************************/
	lcd_initialization();
	send_date();
	
	//					Configuracion del keypad
	// ---------------------------------------------------------
	KEYPAD_CONFIG = 0x0f;	// Nibel alto Columnas, Nible bajo filas
	KEYPAD_PORT = 0xff;
	
	while (1) 
    {
		_delay_ms(10);
		key = keypad();
		if (key != '\0')
		{
			switch(key)
			{
				case 'A': 
					run_time = 0;
					break;
				case 'B': 
					run_time = 1;
					break;
				case '*': 
					if (run_time == 0) move_left();
					break;
				case '#': 
					if (run_time == 0) move_right();
					break;
				case 'C': 
					if (run_time == 0) increment();
					break;
				case 'D': 
					if (run_time == 0) decrement();
					break;
				default:
					break;
			}
			send_date();
			send_time();
		}
    }
}
/******************************************************************************
							FUNCIONES SECUNDARIAS
******************************************************************************/
void send_time(void)
{	
	// Convertir seg, min, hrs a string
	for (uint8_t k = 0; k < 3; k++)
	{
		if (time[k] < 10) sprintf(time_str[k], "0%d", time[k]);
		else sprintf(time_str[k], "%d", time[k]);
	}

	// Concatenar el tiempo => [time]: 00:00:00
	sprintf(time_LCD,"[time]: %s:%s:%s", time_str[2], time_str[1], time_str[0]);
	
	// Enviar el tiempo al LCD
	lcd_goto_xy(1, 0);				// Nos movemos a la linea inferior en la primera casilla
	lcd_send_string(time_LCD);		// Escribimos el tiempo en el LCD
}

void send_date(void)
{
	// Convertir dia, mes, anno a string
	for (uint8_t k = 0; k < 3; k++)
	{
		if (date[k] < 10) sprintf(date_str[k], "0%d", date[k]);
		else sprintf(date_str[k], "%d", date[k]);
	}

	// Concatenar la fecha => [date]: 01/10/02
	sprintf(date_LCD,"[date]: %s/%s/%s", date_str[0], date_str[1], date_str[2]);
	
	// Enviar la fecha al LCD
	lcd_goto_xy(0, 0);				// Nos movemos a la linea superior en la primera casilla
	lcd_send_string(date_LCD);		// Escribimos la fecha en el LCD
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
	if (pos_config < 6) pos_config++;
	else pos_config = 1;
}

void move_left(void)
{
	if (pos_config > 1) pos_config--;
	else pos_config = 6;
}

void increment(void)
{
	switch(pos_config)
	{
		case 1: 
			if (date[0] < 31) date[0]++;
			else date[0] = 1;
			break;
		case 2:
			if (date[1] < 12) date[1]++;
			else date[1] = 1;
			break;
		case 3:
			if (date[2] < 99) date[2]++;
			else date[2] = 0;
			break;
		case 4:
			if (time[2] < 23) time[2]++;
			else time[2] = 0;
			break;
		case 5:
			if (time[1] < 59) time[1]++;
			else time[1] = 0;
			break;
		case 6:
			if (time[0] < 59) time[0]++;
			else time[0] = 0;
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
			if (date[0] > 1) date[0]--;
			else date[0] = 31;
			break;
		case 2:
			if (date[1] > 1) date[1]--;
			else date[1] = 12;
			break;
		case 3:
			if (date[2] > 0) date[2]--;
			else date[2] = 99;
			break;
		case 4:
			if (time[2] > 0) time[2]--;
			else time[2] = 23;
			break;
		case 5:
			if (time[1] > 1) time[1]--;
			else time[1] = 59;
			break;
		case 6:
			if (time[0] > 1) time[0]--;
			else time[0] = 59;
			break;
		default:
			break;
	}
}
/******************************************************************************
						INTERRUPCIONES INTERNAS (TIMER)
******************************************************************************/
ISR(TIMER0_OVF_vect)
{
	if (run_time == 1)
	{
		i++;
		if (i >= 61)
		{
			if (time[0] == 59) // Si segundos llego a 59 => hh:mm:59
			{
				time[0] = 0;	// Resetear segundos 01:59 -> 02:00	
				
				if (time[1] == 59)	// Si minutos llego a 59 => hh:59:59 
				{
					time[1] = 0;	// Resetear minutos y segundos 02:59:59 -> 03:00:00
					
					if (time[2] == 23) // Si horas llego a 23 => 23:59:59
					{
						time[2] = 0;	// Resetear minutos, segundos y horas 23:59:59 -> 00:00:00
						
						if (date[0] == 31)
						{
							date[0] = 1;	// Resetear dia 31/MM/YY => 01/MM+1/YY
							
							if (date[1] == 12)
							{
								date[1] = 1;	// Resetear dia y el mes 31/12/YY => 01/01/YY+1
								
								if (date[2] == 99)
								{
									date[2] = 0;	// Resetear dia, el mes y el anno 31/12/99 => 01/01/00
								}
								else date[2]++;	// Incrementar año
							}
							else date[1]++;	// Incrementar mes
						}
						else date[0]++;	// Incrementar dia
					}
					else time[2]++;	// Incrementar horas
				}
				else time[1]++;	// Incrementar minutos
			}
			else time[0]++;	// Incrementar segundos
			
			send_time();
			i = 0;
		}
	}
}