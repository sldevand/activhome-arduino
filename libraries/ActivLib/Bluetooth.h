#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>
#include <SoftwareSerial.h>


class Bluetooth{
	public:
	
	//CTOR && DTOR	
	Bluetooth();
	explicit Bluetooth(long baudRate);
	Bluetooth(int rxPin,int txPin);
	Bluetooth(int rxPin,int txPin,long baudRate);
	Bluetooth(int rxPin,int txPin,int atPin,long baudRate);
	Bluetooth(int rxPin,int txPin,int atPin,int powerPin,long baudRate);
	

	virtual ~Bluetooth(){}
	
	public:
	
	//METHODS
	void log(const char* data);
	void logInt(const int data);
	void logString(const String data);
	void logFloat(const float data);
	void CR();
	void setRxPin(int rxPin);
	void setTxPin(int txPin);
	void setAtPin(int atPin);
	void setPowerPin(int powerPin);
	void setBaudRate(long baudRate);
	int getRxPin();
	int getTxPin();
	int getAtPin();
	int getPowerPin();
	long getBaudRate();
	bool getAtMode();
	
	void restart();
	
	void setup();
	void send(char* data);
	void sendChar(char data);
	void sendInt(int data);
	String receive();	
	
	void powerOn();
	void powerOff();
	
	#ifdef AT_MODE_ON
		void setAtMode(bool mode);	
		void setRole(bool role);	//master = true , slave = false
		void getRole();	
		void getState();
		void getName();
		void getAddr();
		
		void initSPPLib();	
		void reset();
		void inquireDevicesAround();	
		void pairWithDevice(String address,int time);
		//void clearPrevDevices();
		void atModeToString();
	#endif

	void toString();
	
	
	bool m_serialPrint;
	bool m_masterRole=false;
	
	protected:
	//MEMBERS
	
	int m_rxPin=16,m_txPin=17,m_atPin=15,m_powerPin=18;
	long m_baudRate=9600,m_previousBaudRate=9600;
	SoftwareSerial *blueTooth;
	bool hasMessage,m_atMode=false;
	String m_buffer;
	int m_lgt;

	
	
	
};



#endif
