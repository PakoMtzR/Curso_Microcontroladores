/*
 * my_keypad_lib.h
 *
 * Created: 01/11/2022 10:04:24 p. m.
 *  Author: PakoMtz
 */ 


#ifndef MY_KEYPAD_LIB_H_
#define MY_KEYPAD_LIB_H_

#define KEYPAD_PORT	PORTK
#define KEYPAD_PIN	PINK
#define KEYPAD_CONFIG DDRK

#include <util/delay.h>
uint8_t data = 0;

//  Configuracion del keypad
// --------------------------------------------
void keypad_setup(void)
{
	KEYPAD_CONFIG = 0x0f;
	KEYPAD_PORT = 0xff;
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
#endif /* MY_KEYPAD_LIB_H_ */