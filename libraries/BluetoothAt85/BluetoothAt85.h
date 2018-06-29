#ifndef BLUETOOTHAT85_H
#define BLUETOOTHAT85_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class BluetoothAt85{
	public:
	
	//CTOR && DTOR	
	BluetoothAt85(){}
	BluetoothAt85(uint8_t rxPin,uint8_t txPin,long baudRate);
	~BluetoothAt85(){}
	
	void * operator new(size_t size);
	void operator delete(void * ptr);
	
	//METHODS
	void setup(uint8_t rxPin,uint8_t txPin,long baudRate);
	void send(char* data);
	void sendInt(int data);
	String receive();
	
	//MEMBERS	
	uint8_t m_rxPin,m_txPin;
	long m_baudRate;
	SoftwareSerial *bluetooth;	

	
	
	
};

#endif
