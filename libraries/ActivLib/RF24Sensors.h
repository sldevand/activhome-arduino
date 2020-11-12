#ifndef RF24SENSORS_H
#define RF24SENSORS_H

#include "structures.h"

#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "domusbox_defines.h"
#include <TimeLib.h>
#include <math.h>

class RF24Sensors : public RF24
{
public:
	/*****CTOR DTOR*****/

	RF24Sensors(uint16_t cePin, uint16_t csnPin);

	virtual ~RF24Sensors() {}

	/*****MEMBERS****/
	Sensor m_sensor;
	Thermostat m_thermostat;
	Mode m_mode;
	DayPlan m_dayplan;
	tmElements_t m_rtc;
	Message m_message;

	byte m_cePin = 8;
	byte m_csnPin = 9;

	byte *m_writeAdress[6] = WRITE_PIPES;
	byte *m_readAdress[6] = READ_PIPES;

	int m_channel = 108;

	/*****METHODS****/
	void setupRadio();
	void stopRadio();
	void startRadio();
	uint8_t radioReceive();
	char *getId();
	float getTemp();

	template <typename T>
	void radioTransmit(T buffer)
	{
		stopRadio();
		setPayloadSize(sizeof(buffer));
		write(&buffer, sizeof(buffer));
		delay(1);
		startRadio();
	}
};

#endif
