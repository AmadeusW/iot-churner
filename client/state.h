// state.h

#ifndef _STATE_h
#define _STATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class State
{
public:
	void SetupPins();
	void UpdateState(int newState);
	void UpdateUi(long millis);
	void Error(int errorCode);
private:
	// State of LEDs
	int ledR = LOW;
	int ledG = LOW;
	int ledB = LOW;
	// Pins
	const int pinButton = 2;
	const int pinMotor = 13;
	const int pinLedR = 1;
	const int pinLedG = 3;
	const int pinLedB = 2;
	// State machine              // can go to
	const int UNINITIALIZED = 0;  // 1 4
	const int DOWNLOADING = 1;    // 2 3 4
	const int READY = 2;          // 3
	const int CHURNING = 3;       // 2
	const int ERROR = 4;
	int state = UNINITIALIZED;
	// Misc
	int errorCode = 0;
	// Button state and debouncing
	int buttonState;
	int lastButtonState;
	const unsigned long debounceDelay = 50;
	unsigned long debounceTime = 0;
};

#endif

