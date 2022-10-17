/*
 * my_lcd_lib.h
 *
 * Created: 04/10/2022 12:52:21 a. m.
 *  Author: PakoMtz
 */ 
#include <util/delay.h>
#ifndef MY_LCD_LIB_H_
#define MY_LCD_LIB_H_


#define LCD_DATA	PORTC
#define LCD_PORT	PORTA
#define E_PIN		PA2
#define RW_PIN		PA1
#define RS_PIN		PA0

void lcd_send_command(unsigned char cmd)
{
	LCD_DATA = cmd;
	// RS = 0 / RW = 0 / E = 1
	LCD_PORT = (((1<<E_PIN) | ~(1<<RS_PIN)) & ~(1<<RW_PIN));
	_delay_ms(1);
	LCD_PORT &= ~(1<<E_PIN);
	_delay_ms(1);
}

void lcd_initialization(void)
{
	uint8_t config_commands[7] = {0x30, 0x30, 0x30, 0x38, 0x0c, 0x01, 0x06};
	DDRC = 0xff;
	DDRA = 0x07;
	_delay_ms(15);
	for (int i = 0; i < 7; i++)
	{
		lcd_send_command(config_commands[i]);
		_delay_ms(5);
	}
}

void lcd_send_char(unsigned char m)
{
	LCD_DATA = m;
	// RS = 1 / RW = 0 / E = 1
	LCD_PORT = (((1<<E_PIN) | (1<<RS_PIN)) & ~(1<<RW_PIN));
	_delay_ms(1);
	LCD_PORT &= ~(1<<E_PIN);
	_delay_ms(1);
}

void lcd_goto_xy(int x, int y)
{
	// Line 1 => 0x80, Line 2 => 0xc0
	if (x > 0) lcd_send_command(0xC0 + y);
	else lcd_send_command(0x80 + y);
}

void lcd_send_string(char *data)
{
	// *data = "texto\0"
	while (*data != '\0')
	{
		lcd_send_char(*data);
		data++;
	}
}

void lcd_clean_display(void)
{
	lcd_send_command(0x01);
}

#endif /* MY_LCD_LIB_H_ */