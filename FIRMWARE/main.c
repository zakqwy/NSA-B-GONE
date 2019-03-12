/*
Released under the terms of the MIT License.

The MIT License (MIT)
Copyright (c) 2019 by Zach Fredin
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

NSA-B-GONE
Designed for Thinkpads. Mutes your camera and microphones when you double click the Thinklight key. Makes the HDD light pulse when active and flash SOS when not. Only tested on the X220.

PC0		CAM_MIC //set to block cam/mic
PC1		LED_HDD_IN
PC2		LED_HDD_OUT //clear to turn on HDD LED
PC3		THINKLIGHT_IN //goes low when ThinkLight is on
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "HAL.h"

volatile uint8_t tick = 0;
uint8_t state_thinklight = 0;
uint8_t state_thinklight_prev = 0;
uint8_t state_cammic = 0; //0 = disabled, 1 = enabled
uint16_t counter_thinklight = 0; //time since last TL hit
uint16_t counter_thinklight_reset = 300; //max delay between TL hits
uint8_t toggles_thinklight = 0; //keeps track of TL hits to toggle cam/mic
uint8_t led_count = 0;
uint8_t led_count_reset = 100;
uint8_t led_pos = 0;
uint8_t led_pos_reset = 33;
uint8_t led_sos_array[34] = {1,0,1,0,1,0,0,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0};

ISR(TIMER0_COMPA_vect) {
	tick = 1;
}

int main(void) {
	SystemInit();
	LedOff();
	for(;;){
		while(tick == 0) {} //1 ms tick
		tick = 0;

		//get ThinkLight state and check for status change
		if (PIN_IO & (1<<THINKLIGHT_IN)) {
			state_thinklight = 0;
		}
		else {
			state_thinklight = 1;
		}
		if (state_thinklight != state_thinklight_prev) {
			counter_thinklight = counter_thinklight_reset;
			toggles_thinklight++;
		}

		//toggle cam/mic state if toggles are sufficient
		if (toggles_thinklight > 3) {
			if (state_cammic == 0) {
				state_cammic = 1;
			}
			else {
				state_cammic = 0;
			}
			toggles_thinklight = 0;
		}

		//decrement ThinkLight delay counter as needed, and reset toggle
		if (counter_thinklight > 0) {
			counter_thinklight--;
		}
		else {
			toggles_thinklight = 0;
		}

		//update cam/mic state and HDD LED state
		switch(state_cammic) {
			case 0: //cam/mic should be off, HDD LED is off
				CamMicOff();
				LedOff();
				break;
			default: //cam/mic should be on, HDD LED sends SOS
				CamMicOn();
				if (led_count == led_count_reset) {
					led_count = 0;
					if (led_sos_array[led_pos] == 1) {
						LedOn();
					}
					else {
						LedOff();
					}
					led_pos++;
					if (led_pos > led_pos_reset) {
						led_pos = 0;
					}
				}
				led_count++;
				break;
		}

		state_thinklight_prev = state_thinklight;
	}
}
