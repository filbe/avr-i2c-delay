#include <avr/pgmspace.h>
#include "avr-i2c-delay.h"


const uint8_t sine[] PROGMEM = {127, 130, 133, 136, 139, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173, 176, 179, 182, 184, 187, 190, 193, 195, 198, 200, 203, 205, 208, 210, 213, 215, 217, 219, 221, 224, 226, 228, 229, 231, 233, 235, 236, 238, 239, 241, 242, 244, 245, 246, 247, 248, 249, 250, 251, 251, 252, 253, 253, 254, 254, 254, 254, 254, 255, 254, 254, 254, 254, 254, 253, 253, 252, 251, 251, 250, 249, 248, 247, 246, 245, 244, 242, 241, 239, 238, 236, 235, 233, 231, 229, 228, 226, 224, 221, 219, 217, 215, 213, 210, 208, 205, 203, 200, 198, 195, 193, 190, 187, 184, 182, 179, 176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 139, 136, 133, 130, 127, 124, 121, 118, 115, 111, 108, 105, 102, 99, 96, 93, 90, 87, 84, 81, 78, 75, 72, 70, 67, 64, 61, 59, 56, 54, 51, 49, 46, 44, 41, 39, 37, 35, 33, 30, 28, 26, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 26, 28, 30, 33, 35, 37, 39, 41, 44, 46, 49, 51, 54, 56, 59, 61, 64, 67, 70, 72, 75, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111, 115, 118, 121, 124
                               };
#define MAX_BUFFER_SIZE 8000

uint16_t buffersize = 7000;

volatile uint16_t audio_buf[MAX_BUFFER_SIZE];
volatile uint16_t audio_buf_cursor = 0;

void receive_i2c()
{
	uint8_t command = twi_rxBuffer[0];

	switch (command) {
	case 0x00:

		break;
	}
}

void init_adc()
{
	ADMUX = (1 << REFS0);   //select AVCC as reference
	ADCSRA = (1 << ADEN);// | (1 << ADPS2); //enable and prescale = 128 (16MHz/128 = 125kHz)



	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC

	// No MUX values needed to be changed to use ADC0

	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADSC); // Start A2D Conversions
	ADMUX |= 0; // channel
}

uint16_t read_adc()
{
	ADCSRA |= (1 << ADSC);
	return ADCW;
}


int main()
{

	for (uint16_t i=0;i<MAX_BUFFER_SIZE;i++) {
		audio_buf[i] = 127;
	}

	cli();
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	ICR1 = F_CPU / 33000 / 8 - 1;
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS11);
	sei();

	init_adc();

	DDRB = 255;
	DDRA = 0;


	twi_init();
	twi_setAddress(0x20);
	twi_attachSlaveRxEvent(receive_i2c);

	

	while (1) {

		



	}
	return 0;
}

ISR (TIMER1_COMPA_vect)
{
	audio_buf[audio_buf_cursor] = (read_adc() + ((audio_buf[audio_buf_cursor] * 110) >> 8)) / 2.5;
	audio_buf_cursor++;
	if (audio_buf_cursor >= MAX_BUFFER_SIZE) {
		audio_buf_cursor = 0;
	}
	PORTB = audio_buf[audio_buf_cursor] >> 1;



}