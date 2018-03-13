#include <avr/io.h>
 
#define F_CPU	4000000UL
#include <util/delay.h>
 
int main(void)
{
	DDRA = 0xFF;	// Configure port B as output
 
    while(1)
    {
        //TODO:: Please write your application code
 
		PORTA = 0b00110000;		// Display Number 1
		_delay_ms(1000);		// Wait for 1s
		PORTA = 0b01011011;		// Display Number 2
		_delay_ms(1000);		// Wait for 1s
		PORTA = 0b01001111;		// Display Number 3
		_delay_ms(1000);		// Wait for 1s
		PORTA = 0b01100110;		// Display Number 4
		_delay_ms(1000);		// Wait for 1s
 
		PORTA = 0b01110111;		// Display Letter A
		_delay_ms(1000);		// Wait for 1s
		PORTA = 0b00111001;		// Display Letter C
		_delay_ms(1000);		// Wait for 1s
		PORTA = 0b01111001;		// Display Letter E
		_delay_ms(1000);		// Wait for 1s
		PORTA = 0b01110001;		// Display Letter F
		_delay_ms(1000);		// Wait for 1s
 
    }
 
	return 0;
}
