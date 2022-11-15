/*
 * Practica_13_Led_RGB_v2.c
 *
 * Created: 04/11/2022 09:55:47 p. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "my_keypad_lib.h"
#include "my_lcd_lib.h"

#define TOP 49
int rgb_percent[3] = {0, 0, 0};	// {R, G, B}
char rgb_percent_str[3][4];
char data_text[16];

void concatenate_str(char* RGB, char* level)
{
	strcat(strcpy(data_text, RGB), level);
	strcat(data_text, "%");
}

void send_data()
{
	lcd_clean_display();
	for (uint8_t i = 0; i < 3; i++)
	{
		sprintf(rgb_percent_str[i], "%d", rgb_percent[i]);
	}
	//data_text = "G:%s%s", rgb_percent_str[1], "%";
	
	concatenate_str("R:", rgb_percent_str[0]);
	lcd_goto_xy(0, 1);
	lcd_send_string(data_text);
	
	concatenate_str("G:", rgb_percent_str[1]);
	lcd_goto_xy(0, 9);
	lcd_send_string(data_text);
	
	concatenate_str("B:", rgb_percent_str[2]);
	lcd_goto_xy(1, 1);
	lcd_send_string(data_text);
	
	//lcd_goto_xy(1, 6);
	//sprintf(data_text, "%d, %d, %d", OCR4A, OCR4B, OCR4C);
	//lcd_send_string(data_text);
}

int main(void)
{
	//	Configuracion el Timer 4
	// ---------------------------------------------------------
	// Modo 14 - Fast PWM Mode (TOP = ICR4)
	TCCR4A &= ~(1<<WGM40);
	TCCR4A |= (1<<WGM41); 
	TCCR4B |= (1<<WGM42) | (1<<WGM43);
	
	// Configuramos el prescalador a 64
	TCCR4B |= (1<<CS40) | (1<<CS41);
	TCCR4B &= ~(1<<CS42);
	
	TCCR4C = 0x00;
	
	// Configurar OCR4A, OCR4B y OCR4C en el modo no invertido
	TCCR4A |= (1<<COM4A1) | (1<<COM4B1) | (1<<COM4C1);
	TCCR4A &= (~(1<<COM4A0) | ~(1<<COM4B0) | ~(1<<COM4C0));

	// Inicializamos el contador del timer 4
	TCNT4H = 0x00;
	TCNT4L = 0x00;
	
	ICR4H = 0x00;
	ICR4L = (uint8_t)TOP;
	
	OCR4AH = 0x00;
	OCR4AL = (uint8_t)(TOP*rgb_percent[0]/100);
	OCR4BH = 0x00;
	OCR4BL = (uint8_t)(TOP*rgb_percent[1]/100);
	OCR4CH = 0x00;
	OCR4CL = (uint8_t)(TOP*rgb_percent[2]/100);
	
	TIMSK4 |= (1<<OCIE4A) | (1<<OCIE4B) |(1<<OCIE4C) | (1<<TOIE4);
	
	
	//		Configuracion de interrupciones externas
	// ---------------------------------------------------------
	DDRD &= (~(1<<PD0) | ~(1<<PD1) | ~(1<<PD2));	// Declaramos pines como salidas
	EICRA |= (1<<ISC01) | (1<<ISC00);				// Configurar INT0 con flanco de subida
	EICRA |= (1<<ISC11) | (1<<ISC10);				// Configurar INT1 con flanco de subida
	EICRA |= (1<<ISC21) | (1<<ISC20);				// Configurar INT2 con flanco de subida
	EIMSK |= (1<<INT0) | (1<<INT1) | (1<<INT2);		// Habilitamos las interrupciones externas
	
	//  Configuracion de los pines de salida para las señales PWM
	// ---------------------------------------------------------
	DDRH |= (1<<PH3) | (1<<PH4) | (1<<PH5);
	PORTH = 0x00;
	//PORTH &= (~(1<<PORTH3) | ~(1<<PORTH4) | ~(1<<PORTH5));
	
	sei();
	
	//	Inicializacion del LCD y del teclado matricial
	// --------------------------------------------
	keypad_setup();
	lcd_initialization();
	send_data();
	
    while (1) 
    {
		
    }
}

ISR(TIMER4_OVF_vect)
{
	PORTH ^= ((1<<PORTH3) | (1<<PORTH4) | (1<<PORTH5));
}

ISR(TIMER4_COMPA_vect)
{
	PORTH ^= (1<<PORTH3);
}

ISR(TIMER4_COMPB_vect)
{
	PORTH ^= (1<<PORTH4);
}

ISR(TIMER4_COMPC_vect)
{
	PORTH ^= (1<<PORTH5);
}

ISR(INT0_vect)
{
	if (rgb_percent[0] < 100) rgb_percent[0] += 5;
	else rgb_percent[0] = 0;
	OCR4AH = 0x00;
	OCR4AL = (uint8_t)(TOP*rgb_percent[0]/100);
	send_data();
}

ISR(INT1_vect)
{
	if (rgb_percent[1] < 100) rgb_percent[1] += 5;
	else rgb_percent[1] = 0;
	OCR4BH = 0x00;
	OCR4BL = (uint8_t)(TOP*rgb_percent[1]/100);
	send_data();
}

ISR(INT2_vect)
{
	if (rgb_percent[2] < 100) rgb_percent[2] += 5;
	else rgb_percent[2] = 0;
	OCR4CH = 0x00;
	OCR4CL = (uint8_t)(TOP*rgb_percent[2]/100);
	send_data();
}
