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

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can't be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
    setupPins();
    setupMotor();
    setupWifi();
    state = DOWNLOADING;
    updateUi(0);
    downloadRecipe();
    state = READY;
    updateUi(0);
}

void loop() {
    int now = millis();
    int reading = digitalRead(buttonPin);
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
