#include <avr/io.h>
#include <avr/interrupt.h>
#include "HAL.h"

void SystemInit(void) {
	DDRC |= (1<<CAM_MIC); //Cam and mic toggle is an output
	DDRC |= (1<<LED_HDD_OUT); //HDD LED is an output
	DDRC &= ~(1<<THINKLIGHT_IN); //THINKLIGHT_IN is an input
	PORT_IO |= (1<<THINKLIGHT_IN); //ThinkLight pullup on

	//set up tick timer
	TCCR0A |= (1<<CTC0); //clear on timer compare (OCR0A)
	TCCR0A |= (1<<CS02); //clk/256 (32.5 kHz)
	TIMSK0 |= (1<<OCIE0A); //enable Compare A interrupt
	OCR0A = 30; //ticks are ~1 ms
	sei();
}

void CamMicOff(void) {
	PORT_IO |= (1<<CAM_MIC);
}

void CamMicOn(void) {
	PORT_IO &= ~(1<<CAM_MIC);
}

void LedOn(void) {
	PORT_IO &= ~(1<<LED_HDD_OUT);
}

void LedOff(void) {
	PORT_IO |= (1<<LED_HDD_OUT);
}
