#ifndef BLUETOOTHAT8_H
#define BLUETOOTHAT8_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class BluetoothAt8{
	public:
	
	//CTOR && DTOR	
	BluetoothAt8();
	BluetoothAt8(long baudRate);
	BluetoothAt8(int rxPin,int txPin);
	BluetoothAt8(int rxPin,int txPin,long baudRate);

	~BluetoothAt8(){}
	
	public:
	
	//METHODS
	void log(const char* data);
	void logInt(const int data);
	void logString(const String data);
	void logFloat(const float data);
	void CR();	
	void setup();
	void send(char* data);
	void sendChar(char data);
	void sendInt(int data);
	String receive();	
	void toString();	
	bool m_serialPrint,m_masterMode=true;
	
	protected:
	//MEMBERS	
	int m_rxPin=16,m_txPin=17;
	long m_baudRate=9600;
	SoftwareSerial *bluetooth;
	String m_buffer;
	int m_lgt;
	
	
};



#endif
