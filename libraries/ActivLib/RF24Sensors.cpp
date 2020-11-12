#include "RF24Sensors.h"

RF24Sensors::RF24Sensors(uint16_t cePin, uint16_t csnPin) : RF24(cePin, csnPin)
{

	m_cePin = cePin;
	m_csnPin = csnPin;
	setupRadio();
}

void RF24Sensors::setupRadio()
{

	ce_pin = m_cePin;
	csn_pin = m_csnPin;
	p_variant = false;
	payload_size = 32;
	dynamic_payloads_enabled = false;
	addr_width = 5;
	csDelay = 5;
	pipe0_reading_address[0] = 0;

	begin();
	setAutoAck(1);		// Ensure autoACK is enabled
	setRetries(15, 15); // Max delay between retries & number of retries

	for (byte i = 0; i < 5; i++)
	{
		openReadingPipe(i + 1, m_readAdress[i]); // Reading pipes from device read addresses
	}
	openWritingPipe(m_writeAdress[0]); // Write to device write address

	setPALevel(RF24_PA_HIGH);
	setChannel(m_channel);
	setDataRate(RF24_250KBPS);
	enableDynamicPayloads();
	enableAckPayload();
	startRadio();
	m_sensor.hygro = -1.0;
	m_thermostat.consigne = 19.00;
	m_thermostat.delta = 0.5;
	m_thermostat.tempExt = 20.00;
	m_thermostat.interne = true;
	m_thermostat.etat = false;
	m_thermostat.mode = 1;

	Serial.println(F("init RF24"));
}

void RF24Sensors::stopRadio()
{

	stopListening();
}

void RF24Sensors::startRadio()
{
	startListening();
}

uint8_t RF24Sensors::radioReceive()
{

	if (available())
	{

		uint8_t payloadSize = getDynamicPayloadSize();
		setPayloadSize(payloadSize);

		switch (payloadSize)
		{
		case sizeof(Sensor):
		{
			read(&m_sensor, sizeof(Sensor));
			String str(m_sensor.id);
			break;
		}
		case sizeof(Thermostat):
		{
			read(&m_thermostat, sizeof(Thermostat));
			delay(2);
			break;
		}
		case sizeof(Mode):
		{
			read(&m_mode, sizeof(Mode));
			delay(2);
			break;
		}
		case sizeof(DayPlan):
		{
			read(&m_dayplan, sizeof(DayPlan));
			delay(2);
			break;
		}
		case sizeof(tmElements_t):
		{
			read(&m_rtc, sizeof(tmElements_t));
			break;
		}
		case sizeof(Message):
		{
			read(&m_message, sizeof(Message));
			break;
		}
		default:
		{
			flush_rx();
			return 0;
		}
		}

		return payloadSize;
	}
	else
	{
		return 0;
	}
}

char *RF24Sensors::getId()
{
	return m_sensor.id;
}

float RF24Sensors::getTemp()
{
	return m_sensor.temp;
}
