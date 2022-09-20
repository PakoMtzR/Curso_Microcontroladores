/*
 * practica_05_motor_paso.c
 *
 * Created: 18/09/2022 06:08:12 p. m.
 * Author : PakoMtz
 */ 

#define	F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

void rutina_paso_simple(void){
	// Valores para la rutina de paso simple
	uint8_t paso_simple[4] = {0x01, 0x02, 0x04, 0x08};
		
	while(1)
	{
		// Recorremos cado uno de los valores de la rutina de paso simple
		// y los mandamos al puerto B
		for(int i = 0; i < 4; i++)
		{
			PORTB = paso_simple[i];
			_delay_ms(10);
		}
		
		// Leer el boton de paro
		if (PINC & (1<<PC1)) break;
	}
}

void rutina_paso_normal(void){
	// Valores para la rutina de paso normal
	uint8_t paso_normal[4] = {0x03, 0x06, 0x0C, 0x09};
		
	while(1)
	{
		// Recorremos cado uno de los valores de la rutina de paso normal
		// y los mandamos al puerto B
		for(int i = 0; i < 4; i++)
		{
			PORTB = paso_normal[i];
			_delay_ms(10);
		}
		
		// Leer el boton de paro
		if (PINC & (1<<PC1)) break;
	}
}

void rutina_medio_paso(void){
	// Valores para la rutina de medio paso
	uint8_t medio_paso[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};
		
	while(1)
	{
		// Recorremos cado uno de los valores de la rutina de medio paso
		// y los mandamos al puerto B
		for(int i = 0; i < 8; i++)
		{
			PORTB = medio_paso[i];
			_delay_ms(10);
		}
		
		// Leer el boton de paro
		if (PINC & (1<<PC1)) break;
	}
}

int main(void)
{
	DDRB = 0xff;	// Declaramos el puerto B como salida	(Driver)
	DDRC = 0x00;	// Declaramos el puerto C como entrada	(Botones)	
	DDRL = 0xff;	// Declaramos el puerto L como salida	(Leds)
		
	PORTC = 0x00;
	PORTB = 0x00;
	PORTL = 0x00;
	
    while (1)
    {
		PORTL = 0b00000001;			// Prendemos el led rojo (Paro) - bit 0 puerto L

		// Leer el boton de la rutina - medio paso
		if ((PINC & (1<<PC0)) == 0)	// Activar con un nivel bajo en el bit 0 puerto C
		{
			PORTL = 0b00000010;		// Prendemos el led indicador de la rutina
			rutina_medio_paso();	// Ejecutamos la rutina - medio paso
		}
		
		// Leer el boton de la rutina - paso simple
		if ((PINC & (1<<PC3)) == 0)	// Activar con un nivel bajo en el bit 3 puerto C
		{
			PORTL = 0b00000100;		// Prendemos el led indicador de la rutina
			rutina_paso_simple();	// Ejecutamos la rutina - paso simple
		}
		
		// Leer el boton de la rutina - paso normal
		if (PINC & (1<<PC5))		// Activar con un nivel alto en el bit 5 puerto C
		{
			PORTL = 0b00001000;		// Prendemos el led indicador de la rutina
			rutina_paso_normal();	// Ejecutamos la rutina - paso normal
		}
    }
	return 0;
}



