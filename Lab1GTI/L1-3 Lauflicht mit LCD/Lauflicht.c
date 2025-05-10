#include "LPC17xx.h"

#define SIMULATION 

#ifndef SIMULATION
#define RELOAD_VALUE 			12499999	// Reload value for 125ms (100MHz * 125ms = 1250000) 
#define LONG_PULSE_WIDTH 	16				// Pulse with of a long press (16 * 125ms = 2s)
#else
#define RELOAD_VALUE 			1249999		// Reload value for 12.5ms (100MHz * 12.5ms = 125000) 
#define LONG_PULSE_WIDTH 	16				// Pulse with of a long press (16 * 12.5ms = 200ms)
#endif

#define PIN_BUTTON 		(1 << 10)
#define PIN_LED(pin) 	(1 << pin)
#define LED_PINS   		(PIN_LED(0) | PIN_LED(1) | PIN_LED(2) | PIN_LED(3) | PIN_LED(4) | PIN_LED(5) | PIN_LED(6) | PIN_LED(7))

typedef enum {
	STATE_OFF,
	STATE_LEFT,
	STATE_RIGHT,
} State;

volatile int DETECED_FALLING_EDGE = 0, DETECTED_RISING_EDGE = 0;
volatile int NEW_BUTTON_STATE = 0, OLD_BUTTON_STATE = 0;
volatile int BUTTON_PULSE_WIDTH = 0;

volatile State PREVIOUS_STATE = STATE_OFF, CURRENT_STATE = STATE_OFF;

void debounce_button(void) {
	int rising_edge = 0, falling_edge = 0;
	
	NEW_BUTTON_STATE = LPC_GPIO2->FIOPIN & PIN_BUTTON;
	
	if(NEW_BUTTON_STATE) {
		DETECED_FALLING_EDGE = 0;	// A value of 1 was read, so it can't be a falling edge anymore.
		
		// If the old button state was 0 and the current state is 1, there was a rising edge inbetween.
		rising_edge = OLD_BUTTON_STATE != NEW_BUTTON_STATE;
		DETECTED_RISING_EDGE = rising_edge ;
	} else {
		DETECTED_RISING_EDGE = 0; // A value of 0 was read, so it can't be a rising edge anymore.
		
		// If the old button state was 1 and the current state is 0, there was a falling edge inbetween.
		falling_edge = OLD_BUTTON_STATE != NEW_BUTTON_STATE;
		DETECED_FALLING_EDGE = falling_edge;
		
		// On a new falling edge, reset the button pulse width. If the button doesn't change it's state, increment the pulse width.
		BUTTON_PULSE_WIDTH = falling_edge ? 0 : BUTTON_PULSE_WIDTH + 1;
	}
	
	OLD_BUTTON_STATE = NEW_BUTTON_STATE;
}

int update_state_machine(void) {
	int changed_state = 0;
	
	if(DETECTED_RISING_EDGE && BUTTON_PULSE_WIDTH >= LONG_PULSE_WIDTH) {
		// If the button got pressed for a long time and was released change the states here
		
		if(CURRENT_STATE == STATE_OFF) {
			CURRENT_STATE = STATE_LEFT;
		} else if(CURRENT_STATE == STATE_LEFT || CURRENT_STATE == STATE_RIGHT) {
			CURRENT_STATE = STATE_OFF;
		}
	} else if(DETECTED_RISING_EDGE && BUTTON_PULSE_WIDTH < LONG_PULSE_WIDTH) {
		// If the button got pressed for a short time and was released change the states here
		
		if(CURRENT_STATE == STATE_LEFT) {
			CURRENT_STATE = STATE_RIGHT;
		} else if(CURRENT_STATE == STATE_RIGHT) {
			CURRENT_STATE = STATE_LEFT;
		}
	}
	
	changed_state = (PREVIOUS_STATE != CURRENT_STATE);
	PREVIOUS_STATE = CURRENT_STATE;
	
	return changed_state;
}

void handle_state_transition(void) {
	int leds_state = LPC_GPIO2->FIOPIN & LED_PINS;	// Read the led pin state
	
	switch(CURRENT_STATE) {
		case STATE_OFF: 
			LPC_GPIO2->FIOCLR = LED_PINS;			// Disable all the LEDs
			break;
		case STATE_LEFT: 
			if (leds_state) break;						// If there is already a LED on, just keep it.
			LPC_GPIO2->FIOSET = PIN_LED(0);		// Enable the first LED to start the process.
			break;
		case STATE_RIGHT:
			if (leds_state) break;						// If there is already a LED on, just keep it.
			LPC_GPIO2->FIOSET = PIN_LED(7);		// Enable the last LED to start the process.
			break;
	}
}

void handle_current_state(void) {
	int leds_state = LPC_GPIO2->FIOPIN & LED_PINS;	// Read the led pin state
	
	switch(CURRENT_STATE) {
		case STATE_OFF: 
			// Do nothing here, the off state just waits for state transitions.
			break;
		case STATE_LEFT: 
			LPC_GPIO2->FIOCLR = LED_PINS;	// Disable all active LEDs
		
			if(leds_state & PIN_LED(7)) {
				// In this case only the last LED is on.
				LPC_GPIO2->FIOSET = PIN_LED(0);	// Set the first one, to repeat the pattern
			} else {
				// In this case a LED is on inbetween the first and penultimate LED
				leds_state <<= 1;								// Left-shift the pins state to activate the next LED
				LPC_GPIO2->FIOSET = leds_state;	// Set the shifted pattern
			}
			break;
		case STATE_RIGHT: 
			LPC_GPIO2->FIOCLR = LED_PINS;	// Disable all active LEDs
		
			if(leds_state & PIN_LED(0)) {
				// In this case only the last LED is on.
				LPC_GPIO2->FIOSET = PIN_LED(7);	// Set the last one, to repeat the pattern
			} else {
				// In this case a LED is on inbetween the first and penultimate LED
				leds_state >>= 1;								// Right-shift the pins state to activate the next LED
				LPC_GPIO2->FIOSET = leds_state;	// Set the shifted pattern
			}
			break;
	}
}

void SysTick_Handler(void) {
	debounce_button();
	
	if(update_state_machine()) {
		handle_state_transition();
	} else {
		handle_current_state();
	}
}

void init_systick(void) {
	SysTick->CTRL = 0;	// Turns off the counter
	
	SysTick->LOAD = RELOAD_VALUE;				  // Sets the reload register to 125ms interrupts
	NVIC_SetPriority(SysTick_IRQn, 3);	  // The interrupt priority will be set to least urgent
	SysTick->VAL = 0;										  // Clearing of the current value register
	SysTick->CTRL |= (1 << 2) | (1 << 1); // Enables interrupts and configures the counter to use the processors clock
	
	SysTick->CTRL |= (1 << 0);	// Turns the counter on		
}

void init_gpio(void) {
	LPC_GPIO2->FIODIR &= ~PIN_BUTTON;	// Set the button pin to be an input (1 = output, 0 = input)
	LPC_GPIO2->FIODIR |= LED_PINS;		// Set the LED pins (0..7) to be an output
	
	LPC_GPIO2->FIOCLR = LED_PINS;			// Disable all the LEDs
}

int main(void) {
	init_systick();
	init_gpio();
	
	while(1);
}
