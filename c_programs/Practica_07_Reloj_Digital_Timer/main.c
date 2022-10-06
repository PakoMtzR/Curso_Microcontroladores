/*
 * watch_timer.c
 *
 * Created: 05/10/2022 04:50:21 p. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "my_lcd_lib.h"

// Declaracion de variables
// ---------------------------------------------------------
uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;

uint8_t i = 0;	// Variable para el contador del LCD
uint8_t run_time = 1;
char time_LCD[16];	// Variable donde guardaremos el texto  
					// del tiempo que se enviara al LCD

// Declaracion de Funciones
// ---------------------------------------------------------
void enviar_hora(void);

// Main
// ---------------------------------------------------------
int main(void)
{
	//		Configuracion de interrupciones para el timer
	// ---------------------------------------------------------
	TCCR0A = 0x00;						// timer 0 --> [Modo]: Normal
	TCCR0B |= (1<<CS00) | (1<<CS02);	// clkI/O/(No prescaling)
	TCNT0 = 0x00;						// Iniciar registro contador a 0 (cout = 0)
	TIMSK0 |= (1<<TOIE0);
	
	//		Configuracion de interrupciones externas
	// ---------------------------------------------------------
	EICRA |= (1<<ISC01)|(1<<ISC00);	// Configurar INT0 con flanco de subida
	EICRA |= (1<<ISC11)|(0<<ISC10);	// Configurar INT1 con flanco de bajada
	EICRB |= (1<<ISC41)|(1<<ISC40);	// Configurar INT4 con flanco de subida		
	EICRB |= (1<<ISC51)|(0<<ISC50);	// Configurar INT5 con flanco de bajada
	
	EIMSK |=(1<<INT0)|(1<<INT1)|(1<<INT4)|(1<<INT5);
	
	sei();	// Habilitamos las interrupciones

	/***********************************************************
						INICIALIZACION DEL LCD
	***********************************************************/
	lcd_initialization();
	lcd_goto_xy(0, 1);
	lcd_send_string("DIGITAL CLOCK");
	
	while (1) 
    {
		// No hace nada
    }
}

// Funciones Secundarias
// ---------------------------------------------------------
void enviar_hora(void)
{
	uint8_t time_int[3] = {segundos, minutos, horas};
	char time_str[3][3]; // --> [3][3] -> {"HH\0", "MM\0", "SS\0"}  (\0 caracter nulo)
	
	//// Convertir seg, min, hrs a string
	for (uint8_t k = 0; k < 3; k++)
	{
		if (time_int[k] < 10) sprintf(time_str[k], "0%d", time_int[k]);
		else sprintf(time_str[k], "%d", time_int[k]);
	}

	//// Concatenar el tiempo => [time]: 00:00:00
	sprintf(time_LCD,"[time]: %s:%s:%s", time_str[2], time_str[1], time_str[0]);
	
	// Enviar el tiempo al LCD
	lcd_goto_xy(1, 0);			// Nos movemos a la linea inferior en la primera casilla
	lcd_send_string(time_LCD);		// Escribimos el tiempo en el LCD
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
			// Incrementar segundos
			if (segundos == 59)
			{
				if (minutos == 59)
				{
					if (horas == 24) horas = 0;
					else horas++;
					
					minutos = 0;
					segundos = 0;
				}
				else minutos++;
				
				segundos = 0;
			}
			else segundos++;
			
			enviar_hora();
			i = 0;
		}
	}
}

///******************************************************************************
							//INTERRUPCIONES EXTERNAS
//******************************************************************************/
// Al detectar en INT0 un flanco de subida, configurar los minutos.
ISR (INT0_vect) 
{
	if (minutos == 59)
	{
		if (horas == 24) horas = 0;
		else horas++;
		
		minutos = 0;
		segundos = 0;
	}
	else minutos++;
	enviar_hora();
}

// Al detectar en INT1 un flanco de bajada, configurar las horas.
ISR (INT1_vect)
{
	if (horas == 24) horas = 0;
	else horas++;
	enviar_hora();
}

// Al detectar en INT4 un flanco de subida, permitirá detener el reloj, esto 
// con la finalidad de configurar nuevamente la hora y minuto si se requiere.
ISR (INT4_vect)
{
	run_time = 0;
}

// Al detectar en INT5 un flanco de bajada, permitirá  iniciar el reloj, es decir, una vez 
// que se configuren los minutos y horas del reloj, este botón arrancará el funcionamiento del mismo.
ISR (INT5_vect)
{
	run_time = 1;
}