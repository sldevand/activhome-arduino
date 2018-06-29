#include <BluetoothAt85.h>

BluetoothAt85::BluetoothAt85(uint8_t rxPin,uint8_t txPin,long baudRate){setup(rxPin,txPin,baudRate);}
void * operator new(size_t size){return malloc(size);}
void operator delete(void * ptr){free(ptr);}

void BluetoothAt85::setup(uint8_t rxPin,uint8_t txPin,long baudRate){	
	m_rxPin=rxPin;m_txPin=txPin;m_baudRate=baudRate;
	bluetooth = new SoftwareSerial(m_rxPin, m_txPin);	
	bluetooth->begin(m_baudRate);	
}

void BluetoothAt85::send(char* data){bluetooth->write(data);}
void BluetoothAt85::sendInt(int data){bluetooth->write(data);}


String BluetoothAt85::receive(){
	
	String buffer;
	int lgt=0;	
	while (bluetooth->available()) {
        char tmp=bluetooth->read();
        delay(1);       
		buffer+=tmp;  
	    lgt++;       
    }		
	
	return buffer;
}

