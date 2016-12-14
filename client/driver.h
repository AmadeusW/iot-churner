// motor.h

#ifndef _DRIVER_h
#define _DRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Driver
{
public:
	void SetupMotor();
	void Run();
private:
};

#endif

