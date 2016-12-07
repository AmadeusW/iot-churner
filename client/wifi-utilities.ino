#include <SPI.h>
#include <WiFi101.h>
#include "D:\arduino-wifi.c"
/*
// Included in D:\arduino-wifi.c
char ssid[] = "...";
char pass[] = "...";
char churner-request = "...";
*/

int status = WL_IDLE_STATUS;
char server[] = "amadeusw-iot.azurewebsites.net";
WiFiClient client;

void setupWifi() {
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

void downloadRecipe() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println(churner-request);
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





