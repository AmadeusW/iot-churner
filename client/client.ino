#include <SPI.h>
#include <WiFi101.h>

// State of LEDs
int ledR = LOW;
int ledG = LOW;
int ledB = LOW;
// Pins
const int pinButton = 0;
const int pinMotorIn1 = 6;
const int pinMotorIn2 = 7;
const int pinMotorPwm = 8;
const int pinMotorStandby = 9;
const int pinLedR = 1;
const int pinLedG = 2;
const int pinLedB = 3;
// State machine              // can go to
const int UNINITIALIZED = 0;  // 1 4
const int DOWNLOADING = 1;    // 2 3 4
const int READY = 2;          // 3
const int CHURNING = 3;       // 2
const int ERROR = 4;
int state = UNINITIALIZED;
// Web response parse machine
const int RECEIVING_AWAITING = 0;
const int RECEIVING_TOTAL_TIME = 1;
const int RECEIVING_SPEED = 2;
const int RECEIVING_DURATION = 3;
const int RECEIVING_COMPLETE = 4;
const int RECEIVING_ERROR = -1;
// Errors
const int ERROR_CONNECTING = 1;
const int ERROR_PARSING = 2;
const int ERROR_HARDWARE_WIFI = 3;
int receivingProgram = 0;
int receivingSegment = 0;
int receivingState = RECEIVING_AWAITING;
int p1speeds[10] = { 0 };
int p2speeds[10] = { 0 };
int p1durations[10] = { 0 };
int p2durations[10] = { 0 };
// Misc
int errorCode = 0;
// Button state and debouncing
int buttonState;
int lastButtonState;
const unsigned long debounceDelay = 50;
unsigned long debounceTime = 0;
// WiFi
int status = WL_IDLE_STATUS;
WiFiClient client;
#include "D:\arduino-wifi.c"
/*
// Included in D:\arduino-wifi.c
char ssid[] = "...";
char pass[] = "...";
char request[] = "..."; // "GET /directory/file?param=value HTTP/1.1"
char server[] = "..."; // "your.hostname.com"
*/

void setup() {
    SetupState();
    SetupWifi();
}

void loop() {
    int now = millis();

    if (state == ERROR)
    {
        UpdateUi(now);
        delay(100);
        return;
    }
    if (state == DOWNLOADING)
    {
        delay(2000);
        DownloadRecipe();
        UpdateState(READY);
    }
    if (state == CHURNING)
    {
        Run();
    }

    int reading = digitalRead(pinButton);
    if (reading != lastButtonState) {
        // reset the debouncing timer
        debounceTime = now;
    }

    if (now > debounceTime + debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == HIGH) {
                UpdateState(CHURNING);
            } else {
                UpdateState(READY);
            }
        }
    }

    UpdateUi(now);
    lastButtonState = reading;
}

void UpdateUi(long millis) {
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
            brightness = 100 - (millis - debounceTime) / 100;
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

void Run()
{
    // Start
    digitalWrite(pinMotorStandby, HIGH);
    // Go forward
    digitalWrite(pinMotorIn1, HIGH);
    digitalWrite(pinMotorIn2, LOW);
    // Set speeds
    analogWrite(pinMotorPwm, 100);
    delay(500);
    analogWrite(pinMotorPwm, 255);
    delay(500);
    
    // Brake
    digitalWrite(pinMotorIn1, HIGH);
    digitalWrite(pinMotorIn2, HIGH);
    analogWrite(pinMotorPwm, 0);  
}

void UpdateState(int newState)
{
    Serial.println(newState);
    state = newState;
    UpdateUi(0);
}

void Error(int code)
{
    errorCode = code;
    UpdateState(ERROR);
    Serial.print("Error code: ");
    Serial.println(errorCode);
}

void SetupState() {
    pinMode(pinButton, INPUT);
    pinMode(pinLedR, OUTPUT);
    pinMode(pinLedG, OUTPUT);
    pinMode(pinLedB, OUTPUT);
    digitalWrite(pinLedR, ledR);
    digitalWrite(pinLedG, ledG);
    digitalWrite(pinLedB, ledR);
    pinMode(pinMotorIn1, OUTPUT);
    pinMode(pinMotorIn2, OUTPUT);
    pinMode(pinMotorPwm, OUTPUT);
    pinMode(pinMotorStandby, OUTPUT);
}

void SetupWifi() {
    Serial.begin(9600);

    // Wait for developer to launch the console
    while (!Serial) { ; }

    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        Error(ERROR_HARDWARE_WIFI);
        return;
    }

    delay(1000);
    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(2000);
    }
    UpdateState(DOWNLOADING);
    printWifiStatus();
}

void DownloadRecipe() {
    Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    
    if (client.connectSSL(server, 443)) {
        Serial.println("connected to server");
        // Make a HTTP request:
        Serial.println("GET /api/churn?code=g91FzFXKyVhsFcP8QeDpGaCe8e5ur4okOda8auCCTCA5n7l1R/znGA== HTTP/1.1");
        client.println("GET /api/churn?code=g91FzFXKyVhsFcP8QeDpGaCe8e5ur4okOda8auCCTCA5n7l1R/znGA== HTTP/1.1");
        client.println("Host: amadeusw-iot.azurewebsites.net");
        client.println("Connection: close");
        client.println();
    }
    else {
        Error(ERROR_CONNECTING);
    }

    int success = 0;
    while (!success) {

        while (client.available()) {
            char c = client.read();

            if (receivingState == RECEIVING_ERROR) {
                Error(ERROR_PARSING);
            }
            else if (receivingState == RECEIVING_COMPLETE) {
                
            }
            else {
                parseResponse(c);
            }
        }

        // if the server's disconnected, stop the client:
        if (!client.connected()) {
            Serial.println();
            success = 1;
            Serial.println("disconnecting from server.");
            client.stop();
        }
    }
}

void parseResponse(char c) {

    int awaitingDelimiterCount = 0;
    int totalTime = 0;

    Serial.write(c); // Debugging

    if (receivingState == RECEIVING_AWAITING) {
        if (c == ';') {
            awaitingDelimiterCount++;
            if (awaitingDelimiterCount == 3) {
                receivingState = RECEIVING_TOTAL_TIME;
                receivingSegment = 0;
                Serial.write("TotalTime");
            }
        }
        else {
            awaitingDelimiterCount = 0;
        }
    }
    else if (receivingState = RECEIVING_TOTAL_TIME) {
        if (c == ';') {
            receivingState = RECEIVING_DURATION;
            Serial.write("Speed");
        }
        else {
            int unit = parseDigit(c);
            totalTime *= 10;
            totalTime += unit;
        }
    }
    else if (receivingState = RECEIVING_DURATION) {
        if (c == ':') {
            receivingState = RECEIVING_SPEED;
            Serial.write("Speed");
        }
        else if (c == '.') {
            if (receivingProgram == 0) {
                receivingState = RECEIVING_TOTAL_TIME;
                receivingSegment = 0;
                receivingProgram = 1;
                Serial.write("TotalTimeTwo");
            }
            else {
                receivingState = RECEIVING_COMPLETE;
                Serial.write("Done");
            }
        }
        else {
            int unit = parseDigit(c);
            if (receivingProgram == 0) {
                p1speeds[receivingSegment] *= 10;
                p1speeds[receivingSegment] += unit;
            } else {
                p2speeds[receivingSegment] *= 10;
                p2speeds[receivingSegment] += unit;
            }
        }
    }
    else if (receivingState = RECEIVING_SPEED) {
        if (c == ';') {
            receivingState = RECEIVING_DURATION;
            receivingSegment++;
            Serial.write("Duration");
        }
        else {
            int unit = parseDigit(c);
            if (receivingProgram == 0) {
                p1durations[receivingSegment] *= 10;
                p1durations[receivingSegment] += unit;
            } else {
                p2durations[receivingSegment] *= 10;
                p2durations[receivingSegment] += unit;
            }
        }
    }
}

int parseDigit(char c) {
    int number = c - '0';
    if (number < 0 || number > 9) {
        receivingState = RECEIVING_ERROR;
        Serial.write("Error");
    }
    return number;
}

void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
