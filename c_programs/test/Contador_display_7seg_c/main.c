/*
 * display_3digitos_c_v1.c
 *
 * Created: 15/09/2022 09:12:01 a. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t i = 0;
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t centenas = 0;
uint8_t temp = 0;
uint8_t digt_7seg[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; 

int main(void)
{
	//DDRB = 0xff;	// 255, 0b11111111, (1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0) |
	//DDRB |= (1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
	//DDRC |= (1<<DDB1) | (1<<DDB0);	
	
	DDRA = 0xff;					// Puerto A para el display
	DDRC = 0x00;					// Puerto C para los botones
	DDRB = 0xff;					// Puerto B para los transitores
	
	PORTA = 0x00;					// Apagamos el puerto A
	
    while (1) 
    {
		if ( PINC & (1<<0) ){
			if (i<255) i++;
			else i = 0;
			_delay_ms(100);
		}
		if ( PINC & (1<<1) ){
			if (i>0) i--;
			else i = 255;
			_delay_ms(100);
		}
		
		centenas = i/100;
		temp	 = i%100;
		decenas	 = temp/10;
		unidades = temp%10;
		
		PORTB = 0x01;					// Habilitamos el primer transistor para las unidades
		PORTA = digt_7seg[unidades];	// Enviamos el digito en el puerto A
		_delay_ms(1);

		PORTB = 0x02;					// Habilitamos el segundo transistor para las decenas
		PORTA = digt_7seg[decenas];		// Enviamos el digito en el puerto A
		_delay_ms(1);
		
		PORTB = 0x04;					// Habilitamos el tercer transistor para las centenas
		PORTA = digt_7seg[centenas];	// Enviamos el digito en el puerto A
		_delay_ms(1);
    }
	return 0;
}