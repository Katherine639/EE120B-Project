#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define LED_Direction DDRC		/* define LED Direction */
#define LED_PORT PORTC			/* define LED port */

int main(void)
{
	LED_Direction |= 0xff;		/* define LED port direction is output */
	LED_PORT = 0xff;
	
	char array[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
	/* write hex value for CA display from 0 to 9 */
	
	while(1)
	{
		for(int i=0;i<10;i++)
		{
			LED_PORT = array[i]; /* write data on to the LED port */
			_delay_ms(9000); /* wait for 1 second */
		}
	}
}
