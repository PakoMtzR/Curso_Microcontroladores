/*
 * Practica_14_Servo.c
 *
 * Created: 06/11/2022 08:03:23 p. m.
 * Author : PakoMtz
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_keypad_lib.h"
#include "my_lcd_lib.h"

char key;
char key_value[2];
int degrees[3] = {0, 0, 0};
int degrees_value = 0;
char degrees_str[4] = "0";
char lcd_data[16];
int top = 4999;

void timer5_setup(void)
{
	// Configuracion Fast PWM Mode (modo 14)
	TCCR5A |= (1<<WGM51);
	TCCR5A &= ~(1<<WGM50);
	TCCR5B |= (1<<WGM53) | (1<<WGM52);
	TCCR5C = 0x00;
	
	// Configuracion del prescalador a 64
	TCCR5B |= (1<<CS50) | (1<<CS51);
	TCCR5B &= ~(1<<CS52);
	
	// Modo No Invertido
	TCCR5A &= ~(1<<COM5A0);
	TCCR5A |= (1<<COM5A1);
	
	// Inicializamos el contador del timer 5
	TCNT5H = 0x00;
	TCNT5L = 0x00;
	
	// Definimos el top
	ICR5 = 4999;
	
	// Definimos el ancho del pulso
	//OCR5A = (uint16_t)(25*degrees_value/18 + 249);
	OCR5A = 99;
	
	// Habilitar interrupciones del timer 5
	TIMSK5 |= (1<<OCIE5A) | (1<<TOIE5);
	
	sei();
	
	//  Configuracion de los pines de salida para la señal PWM
	DDRL |= (1<<PL3);
	PORTL &= ~(1<<PORTL3);
}

void lcd_send_data(void)
{
	sprintf(degrees_str, "%d%d%d", degrees[0], degrees[1], degrees[2]);
	sprintf(lcd_data, "[degrees] = %s", degrees_str);
	
	lcd_clean_display();
	lcd_goto_xy(0, 0);
	lcd_send_string("Press A to send");
	lcd_goto_xy(1, 0);
	lcd_send_string(lcd_data);
}

void servo_send_data(void)
{
	degrees_value = atoi(degrees_str);	// Convertimos a valor str a int
	
	if (degrees_value > 180)
	{
		lcd_clean_display();
		lcd_goto_xy(0, 1);
		lcd_send_string("[ERROR]: value");
		lcd_goto_xy(1, 2);
		lcd_send_string("out of range");
	}
	else
	{
		OCR5A = (uint16_t)(25*degrees_value/9 + 99);	// Recalculamos el ancho del pulso
		
		lcd_clean_display();
		lcd_goto_xy(0, 1);
		sprintf(lcd_data, "sending... %dg", degrees_value);
		lcd_send_string(lcd_data);
		lcd_goto_xy(1, 1);
		sprintf(lcd_data, "OCR5A = %d", OCR5A);
		lcd_send_string(lcd_data);
	}
	for (uint8_t i = 0; i < 3; i++) degrees[i] = 0;	// Resetar arreglo
	_delay_ms(2000);	
}

int main(void)
{
	timer5_setup();
	keypad_setup();
	lcd_initialization();
	lcd_send_data();
	
    while (1) 
    {
		_delay_ms(10);
		key = keypad();		// Lectura del teclado matricial
		
		if (key != '\0')
		{
			switch(key)
			{
				case 'A':
					servo_send_data();	// Enviar angulo al servo
					break;
					
				default:
					// Verificar que haya sido un numero lo que presiono
					if ((key != 'B') | (key != 'C') | (key != 'D') | (key != '*') | (key != '#'))
					{
						degrees[0] = degrees[1];		// (1) Recorrer los valores del
						degrees[1] = degrees[2];		//	   arreglo un lugar a la izquierda
						sprintf(key_value, "%c", key);	// (2) Convertir el valor ingresado y
						degrees[2] = atoi(key_value);	//     agregarlo al final del arreglo
					}
					break;
			}
			lcd_send_data();
		}
    }
}

ISR(TIMER5_OVF_vect)
{
	PORTL ^= (1<<PORTL3);
}

ISR(TIMER5_COMPA_vect)
{
	PORTL ^= (1<<PORTL3);
}