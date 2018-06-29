#ifndef DHTXX_H
#define DHTXX_H

#include <Arduino.h>

#define TYPE_DHT11

class DHTXXAt85{
	public:
	
	//CTOR && DTOR	
	DHTXXAt85();
	DHTXXAt85(byte pin);
	virtual ~DHTXXAt85(){}
	
	void * operator new(size_t size);
	void operator delete(void * ptr);
	
	//METHODS
	void setup();
	void readValues(byte pin, float* temperature, float* humidity);	
	byte readDHTxx(byte pin, byte* data, unsigned long start_time, unsigned long timeout);	
	#ifdef TYPE_DHT11
		byte readDHT11();	
	#endif
	#ifdef TYPE_DHT22
		byte readDHT22();
	#endif

	String readValues();	
	
	//MEMBERS
	
	/* Code d'erreur de la fonction readDHT11() et readDHT22() */
	const byte DHT_SUCCESS = 0;        // Pas d'erreur
	const byte DHT_TIMEOUT_ERROR = 1;  // Temps d'attente dépassé
	const byte DHT_CHECKSUM_ERROR = 2; // Données reçues erronées	
	
	byte m_dataPin = 3;

	float m_temperature=-44.0, m_humidity=-44.0;
	
};

#endif
