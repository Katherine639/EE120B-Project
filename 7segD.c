
#include<avr/io.h>
/*Includes io.h header file where all the Input/Output Registers and its Bits are defined for all AVR microcontrollers*/

#define	F_CPU	1000000
/*Defines a macro for the delay.h header file. F_CPU is the microcontroller frequency value for the delay.h header file. Default value of F_CPU in delay.h header file is 1000000(1MHz)*/

#include<util/delay.h>
/*Includes delay.h header file which defines two functions, _delay_ms (millisecond delay) and _delay_us (microsecond delay)*/

#define 	_7SEGMENT_PORT		PORTB
/*_7SEGMENT_PORT is the microcontroller PORT Register to which the data pins of the 7-segment Display are connected. Here it is connected to PORTB*/	

#define 	_7SEGMENT_TYPE		COMMON_ANODE
/*_7SEGMENT_TYPE	is the type of 7-segment Display (Common Cathode or Common Anode) we are interfacing. Here we are connecting Common Anode type*/

#define COMMON_ANODE		0x01
#define COMMON_CATHODE		0x02
/*Macro Definitions*/

/*7 Segment Display Function Declarations*/
void display_7segment(unsigned char number);

int main(void)
{
	DDRB=0xff;
	/*All the 8 pins of PortB are declared output (all pins of 7 segment display are connected)*/

	/*Start of infinite loop*/
	while(1)
	{
		/*For loop to display from 0 to 9 and dot(.) in 7-segment display*/
		for(unsigned char i=0;i<=10;i++)
		{
			display_7segment(i);
			/*Displays a numbers or dot(.) in 7-segment display*/
			
			_delay_ms(7000);
			/*Display stays for 500ms*/
		}
	}
}
/*End of Program*/

/*7 Segment Display Function Definitions*/
void display_7segment(unsigned char number)
{
	if(_7SEGMENT_TYPE==COMMON_ANODE)
	{
		if(number==0)
			_7SEGMENT_PORT=0xc0;
		else if(number==1)
			_7SEGMENT_PORT=0xf9;
		else if(number==2)
			_7SEGMENT_PORT=0xa4;
		else if(number==3)
			_7SEGMENT_PORT=0xb0;
		else if(number==4)
			_7SEGMENT_PORT=0x99;
		else if(number==5)
			_7SEGMENT_PORT=0x92;
		else if(number==6)
			_7SEGMENT_PORT=0x82;
		else if(number==7)
			_7SEGMENT_PORT=0xf8;
		else if(number==8)
			_7SEGMENT_PORT=0x80;
		else if(number==9)
			_7SEGMENT_PORT=0x90;
		else if(number==10)
			_7SEGMENT_PORT=0x7f;
		else 
			;
	}
	else
	{
		if(number==0)
			_7SEGMENT_PORT=!(0xc0);
		else if(number==1)
			_7SEGMENT_PORT=!(0xf9);
		else if(number==2)
			_7SEGMENT_PORT=!(0xa4);
		else if(number==3)
			_7SEGMENT_PORT=!(0xb0);
		else if(number==4)
			_7SEGMENT_PORT=!(0x99);
		else if(number==5)
			_7SEGMENT_PORT=!(0x92);
		else if(number==6)
			_7SEGMENT_PORT=!(0x82);
		else if(number==7)
			_7SEGMENT_PORT=!(0xf8);
		else if(number==8)
			_7SEGMENT_PORT=!(0x80);
		else if(number==9)
			_7SEGMENT_PORT=!(0x90);
		else if(number==10)
			_7SEGMENT_PORT=!(0x7f);
		else 
			;

	}
}
}
