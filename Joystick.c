void ADC_init()
{
	ADMUX = (1<<REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

//Read from the ADC channel ch
uint16_t ReadADC(uint8_t ch)
{	
	//Select ADC Channel
	ch &= 0x07;
	ADMUX = (ADMUX & 0xF8) | ch;

	//Start Single conversion
	ADCSRA |= (1 << ADSC);

	//Wait for conversion to complete
	while(ADCSRA & (1 << ADSC));

	//Clear ADIF
	ADCSRA |= (1<<ADIF);
	return (ADC);
}


int main(void)
{
	DDRA = 0x83; PORTA = 0x7C;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
  

	
	ADC_init();
  
  void moveU_D(int16_t U_D)
{
	u_char tmp = cursor[1];
	if(U_D > 0)
	{
		tmp = (tmp == 7) ? 0 : tmp+1;
	}
	else if(U_D < 0)
	{
		tmp = (tmp == 0) ? 7 : tmp-1;
	}
	cursor[1] = tmp;
}
