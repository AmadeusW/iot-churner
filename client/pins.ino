// Pins

const int pinButton = 2;
const int pinLed = 13;
const int pinLedR = 1;
const int pinLedG = 3;
const int pinLedB = 2;

int ledR = LOW;
int ledG = LOW;
int ledB = LOW;

void setupPins() {
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

void updateUi(int millis) {
    state = newState;
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
        if (millis() - lastDebounceTime < 10000)
        {
            brightness = 100 - (millis - lastDebounceTime)/100;
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