#include <SPI.h>
#include <WiFi101.h>
#include <SparkFun_TB6612.h>

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
// Misc
int errorCode = 0;
// Button state and debouncing
int buttonState;
int lastButtonState;
const unsigned long debounceDelay = 50;
unsigned long debounceTime = 0;
// Motor
Motor motor;
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
	UpdateState(DOWNLOADING);
	DownloadRecipe();
	UpdateState(READY);
    UpdateUi(0);
}

void loop() {
    int now = millis();

    if (state == ERROR)
    {
		UpdateUi(now);
        delay(100);
        return;
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
                state = CHURNING;
            } else {
                state = READY;
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
	motor.drive(100, 500);
	motor.drive(255, 500);
	motor.brake();
}

void UpdateState(int newState)
{
	state = newState;
}

void Error(int code)
{
	errorCode = code;
	UpdateState(ERROR);
}

void SetupState() {
	pinMode(pinButton, INPUT);
	pinMode(pinLedR, OUTPUT);
	pinMode(pinLedG, OUTPUT);
	pinMode(pinLedB, OUTPUT);
	digitalWrite(pinLedR, ledR);
	digitalWrite(pinLedG, ledG);
	digitalWrite(pinLedB, ledR);
	motor = Motor(pinMotorIn1, pinMotorIn2, pinMotorPwm, 1, pinMotorStandby);
}

void SetupWifi() {
	//Initialize serial and wait for port to open
	//TODO: See if I need this for untethered operation
	Serial.begin(9600);
	while (!Serial) {
		;
	}

	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi shield not present");
		while (true);
	}

	// attempt to connect to Wifi network:
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(2000);
	}

	Serial.println("Connected to wifi");
	printWifiStatus();
}

void DownloadRecipe() {
	Serial.println("\nStarting connection to server...");
	// if you get a connection, report back via serial:
	if (client.connect(server, 80)) {
		Serial.println("connected to server");
		// Make a HTTP request:
		client.println(request);
		client.println("Host: amadeusw-iot.azurewebsites.net");
		client.println("Connection: close");
		client.println();
	}

	// if there are incoming bytes available
	// from the server, read them and print them:
	while (client.available()) {
		char c = client.read();
		Serial.write(c);
	}

	// if the server's disconnected, stop the client:
	if (!client.connected()) {
		Serial.println();
		Serial.println("disconnecting from server.");
		client.stop();
	}
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
