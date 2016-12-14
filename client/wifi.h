// wifi.h

#ifndef _WIFI_h
#define _WIFI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Wifi
{
public:
	void SetupWifi();
	void DownloadRecipe();
private:
	void printWifiStatus();

};

#endif