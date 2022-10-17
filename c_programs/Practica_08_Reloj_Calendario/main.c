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

// Declaracion de variables
// ---------------------------------------------------------
uint8_t time[3] = {0, 0, 0};	// {seconds, minutes, hours};
uint8_t date[3] = {16, 10, 22};	// {days, month, years};

char time_str[3][3]; // --> [3][3] -> {"HH\0", "MM\0", "SS\0"}  (\0 caracter nulo)
char date_str[3][3];

char time_LCD[16];	// Variable donde guardaremos el texto
					// del tiempo que se enviara al LCD
char date_LCD[16];	// Variable donde guardaremos el texto
					// del tiempo que se enviara al LCD
uint8_t data = 0;
char key;

uint8_t i = 0;	// Variable para el contador del LCD
uint8_t run_time = 1;


// Declaracion de Funciones
// ---------------------------------------------------------
void send_time(void);
void send_date(void);
char keypad(void);

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
		//key = keypad();
		//if (key != '\0')
		//{
			//lcd_goto_xy(0, 0);
			//lcd_send_char(key);
		//}
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
	lcd_goto_xy(1, 0);			// Nos movemos a la linea inferior en la primera casilla
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

	// Concatenar el tiempo => [time]: 00:00:00
	sprintf(date_LCD,"[date]: %s/%s/%s", date_str[0], date_str[1], date_str[2]);
	
	// Enviar el tiempo al LCD
	lcd_goto_xy(0, 0);			// Nos movemos a la linea inferior en la primera casilla
	lcd_send_string(date_LCD);		// Escribimos el tiempo en el LCD
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

void command(char cmd)
{
	
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
			// Incrementar seconds
			if (time[0] == 59)
			{
				if (time[1] == 59)
				{
					if (time[2] == 23) 
                                        {
                                            time[2] = 0;
                                            if (date[0] == 31)
                                            {date[0] = 1;
                                                if (date[1] == 12)
                                                {date[1] = 1;
                                                    if (date[2] == 99) date[2] = 0;
                                                    else date[2]++;
                                                } 
                                            }
                                            else date[0]++;
                                        }
	  				else time[2]++;
					
					time[1] = 0;
					time[0] = 0;
				}
				else time[1]++;
				time[0] = 0;
			}
			else time[0]++;
			
			send_time();
			i = 0;
		}
	}
}
