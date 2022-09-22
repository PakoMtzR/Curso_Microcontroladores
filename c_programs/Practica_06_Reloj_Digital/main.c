/*
 * practica_06_reloj_v2.c
 *
 * Created: 21/09/2022 09:24:33 a. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;

uint8_t incrementar_minuto = 0;
uint8_t incrementar_hora = 0;

uint8_t digt_7seg[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; 

uint8_t primer_digito = 0;
uint8_t segundo_digito = 0;

void enviar_hora(void){
	uint8_t hora_actual[3] = {segundos, minutos, horas};
	for (int i = 0; i < 3; i++)
	{
		primer_digito = hora_actual[i] % 10;
		segundo_digito = hora_actual[i] / 10;
		//PORTL = 0x00;
		// Enviamos el primer digito 
		PORTL = (1<<(2*i));					// Habilitamos el transistor correspondiente
		PORTC = digt_7seg[primer_digito];	// Enviamos el primer digito (seg, min, hrs)
		_delay_ms(1);
		
		// Enviamos el segundo digito 
		PORTL = (1<<((2*i)+1));				// Habilitamos el transistor correspondiente	
		PORTC = digt_7seg[segundo_digito];	// Enviamos el segundo digito (seg, min, hrs)
		_delay_ms(1);
	}
}


int main(void)
{
	DDRC = 0xff;		// Configuramos puerto C como salida (Display)
	DDRH = 0x00;		// Configuramos puerto H como entrada (Botones)
	DDRL = 0x00;		// Configuramos puerto L como salida (Transistores)
	DDRD &= ~(1<<0);	// Configuramos puerto D como entrada INT0 => PD0 Entrada (0)
	PORTC = 0x00;		// Enviamos 0 en el puerto C
	PORTL = 0x00;		// Enviamos 0 en el puerto B
	EIMSK |= (1<<0);	// Habilitamos solo el INT0 en el registro de interrupciones
	EICRA = 0b00000010;	// Configuramos la interrupcion
	sei();				// Habilitamos las interrupciones

    while (1) 
    {	
		// Incrementar el tiempo
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
		
		// Rellenamos el tiempo de sobra para juntar el minuto
		for (int j = 0; j < 64; j++)
		{
			enviar_hora();
			_delay_ms(10);
		}
    }
}

ISR(INT0_vect){
	while(1)
	{
		segundos = 0;				// Reseteamos los segundos
		
		// Leemos el pin 3 del puerto H para incrementar un minuto
		while(PINH & (1<<3))		
		{
			incrementar_minuto = 1;	// Dejamos una marca para indicar que el boton ha sido presionado
			enviar_hora();
		}
		
		// Leemos el pin 4 del puerto H para incrementar una hora
		while(PINH & (1<<4))
		{
			incrementar_hora = 1;	// Dejamos una marca para indicar que el boton ha sido presionado
			enviar_hora();
		}
		
		if (incrementar_minuto == 1)		// Si el boton de incremento-minuto fue presionado
		{
			if (minutos == 59) minutos = 0;
			else minutos++;
			incrementar_minuto = 0;			// Limpiamos la marca
		}
		
		if (incrementar_hora == 1)			// Si el boton de incremento-hora fue presionado
		{
			if (horas == 23) horas = 0;
			else horas++;
			incrementar_hora = 0;			// Limpiamos la marca
		}
		
		// Leemos el boton para poner en marcha el reloj y salga de la interrupcion
		if (PINH & (1<<6)) break;
		
		enviar_hora();
	}
}
