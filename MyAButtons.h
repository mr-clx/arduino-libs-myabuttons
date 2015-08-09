// MyAButtons.h

#ifndef _MYABUTTONS_h
#define _MYABUTTONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#define MYABUTTONS_PIN A0   // connected to analog pin
#define MYABUTTONS_TICK_INTERVAL 50  // minimal scan interval
//#define MYABUTTONS_DEBOUNCE_DELAY 30 // delay between reading state
#define MYABUTTONS_HOLD_DELAY 1500   // anti-repeat delay (on hold interval)
#define MYABUTTONS_READ_DELTA 6      // reading delta margin (>>2)

// #define MYABUTTONS_DEBUG
#define MYABUTTONS_CAPACITY 5         // capacity of buttons array

// struct describe one analog button
typedef struct {
	uint8_t id;
	uint8_t read_value; // read read value divide by 4
} myabutton_t;


class MyAButtons
{
 protected:
	uint8_t _pin;

	uint8_t debounce_button_id;    // pressed button to check de-bounce
	uint32_t debounce_tick_start; // started de-bounce
 public:
	myabutton_t buttons[MYABUTTONS_CAPACITY];
	uint8_t buttons_count;

	uint32_t last_tick;
	uint8_t last_pressed_id;
	uint8_t last_released_id;
	uint8_t last_holding_id;
	uint32_t last_pressed_tick;
	//uint32_t last_released_tick;
	
	void (*on_press)(uint8_t id);
	void (*on_release)(uint8_t id);
	void (*on_hold)(uint8_t id);
	
	MyAButtons(uint8_t pin);
	void init(void (*onPress)(uint8_t id) = 0, void (*onHold)(uint8_t id)=0, void (*onRelease)(uint8_t)=0);
	void add(uint8_t id, uint8_t readValueDiv4); // add button to list, id must be >0 !
	
	uint8_t tick(uint32_t *tick, bool returnHoldState);
};
// class size = 20+4*2=26 bytes

// extern MyAButtons abuttons;

#endif

