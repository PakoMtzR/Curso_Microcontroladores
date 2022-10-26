/*
 * Practica_10.c
 *
 * Created: 26/10/2022 12:14:16 a. m.
 * Author : PakoMtz
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	// Timer/Counter Control Register A (Configuramos el modo normal de los timers)
	TCCR1A=0x00;
	TCCR3A=0x00;
	TCCR4A=0x00;
	TCCR5A=0x00;
	
	// Timer/Counter Control Register B (Configuramos el preescaler de 1024 en cada timer)
	TCCR1B|=(1<<CS12)|(1<<CS10); 
	TCCR3B|=(1<<CS32)|(1<<CS30); 
	TCCR4B|=(1<<CS42)|(1<<CS40);
	TCCR5B|=(1<<CS52)|(1<<CS50);
	
	// Timer/Counter Control Register C
	TCCR1C=0x00;
	TCCR3C=0x00;
	TCCR4C=0x00;
	TCCR5C=0x00;
	
	/* 
	En un preescalador de 1024, cada pulso tiene una duracion de 64us; esto quiere decir,
	que para generar 1 seg se requiere 15625 pulsos. Sin embargo los contadores de los
	timers 1 3 4 y 5 tiene capacidad de 16 bits y pueden contar hasta 65536 antes de su
	desbordamiento. Entonces, debemos iniciar nuestros contadores a cierto valor para obtener
	los pulsos necesarios.
		
		1 seg --> 15625 pulsos => c = 65535 - 15625 = 49910 = 0xC2F6
		2 seg --> 31250 pulsos => c = 65535 - 31250 = 34285 = 0x85ED
		3 seg --> 46875 pulsos => c = 65535 - 46875 = 18660 = 0x48E4
		4 seg --> 62500 pulsos => c = 65535 - 62500 = 3035  = 0x0BDB
		
	*/
	TCNT1 = 0xC2F6;
	TCNT3 = 0x85ED;
	TCNT4 = 0x48E4;
	TCNT5 = 0x0BDB;
	
	// Habilitamos las interrupciones de los timers
	TIMSK1|=(1<<TOIE1);
	TIMSK3|=(1<<TOIE3);
	TIMSK4|=(1<<TOIE4);
	TIMSK5|=(1<<TOIE5);
	
	// Configuracion de puertos de salida
	DDRH |= (1<<DDB0);
	DDRL |= (1<<DDL2);
	DDRC |= (1<<DDC6);
	DDRF |= (1<<DDF5);
	
	// Apagamos los leds 
	PORTH &= ~(1<<PORTF0);
	PORTL &= ~(1<<PORTF2);
	PORTC &= ~(1<<PORTF6);
	PORTF &= ~(1<<PORTF5);
	
	sei();	// Habilitamos las interrupciones
	
    while (1) 
    {
		//
    }
}

ISR(TIMER1_OVF_vect) 
{
	PORTH ^=(1<<PORTH0);
	TCNT1 = 0xC2F6;
}

ISR(TIMER3_OVF_vect)
{
	PORTL ^=(1<<PORTL2);
	TCNT3 = 0x85ED;
}

ISR(TIMER4_OVF_vect)
{
	PORTC ^=(1<<PORTC6);
	TCNT4 = 0x48E4;
}

ISR(TIMER5_OVF_vect)
{
	PORTF ^=(1<<PORTF5);
	TCNT5 = 0x0BDB;
}