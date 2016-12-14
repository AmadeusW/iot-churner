#include "Arduino.h"
#include "state.h"

void State::SetupPins() {
    pinMode(pinButton, INPUT);
    pinMode(pinLedR, OUTPUT);
    pinMode(pinLedG, OUTPUT);
    pinMode(pinLedB, OUTPUT);
    pinMode(pinMotor, OUTPUT);
    digitalWrite(pinLedR, ledR);
    digitalWrite(pinLedG, ledG);
    digitalWrite(pinLedB, ledR);
    digitalWrite(pinMotor, 0);
}

void State::UpdateState(int newState)
{
	state = newState;
}

void State::UpdateUi(long millis) {
    ledR = 0;
    ledG = 0;
    ledB = 0;
    if (state == UNINITIALIZED)
    {
        ledR = 140;
    }
    else if (state == DOWNLOADING)
    {
        ledB = 50;
    }
    else if (state == READY)
    {
        ledG = 80;
    }  
    else if (state == CHURNING)
    {
        int brightness = 0;
        if (millis - debounceTime < 10000)
        {
            brightness = 100 - (millis - debounceTime)/100;
        }
        ledR = brightness * 1.5;
        ledG = brightness * 0.5;
        ledB = brightness * 0.8;
    }
    else if (state == ERROR)
    {
        // TODO: figure out how to blink LED depending on millis and errorCode
        if (millis % 500 < 250) {
            ledR = 200;
        }
    }
    analogWrite(pinLedR, ledR);
    analogWrite(pinLedG, ledG);
    analogWrite(pinLedB, ledB);
}

void State::Error(int code)
{
	errorCode = code;
	state = ERROR;
}