#include "state.h"
#include <WiFi101.h>
#include "wifi.h"
#include <SparkFun_TB6612.h>
#include "motor.h"

State state;
Wifi wifi;
Motor motor;

void setup() {
	state = State();
	motor = Motor();

	wifi = Wifi();
	state.UpdateState(State.DOWNLOADING);
	downloadRecipe();
	state.UpdateState(State.READY);
    updateUi(0);
}

void loop() {
    int now = millis();

    if (state == ERROR)
    {
        updateUi(now);
        delay(100);
        return;
    }
    if (state == CHURNING)
    {
        runMotor();
    }

    int reading = digitalRead(pinButton);
    if (reading != lastButtonState) {
        // reset the debouncing timer
        debounceTime = now;
    }

    if (now > lastDebounceTime + debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == HIGH) {
                state = CHURNING;
            } else {
                state = READY;
            }
        }
    }

    updateUi(now);
    lastButtonState = reading;
}
