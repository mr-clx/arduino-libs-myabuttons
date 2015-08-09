// 
// 
// 

#include "Arduino.h"
#include "MyAButtons.h"


MyAButtons::MyAButtons(uint8_t pin){
	_pin = pin;
}

void MyAButtons::init(void (*onPress)(uint8_t id), void (*onHold)(uint8_t id), void (*onRelease)(uint8_t)) {
	pinMode (_pin, INPUT_PULLUP);
	//this->adc_delta= adcDelta;
	//this->debounce_delay_ms;
	//this->hold_delay_ms = holdDelay;
	last_pressed_id=last_holding_id=last_released_id=0;
	last_tick=0;
	last_pressed_tick=0;
	this->on_press=onPress;
	this->on_hold=onHold;
	this->on_release=onRelease;
}

void MyAButtons::add(uint8_t id, uint8_t readValueDiv4) {
	if (buttons_count>=MYABUTTONS_CAPACITY) {
		Serial.println(F("Not enough MyAButtons capacity!"));
		return;
	}
	if (!id) {
		Serial.println(F("ButtonId can not be 0!"));
		return;
	}
	buttons[buttons_count].id=id;
	buttons[buttons_count].read_value = readValueDiv4;
	buttons_count++;
	// todo: check for duple!
}

uint8_t MyAButtons::tick(uint32_t *tick, bool returnHoldState) {
	
	if (*tick-last_tick < MYABUTTONS_TICK_INTERVAL) return returnHoldState? last_pressed_id : 0;
	last_tick = *tick;
	
	int v = analogRead(_pin)>>2;
	
	uint8_t bId=0;
	for (uint8_t i=0; i<buttons_count && !bId;i++) {
		int bAdc= buttons[i].read_value;
		if (v>=bAdc-MYABUTTONS_READ_DELTA && v<=bAdc+MYABUTTONS_READ_DELTA) {
			bId=buttons[i].id;
		}
	}
	
	// button changed?, need release previous
	if (bId!=last_pressed_id) {
		
#ifdef MYABUTTONS_DEBOUNCE_DELAY {
		// debouncing
		//if (bId) {
		if (bId==debounce_button_id) {
			if (abs(last_tick-debounce_tick_start)<MYABUTTONS_DEBOUNCE_DELAY) {
#ifdef MYABUTTONS_DEBUG
				Serial.println("Debouncing: wait...");
#endif
				return returnHoldState? last_pressed_id : 0;
			}
			debounce_button_id=0xff;
			debounce_tick_start=0;
#ifdef MYABUTTONS_DEBUG
			Serial.println("Debouncing: end");
#endif
		} else {
			debounce_button_id=bId;
			debounce_tick_start=last_tick;
#ifdef MYABUTTONS_DEBUG
			Serial.println("Debouncing: start");
#endif
			return returnHoldState? last_pressed_id : 0;
		}
#endif
		// debounce_button_id=0; // reset debounce
		last_holding_id=0;
		if (last_pressed_id) {
			if (on_release) on_release(last_pressed_id);
			last_released_id = last_pressed_id;
		}
	} 
	
#ifdef MYABUTTONS_DEBOUNCE_DELAY {
	else if (debounce_button_id!=0xff) {
		debounce_button_id=0xff;
		debounce_tick_start=0;
		// Serial.println("aaa");
	}
#endif
	
	// button pressed?
	if (bId) {
		if (last_pressed_id!=bId) { // new button?
			if (on_press) on_press(bId);
			last_pressed_tick = last_tick;
		} else { // same button, check anti-repeat and hold
			uint32_t elapsed =last_tick-last_pressed_tick;
			if (elapsed>=MYABUTTONS_HOLD_DELAY) {
				last_pressed_tick = last_tick;
				if (on_hold) on_hold(bId);
				last_holding_id= bId;
			}
			else if (!returnHoldState) {
				return 0; // no key pressed because of hold delay
			}
		}
	}

	last_pressed_id = bId;
	return bId;
}

// MyAButtons abuttons();
