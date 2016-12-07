#include motor
#include pins
#include wifi-utilities

// State of buttons
int buttonState;
int lastButtonState;
const unsigned long debounceDelay = 50;
unsigned long debounceTime = 0;

// State machine              // can go to
const int UNINITIALIZED = 0;  // 1 4
const int DOWNLOADING = 1;    // 2 3 4
const int READY = 2;          // 3
const int CHURNING = 3;       // 2
const int ERROR = 4;
int errorCode = 0;
int state = UNINITIALIZED;

void setup() {
    setupPins();
//    setupMotor();
    setupWifi();
    state = DOWNLOADING;
    updateUi(0);
    downloadRecipe();
    state = READY;
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
